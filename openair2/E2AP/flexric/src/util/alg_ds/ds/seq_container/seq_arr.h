#ifndef SEQ_CONTAINER_ARRAY
#define SEQ_CONTAINER_ARRAY

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct seq_arr_s{
    uint8_t* data; 
    size_t size;
    const size_t elt_size;
    size_t cap;
} seq_arr_t;

typedef void (*seq_free_func)(void*); 

void seq_arr_init(seq_arr_t*, size_t);

void seq_arr_free(seq_arr_t*, seq_free_func );

void seq_arr_push_back(seq_arr_t* arr, void* data, size_t len);

void seq_arr_erase(seq_arr_t*, void*, void*);

// void seq_arr_erase_free(seq_arr_t *, void *, void *, void (*free_func)(void
// *));

void seq_arr_swap(seq_arr_t*, void*, void*);

size_t seq_arr_size(seq_arr_t*);

void* seq_arr_front(seq_arr_t*);

void* seq_arr_next(seq_arr_t*, void*);

void* seq_arr_end(seq_arr_t*);

void* seq_arr_at(seq_arr_t*, uint32_t);

ptrdiff_t seq_arr_dist(seq_arr_t*, void*, void*);

bool seq_arr_equal(seq_arr_t*, void*,void*);

void* seq_arr_value(seq_arr_t*, void*);

#endif

