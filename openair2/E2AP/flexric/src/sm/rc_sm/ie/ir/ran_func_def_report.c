#include "ran_func_def_report.h"

#include <assert.h>

void free_ran_func_def_report(ran_func_def_report_t* src)
{
  assert(src != NULL);
  // Sequence of REPORT styles
  // [1 - 63]
  assert(src->sz_seq_report_sty > 0 && src->sz_seq_report_sty < 64);
  for(size_t i = 0; i < src->sz_seq_report_sty; ++i){
    free_seq_report_sty(&src->seq_report_sty[i]);
  } 
  free(src->seq_report_sty);
}

bool eq_ran_func_def_report( ran_func_def_report_t const* m0, ran_func_def_report_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_seq_report_sty != m1->sz_seq_report_sty)
    return false;

  for(size_t i = 0; i < m0->sz_seq_report_sty; ++i){
    if(eq_seq_report_sty(&m0->seq_report_sty[i], &m1->seq_report_sty[i]) == false)
      return false;
  }

  return true;
}

ran_func_def_report_t cp_ran_func_def_report(ran_func_def_report_t const* src)
{
  assert(src != NULL);
  ran_func_def_report_t dst = {0}; 
  // Sequence of REPORT styles
  // [1 - 63]
  assert(src->sz_seq_report_sty > 0 && src->sz_seq_report_sty < 64);


  dst.sz_seq_report_sty = src->sz_seq_report_sty;
  dst.seq_report_sty = calloc(dst.sz_seq_report_sty, sizeof(seq_report_sty_t) );
  assert(dst.seq_report_sty != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_seq_report_sty; ++i){
    dst.seq_report_sty[i] = cp_seq_report_sty(&src->seq_report_sty[i]) ;
  }

  return dst;
}

