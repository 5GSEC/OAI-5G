#include "ran_param_elm_key_true.h"

#include <assert.h>
#include <stdlib.h>


void free_ran_param_elm_key_true(ran_param_elm_key_true_t * src)
{
  assert(src != NULL);
  
  //  9.3.14
  // ran_parameter_value_e
  free_ran_parameter_value(&src->value);

}

bool eq_ran_param_elm_key_true(ran_param_elm_key_true_t const* m0, ran_param_elm_key_true_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_ran_parameter_value(&m0->value, &m1->value) == false)
    return false;

  return true;
}


