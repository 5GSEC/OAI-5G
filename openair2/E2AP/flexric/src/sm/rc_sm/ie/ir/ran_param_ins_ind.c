#include "ran_param_ins_ind.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_ins_ind(ran_param_ins_ind_t const* src)
{
  assert(src != NULL);

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1.. 429496729 ]
  // uint32_t ran_param_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(src->ran_param_def == NULL && "Not implemented"); 

}

bool eq_ran_param_ins_ind(ran_param_ins_ind_t const* m0, ran_param_ins_ind_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1.. 429496729 ]
  assert(m0->ran_param_id > 0);
  assert(m1->ran_param_id > 0);
  if(m0->ran_param_id != m1->ran_param_id)
    return false;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(m0->ran_param_def == NULL && "Not implemented"); 
  assert(m1->ran_param_def == NULL && "Not implemented"); 

  return true;
}

ran_param_ins_ind_t cp_ran_param_ins_ind(ran_param_ins_ind_t const* src)
{
  assert(src != NULL);
  ran_param_ins_ind_t dst = {0}; 
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1.. 429496729 ]
  assert(src->ran_param_id > 0);
  dst.ran_param_id = src->ran_param_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(src->ran_param_def == NULL && "Not implemented");

  return dst;
}

