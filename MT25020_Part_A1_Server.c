// MT25020
#include "MT25020_Common.h"

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
    
    while (1) {
        ssize_t sent;
        sent = send(client_socket, msg->field1, field_size, 0);
        if (sent <= 0) break;
        sent = send(client_socket, msg->field2, field_size, 0);
        if (sent <= 0) break;
        sent = send(client_socket, msg->field3, field_size, 0);
        if (sent <= 0) break;
        sent = send(client_socket, msg->field4, field_size, 0);
        if (sent <= 0) break;
        sent = send(client_socket, msg->field5, field_size, 0);
        if (sent <= 0) break;
        sent = send(client_socket, msg->field6, field_size, 0);
        if (sent <= 0) break;
        sent = send(client_socket, msg->field7, field_size, 0);
        if (sent <= 0) break;
        sent = send(client_socket, msg->field8, field_size, 0);
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
    
    printf("A1 Server listening on 10.0.0.1:%d\n", port);
    
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
