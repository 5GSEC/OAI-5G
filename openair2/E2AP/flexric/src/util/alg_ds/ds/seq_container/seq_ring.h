#ifndef SEQ_CIRCULAR_BUFFER_H
#define SEQ_CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct seq_ring_buf_s
{
  const size_t elt_size;
  uint8_t* array;

  size_t cap;
  uint32_t head;
  uint32_t tail;
} seq_ring_t;

typedef void (*seq_free_func)(void*); 

void seq_ring_init(seq_ring_t*, size_t);

void seq_ring_free(seq_ring_t*, seq_free_func);

void seq_ring_push_back(seq_ring_t* arr, uint8_t* data, size_t len);

void seq_ring_erase(seq_ring_t*, void*, void*);

void seq_ring_swap(seq_ring_t*, void*, void*);

size_t seq_ring_size(seq_ring_t*);

void* seq_ring_front(seq_ring_t*);

void* seq_ring_next(seq_ring_t*, void*);

void* seq_ring_end(seq_ring_t*);

void* seq_ring_at(seq_ring_t* , uint32_t );

int32_t seq_ring_dist(seq_ring_t*, void*, void*);

bool seq_ring_equal(void*,void*);

#endif

