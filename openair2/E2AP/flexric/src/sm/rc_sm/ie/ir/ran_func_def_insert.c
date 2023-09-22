#include "ran_func_def_insert.h"

#include <assert.h>

void free_ran_func_def_insert(ran_func_def_insert_t const* src)
{
  assert(src != NULL);

  // Sequence of INSERT styles
  // [1-63]
  assert(src->sz_seq_ins_sty > 0 && src->sz_seq_ins_sty < 64);
  for(size_t i = 0; i < src->sz_seq_ins_sty; ++i){
    free_seq_ins_sty(&src->seq_ins_sty[i]);
  }
  free(src->seq_ins_sty);
}


bool eq_ran_func_def_insert(ran_func_def_insert_t const* m0, ran_func_def_insert_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of INSERT styles
  // [1-63]
  assert(m0->sz_seq_ins_sty > 0 && m0->sz_seq_ins_sty < 64);
  assert(m1->sz_seq_ins_sty > 0 && m1->sz_seq_ins_sty < 64);

  if(m0->sz_seq_ins_sty != m1->sz_seq_ins_sty)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ins_sty; ++i){
    if(eq_seq_ins_sty(&m0->seq_ins_sty[i], &m1->seq_ins_sty[i]) == false)
      return false;
  }

  return true;
}

ran_func_def_insert_t cp_ran_func_def_insert(ran_func_def_insert_t const* src)
{
  assert(src != NULL);
  ran_func_def_insert_t dst = {0};

  // Sequence of INSERT styles
  // [1-63]
  assert(src->sz_seq_ins_sty > 0 && src->sz_seq_ins_sty < 64);

  dst.sz_seq_ins_sty = src->sz_seq_ins_sty;
  dst.seq_ins_sty = calloc(dst.sz_seq_ins_sty, sizeof(seq_ins_sty_t));
  assert(dst.seq_ins_sty != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ins_sty; ++i){
    dst.seq_ins_sty[i] = cp_seq_ins_sty(&src->seq_ins_sty[i]);
  }

  return dst;
}

