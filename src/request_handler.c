#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <openssl/sha.h>
#include <openssl/evp.h> 
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

int hash_to_number(char* hash) {
    int sum = 0;
    for(int i=0; i<strlen(hash); i++) {
        sum += hash[i];
    }
    return sum;
}

char* ip_hash_algorithm(char* client_ip, char** servers_ip) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;

    OpenSSL_add_all_digests(); // Initialize OpenSSL digest algorithms
    md = EVP_get_digestbyname("sha256");
    if(md == NULL) {
        printf("Unknown message digest\n");
        return NULL;
    }

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, client_ip, strlen(client_ip));
    EVP_DigestFinal_ex(mdctx, hash, NULL);
    EVP_MD_CTX_free(mdctx);

    int hash_number = hash_to_number(hash);
    int server_index = hash_number % SERVER_AMOUNT;
    pthread_mutex_lock(&rr_mutex);
    char* server_ip = servers_ip[server_index];
    pthread_mutex_unlock(&rr_mutex);

    return server_ip;
}

void pass_request_to_server(int client_id, char* client_ip, char** servers_ip, char* load_balancing_algorithm) {
    char* server_ip;
    if(strcmp(load_balancing_algorithm, "ROUND_ROBIN") == 0) {
        server_ip = round_robin_algorithm(servers_ip);
    } else {
        server_ip = ip_hash_algorithm(client_ip, servers_ip);
    }

    printf("Handled client_id: %d, Server IP: %s\n", client_id, server_ip);
    send(client_id, "RESPONSE", 8, 0);
    close(client_id);
}
