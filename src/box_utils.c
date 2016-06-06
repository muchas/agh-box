#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include "box_utils.h"

int insert_into_box(box_entry_t *entries, char* path, time_t global_time, time_t local_time, int is_directory)
{
    box_entry_t *new_entry;

    new_entry = entries;

    while(new_entry->next != NULL) new_entry = new_entry->next;

    new_entry->global_timestamp = global_time;
    new_entry->local_timestamp = local_time;
    new_entry->is_directory = is_directory;
    new_entry->next = malloc(sizeof(box_entry_t));
    strncpy(new_entry->path, path, MAX_PATHLEN);

    new_entry->next->next = NULL;

    return 0;
}


box_entry_t* read_box(char *path) // PATH, returned entries, returns: -1 when error, 0 on success
{
    FILE *handler;
    box_entry_t *head, *it;

    handler = fopen(path, "r");

    head = malloc(sizeof(box_entry_t));
    it = head;

    while(fread(it, sizeof(box_entry_t), 1, handler) > 0) {
        it->next = malloc(sizeof(box_entry_t));
        it = it->next;
    }

    it->next = NULL;

    fclose(handler);

    return head;
}


int write_box(char *path, box_entry_t *entries)
{
    FILE* handler;
    box_entry_t *it;

    handler = fopen(path, "w");
    it = entries;

    while(it->next != NULL) {
        fwrite(it, sizeof(box_entry_t), 1, handler);
        it = it->next;
    }

    fclose(handler);
    return 0;
}


void print_box(box_entry_t* head)
{
    while(head->next != NULL) {
        printf("%s\n", head->path);
        head = head->next;
    }
}


box_entry_t* find_in_box(box_entry_t *box_entry, char *name)
{
    box_entry_t *it;

    it = box_entry;

    while(it->next != NULL) {
        if(strcmp(it->path, name) == 0) {
            return it;
        }
        it = it->next;
    }

    return NULL;
}


void create_or_update(box_entry_t *box_entry, char *name, size_t size, time_t local_time, time_t server_time)
{
    box_entry_t *entry;

    entry = find_in_box(box_entry, name);

    printf("Updating box... Name: %s, local time: %d\n", name, local_time);

    if(entry == NULL) {
        insert_into_box(box_entry, name, server_time, local_time, -1);
    } else {
        strcpy(box_entry->path, name);

        if(local_time != 0) {
            entry->local_timestamp = local_time;
        }

        if(server_time != 0) {
            entry->global_timestamp = server_time;
        }
    }
}
