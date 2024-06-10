#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include "server_parser.h"
#include "request_handler.h"
#include "argument_handler.h"
#include "../config/config.h"

#define MAX_ALGORITHM_NAME_SIZE 100

typedef struct Task {
    int client_id;
    char* client_ip;
    void (*fun)(int, char*, char**, char*);
    char* load_balancing_alghorithm;
} Task;

typedef struct Task_Queue {
    Task queue[MAX_TASKS];
    int task_count;
    char** servers_ip;
} Task_Queue;

pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;

void *handle_request(void *args) {
    Task_Queue *task_queue = (Task_Queue*) args;
    Task task;
    while(1) {
        pthread_mutex_lock(&mutex);
        while (task_queue->task_count == 0) {
            pthread_cond_wait(&task_cond, &mutex);
        }
        task = task_queue->queue[--task_queue->task_count];
        pthread_mutex_unlock(&mutex);
        //printf("Thread: client_id: %d\n", task.client_id);
        if(task.fun != NULL) {
            task.fun(task.client_id, task.client_ip, task_queue->servers_ip, task.load_balancing_alghorithm);
        }
    }
    return NULL;
}

void intialize_thread_pool(Task_Queue *task_queue, int thread_number) {
    pthread_t thread_id[thread_number];
    for(int i=0; i<thread_number; i++) {
        if(pthread_create(&thread_id[i], NULL, handle_request, (void *)task_queue) < 0) {
            fprintf(stderr, "Thread creation failed!");
            continue;
        }
    }
}

void submit_task(Task_Queue *task_queue, Task t) {
    pthread_mutex_lock(&mutex);
    task_queue->queue[task_queue->task_count++] = t;
    pthread_cond_signal(&task_cond);
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char **argv) {
    int thread_number;
    int is_sync_load_balancer = 0;  //synchronous mode(1) trigger only main thread
    char* load_balancing_algorithm = malloc(sizeof(char) * MAX_ALGORITHM_NAME_SIZE); 

    parse_arguments(argc, argv, &thread_number, load_balancing_algorithm, &is_sync_load_balancer);  

    if(is_sync_load_balancer == 0) {
        printf("Number of threads: %d\n", thread_number);
    }

    printf("Load balancing algorithm: %s\n", load_balancing_algorithm);

    char client_ip[INET_ADDRSTRLEN];
    char** servers_ip = parse_server_ips();
    if (servers_ip == NULL) {
        fprintf(stderr, "Failed to parse server IPs\n");
        return EXIT_FAILURE;
    }

    Task_Queue *task_queue;

    if(is_sync_load_balancer == 0) {
        task_queue = malloc(sizeof(Task_Queue));
        task_queue->task_count = 0;
        task_queue->servers_ip = servers_ip;
        intialize_thread_pool(task_queue, thread_number);
    }

    int server_fd;
    struct sockaddr_in server_addr;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Socket failed!");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET; //IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //accepts connections from any network interface
    server_addr.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {  //bind socket to port
        fprintf(stderr, "Bind failed!");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, MAX_CONNECTIONS) < 0) {
        fprintf(stderr, "Bind failed!");  
        exit(EXIT_FAILURE);
    }

    int client_id;

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        if((client_id = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            fprintf(stderr, "Accept failed!");
            continue;
        }

        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        //printf("Handled client with ip address: %s\n", client_ip);


        if(is_sync_load_balancer == 0) {
            Task task = {client_id, client_ip, pass_request_to_server, load_balancing_algorithm};
            submit_task(task_queue, task);
        } else {
            pass_request_to_server(client_id, client_ip, servers_ip, load_balancing_algorithm);
        }
    }

    close(server_fd);
    if(is_sync_load_balancer == 0) {
        free(task_queue);
    }
    free(load_balancing_algorithm);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&task_cond);
}