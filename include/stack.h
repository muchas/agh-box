#ifndef stack_h
#define stack_h

typedef struct Stack {
    void *data;
    struct Stack *next;
} stack_t;


stack_t *init_stack();
int push(stack_t *stack, void *data);
void* pop(stack_t *stack);

#endif