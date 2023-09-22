#include "ran_param_req.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_req(ran_param_req_t* src)
{
  assert(src != NULL);

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // 1 4294967295,
  // uint32_t ran_param_id;

  // RAN Parameter Value Type
  // Mandatory
  // 9.3.11
 free_ran_param_val_type(&src->ran_param);

}

bool eq_ran_param_req( ran_param_req_t const* m0,  ran_param_req_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // 1 4294967295,
  assert(m0->ran_param_id > 0);
  assert(m1->ran_param_id > 0);

  if(m0->ran_param_id != m1->ran_param_id)
    return false;

  // RAN Parameter Value Type
  // Mandatory
  // 9.3.11
  if(eq_ran_param_val_type(&m0->ran_param, &m1->ran_param) == false) 
    return false;

  return true;
}

ran_param_req_t cp_ran_param_req( ran_param_req_t const* src )
{
  assert(src != NULL);
  ran_param_req_t dst = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // 1 4294967295,
  assert(src->ran_param_id > 0);
  dst.ran_param_id = src->ran_param_id;

  // RAN Parameter Value Type
  // Mandatory
  // 9.3.11
  dst.ran_param = cp_ran_param_val_type(&src->ran_param);

  return dst;
}

