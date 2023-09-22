#include "seq_ctrl_act_out.h"

#include <assert.h>
#include <stdlib.h>

void free_seq_ctrl_act_out(seq_ctrl_act_out_t* src)
{
  assert(src != NULL);

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  //  6.2.2.2.
  //  uint32_t ind_ctrl_style;

  //Sequence of Control
  //Actions Outcom
  // [1-63]
  assert(src->sz_ran_param > 0 && src->sz_ran_param < 64); 
  for(size_t i = 0; i < src->sz_ran_param; ++i){
    free_seq_ran_param_2(&src->ran_param[i]);
  }

  assert(src->ran_param != NULL);
  free(src->ran_param);
}

bool eq_seq_ctrl_act_out(seq_ctrl_act_out_t const* m0, seq_ctrl_act_out_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1- 65535]
  assert(m0->ctrl_act_id > 0);
  assert(m1->ctrl_act_id > 0);
  if(m0->ctrl_act_id != m1->ctrl_act_id)
    return false;

  //Sequence of RAN
  //Parameters
  // [1-255]  
  assert(m0->sz_ran_param > 0 && m0->sz_ran_param < 256);
  assert(m1->sz_ran_param > 0 && m1->sz_ran_param < 256);

  if(m0->sz_ran_param != m1->sz_ran_param)
    return false;

  for(size_t i = 0; i < m0->sz_ran_param; ++i){
    if(eq_seq_ran_param_2(&m0->ran_param[i], &m1->ran_param[i]) == false)
      return false;
  }
  
  return true;
}

seq_ctrl_act_out_t cp_seq_ctrl_act_out(seq_ctrl_act_out_t const* src)
{
  assert(src != NULL);
  seq_ctrl_act_out_t dst = {0}; 

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1- 65535]
  assert(src->ctrl_act_id > 0);
  dst.ctrl_act_id = src->ctrl_act_id;

  //Sequence of RAN
  //Parameters
  // [1-255]  
  assert(src->sz_ran_param > 0 && src->sz_ran_param < 256);
  dst.sz_ran_param = src->sz_ran_param;

  dst.ran_param = calloc(dst.sz_ran_param, sizeof(seq_ran_param_2_t));

  for(size_t i = 0; i < dst.sz_ran_param; ++i){
    dst.ran_param[i] = cp_seq_ran_param_2(&src->ran_param[i]);
  }

  return dst;
}

