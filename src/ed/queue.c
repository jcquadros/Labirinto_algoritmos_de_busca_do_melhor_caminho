#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "deque.h"

struct Queue{
    Deque *data;
};

Queue *queue_construct(destroy_queue destroy_fn){
    Queue *queue = (Queue *)calloc(1, sizeof(Queue));
    queue->data = deque_construct(destroy_fn);
    return queue;
}

void queue_push(Queue *queue, void *data){
    deque_push_back(queue->data, data);
}

bool queue_empty(Queue *queue){
    return deque_size(queue->data) == 0;
}

void *queue_pop(Queue *queue){
    return deque_pop_front(queue->data);
}

void queue_destroy(Queue *queue){
    deque_destroy(queue->data);
    free(queue);
}