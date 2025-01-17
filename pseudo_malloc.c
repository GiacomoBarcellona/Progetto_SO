#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>



void malloc_init(buddyAllocator* alloc, bitmap* bit_map, char* buffer, int num_levels, int min_bucket_size, char* memory)
{
    buddyAllocator_init(&alloc, &bit_map, buffer, num_levels, min_bucket_size, memory);
}


void* pseudo_malloc(buddyAllocator* alloc, int size)
{
    if(size < PAGE_SIZE/4)
    {
        void* p = buddyAllocator_malloc(&alloc, size);
        if(p==NULL)
        {
            printf("fallimento allocazione con buddyAllocator");
        }
        return p;
    }
    else
    {
        void* p = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, -1, 0);
        if(p==MAP_FAILED)
        {
            printf("fallimento allocazione con mmap");
        }
        return p;    
    }
}

void pseudo_free(void* p, int size)
{
    
}