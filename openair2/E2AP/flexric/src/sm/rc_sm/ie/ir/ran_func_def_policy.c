#include "ran_func_def_policy.h"

#include <assert.h>

void free_ran_func_def_policy(ran_func_def_policy_t* src)
{
  assert(src != NULL);

  // Sequence of POLICY styles
  // [1-63]
  assert(src->sz_policy_styles > 0 && src->sz_policy_styles < 64);
  for(size_t i =0; i < src->sz_policy_styles; ++i){
    free_seq_policy_sty(&src->seq_policy_sty[i]);
  }

  free(src->seq_policy_sty);
}

bool eq_ran_func_def_policy(ran_func_def_policy_t const* m0,  ran_func_def_policy_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of POLICY styles
  // [1-63]
  assert(m0->sz_policy_styles > 0 && m0->sz_policy_styles < 64);
  assert(m1->sz_policy_styles > 0 && m1->sz_policy_styles < 64);

  if(m0->sz_policy_styles != m1->sz_policy_styles )
    return false;

  for(size_t i = 0; i < m0->sz_policy_styles; ++i){
    if(eq_seq_policy_sty(&m0->seq_policy_sty[i], &m1->seq_policy_sty[i]) == false)
      return false;
  }

  return true;
}

ran_func_def_policy_t cp_ran_func_def_policy(ran_func_def_policy_t const* src)
{
  assert(src != NULL);

  ran_func_def_policy_t dst = {0}; 
  // Sequence of POLICY styles
  // [1-63]
  assert(src->sz_policy_styles > 0 && src->sz_policy_styles < 64);

  dst.sz_policy_styles = src->sz_policy_styles;

  dst.seq_policy_sty = calloc(dst.sz_policy_styles, sizeof(seq_policy_sty_t));
  assert(dst.seq_policy_sty != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_policy_styles; ++i){
   dst.seq_policy_sty[i] = cp_seq_policy_sty(&src->seq_policy_sty[i]); 
  }

  return dst;
}

