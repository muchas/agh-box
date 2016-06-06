#ifndef file_utils_h
#define file_utils_h


typedef struct File {
    time_t modification_time;
    int is_directory;
    int size;
    char *path;

    struct File *next;
} file_t;


#endif