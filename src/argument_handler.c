#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config/config.h"


void parse_arguments(int argc, char **argv, int* thread_number, char* load_balancing_alghorithm, int* is_sync_load_balancer) {
    *thread_number = DEFAULT_NUMBER_OF_THREADS;
    strcpy(load_balancing_alghorithm, DEFAULT_LOAD_BALANCING_ALGORITHM);
    
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-n") == 0) {
            if(i + 1 < argc) {
                int thread_number_from_args = atoi(argv[i+1]);
                if(thread_number_from_args > MAX_NUMBER_OF_THREADS || thread_number_from_args < 1) {
                    fprintf(stderr, "Max number of threads is %d\n! Default number of threads(%d) is taken!", MAX_NUMBER_OF_THREADS, DEFAULT_NUMBER_OF_THREADS);
                } else {
                    *thread_number = thread_number_from_args;
                }
            } else {
                fprintf(stderr, "-n flag requires numeric value! Default number of threads(%d) is taken!", DEFAULT_NUMBER_OF_THREADS);
            }
        }

        if(strcmp(argv[i], "-alg") == 0) {
            if(i + 1 < argc) {
                char* algorithm_name = argv[i+1];
                if(strcmp(algorithm_name, "ROUND_ROBIN") == 0) {
                    strcpy(load_balancing_alghorithm, algorithm_name);
                } else if(strcmp(algorithm_name, "IP_HASH") == 0) {
                    strcpy(load_balancing_alghorithm, algorithm_name);
                } else {
                    fprintf(stderr, "-alg flag requires ROUND_ROBIN or IP_HASH algorithm! Default load balancing algorithm is taken: %s\n", DEFAULT_LOAD_BALANCING_ALGORITHM);
                }

            } else {
                fprintf(stderr, "-alg flag requires value! Default load balacing algorithm is taken: %s\n", DEFAULT_LOAD_BALANCING_ALGORITHM);
            }
        }

        if(strcmp(argv[i], "-sync") == 0) {
            printf("Thread numbers are ignored, load balancer is into synchronous mode!\n");
            *is_sync_load_balancer = 1;
        }
    }
    

}