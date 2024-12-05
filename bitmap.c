#include <assert.h>
#include "bitmap.h"



// calcola i bytes necessari per contenere i bit(nodi albero) nel buffer 
int bitmap_getBytes(int bits)
{
    return bits/8 + (bits%8)!=0;
}


// costruttore bitmap
void bitmap_init(bitmap* bit_map, int bits, uint8_t* buffer)
{
    bit_map->buffer = buffer; 
    bit_map->num_bits = bits;
    bit_map->buffer_size = bitmap_getBytes(bits);

    for (int i=0; i<bits; ++i)
    {
        bitmap_setBit(bit_map, i, 0); // 0, libera
    }
}


// legge il valore del bit nella posizione desiderata 
int bitmap_getBit(const bitmap* bit_map, int bit_num)
{
    int byte_index = bit_num>>3; // eq bit_num/8
    assert(byte_index<bit_map->buffer_size);
    int bit_index = bit_num&0x07; // eq bit_num%8, idx nel byte

    return (bit_map->buffer[byte_index] & (1<<bit_index))!=0;
}


// imposta il valore del bit nella posizione desiderata
void bitmap_setBit(bitmap* bit_map, int bit_num, int status)
{
    int byte_index = bit_num>>3;
    assert(byte_index<bit_map->buffer_size); // valido
    int bit_index = bit_num&0x07;

    if(status==1)
    {
        bit_map->buffer[byte_index] |= (1<<bit_index); 
    }
    else
    {
        bit_map->buffer[byte_index] &= ~(1<<bit_index); 
    }
}