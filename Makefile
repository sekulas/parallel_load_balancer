CC=gcc
SRC_DIR=src
CFLAGS=-I $(SRC_DIR)

load_balancer: $(SRC_DIR)/load_balancer.c $(SRC_DIR)/server_parser.c $(SRC_DIR)/request_handler.c $(SRC_DIR)/argument_handler.c
	$(CC) -o load_balancer $(SRC_DIR)/load_balancer.c $(SRC_DIR)/server_parser.c $(SRC_DIR)/request_handler.c $(SRC_DIR)/argument_handler.c -lssl -lcrypto

clean:
	rm -rf load_balancer
