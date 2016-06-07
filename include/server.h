#ifndef server_h
#define server_h
#include "communication_utils.h"

#define MAX_CLIENT_NUM 64

typedef struct {
    int highest_fd;
    fd_set server_fd_set;
    int socket;
    int client_array[MAX_CLIENT_NUM];
    int client_num;
} server_t;

void run(server_t server);
server_t get_server(int socket);
void register_client(server_t* server);
void add_client(int socket, int* client_array, int* client_num);
void handle_file_request(int socket, message_info_t info);
void handle_client_message(int socket, server_t* server);

#endif
