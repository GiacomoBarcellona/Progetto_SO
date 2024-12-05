#pragma once
#include "bitmap.h"



typedef struct
{
    bitmap bit_map;
    int num_levels;
    int min_bucket_size;
    char* memory;

} buddyAllocator;


void buddyAllocator_init(buddyAllocator* alloc, bitmap bit_map, uint8_t buffer, int num_levels, int min_bucket_size, char* memory);

int first_level_bit(int level);

void* buddyAllocator_malloc(buddyAllocator* alloc, int size);

int free_node_idx(buddyAllocator* alloc, int level);

int splitParent(buddyAllocator* alloc, int parent_idx);

void* memoryAddress(buddyAllocator* alloc, int free_node, int level);

void releaseBuddy(buddyAllocator* alloc, int node_idx);

void* buddyAllocator_free(buddyAllocator* alloc, void* mem);