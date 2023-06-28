#include <stdlib.h>
#include "heap.h"
#define GROWTH_FACTOR 2
#define INITIAL_CAPACITY 10

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
    Heap *heap = (Heap *)malloc(sizeof(Heap));
    heap->nodes = (HeapNode *)calloc(INITIAL_CAPACITY, sizeof(HeapNode));
    heap->hash = h;
    heap->size = 0;
    heap->capacity = INITIAL_CAPACITY;
    return heap;
}
void _hash_table_set(HashTable *h, void *data, int index){
    int *new_index = (int *)malloc(sizeof(int));
    *new_index = index;
    new_index = hash_table_set(h, data, new_index);
    if (new_index != NULL)
        free(new_index); 
}

void _swap(Heap *heap, int i, int j)
{
    HeapNode tmp = heap->nodes[i];
    heap->nodes[i] = heap->nodes[j];
    heap->nodes[j] = tmp;
    
    _hash_table_set(heap->hash, heap->nodes[i].data, i);
    _hash_table_set(heap->hash, heap->nodes[j].data, j);
}

void _heapfy_up(Heap *heap, int idx)
{
    int idx_child = idx;
    int idx_parent = (idx - 1) / 2;
    HeapNode child = heap->nodes[idx_child];
    HeapNode parent = heap->nodes[idx_parent];

    while (idx_child > 0 && child.priority < parent.priority)
    {
        _swap(heap, idx_child, idx_parent);

        idx_child = idx_parent;
        idx_parent = (idx_child - 1) / 2;

        child = heap->nodes[idx_child];
        parent = heap->nodes[idx_parent];
    }
}

void _heapfy_down(Heap *heap, int index)
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
        if (heap->nodes[idx_parent].priority < heap->nodes[idx_child].priority || idx_child >= heap->size)
        {
            break;
        }

        // trocando
        _swap(heap, idx_parent, idx_child);

        // atualizando indices
        idx_parent = idx_child;
        idx_child_left = 2 * idx_parent + 1;
        idx_child_right = 2 * idx_parent + 2;
    }
}

void *heap_push(Heap *heap, void *data, double priority)
{
    int *index = (int *)hash_table_get(heap->hash, data);
    if (index != NULL)
    {
        int index = *(int *)hash_table_get(heap->hash, data);

        if (priority < heap->nodes[index].priority)
        {
            heap->nodes[index].priority = priority;
            void *tmp = heap->nodes[index].data;
            heap->nodes[index].data = data;
            data = tmp;
            _heapfy_up(heap, index);
        }
        
        return data;
    }

    if (heap->size == heap->capacity)
    {
        heap->capacity *= GROWTH_FACTOR;
        heap->nodes = (HeapNode *)realloc(heap->nodes, heap->capacity * sizeof(HeapNode));
    }

    heap->nodes[heap->size].data = data;
    heap->nodes[heap->size].priority = priority;
    heap->size++;
    
    _hash_table_set(heap->hash, heap->nodes[heap->size - 1].data, heap->size - 1);
    _heapfy_up(heap, heap->size - 1);
    return NULL;
}

bool heap_empty(Heap *heap)
{
    return heap->size == 0;
}

void *heap_max(Heap *heap)
{
    if (heap->size == 0)
        return NULL;
    return heap->nodes[0].data;
}

double heap_min_priority(Heap *heap)
{
    if (heap->size == 0)
        return 0;
    return heap->nodes[0].priority;
}

void *heap_pop(Heap *heap)
{
    if (heap->size == 0)
        return NULL;
    void *data = heap->nodes[0].data;
    _swap(heap, 0, heap->size - 1);
    heap->size--;
    void *val = hash_table_pop(heap->hash, data);
    if(val != NULL)
        free(val);
    _heapfy_down(heap, 0);
    return data;
}

void heap_destroy(Heap *heap)
{
    hash_table_destroy(heap->hash);
    free(heap->nodes);
    free(heap);
}
