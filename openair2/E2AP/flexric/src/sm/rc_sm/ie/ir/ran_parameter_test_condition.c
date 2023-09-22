#include "ran_parameter_test_condition.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_test_cond(ran_param_test_cond_t* src)
{
  assert(src != NULL);

// ran_param_test_cond_e cond;
//  union{
//    ran_param_test_cond_comp_e comp;
//    ran_param_test_cond_presence_e presence; 
//  };

}

bool eq_ran_param_test_cond(ran_param_test_cond_t const* m0, ran_param_test_cond_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->cond != m1->cond)
    return false;

  if(m0->cond == COMPARISON_RAN_PARAM_TEST_COND){
    if(m0->comp != m1->comp)
      return false;
  }else if(m0->cond == PRESENCE_RAN_PARAM_TEST_COND){
    if(m0->presence != m1->presence)
      return false;

  } else {
    assert(0!=0 && "Unknown type");
  }

  return true;
}

ran_param_test_cond_t cp_ran_param_test_cond(ran_param_test_cond_t const src)
{
  ran_param_test_cond_t dst = {.cond = src.cond }; 

  if(dst.cond == COMPARISON_RAN_PARAM_TEST_COND){
    dst.comp = src.comp;
  } else if(dst.cond == PRESENCE_RAN_PARAM_TEST_COND){
    dst.presence = src.presence;
  } else {
    assert(0!=0 && "Unknwon cond");
  }

  return dst;
}

