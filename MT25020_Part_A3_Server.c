// MT25020
#include "MT25020_Common.h"
#include <sys/uio.h>
#include <linux/errqueue.h>

#ifndef SO_ZEROCOPY
#define SO_ZEROCOPY 60
#endif

// Helper: send full iovec handling partial sendmsg results
static ssize_t send_iov_all_local(int sock, struct iovec *iov, int iovcnt, int flags) {
    struct msghdr hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.msg_iov = iov;
    hdr.msg_iovlen = iovcnt;

    size_t total = 0;
    for (int i = 0; i < iovcnt; i++) total += iov[i].iov_len;

    size_t sent_total = 0;
    while (sent_total < total) {
        ssize_t n = sendmsg(sock, &hdr, flags);
        if (n <= 0) return n;
        sent_total += n;

        size_t rem = n;
        int idx = 0;
        while (rem > 0 && idx < hdr.msg_iovlen) {
            if (rem >= (size_t)hdr.msg_iov[idx].iov_len) {
                rem -= hdr.msg_iov[idx].iov_len;
                hdr.msg_iov[idx].iov_base = (char*)hdr.msg_iov[idx].iov_base + hdr.msg_iov[idx].iov_len;
                hdr.msg_iov[idx].iov_len = 0;
                idx++;
            } else {
                hdr.msg_iov[idx].iov_base = (char*)hdr.msg_iov[idx].iov_base + rem;
                hdr.msg_iov[idx].iov_len -= rem;
                rem = 0;
            }
        }

        int new_cnt = 0;
        for (int i = 0; i < iovcnt; i++) {
            if (hdr.msg_iov[i].iov_len > 0) {
                if (i != new_cnt) hdr.msg_iov[new_cnt] = hdr.msg_iov[i];
                new_cnt++;
            }
        }
        hdr.msg_iovlen = new_cnt;
    }

    return (ssize_t)sent_total;
}

#ifndef MSG_ZEROCOPY
#define MSG_ZEROCOPY 0x4000000
#endif

void* handle_client(void* arg) {
    ServerThreadArgs *args = (ServerThreadArgs*)arg;
    int client_socket = args->client_socket;
    int message_size = args->message_size;
    int field_size = message_size / NUM_FIELDS;
    
    // Enable zero-copy on socket
    int optval = 1;
    if (setsockopt(client_socket, SOL_SOCKET, SO_ZEROCOPY, &optval, sizeof(optval)) < 0) {
        perror("setsockopt SO_ZEROCOPY failed");
        close(client_socket);
        free(args);
        return NULL;
    }
    
    Message *msg = allocate_message(field_size);
    if (!msg) {
        fprintf(stderr, "Failed to allocate message\n");
        close(client_socket);
        free(args);
        return NULL;
    }
    
    // Setup iovec for all 8 fields
    struct iovec iov[NUM_FIELDS];
    iov[0].iov_base = msg->field1;
    iov[0].iov_len = field_size;
    iov[1].iov_base = msg->field2;
    iov[1].iov_len = field_size;
    iov[2].iov_base = msg->field3;
    iov[2].iov_len = field_size;
    iov[3].iov_base = msg->field4;
    iov[3].iov_len = field_size;
    iov[4].iov_base = msg->field5;
    iov[4].iov_len = field_size;
    iov[5].iov_base = msg->field6;
    iov[5].iov_len = field_size;
    iov[6].iov_base = msg->field7;
    iov[6].iov_len = field_size;
    iov[7].iov_base = msg->field8;
    iov[7].iov_len = field_size;
    
    struct msghdr msg_hdr;
    memset(&msg_hdr, 0, sizeof(msg_hdr));
    msg_hdr.msg_iov = iov;
    msg_hdr.msg_iovlen = NUM_FIELDS;
    
    int send_count = 0;
    int max_pending = 1000;
    
    // Send using sendmsg with MSG_ZEROCOPY flag (use helper to handle partials)
    while (1) {
        ssize_t sent = send_iov_all_local(client_socket, iov, NUM_FIELDS, MSG_ZEROCOPY);
        if (sent <= 0) break;

        send_count++;
        
        // Poll error queue to check completion notifications
        if (send_count % max_pending == 0) {
            struct msghdr err_msg;
            struct iovec err_iov;
            char cbuf[100];
            
            memset(&err_msg, 0, sizeof(err_msg));
            err_msg.msg_control = cbuf;
            err_msg.msg_controllen = sizeof(cbuf);
            err_msg.msg_iov = &err_iov;
            err_msg.msg_iovlen = 1;
            
            // Drain error queue (non-blocking)
            int flags = MSG_ERRQUEUE;
            while (recvmsg(client_socket, &err_msg, flags) >= 0) {
                struct cmsghdr *cmsg;
                for (cmsg = CMSG_FIRSTHDR(&err_msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&err_msg, cmsg)) {
                    if (cmsg->cmsg_level == SOL_IP && cmsg->cmsg_type == IP_RECVERR) {
                        struct sock_extended_err *serr = (struct sock_extended_err *)CMSG_DATA(cmsg);
                        if (serr->ee_origin == SO_EE_ORIGIN_ZEROCOPY) {
                            // Notification received - data has been transmitted
                        }
                    }
                }
            }
        }
    }
    
    free_message(msg);
    close(client_socket);
    free(args);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <message_size> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int message_size = atoi(argv[1]);
    int port = atoi(argv[2]);
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("A3 Server listening on 0.0.0.0:%d\n", port);
    
    int thread_count = 0;
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        
        ServerThreadArgs *args = (ServerThreadArgs*)malloc(sizeof(ServerThreadArgs));
        args->client_socket = client_socket;
        args->thread_id = thread_count++;
        args->message_size = message_size;
        
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, args);
        pthread_detach(thread);
    }
    
    close(server_socket);
    return 0;
}