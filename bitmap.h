#pragma once
#include <stdint.h>

typedef struct
{
    uint8_t *buffer;
    int buffer_size;
    int num_bits;

} bitmap;

void bitmap_init(bitmap* bit_map, int bits, uint8_t* buffer);

int bitmap_getBytes(int bits);

int bitmap_getBit(const bitmap* bit_map, int bit_num);

void bitmap_setBit(bitmap* bit_map, int bit_num, int status);