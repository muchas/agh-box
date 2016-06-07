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
    CLIENT_FILE,
    FILE_REMOVAL
} message_type_t;

typedef struct {
    message_type_t message_type;
    size_t size;
    time_t modification_time;
    char name[MAX_PATHLEN];
} message_info_t;

typedef struct BoxEntry {
    time_t global_timestamp;
    time_t local_timestamp;
    int is_directory;
    char path[MAX_PATHLEN];

    struct BoxEntry* next;
} box_entry_t;


void send_file(int socket, const char* path, message_type_t type, time_t mod_time);
message_info_t receive_message_info(int socket);
void receive_file(int socket, const char* new_path, size_t size);
void send_message_info(int socket, message_type_t type, const char* path, size_t size, time_t mod_time);
message_info_t receive_message_info(int socket);

#endif
