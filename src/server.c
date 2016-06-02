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
        result = select(highest_fd+1, &read_set, NULL, NULL, NULL);
        if(result<0){
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        for(fd=0; fd<=highest_fd; fd++){
            if(FD_ISSET(fd, &read_set))
            {
                if(fd==socket){
                    register_client(&server);
                }
                else{

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
    client_fd = accept(socket_fd, NULL, 0);
    if(client_fd<0){
        perror("accept error");
        exit(EXIT_FAILURE);
    }
}