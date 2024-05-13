#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <pthread.h>

#define THREAD_NUMBER 2
#define MAX_TASKS 100
#define PORT 8681


typedef struct Task {
    int client_id;
    void (*fun)(int);
} Task;

typedef struct Task_Queue {
    Task queue[MAX_TASKS];
    int task_count;
} Task_Queue;

pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;

//mock server
void handle_tcp_connection(int client_id) {
    //lb algorithm
    printf("client_id: %d\n", client_id);
    send(client_id, "RESPONSE", 8, 0);
    close(client_id);
}


void *handle_thread_pool(void *args) {
    Task_Queue *task_queue = (Task_Queue*) args;
    while(1) {
        pthread_mutex_lock(&mutex);
        Task task;
        if(task_queue->task_count != 0) {
            task = task_queue->queue[--task_queue->task_count];
            printf("Thread: client_id: %d\n", task.client_id);

        }
        pthread_mutex_unlock(&mutex);
        if(task.fun != NULL) {
            // printf("Jestem w task.fun!\n");
            task.fun(task.client_id);
        }
    }
    return NULL;
}

void intialize_thread_pool(Task_Queue *task_queue) {
    pthread_t thread_id[THREAD_NUMBER];
    for(int i=0; i<THREAD_NUMBER; i++) {
        if(pthread_create(&thread_id[i], NULL, handle_thread_pool, (void *)task_queue) < 0) {
            fprintf(stderr, "Thread creation failed!");
            continue;
        }
    }
}

void submit_task(Task_Queue *task_queue, Task t) {
    pthread_mutex_lock(&mutex);
    task_queue->queue[task_queue->task_count++] = t;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char **argv) {

    Task_Queue *task_queue;
    task_queue = malloc(sizeof(Task_Queue));
    task_queue->task_count = 0;

    intialize_thread_pool(task_queue);

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

    if(listen(server_fd, 10) < 0) { //max 10 connections
        fprintf(stderr, "Bind failed!");  
        exit(EXIT_FAILURE);
    }

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_id = malloc(sizeof(int));

        if((*client_id = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            fprintf(stderr, "Accept failed!");
            continue;
        }
        printf("Client_id: %d\n", *client_id);

        Task task;
        task.client_id = *client_id;
        task.fun = handle_tcp_connection;
        submit_task(task_queue, task);

        close(client_id);
        free(client_id);
    }
    // close(server_fd);
    free(task_queue);
}
