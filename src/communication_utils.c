#include <stdio.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "communication_utils.h"


void send_message_info(int socket, message_type_t type, const char* path, size_t size)
{
    message_info_t info;
    info.message_type = type;
    strncpy(info.name, path, MAX_PATHLEN);
    info.size = size;

    if(write(socket, &info, sizeof(message_info_t)) < sizeof(info))
    {
        perror("Error send file info");
        exit(EXIT_FAILURE);
    }

}

void send_file_data(int socket, int fd, size_t size)
{
    off_t offset = 0;
    int remain_data = size;
    int sent_bytes;

    while (((sent_bytes = sendfile(socket, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0))
    {
        remain_data -= sent_bytes;
    }
}

void send_file(int socket, const char* path, message_type_t type){
    int fd;
    struct stat file_stat;

    if((fd = open(path, O_RDONLY))<0)
    {
        perror("file open error");
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &file_stat) < 0)
    {
        perror("Error fstat");
        exit(EXIT_FAILURE);
    }

    send_message_info(socket, type, path, file_stat.st_size);
    send_file_data(socket, fd, file_stat.st_size);
}

message_info_t receive_message_info(int socket){
    message_info_t info;
    if(read(socket, &info, sizeof(info))<sizeof(info))
    {
        perror("Error read file info");
        exit(EXIT_FAILURE);
    }
    return info;
}


void receive_file(int socket, const char* new_path, size_t size){
    int len;
    FILE* file;
    char buffer[BUFSIZ];
    int remain_data = size;

    file = fopen(new_path, "w");
    while (((len = read(socket, buffer, BUFSIZ)) > 0) && (remain_data > 0))
    {
        fwrite(buffer, sizeof(char), len, file);
        remain_data -= len;
    }
    fclose(file);
}
