#include "buddy_allocator.h"
#include <stdio.h>
#include <math.h>

// indice assoluto nella bitmap del primo nodo di un livello
int first_level_bit(int level)
{
    return (1<<level) - 1;
}

int buddyIdx(int idx)
{
    if (idx&0x1)
    {
        return idx-1;
    }
    return idx+1;
}

int parentIdx(int idx)
{
    return idx/2;
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
    int level = floor(log2(total_mem/size+4)); // livello più piccolo per contenere size + overhead per un int

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
    size_t mem_offset = mem_start + lvl_offset;
    
    // salvo nei 4 byte iniziali l'indice del nodo per poterlo recuperare
    int* free_address = (int*)mem_offset; 
    *free_address = free_node;
    return (void*)(free_address+1);
}


void releaseBuddy(buddyAllocator* alloc, int node_idx)
{
    int node_buddy = buddyIdx(node_idx);
    
    // controllo se posso fare il merge
    if (bitmap_getBit(&alloc->bit_map, node_buddy)==0)
    {
        bitmap_setBit(&alloc->bit_map, node_buddy, 1); // figli non disponibili
        int parent_idx = parentIdx(node_idx);
        bitmap_setBit(&alloc->bit_map, parent_idx, 0);

        if(parent_idx!=1) 
        {
            // chiamo ricorsivamente se il padre non è radice 
            releaseBuddy(alloc, parent_idx);
        }
    }
    else
    {
        bitmap_setBit(&alloc->bit_map, node_idx, 0);
    }
}

void* buddyAllocator_free(buddyAllocator* alloc, void* mem)
{
    int* allocated_block = (int*)mem;
    int node_idx = *(allocated_block-1); // torno indietro e recupero la posizione

    releaseBuddy(alloc, node_idx);
}