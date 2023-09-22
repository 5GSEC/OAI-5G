#include "e2sm_rc_act_def_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_act_def_frmt_1(e2sm_rc_act_def_frmt_1_t* src)
{
  assert(src != NULL);

  // Parameters to be Reported List
  // [1-65535]
  assert(src->sz_param_report_def > 0);

  for(size_t i = 0; i < src->sz_param_report_def; ++i){
    free_param_report_def(&src->param_report_def[i]);
  }

  assert(src->param_report_def != NULL);
  free(src->param_report_def);
}


bool eq_e2sm_rc_act_def_frmt_1(e2sm_rc_act_def_frmt_1_t const* m0, e2sm_rc_act_def_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  assert(m0->sz_param_report_def > 0);
  assert(m1->sz_param_report_def > 0);

  if(m0->sz_param_report_def != m1->sz_param_report_def)
    return false;

  for(size_t i = 0; i < m0->sz_param_report_def; ++i){
    if(eq_param_report_def(&m0->param_report_def[i], &m1->param_report_def[i]) == false)
      return false;
  }

  return true;
}

e2sm_rc_act_def_frmt_1_t cp_e2sm_rc_act_def_frmt_1(e2sm_rc_act_def_frmt_1_t const* src)
{
  assert(src != NULL);
  e2sm_rc_act_def_frmt_1_t dst = {0}; 

  // Parameters to be Reported List
  // [1-65535]
  assert(src->sz_param_report_def > 0 && src->sz_param_report_def < 65536); 
  dst.sz_param_report_def = src->sz_param_report_def;

  dst.param_report_def = calloc(dst.sz_param_report_def , sizeof(param_report_def_t));
  assert(dst.param_report_def != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_param_report_def; ++i){
    dst.param_report_def[i] = cp_param_report_def(&src->param_report_def[i]);
  }

  return dst;
}

