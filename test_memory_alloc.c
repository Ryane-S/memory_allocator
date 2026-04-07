#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <setjmp.h>
#include "cmocka.h"
#include "memory_alloc.h"

/* Test memory_init() */
void test_memory_init(void** state){
  size_t block_size = 8;
  int nb_blocks = 100;

  struct memory_alloc m;
  memalloc_init(&m, nb_blocks, block_size);

  /* Check that the fields of the memory allocator structure are correctly initialized */
  assert_int_equal(m.nb_prealloc_blocks, nb_blocks);
  assert_int_equal(m.block_size, block_size);
  assert_int_equal(m.available_blocks, nb_blocks);
  assert_ptr_equal(m.first_block, &m.prealloc_blocks[0]); // First block is at the beginning of prealloc_blocks
  assert_int_equal(m.errno, E_SUCCESS);

  /* Check that the linked list is correctly initialized */
  struct memory_block* current = m.first_block;
  for (int i=1; i<m.nb_prealloc_blocks; i++){
    assert_non_null(current->next);
    current = current->next;
  }
  assert_null(current->next); // The last block points to NULL_BLOCK

  /* Free the memory allocator */
  memalloc_finalize(&m); 
  assert_int_equal(m.nb_prealloc_blocks, 0);
  assert_int_equal(m.block_size, 0);
  assert_int_equal(m.available_blocks, 0);
  assert_ptr_equal(m.prealloc_blocks, NULL);
  assert_ptr_equal(m.first_block, NULL);
  assert_int_equal(m.errno, E_SUCCESS);
}

/* Test memalloc_nb_consecutive_blocks() */
void test_memory_nb_consecutive_blocks(void** state){
  size_t block_size = 8;
  int nb_blocks = 100;

  struct memory_alloc m;
  memalloc_init(&m, nb_blocks, block_size);
  
  /* Check that the fields of the memory allocator structure are correctly initialized */
  assert_int_equal(m.nb_prealloc_blocks, nb_blocks);
  assert_int_equal(m.block_size, block_size);
  assert_int_equal(m.available_blocks, nb_blocks);
  assert_ptr_equal(m.first_block, &m.prealloc_blocks[0]);
  assert_int_equal(m.errno, E_SUCCESS);

  /* Right afterr initialization */
  struct memory_block* current = m.first_block;
  for (int i=0; i<m.nb_prealloc_blocks; i++){
    assert_int_equal(memalloc_nb_consecutive_blocks(&m, current), m.nb_prealloc_blocks);
  }

  /* Free the memory allocator */
  memalloc_finalize(&m);
  assert_int_equal(m.nb_prealloc_blocks, 0);
  assert_int_equal(m.block_size, 0);
  assert_int_equal(m.available_blocks, 0);
  assert_ptr_equal(m.prealloc_blocks, NULL);
  assert_ptr_equal(m.first_block, NULL);
  assert_int_equal(m.errno, E_SUCCESS);
}

void test_find_index_memory_block(void** state){
  size_t block_size = 64;
  int nb_blocks = 10;
  void* pointers[nb_blocks];

  struct memory_alloc m;
  memalloc_init(&m, nb_blocks, block_size);

  /* Check that the fields of the memory allocator structure are correctly initialized */
  assert_int_equal(m.nb_prealloc_blocks, nb_blocks);
  assert_int_equal(m.block_size, block_size);
  assert_int_equal(m.available_blocks, nb_blocks);
  assert_ptr_equal(m.first_block, &m.prealloc_blocks[0]);
  assert_int_equal(m.errno, E_SUCCESS);

  /* Find index of the block containing the given address*/
  struct memory_block* current = m.first_block;
  for (int i=0; i<m.nb_prealloc_blocks-1; i++){
    assert_int_equal(find_index_memory_block(&m, (void*)current), i);
    current = current->next;
  }
  assert_int_equal(find_index_memory_block(&m, (void*)current), m.nb_prealloc_blocks-1);

  /* Free the memory allocator */
  memalloc_finalize(&m);
  assert_int_equal(m.nb_prealloc_blocks, 0);
  assert_int_equal(m.block_size, 0);
  assert_int_equal(m.available_blocks, 0);
  assert_ptr_equal(m.prealloc_blocks, NULL);
  assert_ptr_equal(m.first_block, NULL);
  assert_int_equal(m.errno, E_SUCCESS);
}

/* Test memory_alloc() */
void test_memory_alloc(void** state){
  size_t block_size = 64;
  int nb_blocks = 10;
  void* pointers[nb_blocks];

  struct memory_alloc m;
  memalloc_init(&m, nb_blocks, block_size);

  /* Check that the fields of the memory allocator structure are correctly initialized */
  assert_int_equal(m.nb_prealloc_blocks, nb_blocks);
  assert_int_equal(m.block_size, block_size);
  assert_int_equal(m.available_blocks, nb_blocks);
  assert_ptr_equal(m.first_block, &m.prealloc_blocks[0]);
  assert_int_equal(m.errno, E_SUCCESS);

  /* Allocating too much memory should fail */
  pointers[0] = memalloc_allocate(&m, block_size * nb_blocks + 1);
  assert_null(pointers[0]);
  assert_int_equal(m.errno, E_NOMEM);

  /* Allocate a few buffers */
  for(int i = 0; i<nb_blocks; i++) {
    pointers[i] = memalloc_allocate(&m, block_size);
    assert_non_null(pointers[i]);
    assert_int_equal(m.available_blocks, nb_blocks - (i+1));
    assert_int_equal(m.errno, E_SUCCESS);

    /* Write something to the buffer */
    memset(pointers[i], 7, block_size);
  }

  /* Free the allocated buffers */
  for(int i = 0; i<nb_blocks; i++) {
    memalloc_free(&m, pointers[nb_blocks-(i+1)], block_size);
    assert_int_equal(m.available_blocks, i+1);
    assert_int_equal(m.errno, E_SUCCESS);
  }

  /* Free the memory allocator */
  memalloc_finalize(&m); 
  assert_int_equal(m.nb_prealloc_blocks, 0);
  assert_int_equal(m.block_size, 0);
  assert_int_equal(m.available_blocks, 0);
  assert_ptr_equal(m.prealloc_blocks, NULL);
  assert_ptr_equal(m.first_block, NULL);
  assert_int_equal(m.errno, E_SUCCESS);
}

int main(int argc, char**argv) {
  const struct CMUnitTest tests[] = {
    /* a few tests for memory allocator.
     *
     * If you implemented correctly the functions, all these tests should be successfull
     * Of course this test suite may not cover all the tricky cases, and you should add
     * your own tests.
     */
    cmocka_unit_test(test_memory_init),
    cmocka_unit_test(test_memory_nb_consecutive_blocks),
    cmocka_unit_test(test_find_index_memory_block),
    cmocka_unit_test(test_memory_alloc)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}