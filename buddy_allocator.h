#pragma once
#include "bitmap.h"
#define MEMORY_SIZE 1024*1024



typedef struct
{
    bitmap bit_map;
    int num_levels;
    int min_bucket_size;
    char* memory;

} buddyAllocator;


void buddyAllocator_init(buddyAllocator* alloc, bitmap* bit_map, char* buffer, int num_levels, int min_bucket_size, char* memory);

int first_level_bit(int level);

void* buddyAllocator_malloc(buddyAllocator* alloc, int size);

int free_node_idx(buddyAllocator* alloc, int level);

void update_bitmap(bitmap* bit_map, int idx, int status);

void update_parent(bitmap* bit_map, int idx);

void* memoryAddress(buddyAllocator* alloc, int free_node, int level);

void releaseBuddy(buddyAllocator* alloc, int node_idx);

void* buddyAllocator_free(buddyAllocator* alloc, void* mem);