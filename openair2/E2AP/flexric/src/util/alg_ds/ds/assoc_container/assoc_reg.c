

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "assoc_reg.h"

static
const size_t MIN_SIZE = 16;

static inline
size_t bucket_sz(assoc_reg_t* reg)
{
  return sizeof(registry_bucket_t) + sizeof( uint8_t[ reg->elm_sz] );
}

static inline
bool has_value_buckets(void* b_v)
{
  registry_bucket_t* b = (registry_bucket_t* )b_v; 
  return b->has_value;
}

static inline
bool iterator_in_ds(assoc_reg_t* reg, void* it)
{
  assert(reg != NULL);
  assert(it != NULL);

  assert(it >= reg->arr);

  const size_t b_size = bucket_sz(reg);
  size_t const idx = (it-reg->arr)/b_size;
  if( idx < reg->occ_bucket)
    return true;
  return false;
}

static
size_t copy_if(size_t elm_sz, void* it_start, size_t n_memb, void* it_output, bool (*fp)(void*))
{
  assert(it_start != NULL);
  assert(it_output != NULL);

  size_t i = 0;
  size_t i_out = 0;
  while(i < n_memb){
    if(fp(it_start + i*elm_sz) == true){
       memcpy(it_output + elm_sz*i_out, it_start+i*elm_sz, elm_sz);
       ++i_out;
    }
    ++i;
  }
  return i_out;
}

static 
size_t partition(size_t elm_sz, void* it_start, size_t n_memb, bool (*fp)(void*))
{
  assert(it_start != NULL);
  size_t i = 0;
  size_t j = 0;
  while(i < n_memb){
    if(fp(it_start + elm_sz*i)){
      memmove(it_start + elm_sz*j, it_start+elm_sz*i, elm_sz);
      ++j;
    }
    ++i;
  }
  return j; 
}

static
void shrink_if_necessary(assoc_reg_t* reg)
{
  if(reg->cap == MIN_SIZE || reg->sz*4 > reg->cap )
    return;

  const size_t b_size = bucket_sz(reg);
  const size_t idx = partition(b_size, reg->arr, reg->occ_bucket, has_value_buckets);
  assert(idx <= reg->occ_bucket);

  memset( reg->arr + b_size*idx , 0, (reg->cap - idx)* b_size);

  registry_bucket_t* tmp = calloc(reg->cap/2, b_size);
  assert(tmp != NULL);
  memcpy(tmp, reg->arr, idx*b_size);

  free(reg->arr);
  reg->arr = tmp;
  reg->occ_bucket = idx;
  reg->cap = reg->cap/2;
}

static
void compact_if_necessary(assoc_reg_t* reg)
{
  if(reg->sz * 2 > reg->occ_bucket )
    return;

  const size_t b_size = bucket_sz(reg);
  const size_t idx = partition(b_size, reg->arr, reg->occ_bucket, has_value_buckets);
  assert(idx <= reg->occ_bucket);

  memset(reg->arr + b_size*idx , 0, (reg->cap - idx)* b_size);

  reg->occ_bucket = idx;
}

static
void expand_if_neccesary(assoc_reg_t* reg)
{
  assert(reg != NULL);
  if(reg->occ_bucket < reg->cap - 1)
    return;

  const size_t b_size = bucket_sz(reg);
  registry_bucket_t* tmp_arr = calloc(reg->cap*2, b_size);
  assert(tmp_arr != NULL);
  
  const size_t out_idx = copy_if( b_size, reg->arr, reg->occ_bucket , tmp_arr, has_value_buckets);

  free(reg->arr);

  reg->arr = tmp_arr;
  reg->occ_bucket = out_idx;
  reg->cap = reg->cap*2;
}

void assoc_reg_init(assoc_reg_t* reg, size_t elm_sz)
{
  assert(reg != NULL);

  reg->sz = 0;
  reg->cap = MIN_SIZE; 
  reg->elm_sz = elm_sz;
  reg->occ_bucket = 0;
  reg->key_id = 1;


  // http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1548.pdf
  // From C11 draft, page 112
  // A structure or union shall not contain a member with incomplete 
  // or function type (hence,a structure shall not contain an instance of 
  // itself, but may contain a pointer to an instanceof  itself),  except  
  // that  the  last  member  of  a  structure  with  more  than  one  named  
  // member may  have  incomplete  array  type;  such  a  structure  (and  any  
  // union  containing,  possibly recursively, a member that is such a 
  // structure) shall not be a member of a structure or anelement of an array.
   
  //EXAMPLE 2 @ page 114 of C11 draft: After the declaration:
  //struct s { int n; double d[]; };
  //the structure struct s has a flexible array member d. A typical way to use 
  //this is:
  //int m = /*some value*/;
  //struct s *p = malloc(sizeof (struct s) + sizeof (double [m]));
  //and assuming that the call to malloc succeeds, the object pointed to 
  //by p behaves, for most purposes, as if p had been declared as:
  //struct { int n; double d[m]; } *p;
 
  const size_t b_size = bucket_sz(reg);
  reg->arr = calloc( MIN_SIZE, b_size); //  
  assert(reg->arr != NULL);
}

