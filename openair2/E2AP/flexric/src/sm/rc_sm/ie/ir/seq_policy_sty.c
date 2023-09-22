#include "seq_policy_sty.h"
#include <assert.h>


void free_seq_policy_sty(seq_policy_sty_t* src)
{
  assert(src != NULL);

  // RIC Policy Style Type
  // Mandatory
  // 9.3.3
   // uint32_t style_type;

  // RIC Policy Style Name
  // Mandatory
  // 9.3.4
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // uint32_t ev_trg_style_type;

  // Sequence of Policy Actions
  // [0-65535]
  assert(src->sz_seq_pol_action < 65536);
  for(size_t i = 0; i < src->sz_seq_pol_action; ++i){
    free_seq_pol_action(&src->seq_pol_action[i]);
  } 
  if(src->seq_pol_action != NULL)
    free(src->seq_pol_action);

}

bool eq_seq_policy_sty(seq_policy_sty_t const* m0, seq_policy_sty_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RIC Policy Style Type
  // Mandatory
  // 9.3.3
  if(m0->style_type != m1->style_type)
    return false;

  // RIC Policy Style Name
  // Mandatory
  // 9.3.4
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  if(m0->ev_trg_style_type != m1->ev_trg_style_type)
    return false;

  // Sequence of Policy Actions
  // [0-65535]
  assert(m0->sz_seq_pol_action < 65536);
  assert(m1->sz_seq_pol_action < 65536);

  if(m0-> sz_seq_pol_action != m1-> sz_seq_pol_action )
    return false;

  for(size_t i = 0; i < m0->sz_seq_pol_action; ++i){
    if(eq_seq_pol_action(&m0->seq_pol_action[i], &m1->seq_pol_action[i]) == false)
      return false;
  }

  return true;
}

seq_policy_sty_t cp_seq_policy_sty(seq_policy_sty_t const* src)
{
  assert(src != NULL);
  seq_policy_sty_t dst = {0};  
  // RIC Policy Style Type
  // Mandatory
  // 9.3.3
  dst.style_type= src->style_type;

  // RIC Policy Style Name
  // Mandatory
  // 9.3.4
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  dst.ev_trg_style_type = src->ev_trg_style_type;

  // Sequence of Policy Actions
  // [0-65535]
  assert(src->sz_seq_pol_action < 65536);
  if(src->sz_seq_pol_action > 0){
    dst.sz_seq_pol_action = src->sz_seq_pol_action; 
    dst.seq_pol_action = calloc(dst.sz_seq_pol_action, sizeof( seq_pol_action_t) );
    assert(dst.seq_pol_action != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < src-> sz_seq_pol_action; ++i){
    dst.seq_pol_action[i] = cp_seq_pol_action(&src->seq_pol_action[i]);
  }

  return dst;
}

