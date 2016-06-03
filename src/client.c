#include <pthread.h>
#include <unistd.h>
#include <communication_utils.h>
#include "socket_utils.h"
#include "communication_utils.h"
#define SERVER_BOX_FILENAME ".server_box"
#define LOCAL_BOX_FILENAME ".box"
#define SYNC_TIME 10



int file_exists(char* filename)
{
    return access(filename, F_OK);
}


void* pull_changes(void *parameters)
{
    while(1) {

    }

    return NULL;
}


void* track_directory(void *parameters)
{
    while(1) {

        sleep(SYNC_TIME);
    }


    return NULL;
}


box_entry_t* receive_server_box(int socket_fd)
{
    box_entry_t *box_entries;
    message_info_t message_info;
    FILE* file;

    message_info = receive_message_info(socket_fd);

    if(!message_info.message_type == MESSAGE_BOX) {
        printf("Message box expected\n");
        exit(EXIT_FAILURE);
    }

    receive_file(socket_fd, SERVER_BOX_FILENAME, message_info.size);

    box_entries = malloc(message_info.size);

    file = fopen(SERVER_BOX_FILENAME, "r");
    fread()

    fclose(file);

    return box_entries;

}


void initialize_local_box()
{
    if(file_exists(LOCAL_BOX_FILENAME)) return;
}



int main(int argc, char *argv[])
{
    pthread_t tracker_id, listener_id;
    int socket_fd;

    socket_fd = get_client_socket(argv[1], atoi(argv[2]));

    receive_server_box(socket_fd);

    initialize_local_box();


    if(pthread_create(&tracker_id, NULL, &track_directory, NULL) < 0) {
        perror("pthread_create failed");
        return EXIT_FAILURE;
    }

    if(pthread_create(&listener_id, NULL, &pull_changes, NULL) < 0) {
        perror("pthread_create failed");
        return EXIT_FAILURE;
    }

    if(pthread_join(tracker_id, NULL) < 0) { perror("pthread_join failed"); return EXIT_FAILURE; }
    if(pthread_join(listener_id, NULL) < 0) { perror("pthread_join failed"); return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}
