
#include "seq_ring_lck.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// For working correctly, maintain the default elements to a multiple of 2
#define DEFAULT_ELM 256

inline static
uint32_t mask(uint32_t cap, uint32_t val)
{
  return val & (cap-1);
}

size_t seq_ring_lck_size(seq_ring_lck_t* r)
{
  return r->head - r->tail; 
};

static 
bool full(seq_ring_lck_t* r)
{
  return (r->head - r->tail) == r->cap;
}

static
void enlarge_buffer(seq_ring_lck_t* r)
{
  assert(r != NULL);
  assert(full(r));

  const uint32_t factor = 2;
  uint8_t* tmp_buffer = calloc(r->cap * factor, r->elt_size);
  assert(tmp_buffer != NULL);

  const uint32_t head_pos = mask(r->cap, r->head);
  const uint32_t tail_pos = mask(r->cap, r->tail);

  if(head_pos > tail_pos){
    memcpy(tmp_buffer, r->array + tail_pos*r->elt_size , (head_pos-tail_pos)*r->elt_size );
  } else {
    memcpy(tmp_buffer, r->array + tail_pos*r->elt_size , (r->cap-tail_pos)*r->elt_size);
    memcpy(tmp_buffer + (r->cap-tail_pos)*r->elt_size, r->array, head_pos*r->elt_size);
  }
  r->cap *= factor;
  free(r->array);
  r->array = tmp_buffer;
}

void seq_ring_lck_init(seq_ring_lck_t* r, size_t elt_size)
{
  assert(r != NULL);
  uint8_t* tmp_buffer = calloc(DEFAULT_ELM, elt_size); 
  assert(tmp_buffer != NULL);
  seq_ring_lck_t tmp = {.elt_size = elt_size, .array = tmp_buffer, .head = 0, .tail = 0, .cap = DEFAULT_ELM};
  memcpy(r, &tmp, sizeof(seq_ring_lck_t));
  
  int const rc = pthread_mutex_init(&r->mtx, NULL); 
  assert(rc == 0);
}

void seq_ring_lck_free(seq_ring_lck_t* r, seq_free_func fp)
{
  assert(r != NULL);

  if(fp != NULL){
    void* start_it = seq_ring_lck_front(r);
    void* end_it = seq_ring_lck_end(r);
    while(start_it != end_it){
      fp(start_it);
      start_it = seq_ring_lck_next(r, start_it);
    }
  }

  free(r->array);
  const int rc = pthread_mutex_destroy(&r->mtx);
  assert(rc == 0);
}

void seq_ring_lck_push_back(seq_ring_lck_t* r, uint8_t* data, size_t len)
{
  assert(r != NULL);
  assert(len == r->elt_size);

  pthread_mutex_lock(&r->mtx); 

  if(full(r))
    enlarge_buffer(r);
  
  const uint32_t pos = mask(r->cap, r->head);
  memcpy(r->array + r->elt_size*pos, data, r->elt_size);
  r->head += 1;

  pthread_mutex_unlock(&r->mtx); 
};

void seq_ring_lck_erase(seq_ring_lck_t* r, void* it_start, void* it_end)
{

  pthread_mutex_lock(&r->mtx); 

  assert(it_start == r->array + mask(r->cap,r->tail)*r->elt_size && "Only deletions from tail implemented");
  if(it_start == it_end){
    printf("Start iterator equal to end iterator\n" );

    pthread_mutex_unlock(&r->mtx); 
    return; // closed semi-open range [ ) 
  }
  const uint32_t start_pos = ((uint8_t*)it_start - r->array)/r->elt_size; 
  const uint32_t end_pos = ((uint8_t*)it_end - r->array)/r->elt_size; 
  assert(end_pos <= r->cap);
  assert(end_pos != start_pos);

  if(end_pos > start_pos){
    memset(it_start, 0, (end_pos - start_pos)*r->elt_size);
    r->tail += end_pos - start_pos;
  } else {
    assert(end_pos < start_pos); 
    memset(it_start, 0, (r->cap - start_pos)*r->elt_size);
    memset(r->array, 0, end_pos*r->elt_size);
    r->tail +=  r->cap - start_pos + end_pos ;
  }
  assert(mask(r->cap, r->tail) == end_pos);

  pthread_mutex_unlock(&r->mtx); 
}

void seq_ring_swap(seq_ring_lck_t* cb, void* it_1, void* it_2)
{
  assert(0!=0 && "Not implemented");
};

void* seq_ring_front(seq_ring_lck_t* r)
{
  pthread_mutex_lock(&r->mtx); 

  void * it = r->array + mask(r->cap, r->tail)*r->elt_size;

  pthread_mutex_unlock(&r->mtx); 

  return it;
};

void* seq_ring_next(seq_ring_lck_t* r, void* it)
{

  pthread_mutex_lock(&r->mtx); 
  assert(r != NULL);
  assert(it != NULL);
  const uint32_t pos = ((uint8_t*)it - r->array)/r->elt_size;

  if(pos + 1 == r->cap){ 
    void* it_ret = r->array;
    pthread_mutex_unlock(&r->mtx); 
    return it_ret;
  }

  void* it_ret = it + r->elt_size;
  pthread_mutex_unlock(&r->mtx); 
  return it_ret;
}

void* seq_ring_end(seq_ring_lck_t* r)
{
  pthread_mutex_lock(&r->mtx); 
  void* it = r->array + mask(r->cap, r->head)*r->elt_size;
  pthread_mutex_unlock(&r->mtx); 
  return it;
}

void* seq_ring_at(seq_ring_lck_t* r, uint32_t pos)
{
  pthread_mutex_lock(&r->mtx); 
  void* it = r->array + mask(r->cap, r->tail + pos)*r->elt_size;
  pthread_mutex_unlock(&r->mtx); 
  return it;
}

int32_t seq_ring_dist(seq_ring_lck_t* r, void* first, void* last)
{
  pthread_mutex_lock(&r->mtx); 
  // Precondition: random access iterators belonging to a sequence.
  assert(r != NULL);
  assert(first != NULL);
  assert(last != NULL);

  const uint32_t last_pos = ((uint8_t*)last - r->array) / r->elt_size; 
  const uint32_t first_pos = ((uint8_t*)first - r->array) / r->elt_size; 
  if(last_pos > first_pos){
    int32_t const pos = last_pos - first_pos; 
    pthread_mutex_unlock(&r->mtx); 
    return pos; 
  }
  int32_t const pos = (r->cap - first_pos) + last_pos;

  pthread_mutex_unlock(&r->mtx); 
  return pos; 
}

bool seq_ring_lck_equal(void* it_1,void* it_2)
{
  return (uint8_t*)it_1 == (uint8_t*)it_2;  
}


