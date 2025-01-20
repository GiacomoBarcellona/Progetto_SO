#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>


static char buffer[BUFFER_SIZE];
static char memory[MEMORY_SIZE];
static bitmap bmap;
static buddyAllocator alloc;
// dichiarato globalmente per non costringere 
// l'utente a inserirlo come parametro ogni volta


void pseudoMalloc_init(int num_levels)
{
    char buffer[BUFFER_SIZE];
    char memory[MEMORY_SIZE];
    buddyAllocator_init(&alloc, &bmap, buffer, num_levels, MIN_BUCKET_SIZE, memory);
}


void* pseudo_malloc(int size)
{   
    if((size+4) < PAGE_SIZE/4) // overhead necessario se uso buddy 
    {
        void* p = buddyAllocator_malloc(&alloc, size+4); 
        if(p==NULL)
        {
            printf("fallimento allocazione con buddyAllocator");
        }
        return p;
    }
    else
    {
        // per usarla come malloc:
        void* p = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0); 
        if(p==MAP_FAILED)
        {
            printf("fallimento allocazione con mmap");
        }
        return p;    
    }
}

void pseudo_free(void* p, int size)
{
    if(p==NULL)
    {
        printf("Errore free: puntatore non valido.\n");
        return;
    }
    if(size<=0)
    {
        printf("Errore free: dimensione non valida.\n");
        return;
    }


    if((size+4) < PAGE_SIZE/4)
    {
        buddyAllocator_free(&alloc, p);
    }
    else
    {
        int mnp = munmap(p, size);
        if(mnp==-1) printf("fallimento deallocazione con munmap");
    }
}