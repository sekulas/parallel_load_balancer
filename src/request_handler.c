#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <openssl/sha.h>
#include "../config/config.h"

pthread_mutex_t rr_mutex = PTHREAD_MUTEX_INITIALIZER;
int current_server = 0;

char* round_robin_algorithm(char** servers_ip) {
    pthread_mutex_lock(&rr_mutex);
    char* server_ip = servers_ip[current_server];
    current_server = (current_server + 1) % SERVER_AMOUNT;
    pthread_mutex_unlock(&rr_mutex);
    return server_ip;
}


void pass_request_to_server(int client_id, char* client_ip, char** servers_ip) {
    char* server_ip = round_robin_algorithm(servers_ip);
    printf("Handled client_id: %d, Server IP: %s\n", client_id, server_ip);
    send(client_id, "RESPONSE", 8, 0);
    close(client_id);
}



// char* ip_hash_algorithm(char* client_ip) {
//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     SHA256_CTX sha256;

//     SHA256_Init(&sha256);
//     SHA256_Update(&sha256, client_ip, strlen(client_ip));
//     SHA256_Final(hash, &sha256);
//     printf("SHA-256 hash of '%s': ", hash);

//     // int server_index = hash % SERVER_AMOUNT;
//     pthread_mutex_lock(&rr_mutex);
//     // char* server_ip = servers_ip[server_index];
//     pthread_mutex_unlock(&rr_mutex);

//     return NULL;
// }