#ifndef communication_utils_h
#define communication_utils_h

#define MAX_PATHLEN 128
#define BUFSIZE 128

#include <stdlib.h>

typedef enum {
    SERVER_BOX,
    FILE_REQUEST,
    SERVER_FILE,
    CLIENT_FILE
} message_type_t;

typedef struct {
    message_type_t message_type;
    size_t size;
    char name[MAX_PATHLEN];
} message_info_t;

void send_file(int socket, const char* path, message_type_t type);
#endif
