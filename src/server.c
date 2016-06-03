#include <sys/select.h>
#include "server.h"
#include "communication_utils.h"
#include "socket_utils.h"

int main(){
    struct sockaddr addr;
    socklen_t len;

    int socket = init_server_socket(9000);
    accept(socket, &addr, &len);

    printf("Success\n");
    return EXIT_SUCCESS;
}


void run(server_t server){
    int result, fd;
    fd_set read_set;

    while(1){
        read_set = server.server_fd_set;
        result = select(server.highest_fd+1, &read_set, NULL, NULL, NULL);
        if(result<0){
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        for(fd=0; fd<=server.highest_fd; fd++){
            if(FD_ISSET(fd, &read_set))
            {
                if(fd==server.socket){
                    register_client(&server);
                }
                else{
                    handle_client_message(fd);
                }
            }
        }
    }

}

server_t get_server(int socket){
    server_t server;
    server.socket = socket;
    FD_ZERO(&server.server_fd_set);
    FD_SET(socket, &server.server_fd_set);
    server.highest_fd = socket;
    server.client_num=0;

    return server;
}

void register_client(server_t* server){
    int client_fd;
    client_fd = accept(server->socket, NULL, 0);
    if(client_fd<0){
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    add_client(client_fd, server->client_array, &server->client_num);
}

void add_client(int socket, int* client_array, int* client_num){
    if (*client_num >= MAX_CLIENT_NUM){
        printf("Client add error");
        exit(0);
    }
    client_array[*client_num]=socket;
    *client_num += 1;
}

void handle_file_request(int socket, message_info_t info){
    send_file(socket, info.name, SERVER_FILE);
}

void handle_client_message(int socket){
    message_info_t info;
    read(socket, &info, sizeof(info));

    switch(info.message_type){
        case FILE_REQUEST:
            handle_file_request(socket, info);
        default:
            break;
    }
}