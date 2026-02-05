// MT25020_Part_A2_Server.c
#include "MT25020_Common.h"
#include <sys/uio.h>

// Helper to send entire iovec via sendmsg handling partial writes
static ssize_t send_iov_all(int sock, struct iovec *iov, int iovcnt, int flags) {
    struct msghdr hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.msg_iov = iov;
    hdr.msg_iovlen = iovcnt;

    // Total bytes to send
    size_t total = 0;
    for (int i = 0; i < iovcnt; i++) total += iov[i].iov_len;

    size_t sent_total = 0;
    while (sent_total < total) {
        ssize_t n = sendmsg(sock, &hdr, flags);
        if (n <= 0) return n;
        sent_total += n;

        // Advance iovecs by n bytes (handling partial sends)
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
        
        // Compact iov array
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

void* handle_client(void* arg) {
    ServerThreadArgs *args = (ServerThreadArgs*)arg;
    int client_socket = args->client_socket;
    int message_size = args->message_size;
    int field_size = message_size / NUM_FIELDS;
    
    Message *msg = allocate_message(field_size);
    if (!msg) {
        close(client_socket);
        free(args);
        return NULL;
    }
    
    struct iovec iov[NUM_FIELDS];
    
    while (1) {
        // ONE-COPY: Reset iovec pointers every loop
        iov[0].iov_base = msg->field1; iov[0].iov_len = field_size;
        iov[1].iov_base = msg->field2; iov[1].iov_len = field_size;
        iov[2].iov_base = msg->field3; iov[2].iov_len = field_size;
        iov[3].iov_base = msg->field4; iov[3].iov_len = field_size;
        iov[4].iov_base = msg->field5; iov[4].iov_len = field_size;
        iov[5].iov_base = msg->field6; iov[5].iov_len = field_size;
        iov[6].iov_base = msg->field7; iov[6].iov_len = field_size;
        iov[7].iov_base = msg->field8; iov[7].iov_len = field_size;
        
        ssize_t sent = send_iov_all(client_socket, iov, NUM_FIELDS, 0);
        if (sent <= 0) break;
    }
    
    free_message(msg);
    close(client_socket);
    free(args);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) { fprintf(stderr, "Usage: %s <size> <port>\n", argv[0]); exit(1); }
    int message_size = atoi(argv[1]);
    int port = atoi(argv[2]);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // <--- CRITICAL FIX
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed"); exit(1);
    }
    listen(server_socket, MAX_CLIENTS);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);
        if (client_socket < 0) continue;
        
        ServerThreadArgs *args = malloc(sizeof(ServerThreadArgs));
        args->client_socket = client_socket;
        args->message_size = message_size;
        pthread_t t;
        pthread_create(&t, NULL, handle_client, args);
        pthread_detach(t);
    }
    return 0;
}