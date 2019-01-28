#ifndef _MEM_POOL_H__
#define _MEM_POOL_H__

#include <stdlib.h>

typedef struct {
    size_t block_num;
    size_t block_size;
    size_t block_allocated;
    void *next;
    void *blocks;
} mem_pool_t;

int pool_create(mem_pool_t *pool, size_t nmemb, size_t size);

void pool_destroy(mem_pool_t *pool);

void *pool_alloc(mem_pool_t *pool);

void pool_free(mem_pool_t *pool, void *ptr);

#endif