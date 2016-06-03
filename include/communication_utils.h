#ifndef communication_utils_h
#define communication_utils_h

#define MAX_PATHLEN 128
#define BUFSIZE 128

#include <stdlib.h>
#include <time.h>

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


typedef struct {
    time_t global_timestamp;
    time_t local_timestamp;
    int is_directory;
    char path[MAX_PATHLEN];
} box_entry_t;

void send_file(int socket, const char* path, message_type_t type);
message_info_t receive_message_info(int socket);
void receive_file(int socket, const char* new_path, size_t size);

#endif
