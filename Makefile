CC=gcc
SRC_DIR=src
CFLAGS=-I $(SRC_DIR)

loadBalancer: $(SRC_DIR)/load_balancer.c $(SRC_DIR)/server_parser.c 
	$(CC) -o loadBalancer $(SRC_DIR)/load_balancer.c $(SRC_DIR)/server_parser.c 

clean:
	rm -rf loadBalancer