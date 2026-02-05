// MT25020
#include "MT25020_Common.h"

void* client_thread(void* arg) {
    ClientThreadArgs *args = (ClientThreadArgs*)arg;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return NULL;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(args->port);
    inet_pton(AF_INET, args->server_ip, &server_addr.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        close(sock);
        return NULL;
    }
    
    char *buffer = (char*)malloc(args->message_size);
    if (!buffer) {
        close(sock);
        return NULL;
    }
    
    long long bytes_received = 0;
    long long start_time = get_time_in_microseconds();
    double end_time = get_time_in_seconds() + args->duration;
    
    int latency_count = 0;
    long long total_latency = 0;
    
    while (get_time_in_seconds() < end_time) {
        long long msg_start = get_time_in_microseconds();
        
        ssize_t received = 0;
        while (received < args->message_size) {
            ssize_t n = recv(sock, buffer + received, args->message_size - received, 0);
            if (n <= 0) {
                free(buffer);
                close(sock);
                return NULL;
            }
            received += n;
        }
        bytes_received += received;
        
        long long msg_end = get_time_in_microseconds();
        total_latency += (msg_end - msg_start);
        latency_count++;
    }
    
    long long end_time_us = get_time_in_microseconds();
    double elapsed = (end_time_us - start_time) / 1000000.0;
    
    args->throughput[args->thread_id] = (bytes_received * 8.0) / (elapsed * 1e9); // Gbps
    args->latency[args->thread_id] = latency_count > 0 ? (double)total_latency / latency_count : 0;
    args->bytes_sent[args->thread_id] = bytes_received;
    
    free(buffer);
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <server_ip> <port> <message_size> <num_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    int message_size = atoi(argv[3]);
    int num_threads = atoi(argv[4]);
    
    pthread_t threads[num_threads];
    ClientThreadArgs args[num_threads];
    double throughput[num_threads];
    double latency[num_threads];
    long long bytes_sent[num_threads];
    
    for (int i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        args[i].server_ip = server_ip;
        args[i].port = port;
        args[i].message_size = message_size;
        args[i].duration = DURATION_SEC;
        args[i].throughput = throughput;
        args[i].latency = latency;
        args[i].bytes_sent = bytes_sent;
        
        pthread_create(&threads[i], NULL, client_thread, &args[i]);
    }
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Calculate aggregate statistics
    double total_throughput = 0;
    double avg_latency = 0;
    long long total_bytes = 0;
    
    for (int i = 0; i < num_threads; i++) {
        total_throughput += throughput[i];
        avg_latency += latency[i];
        total_bytes += bytes_sent[i];
    }
    avg_latency /= num_threads;
    
    printf("Throughput: %.6f Gbps\n", total_throughput);
    printf("Latency: %.6f us\n", avg_latency);
    printf("Total bytes: %lld\n", total_bytes);
    
    return 0;
}