#include "e2sm_rc_ctrl_out_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ctrl_out_frmt_1(e2sm_rc_ctrl_out_frmt_1_t* src)
{
  assert(src != NULL);

  // Sequence of RAN
  // Parameters
  // [0 - 255]
  assert(src->sz_seq_ran_param_2 < 256);
  for(size_t i = 0; i < src->sz_seq_ran_param_2; ++i){
    free_seq_ran_param_2(&src->ran_param[i]);
  } 
  if(src->ran_param != NULL)
    free(src->ran_param);
}

bool eq_e2sm_rc_ctrl_out_frmt_1(e2sm_rc_ctrl_out_frmt_1_t const* m0, e2sm_rc_ctrl_out_frmt_1_t const* m1)
{

  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of RAN
  // Parameters
  // [0 - 255]
  assert(m0->sz_seq_ran_param_2 < 256); 
  assert(m1->sz_seq_ran_param_2 < 256); 

  if(m0->sz_seq_ran_param_2 != m1->sz_seq_ran_param_2)
    return false;

  for(size_t i =0; i < m0->sz_seq_ran_param_2; ++i){
    if(eq_seq_ran_param_2(&m0->ran_param[i], &m1->ran_param[i]  ) == false)
      return false;
  }

  return true;
}

e2sm_rc_ctrl_out_frmt_1_t cp_e2sm_rc_ctrl_out_frmt_1(e2sm_rc_ctrl_out_frmt_1_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ctrl_out_frmt_1_t dst = {0}; 
  // Sequence of RAN
  // Parameters
  // [0 - 255]
  assert(src->sz_seq_ran_param_2 < 256); 
  if(src->sz_seq_ran_param_2 > 0){
    dst.sz_seq_ran_param_2 = src->sz_seq_ran_param_2;
    dst.ran_param = calloc(dst.sz_seq_ran_param_2, sizeof(seq_ran_param_2_t) );
    assert(dst.ran_param != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_ran_param_2; ++i){
    dst.ran_param[i] = cp_seq_ran_param_2(&src->ran_param[i]); 
  }

  return dst;
}


