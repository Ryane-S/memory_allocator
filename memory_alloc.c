#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "cmocka.h"
#include "memory_alloc.h"


/* Starting from b, skip inc blocks of size block_size
 * retrun the address of the resulting block
 */
static struct memory_block* memalloc_get_address(struct memory_block* b, int inc, size_t block_size) {
  uintptr_t base = (uintptr_t) b;
  uintptr_t result = base + (inc * block_size);
  return (struct memory_block*) result;
}

/* compute the address of index th block of the allocator */
static struct memory_block* memalloc_get(struct memory_alloc* m, int index) {
  assert(index >= 0 && index < m->nb_prealloc_blocks);
  return memalloc_get_address(m->prealloc_blocks, index, m->block_size);
}

/* compute the address of next block */
static struct memory_block* memalloc_get_next(struct memory_alloc* m, struct memory_block* current) {
  return memalloc_get_address(current, 1, m->block_size);
}

/* Initialize the memory_alloc structure */
void memalloc_init(struct memory_alloc* m, int nb_blocks, size_t block_size) {
  /* Check that the given parameters are correct */
  assert(m);
  assert(nb_blocks > 0);
  assert(block_size > 0);

  /* Initialize the memory allocator fields */
  m->nb_prealloc_blocks = (size_t)nb_blocks; // Number of pre-allocated blocks
  m->block_size = block_size; // Size of one block
  m->prealloc_blocks = malloc(m->block_size*m->nb_prealloc_blocks); // Blocks that can be allocated
  assert(m->prealloc_blocks); // Check is malloc succeeded
  m->available_blocks = (size_t)nb_blocks; // Number of blocks that are available
  m->first_block = memalloc_get(m,0); // First available block

  /* Build the linked list of available blocks */
  struct memory_block* current = m->first_block;
  for (int i=1; i<m->nb_prealloc_blocks; i++){
    current->next = memalloc_get(m,i);
    current = current->next;
  }
  current->next = NULL; 

  m->errno = E_SUCCESS; // The initialization was successful
}

/* Finalize the memory_alloc structure, and free all the memory that was malloc'd */
void memalloc_finalize(struct memory_alloc* m) {
  /* Check that the given parameter is correct */
  assert(m);

  /* Reset the memory allocator fields */
  m->nb_prealloc_blocks = 0; // Set the number of pre-allocated blocks to 0
  m->block_size = 0; // Set the size of one block to 0;
  free(m->prealloc_blocks); // Free the memory that was malloc'd
  m->prealloc_blocks = NULL;
  m->available_blocks = 0; // Set the number of available blocks to 0
  m->first_block = NULL;

  m->errno = E_SUCCESS; // The finalization was successful
}

/* Return the number of consecutive blocks starting from first */
int memalloc_nb_consecutive_blocks(struct memory_alloc* m, struct memory_block* first) {
  /* Check that the given parameters are correct */
  assert(m);
  assert(first);

  /* Count the number of free consecutive blocks starting from first */
  int nb_consecutive_blocks = 0;
  struct memory_block* current = first;
  while(current != NULL){
    nb_consecutive_blocks ++;
    if (current->next != memalloc_get_next(m, current)){
      break; // If the next block in the linked list is NULL_BLOCK or not consecutive to the current block in prealloc_blocks, break the loop
    }
    current = current->next;
  }

  return nb_consecutive_blocks;
}

void memalloc_reorder(struct memory_alloc* m) {
  /* Not yet implemented */
}


/* Initialize an allocated buffer with zeros */
static void initialize_buffer(struct memory_block* block,
			      size_t size) {
  char* ptr = (char*)block;
  for(int i=0; i<size; i++) {
    ptr[i]=0;
  }
}

void* memalloc_allocate(struct memory_alloc* m, size_t size) {
  /* Not yet implemented */

  return NULL;
}

void memalloc_free(struct memory_alloc* m, void* address, size_t size) {
  /* Not yet implemented */
}

void memalloc_print(struct memory_alloc* m) {
  printf("---------------------------------\n");
  printf("Block size: %ld\n", m->block_size);
  printf("Available_blocks: %ld\n", m->available_blocks);
  printf("First free: %p\n", m->first_block);
  printf("Status: ");
  memalloc_error_print(m->errno);
  printf("Content : ");
  struct memory_block* current = m->first_block;
  for (int i=0; i<m->available_blocks; i++){
    printf("[%p] -> ", current);
    current = current->next;
  }
  printf("NULL\n");
  printf("---------------------------------\n");
}
/*
Block size: 16
Available blocks: 8
First free: 0x558c609a52a0
Status: Success
Content:  [0x558c609a52a0] -> [0x558c609a52b0] -> [0x558c609a52c0] -> [0x558c609a52d0] -> [0x558c609a52e0] -> [0x558c609a52f0] -> [0x558c609a5300] -> [0x558c609a5310] -> NULL
*/


void memalloc_error_print(enum memory_errno error_number) {
  switch(error_number) {
  case E_SUCCESS:
    printf("Success\n");
    break;
  case E_NOMEM:
    printf("Not enough memory\n");
    break;
  case  E_SHOULD_PACK:
    printf("Not enough contiguous blocks\n");
    break;
  default:
    printf("Unknown\n");
    break;
  }
}

void* memalloc_lifelike_malloc(struct memory_alloc *m, size_t size) {
  /* Not yet implemented */
  return NULL;
}

void memalloc_lifelike_free(struct memory_alloc *m, void* addr) {
  /* Not yet implemented */
}

void* memalloc_lifelike_realloc(struct memory_alloc *m, void* addr, size_t size){
  /* Not yet implemented */
  return NULL;
}
