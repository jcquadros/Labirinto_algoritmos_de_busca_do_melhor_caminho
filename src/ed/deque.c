#include "deque.h"
#include <stdlib.h>
#include <stdio.h>

#define BLOCK_SIZE 4
#define INITIAL_SIZE 2
#define GROWTH_FACTOR 2
// Create a new empty deque
struct Deque
{
    void ***blocks;
    int first;
    int last;
    int first_block;
    int last_block;
    int size_block;
    int capacity_block;
    int size;
    destroy_deque destroy_fn;
};

// Create a new empty deque
Deque *deque_construct(destroy_deque destroy_fn)
{
    Deque *d = (Deque *)calloc(1, sizeof(Deque));
    d->blocks = (void ***)calloc(INITIAL_SIZE, sizeof(void **));
    d->first_block = INITIAL_SIZE / 2;
    d->last_block = INITIAL_SIZE / 2;
    d->size_block = BLOCK_SIZE;
    d->capacity_block = INITIAL_SIZE;
    d->destroy_fn = destroy_fn;
    d->blocks[d->first_block] = (void **)calloc(d->size_block, sizeof(void *));
    return d;
}

void _deque_realloc(Deque *d)
{
    d->capacity_block *= GROWTH_FACTOR;

    void ***new_blocks = (void ***)calloc(d->capacity_block, sizeof(void **));
    int qtdd_blocks = d->last_block - d->first_block + 1;
    int new_first_block = (d->capacity_block - qtdd_blocks) / 2;
    int new_last_block = new_first_block + qtdd_blocks - 1;

    int new_idx = new_first_block;
    for (int i = d->first_block; i <= d->last_block; i++)
    {
        new_blocks[new_idx] = d->blocks[i];
        new_idx++;
    }

    free(d->blocks);
    d->blocks = new_blocks;
    d->first_block = new_first_block;
    d->last_block = new_last_block;


    
}

void deque_push_back(Deque *deque, void *data)
{
    if(deque->last == deque->size_block){
        deque->last = 0;
        if(deque->last_block == deque->capacity_block - 1){
            _deque_realloc(deque);
        }
        deque->last_block++;
        deque->blocks[deque->last_block] = (void **)calloc(deque->size_block, sizeof(void *));
    }

    deque->blocks[deque->last_block][deque->last] = data;
    deque->last++;
    deque->size++;
}


void deque_push_front(Deque *deque, void *data)
{
    deque->first--;
    if(deque->first < 0){
        deque->first = deque->size_block - 1;
        if(deque->first_block == 0){
            _deque_realloc(deque);
        }
        deque->first_block--;
        deque->blocks[deque->first_block] = (void **)calloc(deque->size_block, sizeof(void *));
    }
    deque->blocks[deque->first_block][deque->first] = data;
    deque->size++;
}

void *deque_pop_back(Deque *deque)
{
    if (deque->size == 0)
    {
        printf("deque_pop_back: Deque is empty\n");
        exit(1);
    }

    deque->last--;
    if(deque->last < 0){
        deque->last = deque->size_block - 1;
        free(deque->blocks[deque->last_block]);
        deque->last_block--; 
    }

    void *data = deque->blocks[deque->last_block][deque->last];
    deque->blocks[deque->last_block][deque->last] = NULL;

    deque->size--;
    return data;
}

void *deque_pop_front(Deque *deque)
{
    if (deque->size == 0)
    {
        printf("deque_pop_front: Deque is empty\n");
        exit(1);
    }
    
    void *data = deque->blocks[deque->first_block][deque->first];
    deque->blocks[deque->first_block][deque->first] = NULL;

    deque->first++;
    if(deque->first == deque->size_block){
        deque->first = 0;
        free(deque->blocks[deque->first_block]);
        deque->first_block++;
    }

    deque->size--;
    return data;
}

void *deque_get(Deque *deque, int index)
{
    if (index >= deque->size)
    {
        printf("deque_get: Index out of bounds\n");
        exit(1);
    }
    int block = ((index + deque->first) / deque->size_block) + deque->first_block;
    int i = (index + deque->first) % deque->size_block;
    void *data = deque->blocks[block][i];
    if (data == NULL)
    {
        printf("deque_get: Error! Data is NULL\n");
        exit(1);
    }
    return deque->blocks[block][i];
}
int deque_size(Deque *deque){
    return deque->size;
}
void deque_remove(Deque *deque){
    while(deque->size > 0){
        void *data = deque_pop_back(deque);
        if(data && deque->destroy_fn){
            deque->destroy_fn(data);
        }
    }
}

void deque_destroy(Deque *deque)
{
    deque_remove(deque);
    for(int i = deque->first_block; i <= deque->last_block; i++){
        free(deque->blocks[i]);
    }
    free(deque->blocks);
    free(deque);
}
