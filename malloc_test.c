#include "pseudo_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv)
{ 
    pseudoMalloc_init(9); 

    printf("Allocazione gestibile dal buddy\n"); 
    void* request1 = pseudo_malloc(250); 
    if(request1!=NULL){printf("Riuscita\n");}

    memset(request1, 1, 250); 
    printf("Valore scritto in prima pos: %c\n", ((char*)request1)[0]); 
    printf("Valore scritto in ultima pos: %c\n", ((char*)request1)[249]); 

    pseudo_free(request1, 250); 

    printf("Allocazione da fare con mmap\n");
    void* request2 = pseudo_malloc(2500); 
    if(request2!=NULL){printf("Riuscita\n");}

    memset(request2, 2, 2500); 
    pseudo_free(request2, 2500); 
    
    void* req3 = pseudo_malloc(500); 
    void* req4 = pseudo_malloc(500); 
    void* req5 = pseudo_malloc(500);
    pseudo_free(req3, 500); 
    pseudo_free(req4, 500); 
    pseudo_free(req5, 500); 
    pseudo_free(req5, 500);

    void* req6 = NULL;
    pseudo_free(req6, 1000); 
    
    return 0; 
}  