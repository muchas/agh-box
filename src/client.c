#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <file_utils.h>
#include <communication_utils.h>
#include "socket_utils.h"
#include "communication_utils.h"
#include "stack.h"
#include "file_utils.h"
#include "box_utils.h"
#define SERVER_BOX_FILENAME ".server_box"
#define LOCAL_BOX_FILENAME ".box"
#define SYNC_TIME 1
typedef struct dirent dirent_t;
typedef struct stat  stat_t;


static int socket_fd, server_entries_count, local_entries_count;
static box_entry_t *server_box, *local_box;


message_info_t* create_info_message(message_type_t type, char *name, time_t modification_time, size_t size)
{
    message_info_t *message;

    message = malloc(sizeof(message_info_t));
    message->message_type = type;
    strcpy(message->name, name);
    message->modification_time = modification_time;
    message->size = size;

    return message;
}


int file_exists(char* filename)
{
    return access(filename, F_OK);
}



void print_files_list(file_t *head)
{
    while(head->next != NULL) {
        printf("%s\n", head->path);
        head = head->next;
    }
}


file_t* get_local_files_list(char* name) // returns: number of entries, -1 on error
{
    DIR *dir;
    dirent_t *ent;
    file_t *file_it, *local_files;
    char *entry_path;
    stat_t entry_stat;

    dir = opendir(name);

    if(dir == NULL) {
        printf("Invalid directory name\n");
        return NULL;
    }

    local_files = malloc(sizeof(file_t));
    file_it = local_files;

    while(ent = readdir(dir)) {
        if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;

        entry_path = malloc(strlen(name) + strlen(ent->d_name) + 2);
        sprintf(entry_path, "%s/%s", name, ent->d_name);
        stat(entry_path, &entry_stat);

        if(S_ISREG(entry_stat.st_mode) && ent->d_name[0] != '.') {
            file_it->size = (int) entry_stat.st_size;
            file_it->path = strdup(entry_path);
            file_it->is_directory = -1;
            file_it->modification_time = entry_stat.st_mtime;

            file_it->next = malloc(sizeof(file_t));
            file_it = file_it->next;
        }

//        if(S_ISDIR(entry_stat.st_mode)) {
//            li(entry_path, mode_pattern);
//        }

        free(entry_path);
    }

    file_it->next = NULL;

    closedir(dir);

    return local_files;
}


stack_t* detect_local_changes(file_t *local_files, box_entry_t* box_entries)
{
    stack_t *stack;
    file_t *files_it;
    box_entry_t *box_it;
    message_info_t *message;
    int is_found;

    stack = init_stack();

    files_it = local_files;



    while(files_it->next != NULL) {
        box_it = box_entries;
        is_found = -1;

        while(box_it->next != NULL) {
            if(strcmp(files_it->path, box_it->path) == 0) {
                is_found = 1;

                if(files_it->modification_time > box_it->local_timestamp) {
                    message = create_info_message(CLIENT_FILE, files_it->path, files_it->modification_time, (size_t)files_it->size);
                    push(stack, message);
                    break;
                }
            }
            box_it = box_it->next;
        }

        if(is_found < 0) {
            message = create_info_message(CLIENT_FILE, files_it->path, files_it->modification_time, (size_t)files_it->size);
            push(stack, message);
        }

        files_it = files_it->next;
    }

    box_it = box_entries;

    while(box_it->next != NULL) {
        files_it = local_files;
        is_found = -1;

        while(files_it->next != NULL) {
            if(strcmp(files_it->path, box_it->path) == 0) {
                is_found = 1;
                break;
            }

            files_it = files_it->next;
        }

        if(is_found < 0) {
            printf("Box entry is not found in the files\n");
            message = create_info_message(FILE_REMOVAL, box_it->path, time(NULL), 0);
            push(stack, message);
        }
        box_it = box_it->next;
    }


    return stack;
}


