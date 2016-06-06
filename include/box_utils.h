#include "communication_utils.h"

#ifndef box_utils_h
#define box_utils_h
int insert_into_box(box_entry_t *entries, char* path, time_t global_time, time_t local_time, int is_directory);
box_entry_t* read_box(char *path);
int write_box(char *path, box_entry_t *entries);
void print_box(box_entry_t* head);
box_entry_t* find_in_box(box_entry_t *box_entry, char *name);
void create_or_update(box_entry_t *box_entry, char *name, size_t size, time_t local_time, time_t server_time);

#endif
