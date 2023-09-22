#include "seq_ctrl_sma.h"

#include <assert.h>

void free_seq_ctrl_sma(seq_ctrl_sma_t* src)
{
  assert(src != NULL);

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  // uint32_t ctrl_style;

  // Sequence of Control Actions
  // [1-63]
  assert(src->sz_seq_ctrl_act > 0 && src->sz_seq_ctrl_act < 64); 

  for(size_t i = 0; i < src->sz_seq_ctrl_act; ++i){
   free_seq_ctrl_act(&src->seq_ctrl_act[i]);
  }
  assert(src->seq_ctrl_act != NULL);
  free(src->seq_ctrl_act);
}

bool eq_seq_ctrl_sma( seq_ctrl_sma_t const* m0, seq_ctrl_sma_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  if(m0->ctrl_style != m1->ctrl_style) 
    return false;

  // Sequence of Control Actions
  // [1-63]
  assert(m0->sz_seq_ctrl_act > 0 && m0->sz_seq_ctrl_act < 64);
  assert(m1->sz_seq_ctrl_act > 0 && m1->sz_seq_ctrl_act < 64);
  
  if(m0->sz_seq_ctrl_act != m1->sz_seq_ctrl_act)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ctrl_act; ++i){
    if(eq_seq_ctrl_act(&m0->seq_ctrl_act[i], &m1->seq_ctrl_act[i]) == false)
      return false;
  }

  return true;
}

seq_ctrl_sma_t cp_seq_ctrl_sma(seq_ctrl_sma_t const* src)
{
  assert(src != NULL);
  seq_ctrl_sma_t dst = {0};

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.ctrl_style = src->ctrl_style;

  // Sequence of Control Actions
  // [1-63]
  assert(src->sz_seq_ctrl_act > 0 && src->sz_seq_ctrl_act < 64);
  dst.sz_seq_ctrl_act = src->sz_seq_ctrl_act;

  dst.seq_ctrl_act = calloc(dst.sz_seq_ctrl_act, sizeof(seq_ctrl_act_t));
  assert(dst.seq_ctrl_act != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ctrl_act; ++i){
    dst.seq_ctrl_act[i] = cp_seq_ctrl_act(&src->seq_ctrl_act[i]);
  }

  return dst;
}

