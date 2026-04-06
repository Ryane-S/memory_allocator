#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "memory_alloc.h"

int main(int argc, char**argv) {

    
    struct memory_alloc m;
    int nb_blocks = 8;
    size_t block_size = 16;

    memalloc_init(&m, nb_blocks, block_size);
    memalloc_print(&m);
    
    size_t alloc_size = (nb_blocks * block_size) / 3;
    
    /*
    char* buffer = memalloc_allocate(&m, 64);
    for(int i=0; i<alloc_size; i++)
        buffer[i]='a';

    buffer[alloc_size-1] = '\0';
    printf("%s\n", buffer);
    memalloc_free(&m, buffer, alloc_size);
    */

    // Lifelike functions
    char* lifelike_buffer = memalloc_lifelike_malloc(&m, alloc_size);
    assert(lifelike_buffer);
    for(int i=0; i<alloc_size-1; i++)
        lifelike_buffer[i]='x';
    lifelike_buffer[alloc_size-1] = '\0';
    printf("%s\n", lifelike_buffer);

    size_t new_size = alloc_size;
    lifelike_buffer = memalloc_lifelike_realloc(&m, lifelike_buffer, new_size);
    assert(lifelike_buffer);
    for(int i=alloc_size; i<new_size-1; i++)
        lifelike_buffer[i]='y';
    lifelike_buffer[new_size-1] = '\0';
    printf("%s\n", lifelike_buffer);

    memalloc_lifelike_free(&m, lifelike_buffer);

    memalloc_print(&m);

    memalloc_finalize(&m);

    return EXIT_SUCCESS;
}