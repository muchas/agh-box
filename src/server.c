#include "server.h"
#include "message.h"
#include "socket_utils.h"

int main(){
    struct sockaddr addr;
    socklen_t len;

    int socket = init_server_socket(9000);
    accept(socket, &addr, &len);

    printf("Success\n");
    return EXIT_SUCCESS;
}


