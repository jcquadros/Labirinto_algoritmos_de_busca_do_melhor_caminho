
#ifndef _STACK_H_
#define _STACK_H_

#include "types.h"

typedef struct Stack Stack;
typedef void(*destroy_stack)(void*);

Stack *stack_construct(destroy_stack destroy_fn);
void stack_push(Stack *stack, void *data);
bool stack_empty(Stack *stack);
void *stack_pop(Stack *stack);
void stack_destroy(Stack *stack);

#endif