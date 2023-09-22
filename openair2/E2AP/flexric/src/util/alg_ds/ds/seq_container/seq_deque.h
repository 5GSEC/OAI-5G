#ifndef SEQ_DEQUE
#define SEQ_DEQUE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct seq_deque_chunk
{
  void* current_it;
  void* end_it;
  struct seq_deque_chunk* next;
  
} seq_deque_chunk_t;


typedef struct seq_deque_s
{
  void* start_it;
  void* end_it;
  const uint32_t chunk_size;
  const size_t elt_size;
  size_t size;
} seq_deque_t;

typedef void (*seq_free_func)(void*); 

void seq_deque_init(seq_deque_t*, size_t);

void seq_deque_free(seq_deque_t*, seq_free_func);

void seq_deque_push_back(seq_deque_t* arr, uint8_t* data, size_t len);

void seq_deque_erase(seq_deque_t*, void*);

void seq_deque_swap(seq_deque_t*, void*, void*);

size_t seq_deque_size(seq_deque_t*);

void* seq_deque_front(seq_deque_t*);

void* seq_deque_next(seq_deque_t*, void*);

void* seq_deque_end(seq_deque_t*);

bool seq_deque_equal(void*,void*);

void* seq_deque_value(void*);


#endif
