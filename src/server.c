#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <server.h>
#include <communication_utils.h>
#include "server.h"
#include "communication_utils.h"
#include "box_utils.h"
#include "socket_utils.h"
#define SERVER_BOX_FILENAME ".origin_server_box"


int max(int a, int b) {
    return a > b ? a : b;
}


int file_exists(char* filename)
{
    return access(filename, F_OK);
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

    printf("Registering new client, fd: %d \n", client_fd);

    if(client_fd<0){
        perror("accept error");
        exit(EXIT_FAILURE);
    }

    server->highest_fd = max(server->highest_fd, client_fd);
    FD_SET(client_fd, &server->server_fd_set);

    add_client(client_fd, server->client_array, &server->client_num);

    printf("Sending server box to client\n");

    send_file(client_fd, SERVER_BOX_FILENAME, SERVER_BOX);
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

void handle_client_file(int socket, message_info_t *info){
    box_entry_t* head;

    receive_file(socket, info->name, info->size);

    head = read_box(SERVER_BOX_FILENAME);
    create_or_update(head, info->name, info->size, 0, info->modification_time);


    // broadcast
}

void handle_client_message(int socket){
    ssize_t read_bytes;
    message_info_t info;

    read_bytes = read(socket, &info, sizeof(info));

    printf("Handle client message, size: %d\n", read_bytes);

    switch(info.message_type){
        case FILE_REQUEST:
            handle_file_request(socket, info);
        case CLIENT_FILE:
            printf("Received CLIENT_FILE %s\n", info.name);
            handle_client_file(socket, &info);
        default:
            break;
    }
}


void init()
{
    int fd;

    if(file_exists(SERVER_BOX_FILENAME) != 0)  {
        printf("Creating server box\n");
        fd = creat(SERVER_BOX_FILENAME, 0666);
        close(fd);
    }
}


int main(int argc, char *argv[]){
    struct sockaddr addr;
    server_t server;
    socklen_t len;

    chdir("./server_");

    init();

    int socket = init_server_socket(atoi(argv[1]));
    server=get_server(socket);
    run(server);

    return EXIT_SUCCESS;
}