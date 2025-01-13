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
        return idx+1;
    }
    return idx-1;
}

int parentIdx(int idx)
{
    if (idx&0x1)
    {
        return (idx-1)/2;
    }
    return (idx-2)/2;
}

int leftChildIdx(int idx)
{
    return 2*idx+1;
}

int rightChildIdx(int idx)
{
    return 2*idx+2;
}

// costruttore allocator
void buddyAllocator_init(buddyAllocator* alloc, bitmap* bit_map, char* buffer, int num_levels, int min_bucket_size, char* memory)
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
    int min_bucket = alloc->min_bucket_size;
    int total_mem = (1<<alloc->num_levels-1)*(min_bucket); // num blocchi ultimo livello * dim blocco minimo
    
    
    // cerco livello più piccolo per contenere size + overhead per un int
    int level = floor(log2(total_mem/(size + 4))); 

    printf("livello calcolato: %d\n", level);

    if (level>=(alloc->num_levels)) level = alloc->num_levels - 1; // approssimo al blocco più piccolo che ho

    printf("richiesti %d bytes, da allocare al livello %d\n", size, level);

    int free_node = free_node_idx(alloc, level);
    if(free_node == -1) 
    {
        printf("Blocco libero non trovato, allocazione non esaudibile.\n");
        return NULL;
    } 

    // in caso positivo devo restituire l'indirizzo di memoria
    return memoryAddress(alloc, free_node, level);
}


int free_node_idx(buddyAllocator* alloc, int level)
{
    int first_lvl_bit = first_level_bit(level);
    int last_lvl_bit = first_lvl_bit + (1<<(level)) - 1;
    printf("il primo bit del livello %d è %d\n", level, first_lvl_bit);
    printf("l'ultimo bit del livello %d è %d\n", level, last_lvl_bit);

    // ciclo sul livello della dimensione giusta
    for(int idx = first_lvl_bit; idx <= last_lvl_bit; ++idx)
    {
        printf("controllo il bit: %d\n", idx);
        if(bitmap_getBit(&alloc->bit_map, idx) == 0)
        {
            printf("trovato nodo libero a index %d, setto a 1\n", idx);
            
            // aggiorno coerentemente la bitmap dai nodi figli
            update_bitmap(&alloc->bit_map, idx, 1);

            return idx;
        } 
    }



    return -1;

}

// segna le partizioni di memoria del blocco scelto come occupate(libere) 
// evitando allocazioni in eccesso rispetto alle possibilità
void update_bitmap(bitmap* bit_map, int idx, int status)
{
    bitmap_setBit(bit_map, idx, status);
    if(leftChildIdx(idx) < bit_map->num_bits) //l'albero è completo
    {
        update_bitmap(bit_map, leftChildIdx(idx), status);
        update_bitmap(bit_map, rightChildIdx(idx), status);
    }
}


void* memoryAddress(buddyAllocator* alloc, int free_node, int level)
{
    size_t block_size = (alloc->min_bucket_size)<<level; // dimensione dei blocchi al livello dato
    size_t mem_start = (size_t)alloc->memory;

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
    update_bitmap(&alloc->bit_map, node_idx, 0);

    printf("rilasciando nodo a indice %d\n", node_idx);

    int node_buddy = buddyIdx(node_idx);
    
    // controllo se posso fare il merge
    if (bitmap_getBit(&alloc->bit_map, node_buddy)==0)
    {
        int parent_idx = parentIdx(node_idx);

        printf("merging con buddy con indice %d\n", node_buddy);
        printf("il loro padre è: %d\n", parent_idx);

        if(parent_idx!=0) 
        {
            // chiamo ricorsivamente se il padre non è radice 
            releaseBuddy(alloc, parent_idx);
        }
        else 
        {
            printf("sono risalito fino alla radice, la libero\n");
            bitmap_setBit(&alloc->bit_map, parent_idx, 0);
        }
    }
}

void* buddyAllocator_free(buddyAllocator* alloc, void* mem)
{
    if(mem==NULL)
    {
        printf("Zona di memoria da deallocare non valida.\n");
        return NULL;
    }
    int* allocated_block = (int*)mem;
    int node_idx = *(allocated_block-1); // torno indietro e recupero la posizione

    printf("libero blocco a indice %d\n", node_idx);
    releaseBuddy(alloc, node_idx);
}