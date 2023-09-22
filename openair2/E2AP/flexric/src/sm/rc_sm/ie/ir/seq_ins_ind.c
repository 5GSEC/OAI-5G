#include "seq_ins_ind.h"

#include <assert.h>
#include <stdlib.h>

void free_seq_ins_ind(seq_ins_ind_t* src)
{
  assert(src != NULL);

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1-65535]
  assert(src->id > 0); 
  // uint16_t id;

  // Insert Indication Name
  // Mandatory
  // 9.3.17
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // Sequence of Insert Indications
  // [0-65535]
  assert(src->sz_seq_ins_ind < 65536);
  for(size_t i = 0; i < src->sz_seq_ins_ind; ++i){
    free_seq_ran_param_3(&src->seq_ins_ind[i]);
  }
   
  if(src->sz_seq_ins_ind > 0)
    free(src->seq_ins_ind);
}

bool eq_seq_ins_ind(seq_ins_ind_t const* m0, seq_ins_ind_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1-65535]
  assert(m0->id > 0);
  assert(m1->id > 0);
  if(m0->id != m1->id)
    return false;

  // Insert Indication Name
  // Mandatory
  // 9.3.17
  // [1-150]
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Sequence of Insert Indications
  // [0-65535]
  assert(m0->sz_seq_ins_ind < 65536); 
  assert(m1->sz_seq_ins_ind < 65536); 
  
  if(m0->sz_seq_ins_ind != m1->sz_seq_ins_ind)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ins_ind; ++i){
    if(eq_seq_ran_param_3(&m0->seq_ins_ind[i], &m1->seq_ins_ind[i]) == false)
      return false;
  }

  return true;
}

seq_ins_ind_t cp_seq_ins_ind(seq_ins_ind_t const* src)
{
  assert(src != NULL);
  seq_ins_ind_t dst = {0};

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1-65535]
  assert(src ->id > 0);
  dst.id = src->id;

  // Insert Indication Name
  // Mandatory
  // 9.3.17
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);

  // Sequence of Insert Indications
  // [0-65535]
  assert(src->sz_seq_ins_ind < 65536);
  if(src->sz_seq_ins_ind > 0){
    dst.sz_seq_ins_ind = src->sz_seq_ins_ind;
    dst.seq_ins_ind = calloc(dst.sz_seq_ins_ind, sizeof(seq_ran_param_3_t));
    assert(dst.seq_ins_ind != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_ins_ind; ++i){
    dst.seq_ins_ind[i] = cp_seq_ran_param_3(&src->seq_ins_ind[i]);
  }

  return dst;
}

