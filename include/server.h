#ifndef server_h
#define server_h

#define MAX_CLIENT_NUM 64

typedef struct {
    int highest_fd;
    fd_set server_fd_set;
    int socket;
    int client_array[MAX_CLIENT_NUM];
    int client_num;
} server_t;

#endif
