#include <stdio.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "socket_utils.h"

struct sockaddr_in get_sockaddr(int port, uint32_t s_addr){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = s_addr;

    return addr;
}

int get_socket_fd(){
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd<0){
        perror("Socket init failed");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}

int init_server_socket(int port){
    int socket_fd, result;
    struct sockaddr_in address;

    address = get_sockaddr(port, htonl(INADDR_ANY));
    socket_fd = get_socket_fd();

    result = bind(socket_fd, (struct sockaddr*) &address, sizeof(address));
    if(result != 0){
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    result = listen(socket_fd, SOMAXCONN);
    if(result != 0){
        perror("Socket listening start failed");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}

int get_client_socket(const char* ip, int port){
    int socket_fd;
    struct in_addr iaddr;
    struct sockaddr_in address;

    socket_fd = get_socket_fd();

    if(inet_aton(ip, &iaddr)==0){
        perror("inet aton failed");
        exit(EXIT_FAILURE);
    }
    address = get_sockaddr(port, iaddr.s_addr);
    if(connect(socket_fd, (struct sockaddr*) &address, sizeof(address))<0){
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}
