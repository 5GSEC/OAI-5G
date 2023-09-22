#include "e2sm_rc_ctrl_msg_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ctrl_msg_frmt_1( e2sm_rc_ctrl_msg_frmt_1_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz_ran_param; ++i){
    free_seq_ran_param(&src->ran_param[i]);
  }

  if(src->ran_param != NULL)
    free(src->ran_param);
}

bool eq_e2sm_rc_ctrl_msg_frmt_1(e2sm_rc_ctrl_msg_frmt_1_t const* m0, e2sm_rc_ctrl_msg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ran_param != m1->sz_ran_param)
    return false;

  for(size_t i = 0; i < m0->sz_ran_param; ++i){
    if(eq_seq_ran_param(&m0->ran_param[i], &m1->ran_param[i]) == false)
      return false;
  }

  return true;
}

e2sm_rc_ctrl_msg_frmt_1_t cp_e2sm_rc_ctrl_msg_frmt_1(e2sm_rc_ctrl_msg_frmt_1_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0};

  // List of RAN parameters
  // [0- 65535]
  assert(src->sz_ran_param < 65536);

  if(src->sz_ran_param > 0){
    dst.sz_ran_param = src->sz_ran_param;
    dst.ran_param = calloc(dst.sz_ran_param, sizeof(seq_ran_param_t)); 
    assert(dst.ran_param != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_ran_param; ++i){
    dst.ran_param[i] = cp_seq_ran_param(&src->ran_param[i]);
  }

  return dst;
}

