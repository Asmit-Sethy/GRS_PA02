// MT25020
#include "MT25020_Common.h"
#include <sys/uio.h>

void* handle_client(void* arg) {
    ServerThreadArgs *args = (ServerThreadArgs*)arg;
    int client_socket = args->client_socket;
    int message_size = args->message_size;
    int field_size = message_size / NUM_FIELDS;
    
    Message *msg = allocate_message(field_size);
    if (!msg) {
        fprintf(stderr, "Failed to allocate message\n");
        close(client_socket);
        free(args);
        return NULL;
    }
    
    // Setup iovec for all 8 fields - this allows one-copy by pre-registering buffers
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
    
    // Send using sendmsg with pre-registered buffer (iovec)
    while (1) {
        ssize_t sent = sendmsg(client_socket, &msg_hdr, 0);
        if (sent <= 0) break;
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
    server_addr.sin_addr.s_addr = inet_addr("10.0.0.1");
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
    
    printf("A2 Server listening on 10.0.0.1:%d\n", port);
    
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