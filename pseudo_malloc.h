#include "buddy_allocator.h"
#define PAGE_SIZE 4096


void malloc_init(buddyAllocator* alloc, bitmap* bit_map, char* buffer, int num_levels, int min_bucket_size, char* memory);

void* pseudo_malloc();

void pseudo_free();


