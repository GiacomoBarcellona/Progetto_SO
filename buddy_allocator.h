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


