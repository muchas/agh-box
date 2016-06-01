#include <pthread.h>
#include <unistd.h>
#include "message.h"
#include "socket_utils.h"
#define SYNC_TIME 10
#define SERVER_BOX_FILENAME ".server_box"
#define LOCAL_BOX_FILENAME ".box"



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


void receive_server_box(int socket_fd)
{

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
