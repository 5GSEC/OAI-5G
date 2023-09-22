#include "e2sm_rc_ctrl_out_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ctrl_out_frmt_2(e2sm_rc_ctrl_out_frmt_2_t* src)
{
  assert(src != NULL);

// Sequence of Control Styles
// for Multiple Outcomes
// [1-63]
  assert(src->sz_seq_ctrl_sty_mul_out > 0 && src->sz_seq_ctrl_sty_mul_out < 64);

  for(size_t i = 0; i < src->sz_seq_ctrl_sty_mul_out; ++i){
    free_seq_ctrl_sty_mul_out(&src->seq_ctrl_sty_mul_out[i]);
  }

  assert(src->seq_ctrl_sty_mul_out != NULL);
  free(src->seq_ctrl_sty_mul_out);
}

bool eq_e2sm_rc_ctrl_out_frmt_2(e2sm_rc_ctrl_out_frmt_2_t const* m0, e2sm_rc_ctrl_out_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of Control Styles
  // for Multiple Outcomes
  // [1-63]
  assert(m0->sz_seq_ctrl_sty_mul_out > 0 && m0->sz_seq_ctrl_sty_mul_out < 64); 
  assert(m1->sz_seq_ctrl_sty_mul_out > 0 && m1->sz_seq_ctrl_sty_mul_out < 64); 

  if(m0->sz_seq_ctrl_sty_mul_out != m1->sz_seq_ctrl_sty_mul_out){
    assert(0!=0 && "Debug");
    return false;
  }

  for(size_t i = 0; i < m0->sz_seq_ctrl_sty_mul_out; ++i){
    if(eq_seq_ctrl_sty_mul_out(&m0->seq_ctrl_sty_mul_out[i], &m1->seq_ctrl_sty_mul_out[i]) == false){
      assert(0!=0 && "Debug");
      return false;
    }
  }

  return true;
}

e2sm_rc_ctrl_out_frmt_2_t cp_e2sm_rc_ctrl_out_frmt_2(e2sm_rc_ctrl_out_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ctrl_out_frmt_2_t dst = {0}; 

  // Sequence of Control Styles
  // for Multiple Outcomes
  // [1-63]
  assert(src->sz_seq_ctrl_sty_mul_out > 0 && src->sz_seq_ctrl_sty_mul_out < 64);
  dst.sz_seq_ctrl_sty_mul_out = src->sz_seq_ctrl_sty_mul_out;

  dst.seq_ctrl_sty_mul_out = calloc(dst.sz_seq_ctrl_sty_mul_out, sizeof(seq_ctrl_sty_mul_out_t ) );
  assert(dst.seq_ctrl_sty_mul_out != NULL && "Memory exhausted");

  for(size_t i = 0 ; i < dst.sz_seq_ctrl_sty_mul_out; ++i){
    dst.seq_ctrl_sty_mul_out[i] = cp_seq_ctrl_sty_mul_out(&src->seq_ctrl_sty_mul_out[i]);
  }

  return dst;
}


