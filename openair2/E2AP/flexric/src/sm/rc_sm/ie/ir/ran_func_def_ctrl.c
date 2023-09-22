#include "ran_func_def_ctrl.h"
#include <assert.h>

void free_ran_func_def_ctrl(ran_func_def_ctrl_t* src)
{
  assert(src != NULL);
  // Sequence of CONTROL styles
  // [1 - 63]
  assert(src-> sz_seq_ctrl_style > 0 && src->sz_seq_ctrl_style < 64);
  for(size_t i = 0; i < src->sz_seq_ctrl_style; ++i){
    free_seq_ctrl_style(&src->seq_ctrl_style[i]);
  }
  free(src->seq_ctrl_style);
}

bool eq_ran_func_def_ctrl(ran_func_def_ctrl_t const* m0, ran_func_def_ctrl_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of CONTROL styles
  // [1 - 63]
  assert(m0->sz_seq_ctrl_style > 0 && m0->sz_seq_ctrl_style < 64);
  assert(m1->sz_seq_ctrl_style > 0 && m1->sz_seq_ctrl_style < 64);

  if(m0->sz_seq_ctrl_style != m1->sz_seq_ctrl_style)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ctrl_style; ++i){
    if(eq_seq_ctrl_style(&m0->seq_ctrl_style[i], &m1->seq_ctrl_style[i]) == false)
      return false;
  }

  return true;
}

ran_func_def_ctrl_t cp_ran_func_def_ctrl(ran_func_def_ctrl_t const* src)
{
  assert(src != NULL);

  ran_func_def_ctrl_t dst = {0}; 

  assert(src->sz_seq_ctrl_style > 0 && src->sz_seq_ctrl_style < 64);
  dst.sz_seq_ctrl_style = src->sz_seq_ctrl_style;

  dst.seq_ctrl_style = calloc(dst.sz_seq_ctrl_style, sizeof(seq_ctrl_style_t));
  assert(dst.seq_ctrl_style != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ctrl_style; ++i){
    dst.seq_ctrl_style[i] = cp_seq_ctrl_style(&src->seq_ctrl_style[i]); 
  }

  return dst;
}

