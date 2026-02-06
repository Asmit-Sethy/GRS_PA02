#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#define MAX_CLIENTS 100
#define DEFAULT_PORT 8080
#define DURATION_SEC 10
#define NUM_FIELDS 8

typedef struct {
    char *field1;
    char *field2;
    char *field3;
    char *field4;
    char *field5;
    char *field6;
    char *field7;
    char *field8;
} Message;

typedef struct {
    int thread_id;
    char *server_ip;
    int port;
    int message_size;
    int duration;
    double *throughput;
    double *latency;
    long long *bytes_sent;
} ClientThreadArgs;

typedef struct {
    int client_socket;
    int thread_id;
    int message_size;
} ServerThreadArgs;

static inline double get_time_in_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static inline long long get_time_in_microseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

static inline Message* allocate_message(int field_size) {
    Message *msg = (Message*)malloc(sizeof(Message));
    if (!msg) return NULL;
    
    msg->field1 = (char*)malloc(field_size);
    msg->field2 = (char*)malloc(field_size);
    msg->field3 = (char*)malloc(field_size);
    msg->field4 = (char*)malloc(field_size);
    msg->field5 = (char*)malloc(field_size);
    msg->field6 = (char*)malloc(field_size);
    msg->field7 = (char*)malloc(field_size);
    msg->field8 = (char*)malloc(field_size);
    
    if (!msg->field1 || !msg->field2 || !msg->field3 || !msg->field4 ||
        !msg->field5 || !msg->field6 || !msg->field7 || !msg->field8) {
        free(msg->field1); free(msg->field2); free(msg->field3); free(msg->field4);
        free(msg->field5); free(msg->field6); free(msg->field7); free(msg->field8);
        free(msg);
        return NULL;
    }
    
    memset(msg->field1, 'A', field_size);
    memset(msg->field2, 'B', field_size);
    memset(msg->field3, 'C', field_size);
    memset(msg->field4, 'D', field_size);
    memset(msg->field5, 'E', field_size);
    memset(msg->field6, 'F', field_size);
    memset(msg->field7, 'G', field_size);
    memset(msg->field8, 'H', field_size);
    
    return msg;
}

static inline void free_message(Message *msg) {
    if (msg) {
        free(msg->field1); free(msg->field2); free(msg->field3); free(msg->field4);
        free(msg->field5); free(msg->field6); free(msg->field7); free(msg->field8);
        free(msg);
    }
}

#endif
