#ifndef message_h
#define message_h

typedef enum {
    SERVER_BOX,
    FILE_REQUEST,
    SERVER_FILE,
    CLIENT_FILE
} message_type_t;

typedef struct{
    message_type_t message_type;
    size_t size;
    char* name;
} message_info_t;
#endif
