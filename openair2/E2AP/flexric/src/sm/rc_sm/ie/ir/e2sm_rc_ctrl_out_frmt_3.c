#include "e2sm_rc_ctrl_out_frmt_3.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ctrl_out_frmt_3( e2sm_rc_ctrl_out_frmt_3_t* src)
{
  assert(src != NULL);

  assert(src->sz_seq_ran_param < 256);

  for(size_t i = 0; i < src->sz_seq_ran_param; ++i){
    free_seq_ran_param(&src->ran_param[i]);
  }

  if(src->ran_param != NULL)
    free(src->ran_param);
}

bool eq_e2sm_rc_ctrl_out_frmt_3( e2sm_rc_ctrl_out_frmt_3_t const* m0,  e2sm_rc_ctrl_out_frmt_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //  Sequence of RAN Parameters
  //  [0-255]
  assert(m0->sz_seq_ran_param < 256);
  assert(m1->sz_seq_ran_param < 256);

  if(m0->sz_seq_ran_param != m1->sz_seq_ran_param)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ran_param; ++i){
    if(eq_seq_ran_param(&m0->ran_param[i], &m1->ran_param[i]) == false)
      return false;
  }

  return true;
}

e2sm_rc_ctrl_out_frmt_3_t cp_e2sm_rc_ctrl_out_frmt_3(e2sm_rc_ctrl_out_frmt_3_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ctrl_out_frmt_3_t dst = {0};

  //  Sequence of RAN Parameters
  //  [0-255]
  assert(src->sz_seq_ran_param < 256);
  if(src->sz_seq_ran_param > 0){
    dst.sz_seq_ran_param = src->sz_seq_ran_param;
    dst.ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t));
    assert(dst.ran_param != NULL && "memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.ran_param[i] = cp_seq_ran_param(&src->ran_param[i]);
  }

  return dst;
}