void assoc_reg_free(assoc_reg_t* reg)
{
  assert(reg != NULL);
  free(reg->arr);
}

uint32_t assoc_reg_push_back(assoc_reg_t* reg, void* value, size_t elm_sz)
{
  assert(reg != NULL);
  assert(elm_sz == reg->elm_sz);

  expand_if_neccesary(reg);

  const size_t b_size = bucket_sz(reg);
  registry_bucket_t* b = reg->arr + b_size* reg->occ_bucket;

  memcpy(b->val, value, elm_sz);
  const uint32_t key = reg->key_id;
  b->key = key;
  b->has_value = true;

  assert(reg->key_id != SIZE_MAX && "Overflow reached!");

  ++reg->key_id;
  ++reg->occ_bucket;
  ++reg->sz;

  return key;
}

// Get the key from an iterator 
void* assoc_reg_key(assoc_reg_t* reg, void* it)
{
  assert(reg != NULL);
  assert(it != NULL);
  assert(iterator_in_ds(reg, it) == true && "Iterator out of range");
  registry_bucket_t* b = (registry_bucket_t*)it;
  assert(b->has_value == true);
  return &b->key;
}

// Get the value pointer from an iterator
void* assoc_reg_value(assoc_reg_t* reg, void* it)
{
 assert(reg != NULL);
  assert(it != NULL);

  assert(iterator_in_ds(reg, it) == true && "Iterator out of range");
  registry_bucket_t* b = (registry_bucket_t*)it;

  assert(b->has_value == true);
  return b->val;
}

// Erase the elements in the range
void assoc_reg_erase(assoc_reg_t* reg, void* start_it, void* end_it, void(free_func)(void*))
{
  assert(reg != NULL);
  assert(start_it != NULL);
  assert(end_it != NULL);
  if(start_it == end_it) 
    return;

  assert(iterator_in_ds(reg, start_it) == true && "Iterator out of range");

  while(start_it != end_it) {
    registry_bucket_t* b = (registry_bucket_t*)start_it;
    start_it = assoc_reg_next(reg, start_it);
    b->has_value = false;
    if (free_func != NULL)
      free_func(&b->val);
    --reg->sz;
  }

  compact_if_necessary(reg);
  shrink_if_necessary(reg);
}


size_t assoc_reg_size(assoc_reg_t* reg)
{
  assert(reg != NULL);
  return reg->sz;
}

void* assoc_reg_front(assoc_reg_t* reg)
{
  assert(reg != NULL);
 
  if(reg->occ_bucket == 0)
    return reg->arr;

  const size_t b_size = bucket_sz(reg);
  size_t idx = 0;

  do {
    registry_bucket_t* b = reg->arr + b_size*idx;
    if(b->has_value == true)
      return b;

    idx += 1;
  } while(idx < reg->occ_bucket);

  assert(idx == reg->occ_bucket);
  return reg->arr + b_size* reg->occ_bucket;
}

void* assoc_reg_next(assoc_reg_t* reg, void* it)
{
  assert(reg != NULL);
  assert(it != NULL);

  const size_t b_size = bucket_sz(reg);

  registry_bucket_t* b = (registry_bucket_t*)it;
  assert(b->has_value == true);

  assert(iterator_in_ds(reg, it) == true && "Iterator out of range");

  void* end = assoc_reg_end(reg);
  size_t idx = 0;
  b = it + b_size; 
  while(b != end && b->has_value == false) {
    ++idx;
    b = it + idx*b_size; 
  }
  assert( b != end ? b->has_value == true : true );
  return b;
}

void* assoc_reg_end(assoc_reg_t* reg)
{
  assert(reg != NULL);

  const size_t b_size = bucket_sz(reg);
  return reg->arr + b_size*reg->occ_bucket;
}

void test_invariants_registry_hold(assoc_reg_t* reg)
{
  assert(reg != NULL);
  assert(reg->cap >= reg->occ_bucket);
  assert(reg->cap >= reg->sz);
 
  const size_t b_size = bucket_sz(reg);

  uint32_t counter = 0;
  int64_t last_key = -1;
  // Test monotonic order
  for(size_t i =0; i < reg->occ_bucket; ++i){
    registry_bucket_t* b = reg->arr + b_size*i;
    if(b->has_value)
      ++counter;
    assert(b->key > last_key);
    last_key = b->key;
  }
  assert(counter == reg->sz);
}

seq_arr_t remove_if_range_reg(assoc_reg_t* reg, bool (*f)(void* data))
{
  assert(reg != NULL);
  assert(f != NULL);

  seq_arr_t arr = {0}; 

  seq_init(&arr, sizeof(uint32_t));

  const size_t b_size = bucket_sz(reg);
  for(size_t i =0; i < reg->occ_bucket; ++i){
    registry_bucket_t* b = reg->arr + b_size*i;
    if(b->has_value && f(b->val) == true){
      static_assert(sizeof( b->key ) == sizeof(uint32_t), "Data size mismatch");
      seq_push_back(&arr, &b->key, sizeof( b->key ));
      --reg->sz;
      b->has_value = false; 
    }
  } 

  compact_if_necessary(reg);
  shrink_if_necessary(reg);
  return arr;
}

