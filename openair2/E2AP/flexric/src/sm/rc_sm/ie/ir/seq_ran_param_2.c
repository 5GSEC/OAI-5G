#include "seq_ran_param_2.h"

#include <assert.h>
#include <stdlib.h>

void free_seq_ran_param_2(seq_ran_param_2_t* src)
{
  assert(src != NULL);
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  assert(src->ran_param_id > 0);
  // uint32_t ran_param_id;

  // RAN Parameter Value
  // Mandatory
  // 9.3.14
  free_ran_parameter_value(&src->ran_param_value);

}

bool eq_seq_ran_param_2(seq_ran_param_2_t const* m0, seq_ran_param_2_t const* m1)
{
  if(m0 == m1)
    return true;
 
  if(m0 == NULL || m1 == NULL)
    return false;

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  assert(m0->ran_param_id > 0);
  assert(m1->ran_param_id > 0);
  if(m0->ran_param_id != m1->ran_param_id)
    return false;

  // RAN Parameter Value
  // Mandatory
  // 9.3.14
  if(eq_ran_parameter_value(&m0->ran_param_value, &m1->ran_param_value) == false)
    return false;

  return true;
}

seq_ran_param_2_t cp_seq_ran_param_2( seq_ran_param_2_t const* src)
{
  assert(src != NULL);

  seq_ran_param_2_t dst = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  assert(src->ran_param_id > 0);
  dst.ran_param_id = src->ran_param_id;

  // RAN Parameter Value
  // Mandatory
  // 9.3.14
  dst.ran_param_value = cp_ran_parameter_value(&src->ran_param_value);

  return dst;
}

