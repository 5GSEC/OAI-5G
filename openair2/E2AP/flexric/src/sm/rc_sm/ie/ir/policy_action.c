#include "policy_action.h"

#include <assert.h>

void free_policy_action(policy_action_t* src)
{
  assert(src != NULL);

  //  Policy Action ID
  //  Mandatory
  //  9.3.6
  //  [1 - 65535]
  // uint16_t policy_act_id;

  assert(src->policy_act_id > 0);

  // Sequence of RAN Parameters
  // [0- 65535]
  for(size_t i = 0; i < src->sz_seq_ran_param; ++i){
    free_seq_ran_param(&src->seq_ran_param[i]);
  }

  if(src->sz_seq_ran_param > 0)
    free(src->seq_ran_param);

  // RIC Policy decision
  // Optional
  assert(src->pol_dec == NULL && "Not implmented"); 

}

bool eq_policy_action(policy_action_t const* m0, policy_action_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  //  Policy Action ID
  //  Mandatory
  //  9.3.6
  //  [1 - 65535]
  assert(m0->policy_act_id > 0 );
  assert(m1->policy_act_id > 0 );

  if(m0->policy_act_id != m1->policy_act_id){
    return false;
  }

  // Sequence of RAN Parameters
  // [0- 65535]
  assert(m0->sz_seq_ran_param <  65535);
  assert(m1->sz_seq_ran_param <  65535);

  if(m0->sz_seq_ran_param != m1->sz_seq_ran_param){
    return false;
  }

  for(size_t i = 0; i < m0->sz_seq_ran_param; ++i){
    if(eq_seq_ran_param(&m0->seq_ran_param[i], &m1->seq_ran_param[i]) == false){
      return false;
    }
  }

  // RIC Policy decision
  // Optional
  assert(m0->pol_dec == NULL && "Not implemented");
  assert(m1->pol_dec == NULL && "Not implemented");

  return true;
}

policy_action_t cp_policy_action(policy_action_t const* src)
{
  assert(src != NULL);
  policy_action_t dst = {0};  
  //  Policy Action ID
  //  Mandatory
  //  9.3.6
  //  [1 - 65535]
  assert(src->policy_act_id > 0);;
  dst.policy_act_id = src->policy_act_id;

  // Sequence of RAN Parameters
  // [0- 65535]
  assert(src->sz_seq_ran_param <  65536);
  if(src->sz_seq_ran_param > 0){
    dst.sz_seq_ran_param = src->sz_seq_ran_param;
    dst.seq_ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t) );
    assert(dst.seq_ran_param != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.seq_ran_param[i] = cp_seq_ran_param(&src->seq_ran_param[i]);
  }

  // RIC Policy decision
  // Optional
  assert(src->pol_dec == NULL && "Not implemented"); 

  return dst;
}


