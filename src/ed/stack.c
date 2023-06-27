#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "deque.h"

struct Stack{
    Deque *data;
};

Stack *stack_construct(destroy_stack destroy_fn){
    Stack *stack = (Stack *)calloc(1, sizeof(Stack));
    stack->data = deque_construct(destroy_fn);
    return stack;
}

void stack_push(Stack *stack, void *data){
    deque_push_back(stack->data, data);
}

void *stack_get(Stack *stack, int index){
    return deque_get(stack->data, index);
}

bool stack_empty(Stack *stack){
    return deque_size(stack->data) == 0;
}

int stack_size(Stack *stack){
    return deque_size(stack->data);
}

void *stack_pop(Stack *stack){
    return deque_pop_back(stack->data);
}

void stack_destroy(Stack *stack){
    deque_destroy(stack->data);
    free(stack);
}