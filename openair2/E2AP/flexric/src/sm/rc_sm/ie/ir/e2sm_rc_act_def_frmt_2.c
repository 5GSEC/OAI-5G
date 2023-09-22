#include "e2sm_rc_act_def_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_act_def_frmt_2(e2sm_rc_act_def_frmt_2_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz_policy_cond; ++i){
    free_policy_cond(&src->policy_cond[i]); 
  }
  
  assert(src->policy_cond != NULL);
  free(src->policy_cond);

}

bool eq_e2sm_rc_act_def_frmt_2(e2sm_rc_act_def_frmt_2_t const* m0, e2sm_rc_act_def_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of Policy Conditions
  // [1 - 65535]
  assert(m0->sz_policy_cond > 0 && m0->sz_policy_cond <  65535 +1);
  assert(m1->sz_policy_cond > 0 && m1->sz_policy_cond <  65535 +1);

  if(m0->sz_policy_cond != m1->sz_policy_cond)
    return false;

  for(size_t i = 0; i < m0->sz_policy_cond; ++i){
    if(eq_policy_cond(&m0->policy_cond[i], &m1->policy_cond[i]) == false )
      return false;
  }

  return true;
}

e2sm_rc_act_def_frmt_2_t cp_e2sm_rc_act_def_frmt_2(e2sm_rc_act_def_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_rc_act_def_frmt_2_t dst = {0}; 
  // Sequence of Policy Conditions
  // [1 - 65535]
  assert(src->sz_policy_cond > 0 && src->sz_policy_cond < 65536);
  dst.sz_policy_cond = src->sz_policy_cond;

  dst.policy_cond = calloc(src->sz_policy_cond, sizeof(policy_cond_t));
  assert(dst.policy_cond != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_policy_cond; ++i){
    dst.policy_cond[i] = cp_policy_cond(&src->policy_cond[i]);
  }

  return dst;
}
