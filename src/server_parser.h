#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

char** parse_server_ips();
void free_server_ips(char** ips, int server_amount);

#endif