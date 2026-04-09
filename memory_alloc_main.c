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
    size_t alloc_size = (nb_blocks * block_size) / 3; // 42

    printf("Initialization :\n");
    memalloc_init(&m, nb_blocks, block_size);
    memalloc_print(&m);
  
    printf("Allocation :\n");
    char* buffer = memalloc_allocate(&m, 64); // 4 blocks allocated
    memalloc_print(&m);

    for(int i=0; i<alloc_size; i++)
        buffer[i]='a';

    buffer[alloc_size-1] = '\0';
    printf("%s\n", buffer);

    printf("Free :\n"); // Free 3 blocks
    memalloc_free(&m, buffer, alloc_size);
    memalloc_print(&m);
    memalloc_finalize(&m);

    /* Lifelike functions */
    memalloc_init(&m, nb_blocks, block_size);

    printf("Lifelike Allocation :\n");
    char* lifelike_buffer = memalloc_lifelike_malloc(&m, alloc_size); // 42 -> 3 blocks + 1 header
    assert(lifelike_buffer);
    memalloc_print(&m);

    for(int i=0; i<alloc_size-1; i++)
        lifelike_buffer[i]='x';

    lifelike_buffer[alloc_size-1] = '\0';
    printf("%s\n", lifelike_buffer);

    printf("Lifelike Reallocation :\n");
    size_t new_size = alloc_size;
    lifelike_buffer = memalloc_lifelike_realloc(&m, lifelike_buffer, new_size);
    memalloc_print(&m);
    assert(lifelike_buffer);

    for(int i=alloc_size; i<new_size-1; i++)
        lifelike_buffer[i]='y';

    lifelike_buffer[new_size-1] = '\0';
    printf("%s\n", lifelike_buffer);

    printf("Lifelike Free :\n");
    memalloc_lifelike_free(&m, lifelike_buffer);
    memalloc_print(&m);

    printf("After reorder :\n");
    memalloc_reorder(&m);
    memalloc_print(&m);
    memalloc_finalize(&m);

    return EXIT_SUCCESS;
}