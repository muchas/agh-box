#ifndef socket_utils_h
#define socket_utils_h

#include <stdio.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct sockaddr_in get_sockaddr(int port, uint32_t s_addr);
int get_socket_fd();
int init_server_socket(int port);
int get_client_socket(const char* ip, int port);

#endif
