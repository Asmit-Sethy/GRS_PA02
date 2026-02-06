#include "MT25020_Common.h"
#include <sys/uio.h>
#include <linux/errqueue.h>

#ifndef SO_ZEROCOPY
#define SO_ZEROCOPY 60
#endif
#ifndef MSG_ZEROCOPY
#define MSG_ZEROCOPY 0x4000000
#endif

void* handle_client(void* arg) {
    ServerThreadArgs *args = (ServerThreadArgs*)arg;
    int client_socket = args->client_socket;
    int message_size = args->message_size;
    int field_size = message_size / NUM_FIELDS;
    
    // Try enabling Zero-Copy, but don't crash if it fails
    int optval = 1;
    if (setsockopt(client_socket, SOL_SOCKET, SO_ZEROCOPY, &optval, sizeof(optval)) < 0) {
        // Just continue, it will fall back to normal copy
    }
    
    Message *msg = allocate_message(field_size);
    struct iovec iov[NUM_FIELDS];
    
    while (1) {
        // Reset iov
        iov[0].iov_base = msg->field1; iov[0].iov_len = field_size;
        iov[1].iov_base = msg->field2; iov[1].iov_len = field_size;
        iov[2].iov_base = msg->field3; iov[2].iov_len = field_size;
        iov[3].iov_base = msg->field4; iov[3].iov_len = field_size;
        iov[4].iov_base = msg->field5; iov[4].iov_len = field_size;
        iov[5].iov_base = msg->field6; iov[5].iov_len = field_size;
        iov[6].iov_base = msg->field7; iov[6].iov_len = field_size;
        iov[7].iov_base = msg->field8; iov[7].iov_len = field_size;
        
        struct msghdr mh = {0};
        mh.msg_iov = iov;
        mh.msg_iovlen = NUM_FIELDS;
        
        // Send with MSG_ZEROCOPY
        if (sendmsg(client_socket, &mh, MSG_ZEROCOPY) <= 0) break;
        
        // Essential: Clean the error queue to prevent memory leaks in kernel
        char buf[1024];
        struct msghdr err_msg = {0};
        struct iovec err_iov = { .iov_base = buf, .iov_len = sizeof(buf) };
        char control[1024];
        
        err_msg.msg_iov = &err_iov;
        err_msg.msg_iovlen = 1;
        err_msg.msg_control = control;
        err_msg.msg_controllen = sizeof(control);
        
        // Non-blocking check for completion notifications
        recvmsg(client_socket, &err_msg, MSG_ERRQUEUE | MSG_DONTWAIT);
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