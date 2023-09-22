#include "seq_ctrl_sty_mul_out.h"

#include <assert.h>
#include <stdlib.h>

void free_seq_ctrl_sty_mul_out( seq_ctrl_sty_mul_out_t* src)
{
  assert(src != NULL);

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  //  6.2.2.2.
  // uint32_t ind_ctrl_style;

  //Sequence of Control
  //Actions Outcom
  // [1-63]
  assert(src->sz_seq_ctrl_act_out > 0 && src->sz_seq_ctrl_act_out < 64); 

  for(size_t i = 0; i < src->sz_seq_ctrl_act_out; ++i){
    free_seq_ctrl_act_out(&src->seq_ctrl_act_out[i]);
  }
  
  assert(src->seq_ctrl_act_out != NULL);
  free(src->seq_ctrl_act_out);
}

bool eq_seq_ctrl_sty_mul_out(seq_ctrl_sty_mul_out_t const* m0, seq_ctrl_sty_mul_out_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  //  6.2.2.2.
  if(m0->ind_ctrl_style != m1->ind_ctrl_style)
    return false;

  //Sequence of Control
  //Actions Outcom
  // [1-63]
  assert(m0->sz_seq_ctrl_act_out > 0 && m0->sz_seq_ctrl_act_out < 64);
  assert(m1->sz_seq_ctrl_act_out > 0 && m1->sz_seq_ctrl_act_out < 64);

  if(m0->sz_seq_ctrl_act_out != m1->sz_seq_ctrl_act_out)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ctrl_act_out; ++i){
    if(eq_seq_ctrl_act_out(&m0->seq_ctrl_act_out[i], &m1->seq_ctrl_act_out[i]) == false)
      return false;
  }

  return true;
}

seq_ctrl_sty_mul_out_t cp_seq_ctrl_sty_mul_out( seq_ctrl_sty_mul_out_t const* src)
{
  assert(src != NULL);

  seq_ctrl_sty_mul_out_t dst = {0};

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  //  6.2.2.2.
  dst.ind_ctrl_style = src->ind_ctrl_style;

  //Sequence of Control
  //Actions Outcom
  // [1-63]
  assert(src->sz_seq_ctrl_act_out > 0 && src->sz_seq_ctrl_act_out < 64);

  dst.sz_seq_ctrl_act_out = src->sz_seq_ctrl_act_out;

  dst.seq_ctrl_act_out = calloc(dst.sz_seq_ctrl_act_out , sizeof(seq_ctrl_act_out_t));
  assert(dst.seq_ctrl_act_out != NULL && "memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ctrl_act_out; ++i){
    dst.seq_ctrl_act_out[i] = cp_seq_ctrl_act_out( &src->seq_ctrl_act_out[i] );
  }

  return dst;

}

