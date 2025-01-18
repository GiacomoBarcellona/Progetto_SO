#pragma once
#include "buddy_allocator.h"
// per ora costanti
#define PAGE_SIZE 4096
#define BUFFER_SIZE 64 
#define BUDDY_LEVELS 9
#define MEMORY_SIZE 1024*1024 // il buddy da consegna dovrà gestire 1MB
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS-1)) // MEMORY_SIZE/(2^(BUDDY_LEVELS-1))

void pseudoMalloc_init(int num_levels); // faccio passare unico parametro indipendente

void* pseudo_malloc(int size);

void pseudo_free(void* p, int size);


