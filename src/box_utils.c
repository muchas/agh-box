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