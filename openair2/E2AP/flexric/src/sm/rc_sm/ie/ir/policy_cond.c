#include "policy_cond.h"

#include <assert.h>

void free_policy_cond(policy_cond_t* src)
{
  assert(src != NULL);

  // Policy Action Definition
  // Mandatory
  // 9.3.20
  free_policy_action(&src->pol_act);

  // Policy Condition Definition
  // Optional
  // 9.3.29
  assert(src->pol == NULL && "Not implemented");

}

bool eq_policy_cond(policy_cond_t const* m0, policy_cond_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false; 

  // Policy Action Definition
  // Mandatory
  // 9.3.20
  if(eq_policy_action(&m0->pol_act, &m1->pol_act) == false) 
    return false;

  // Policy Condition Definition
  // Optional
  // 9.3.29
  assert(m0->pol == NULL && "Not implmented"); 
  assert(m1->pol == NULL && "Not implmented"); 

  return true;
}

policy_cond_t cp_policy_cond(policy_cond_t const* src)
{
  assert(src != NULL);

  policy_cond_t dst = {0};

  // Policy Action Definition
  // Mandatory
  // 9.3.20
  dst.pol_act = cp_policy_action(&src->pol_act); 

  // Policy Condition Definition
  // Optional
  // 9.3.29
  assert(src->pol == NULL && "Not implemented");

  return dst;
}


