#include "lower_bound.h"

#include <assert.h>
#include <stdio.h>

static inline
size_t bucket_sz(assoc_reg_t* reg)
{
  return sizeof(registry_bucket_t) + sizeof( uint8_t[ reg->elm_sz] );
}


void* lower_bound_reg(assoc_reg_t* reg, void* start_it, void* end_it, uint32_t key)
{
  assert(reg != NULL);
  assert(start_it != NULL);
  assert(end_it != NULL);

  if(start_it == end_it)
    return end_it;

  size_t const b_size = bucket_sz(reg);  
  size_t const sz = (end_it - start_it)/b_size; 

  assert(sz < reg->cap && "iterator out of the range"); 

  size_t lower = 0;
  int len =  sz;
  size_t idx = 0;
  while(len > 0){
    int half = len/2;
    idx = lower + half;
    registry_bucket_t* b = start_it + idx*b_size;
    if(b->key < key){
      lower = idx+1;
      len = len - half -1;
    } else {
      len = half;
    }
  }

  return start_it + lower*b_size; 
}


