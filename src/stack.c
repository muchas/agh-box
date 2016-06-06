#include <stdlib.h>
#include "stack.h"


stack_t* init_stack()
{
    stack_t *new_entry;

    new_entry = malloc(sizeof(stack_t));
    new_entry->data = NULL;
    new_entry->next = NULL;

    return new_entry;
}


int push(stack_t *stack, void *data)
{
    stack_t *new_entry;

    new_entry = malloc(sizeof(stack_t));
    new_entry->data = data;
    new_entry->next = NULL;

    while(stack->next != NULL) stack = stack->next;
    stack->next = new_entry;

    return 0;
}


void *pop(stack_t *stack)
{
    stack_t *entry, *prev;
    void *data;

    if(stack->next == NULL) return NULL;

    prev = stack;
    entry = prev->next;

    while(entry->next != NULL) {
        prev = entry;
        entry = entry->next;
    }
    prev->next = NULL;

    data = entry->data;
    free(entry);

    return data;
}