int push_local_changes(stack_t* changes)
{
    message_info_t* message;

    while((message = (message_info_t *) pop(changes)) != NULL) {
        switch(message->message_type) {
            case CLIENT_FILE:
                printf("CLIENT_FILE, sending file to server, filename: %s\n", message->name);
                send_file(socket_fd, message->name, message->message_type, message->modification_time);
                create_or_update(local_box, message->name, message->size, message->modification_time, message->modification_time);
                break;
            case FILE_REMOVAL:
                //printf("FILE REMOVAL; LET'S PRETEND IT'S REMOVED. TODO :-)\n");
                break;
            default:
                //printf("I have no idea what is happening he;)\n");
                break;
        }
    }


    return 0;
}


stack_t *detect_server_changes(box_entry_t* local_box_entries, box_entry_t* server_box_entries)
{
    stack_t *stack;
    box_entry_t *local_it, *server_it;
    message_info_t *message;

    stack = init_stack();

    server_it = server_box_entries;

    while(server_it->next != NULL) {
        local_it = find_in_box(local_box_entries, server_it->path);
        if(local_it == NULL || server_it->global_timestamp > local_it->global_timestamp) {
            message = create_info_message(FILE_REQUEST, server_it->path, server_it->global_timestamp, 0);
            push(stack, message);
        }
        server_it = server_it->next;
    }

    return stack;
}


int apply_server_changes(stack_t *changes)
{
    message_info_t *message, received_message;
    struct stat st;

    while((message = (message_info_t *) pop(changes)) != NULL) {
        switch(message->message_type) {
            case FILE_REQUEST:
                printf("FILE REQUEST, requesting file: %s\n", message->name);
                send_message_info(socket_fd, FILE_REQUEST, message->name, 0, 0);
                received_message = receive_message_info(socket_fd);

                if(received_message.message_type == SERVER_FILE) {
                    receive_file(socket_fd, received_message.name, received_message.size);
                    stat(received_message.name, &st);
                    create_or_update(local_box, received_message.name, received_message.size, st.st_mtime, received_message.modification_time);
                    printf("Updated box\n");
                } else if(received_message.message_type == SERVER_BOX) {
                    printf("Hey server, you fucked up\n");
                }
                break;
            default:
                printf("I have no idea what is happening here - \n");
                break;
        }
    }


    return 0;
}



box_entry_t* receive_server_box(int socket_fd)
{
    message_info_t message_info;

    message_info = receive_message_info(socket_fd);

    if(!message_info.message_type == SERVER_BOX) {
        printf("Message box expected\n");
        exit(EXIT_FAILURE);
    }

    receive_file(socket_fd, SERVER_BOX_FILENAME, message_info.size);

    return read_box(SERVER_BOX_FILENAME);
}

void* pull_changes(void *parameters)
{
    stack_t *changes;

    int i;
    i = 0;
    while(1) {
        server_box = receive_server_box(socket_fd);
        changes = detect_server_changes(local_box, server_box);
        apply_server_changes(changes);
        write_box(LOCAL_BOX_FILENAME, local_box);


    }

    return NULL;
}


void* track_directory(void *parameters)
{
    stack_t *changes;
    file_t *local_files;

    int i;
    i = 0;
    while(1) {
        // acquire lock]
        local_files = get_local_files_list(".");
        changes = detect_local_changes(local_files, local_box);

        push_local_changes(changes);

        write_box(LOCAL_BOX_FILENAME, local_box);
        // release lock
        i += 1;
        sleep(SYNC_TIME);
    }

    return NULL;
}


void init()
{
    int fd;
    file_t *local_files;
    stack_t *changes;

    server_box = receive_server_box(socket_fd);

    if(file_exists(LOCAL_BOX_FILENAME) != 0)  {
        fd = creat(LOCAL_BOX_FILENAME, 0666);
        close(fd);
    }

    local_box = read_box(LOCAL_BOX_FILENAME);

    // pull
    changes = detect_server_changes(local_box, server_box);
    apply_server_changes(changes);
    write_box(LOCAL_BOX_FILENAME, local_box);

    // push
    local_files = get_local_files_list(".");
    changes = detect_local_changes(local_files, local_box);
    push_local_changes(changes);
    write_box(LOCAL_BOX_FILENAME, local_box);
}



int main(int argc, char *argv[])
{

    pthread_t tracker_id, listener_id;

    chdir(argv[3]);

    socket_fd = get_client_socket(argv[1], atoi(argv[2]));

    init();

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
