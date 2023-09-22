#include "ran_param_insertion.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_ins(ran_param_ins_t* src)
{
  assert(src != NULL);


  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  assert(src->ran_id > 0); 
  // uint32_t ran_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(src->def == NULL && "Not implemented");

}

bool eq_ran_param_ins(ran_param_ins_t const* m0, ran_param_ins_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  if(m0->ran_id != m1->ran_id)
    return false;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(m0->def == NULL && "Not implemented");
  assert(m1->def == NULL && "Not implemented");

  return true;
}

ran_param_ins_t cp_ran_param_ins(ran_param_ins_t const* src)
{
  assert(src != NULL);
  ran_param_ins_t dst = {0}; 
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  assert(src->ran_id > 0);
  dst.ran_id = src->ran_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(src->def == NULL && "Not implemented"); 

  return dst;
}

