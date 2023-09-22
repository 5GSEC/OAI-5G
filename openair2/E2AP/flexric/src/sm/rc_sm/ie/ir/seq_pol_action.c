#include "seq_pol_action.h"

#include <assert.h>

void free_seq_pol_action(seq_pol_action_t* src)
{
  assert(src != NULL);
  // Policy Action ID
  // Mandatory
  // 9.3.6
  // uint32_t action_id; 

  // Policy Action Name
  // Mandatory
  // 9.3.7
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // RIC Action Definition Format Type
  // Mandatoyr
  // 9.3.5
  // uint32_t frmt_type;

  // Sequence of Associated RAN Parameters for Policy Action
  // [0- 65535]
  assert(src-> sz_seq_assoc_rp_action < 65536);
  for(size_t i = 0 ; i < src->sz_seq_assoc_rp_action; ++i){
    free_seq_ran_param_3(&src->seq_assoc_rp_action[i]);
  }
  if(src->seq_assoc_rp_action != NULL)
    free(src->seq_assoc_rp_action);

  // Sequence of Associated RAN Parameters for Policy Condition
  // [0- 65535]
  assert(src-> sz_seq_assoc_rp_policy < 65536); 
  for(size_t i = 0; i < src->sz_seq_assoc_rp_policy; ++i){
    free_seq_ran_param_3(&src->seq_assoc_rp_policy[i]);
  }
  if(src->seq_assoc_rp_policy != NULL )
    free(src->seq_assoc_rp_policy);
}

bool eq_seq_pol_action(seq_pol_action_t const* m0, seq_pol_action_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Policy Action ID
  // Mandatory
  // 9.3.6
  if(m0->action_id != m1->action_id) 
    return false;

  // Policy Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // RIC Action Definition Format Type
  // Mandatoyr
  // 9.3.5
  if(m0->frmt_type != m1->frmt_type)
    return false;

  // Sequence of Associated RAN Parameters for Policy Action
  // [0- 65535]
  assert(m0->sz_seq_assoc_rp_action <  65536);
  assert(m1->sz_seq_assoc_rp_action <  65536);
  if(m0-> sz_seq_assoc_rp_action != m1-> sz_seq_assoc_rp_action )
    return false;
  for(size_t i = 0; i < m0->sz_seq_assoc_rp_action; ++i){
    if(eq_seq_ran_param_3(&m0->seq_assoc_rp_action[i], &m1->seq_assoc_rp_action[i]) == false)
      return false;
  }

  // Sequence of Associated RAN Parameters for Policy Condition
  // [0- 65535]
  assert(m0-> sz_seq_assoc_rp_policy<  65536);
  assert(m1-> sz_seq_assoc_rp_policy<  65536);
  if(m0-> sz_seq_assoc_rp_policy != m1-> sz_seq_assoc_rp_policy )
    return false;
  for(size_t i = 0; i < m0-> sz_seq_assoc_rp_policy; ++i){
    if(eq_seq_ran_param_3(&m0->seq_assoc_rp_policy[i], &m1->seq_assoc_rp_policy[i]) == false)
      return false;
  }

  return true;
}

seq_pol_action_t cp_seq_pol_action(seq_pol_action_t* src)
{
  assert(src != NULL);
  seq_pol_action_t dst = {0}; 

  // Policy Action ID
  // Mandatory
  // 9.3.6
  dst.action_id = src->action_id; 

  // Policy Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151); 
  dst.name = copy_byte_array(src->name);

  // RIC Action Definition Format Type
  // Mandatoyr
  // 9.3.5
  dst.frmt_type = src->frmt_type;

  // Sequence of Associated RAN Parameters for Policy Action
  // [0- 65535]
  assert(src->sz_seq_assoc_rp_action < 65536);
  if(src->sz_seq_assoc_rp_action > 0){
    dst.sz_seq_assoc_rp_action = src->sz_seq_assoc_rp_action;
    dst.seq_assoc_rp_action = calloc(dst.sz_seq_assoc_rp_action, sizeof(seq_ran_param_3_t));
  }
  for(size_t i = 0; i < dst.sz_seq_assoc_rp_action; ++i){
    dst.seq_assoc_rp_action[i] = cp_seq_ran_param_3(&src->seq_assoc_rp_action[i]);
  }

  // Sequence of Associated RAN Parameters for Policy Condition
  // [0- 65535]
  assert(src->sz_seq_assoc_rp_policy < 65536);
  if(src->sz_seq_assoc_rp_policy > 0){
    dst.sz_seq_assoc_rp_policy = src->sz_seq_assoc_rp_policy;  
    dst.seq_assoc_rp_policy = calloc(dst.sz_seq_assoc_rp_policy, sizeof(seq_ran_param_3_t));
    assert(dst.seq_assoc_rp_policy != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < src->sz_seq_assoc_rp_policy; ++i){
    dst.seq_assoc_rp_policy[i] = cp_seq_ran_param_3(&src->seq_assoc_rp_policy[i]);
  } 

  return dst;
}

