
#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 10
#define GROWTH_FACTOR 2

typedef struct HeapNode
{
    void *data;
    double priority;
} HeapNode;

struct Heap
{
    HeapNode *nodes;
    HashTable *hash;
    int size;
    int capacity;
};

Heap *heap_construct(HashTable *h)
{
    Heap *heap = calloc(1, sizeof(Heap));
    heap->nodes = calloc(INITIAL_CAPACITY, sizeof(HeapNode));
    heap->capacity = INITIAL_CAPACITY;
    heap->size = 0;
    heap->hash = h;
    return heap;
}

void _heapify(Heap *heap, int idx)
{
    int idx_child = idx;
    int idx_parent = (idx - 1) / 2;
    HeapNode child = heap->nodes[idx_child];
    HeapNode parent = heap->nodes[idx_parent];

    while (idx_child > 0 && child.priority < parent.priority)
    {
        heap->nodes[idx_child] = parent;
        heap->nodes[idx_parent] = child;

        idx_child = idx_parent;
        idx_parent = (idx_child - 1) / 2;

        child = heap->nodes[idx_child];
        parent = heap->nodes[idx_parent];
    }
}

void _heapify_down(Heap *heap)
{
    int idx_parent = 0;
    int idx_child_left = 2 * idx_parent + 1;
    int idx_child_right = 2 * idx_parent + 2;

    while (idx_child_left < heap->size)
    {
        // decidindo qual filho trocar
        int idx_child = idx_child_left;
        if (idx_child_right < heap->size)
        {
            if (heap->nodes[idx_child_right].priority < heap->nodes[idx_child_left].priority)
            {
                idx_child = idx_child_right;
            }
        }
        // se o pai for menor que o filho, não precisa trocar
        if (heap->nodes[idx_parent].priority < heap->nodes[idx_child].priority)
        {
            break;
        }
        // trocando pai com filho
        HeapNode temp = heap->nodes[idx_parent];
        heap->nodes[idx_parent] = heap->nodes[idx_child];
        heap->nodes[idx_child] = temp;

        // atualizando indices
        idx_parent = idx_child;
        idx_child_left = 2 * idx_parent + 1;
        idx_child_right = 2 * idx_parent + 2;
    }
}

void _heap_att_hash(Heap *heap){
    for(int i = 0; i < heap->size; i++){
        int *idx = malloc(sizeof(int));
        idx = hash_table_set(heap->hash, heap->nodes[i].data, idx);
        if(idx != NULL){
            free(idx);
        }
    }
}

// to do: a heap push function that updates the priority of an element if it is already in the heap and retorna um void pointer para a data caso ela ja esteja no heap e tenha sido atualizada ou retorna nulo caso contrario
void *heap_push(Heap *heap, void *data, double priority)
{
    // se o elemento ja estiver no heap, atualiza a prioridade
    int *idx = hash_table_get(heap->hash, data);
    if (idx != NULL)
    {
        if (priority < heap->nodes[*idx].priority)
        {
            heap->nodes[*idx].priority = priority;
            void *data_aux = heap->nodes[*idx].data;
            heap->nodes[*idx].data = data;
            data = data_aux;
            _heapify(heap, *idx);
            _heap_att_hash(heap);
        }

        return data;
    }

    // se o heap estiver cheio, aumenta a capacidade
    if (heap->size == heap->capacity)
    {
        heap->capacity *= GROWTH_FACTOR;
        heap->nodes = realloc(heap->nodes, heap->capacity * sizeof(HeapNode));
    }

    // adicionando o elemento no heap
    heap->nodes[heap->size].data = data;
    heap->nodes[heap->size].priority = priority;
    heap->size += 1;
    
    int *idx_ = malloc(sizeof(int));
    *idx_ = heap->size - 1;
    hash_table_set(heap->hash, data, idx);

    _heapify(heap, heap->size - 1);
    _heap_att_hash(heap);
    return NULL;
}

void *heap_pop(Heap *heap)
{
    if (heap_empty(heap))
    {
        return NULL;
    }
    void *data = heap->nodes[0].data;
    HashTableItem *item = hash_table_pop(heap->hash, data);
    free(item->val);
    free(item);

    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size -= 1;
    _heapify_down(heap);
    _heap_att_hash(heap);
    return data;
}

bool heap_empty(Heap *heap)
{
    return heap->size == 0;
}

void *heap_max(Heap *heap)
{
    return heap->nodes[0].data;
}

double heap_min_priority(Heap *heap)
{
    return heap->nodes[0].priority;
}

void heap_destroy(Heap *heap)
{
    hash_table_destroy(heap->hash);
    free(heap->nodes);
    free(heap);
}
