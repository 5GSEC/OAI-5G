#include "seq_ctrl_act_2.h"

#include <assert.h>

void free_seq_ctrl_act_2(seq_ctrl_act_2_t* src)
{
  assert(src != NULL);

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1-65535]
  assert(src->id > 0);

  // Control Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // Sequence of Associated RAN Parameters
  // [0-65535]
  assert(src-> sz_seq_assoc_ran_param < 65536);

  for(size_t i = 0; i < src-> sz_seq_assoc_ran_param; ++i){
    free_seq_ran_param_3(&src->assoc_ran_param[i]); 
  }
  if(src->assoc_ran_param != NULL)
    free(src->assoc_ran_param);
}

bool eq_seq_ctrl_act_2( seq_ctrl_act_2_t const* m0, seq_ctrl_act_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1-65535]
  assert(m0->id > 0);
  assert(m1->id > 0);
  if(m0->id != m1->id) 
    return false;

  // Control Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Sequence of Associated RAN Parameters
  // [0-65535]
  assert(m0->sz_seq_assoc_ran_param < 65536);
  assert(m1->sz_seq_assoc_ran_param < 65536);
 
  if(m0->sz_seq_assoc_ran_param != m1->sz_seq_assoc_ran_param)
    return false;

  for(size_t i = 0; i < m0->sz_seq_assoc_ran_param; ++i){
    if(eq_seq_ran_param_3(&m0->assoc_ran_param[i], &m1->assoc_ran_param[i]) == false)
      return false;
  }

  return true;
}

seq_ctrl_act_2_t cp_seq_ctrl_act_2(seq_ctrl_act_2_t const* src)
{
  assert(src != NULL);

  seq_ctrl_act_2_t dst = {0}; 
 
  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1-65535]
  assert(src->id > 0);
  dst.id = src->id;

  // Control Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151); 
  dst.name = copy_byte_array(src->name);

  // Sequence of Associated RAN Parameters
  // [0-65535]
  assert(src->sz_seq_assoc_ran_param < 65536);
  if(src->sz_seq_assoc_ran_param > 0){
    dst.sz_seq_assoc_ran_param = src->sz_seq_assoc_ran_param;
    dst.assoc_ran_param = calloc(src->sz_seq_assoc_ran_param, sizeof(seq_ran_param_3_t) );
    assert(dst.assoc_ran_param != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_assoc_ran_param; ++i){
    dst.assoc_ran_param[i] = cp_seq_ran_param_3(&src->assoc_ran_param[i]);
  }

  return dst;
}

