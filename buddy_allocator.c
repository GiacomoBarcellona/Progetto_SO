#include "buddy_allocator.h"
#include <stdio.h>
#include <math.h>

// indice assoluto nella bitmap del primo nodo di un livello
int first_level_bit(int level)
{
    return (1<<level) - 1;
}

// costruttore allocator
void buddyAllocator_init(buddyAllocator* alloc, bitmap bit_map, uint8_t buffer, int num_levels, int min_bucket_size, char* memory)
{
    alloc->num_levels=num_levels;
    alloc->min_bucket_size=min_bucket_size;
    alloc->memory=memory;

    // calcolo il numero di bit(quindi di nodi)
    // inizializzo la bitmap utilizzata al posto delle liste
    int bits = (1<<num_levels)-1; // 2^num_levels - 1
    bitmap_init(&alloc->bit_map, bits, buffer);
}


void* buddyAllocator_malloc(buddyAllocator* alloc, int size)
{
    int total_mem = (1<<alloc->num_levels)*(alloc->min_bucket_size); // blocchi ultimo livello * dim blocco minimo
    int level = floor(log2(total_mem/size+8)); // calcolo il livello più piccolo che può soddisfare la richiesta

    if (level>=(alloc->num_levels)) level = alloc->num_levels - 1; // approssimo al blocco più piccolo che ho

    int free_node = free_node_idx(alloc, level);
    if(free_node == -1) return NULL; 

    // in caso positivo devo restituire l'indirizzo di memoria
    return memoryAddress(alloc, free_node, level);
}


int free_node_idx(buddyAllocator* alloc, int level)
{
    int first_lvl_bit = first_level_bit(level);
    int last_lvl_bit = first_lvl_bit + (1<<(level+1)) - 1;
    
    // ciclo sul livello della dimensione giusta
    for(int idx = first_lvl_bit; idx <= last_lvl_bit; ++idx)
    {
        if(bitmap_getBit(&alloc->bit_map, idx) == 0)
        {
            bitmap_setBit(&alloc->bit_map, idx, 1);
            return idx;
        } 
    }

    // se non lo trovo riprovo ricorsivamente al livello superiore
    if(level>0)
    {
        int parent_idx = free_node_idx(alloc, level-1);
        if(parent_idx!=-1)
        {
            // ho trovato un nodo più grande disponibile, devo eseguire lo splitting
            int left_child = splitParent(alloc, parent_idx);
            return left_child;
        }
    }

    return -1;

}


int splitParent(buddyAllocator* alloc, int parent_idx)
{
    bitmap_setBit(&alloc->bit_map, parent_idx, 1);

    int left_child = parent_idx * 2 + 1;
    int right_child  = parent_idx * 2 + 2;

    bitmap_setBit(&alloc->bit_map, left_child, 0); // ora disponibili
    bitmap_setBit(&alloc->bit_map, right_child, 0);
    return left_child;
}


void* memoryAddress(buddyAllocator* alloc, int free_node, int level)
{
    size_t block_size = (alloc->min_bucket_size)<<level; // dimensione dei blocchi al livello dato
    size_t mem_start = alloc->memory;

    // offset dall'inizio della memoria = offset dall'inizio del livello
    int idx_in_lvl = free_node - first_level_bit(level);
    size_t lvl_offset = block_size * idx_in_lvl;
    size_t free_address = mem_start + lvl_offset;

    return free_address;
}