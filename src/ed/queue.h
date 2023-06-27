
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "types.h"
typedef void (*destroy_queue)(void*);
typedef struct Queue Queue;

Queue *queue_construct(destroy_queue destroy_fn);
void queue_push(Queue *queue, void *data);
bool queue_empty(Queue *queue);
void *queue_pop(Queue *queue);
void queue_destroy(Queue *queue);

#endif