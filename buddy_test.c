#include "buddy_allocator.h"

#define BUFFER_SIZE 64 
#define BUDDY_LEVELS 9
#define MEMORY_SIZE 1024*1024 // il buddy da consegna dovrà gestire 1MB
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS-1)) // MEMORY_SIZE/(2^(BUDDY_LEVELS-1))

char buffer[BUFFER_SIZE];
char memory[MEMORY_SIZE];

buddyAllocator allocator;
bitmap bmap;

int main(int argc, char** argv)
{
    //inizializzo il buddy allocator, che a sua volta inizializza bitmap 
    
    buddyAllocator_init(&allocator, &bmap, buffer, BUDDY_LEVELS, MIN_BUCKET_SIZE, memory);


    void* request1 = buddyAllocator_malloc(&allocator, 1048572);
    buddyAllocator_free(&allocator, request1);
      
    void* request2 = buddyAllocator_malloc(&allocator, 500000);
    void* request3 = buddyAllocator_malloc(&allocator, 500000);
    void* request4 = buddyAllocator_malloc(&allocator, 500000);
    buddyAllocator_free(&allocator, request2);
    buddyAllocator_free(&allocator, request3);
    buddyAllocator_free(&allocator, request4);

    void* request5 = buddyAllocator_malloc(&allocator, 25000000);
    buddyAllocator_free(&allocator, request5);
    void* request6 = buddyAllocator_malloc(&allocator, 10);
    buddyAllocator_free(&allocator, request6);
}