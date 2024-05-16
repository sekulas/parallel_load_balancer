#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

void free_server_ips(char** ips, int server_amount) {
    for (int i = 0; i < server_amount; i++) {
        free(ips[i]);
    }
    free(ips);
}

char* remove_newline_from_ip(char* line) {
    for(int i = MAX_IP_LEN - 1; i >= 0; i--) {
        if(line[i]=='\n') {
            line[i] = '\0';
            break;
        }
    }
    return line;
}

char** parse_server_ips() {
    FILE *file = fopen(FILE_WITH_SERVERS, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
    
    char** ips = malloc(SERVER_AMOUNT * sizeof(char*));
    if (!ips) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    char* line = malloc(MAX_IP_LEN);

    for(int i = 0; i < SERVER_AMOUNT; i++) {
        fgets(line, MAX_IP_LEN, file);
        line = remove_newline_from_ip(line);
        ips[i] = strdup(line);
        if (!ips[i]) {
            perror("Error duplicating string");
            free_server_ips(ips, i);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return ips;
}

