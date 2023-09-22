#ifndef SEQ_CONTAINER_LIST
#define SEQ_CONTAINER_LIST

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct lst_node_s{
  struct lst_node_s* next;
  void* data;
} lst_node_t;

typedef struct seq_list_s{
   lst_node_t* root;
   size_t size;
} seq_list_t;

typedef void (*seq_free_func)(void*); 

void seq_list_init(seq_list_t*);

void seq_list_free(seq_list_t*, seq_free_func);

void seq_list_push_back(seq_list_t*, void*);

void seq_list_erase(seq_list_t*, void*);

void* seq_list_front(seq_list_t*);

void* seq_list_front_const(const seq_list_t*);
 
void* seq_list_next(seq_list_t*, void*);

void* seq_list_next_const(const seq_list_t*, void*);

void* seq_list_end(seq_list_t*);

void* seq_list_end_const(const seq_list_t*);

void* seq_list_at(seq_list_t*, uint32_t);

int32_t seq_list_dist(seq_list_t* ,void*, void*);

bool seq_list_equal(void* a, void* b);

void* seq_list_value(void* a);

size_t seq_list_size(seq_list_t*);

#endif

