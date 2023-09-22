#include "seq_ran_param.h"

#include <assert.h>
#include <stdlib.h>

void free_seq_ran_param(seq_ran_param_t* src)
{
  assert(src != NULL);

  //RAN Parameter ID
  //Mandatory
  //9.3.8
  // [1 - 4294967295]
  // uint32_t ran_param_id;
  assert(src->ran_param_id > 0); 

  // RAN Parameter Value Type
  // 9.3.11
  // Mandatory
  free_ran_param_val_type(&src->ran_param_val);
  
}

bool eq_seq_ran_param(seq_ran_param_t const* m0, seq_ran_param_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->ran_param_id != m1->ran_param_id)
    return false;

  if(eq_ran_param_val_type(&m0->ran_param_val, &m1->ran_param_val) == false)
    return false;

  return true;
}

seq_ran_param_t cp_seq_ran_param(seq_ran_param_t const* src)
{
  assert(src != NULL);

  seq_ran_param_t dst = {0}; 


  //RAN Parameter ID
  //Mandatory
  //9.3.8
  // [1 - 4294967295]
  assert(src->ran_param_id > 0);
  dst.ran_param_id = src->ran_param_id;

  // RAN Parameter Value Type
  // 9.3.11
  // Mandatory
  dst.ran_param_val = cp_ran_param_val_type(&src->ran_param_val);

  return dst;
}


