#include "ran_param_elm_key_false.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_elm_key_false(ran_param_elm_key_false_t* src)
{
  assert(src != NULL);

  // RAN Parameter Test Condition
  // 9.3.31
  // Mandatory
  free_ran_param_test_cond(&src->test_cond);

  // RAN Parameter Value
  // Optional
  // 9.3.14
  assert(src->value== NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(src->logical_or == NULL && "Not implemented");

}

bool eq_ran_param_elm_key_false(ran_param_elm_key_false_t const* m0, ran_param_elm_key_false_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RAN Parameter Test Condition
  // 9.3.31
  // Mandatory
  if(eq_ran_param_test_cond(&m0->test_cond, &m1->test_cond) == false) 
    return false;

  // RAN Parameter Value
  // Optional
  // 9.3.14
  assert(m0->value == NULL && "Not implemented");
  assert(m1->value == NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(m0->logical_or == NULL && "Not implemented");
  assert(m1->logical_or == NULL && "Not implemented");

  return true;
}

ran_param_elm_key_false_t cp_ran_param_elm_key_false( ran_param_elm_key_false_t const* src)
{
  assert(src != NULL);
  ran_param_elm_key_false_t dst = {0}; 

  // RAN Parameter Test Condition
  // 9.3.31
  // Mandatory
  dst.test_cond = cp_ran_param_test_cond(src->test_cond);

  // RAN Parameter Value
  // Optional
  // 9.3.14
  assert(src->value == NULL && "Not implemented"); 

  // Logical OR
  // Optional
  // 9.3.25
  assert(src->logical_or == NULL && "Not implemented");

  return dst;
}


