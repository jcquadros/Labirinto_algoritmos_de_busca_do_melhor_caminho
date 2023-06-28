
#include <stdlib.h>
#include <stdio.h>
#include "hash.h"
#include "forward_list.h"

struct HashTable
{
    ForwardList **buckets;
    HashFunction hash_fn;
    CmpFunction cmp_fn;
    int table_size;
    int n_elements;
};

struct HashTableIterator{
    HashTable *hash;
    int bucket_idx;
    Node *node;
};

HashTableItem *_hash_pair_construct(void *key, void *val)
{
    HashTableItem *p = calloc(1, sizeof(HashTableItem));
    p->key = key;
    p->val = val;
    return p;
}

void _hash_pair_destroy(HashTableItem *p)
{
    //free(p->key);
    //free(p->val);
    free(p);
}


void * _hash_table_get_pair(HashTable * h, void *key){
    int hash = h->hash_fn(h, key) % h->table_size;
    ForwardList *list = h->buckets[hash];

    if(!list){
        return NULL;
    }

    Node *n = list->head;
    while(n != NULL){
        HashTableItem *pair = n->value;
        if(h->cmp_fn(pair->key, key) == 0){
            return pair;
        }
        n = n->next;
    }

    return NULL;
}

HashTable *hash_table_construct(int table_size, HashFunction hash_fn, CmpFunction cmp_fn)
{
    HashTable *hash_tbl = calloc(1, sizeof(HashTable));

    hash_tbl->table_size = table_size;
    hash_tbl->hash_fn = hash_fn;
    hash_tbl->cmp_fn = cmp_fn;
    hash_tbl->buckets = calloc(table_size, sizeof(ForwardList *));
    hash_tbl->n_elements = 0;

    return hash_tbl;
}

void *hash_table_set(HashTable *h, void *key, void *val)
{
    int hash = h->hash_fn(h, key) % h->table_size;
    ForwardList *list = h->buckets[hash];
    if (!list)
    {
        list = forward_list_construct();
        h->buckets[hash] = list;
    }

    HashTableItem *item = _hash_table_get_pair(h, key);

    if (!item)
    {
        HashTableItem *pair = _hash_pair_construct(key, val);
        forward_list_push_front(h->buckets[hash], pair);
        h->n_elements++;
    }
    else
    {
        void * old_val = item->val;
        item->val = val;
        return old_val;
    }
    return NULL;
}

void *hash_table_get(HashTable *h, void *key)
{
    HashTableItem *item = _hash_table_get_pair(h, key);

    if (!item)
    {
        return NULL;
    }
    return item->val;
}

void *hash_table_pop(HashTable *h, void *key)
{
    int hash = h->hash_fn(h, key) % h->table_size;
    ForwardList *list = h->buckets[hash];

    if (!list)
    {
        return NULL;
    }

    HashTableItem *item = _hash_table_get_pair(h, key);

    if (!item)
    {
        return NULL;
    }

    void *val = item->val;
    forward_list_remove(list, item);
    if(forward_list_size(list) == 0){
        forward_list_destroy(list);
        h->buckets[hash] = NULL;
    }
    _hash_pair_destroy(item);
    h->n_elements--;

    return val;
}


int hash_table_size(HashTable *h)
{
    return h->table_size;
}

int hash_table_num_elems(HashTable *h)
{
    return h->n_elements;
}

void hash_table_destroy(HashTable *h)
{
    for (int i = 0; i < h->table_size; i++)
    {
        if (h->buckets[i] != NULL)
        {
            Node *n = h->buckets[i]->head;

            while (n != NULL)
            {
                HashTableItem *pair = n->value;
                _hash_pair_destroy(pair);
                n = n->next;
            }

            forward_list_destroy(h->buckets[i]);
        }
    }

    free(h->buckets);
    free(h);
}

HashTableIterator *hash_table_iterator(HashTable *h){
    HashTableIterator *it = calloc(1, sizeof(HashTableIterator));
    it->hash = h;
    it->bucket_idx = 0;
    it->node = NULL;

    while(!hash_table_iterator_is_over(it) && it->hash->buckets[it->bucket_idx] == NULL){
        it->bucket_idx++;
    }
    if(!hash_table_iterator_is_over(it)){
        it->node = it->hash->buckets[it->bucket_idx]->head;
    }
    
    return it;
}

int hash_table_iterator_is_over(HashTableIterator *it){
    if(it->bucket_idx == it->hash->table_size){
        return 1;
    }
    return 0;
}

HashTableItem *hash_table_iterator_next(HashTableIterator *it){
    
    
    HashTableItem *pair = it->node->value;
    it->node = it->node->next;

    if(!it->node){
        it->bucket_idx++;
        while(!hash_table_iterator_is_over(it) && it->hash->buckets[it->bucket_idx] == NULL){
            it->bucket_idx++;
        }
        if(!hash_table_iterator_is_over(it)){
            it->node = it->hash->buckets[it->bucket_idx]->head;
        }
    }
    return pair;
}

void hash_table_iterator_destroy(HashTableIterator *it){
    free(it);
}
