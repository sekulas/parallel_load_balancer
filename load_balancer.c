#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <pthread.h>

#define PORT 8676

//mock server
void* handle_tcp_connection(void *arg) {
    int client_id = *((int*)arg);
    send(client_id, "RESPONSE", 8, 0);
    return NULL;
}

int main(int argc, char **argv) {

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

        //new thread creation and handle tcp connection
        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, handle_tcp_connection, (void*) client_id) < 0) {
            fprintf(stderr, "Thread creation failed!");
            continue;
        }
        pthread_detach(thread_id);

    }

}
