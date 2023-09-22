#include "e2sm_rc_ind_msg_frmt_5.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ind_msg_frmt_5(e2sm_rc_ind_msg_frmt_5_t* src)
{
  assert(src != NULL);

  // List of RAN parameters requested
  // [0-65535]
  for(size_t i = 0; i < src->sz_seq_ran_param; ++i){
    free_seq_ran_param(&src->seq_ran_param[i]);
  }
  
  if(src->seq_ran_param != NULL)
    free(src->seq_ran_param);
}


bool eq_e2sm_rc_ind_msg_frmt_5(e2sm_rc_ind_msg_frmt_5_t const* m0, e2sm_rc_ind_msg_frmt_5_t const* m1)
{

  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  // List of RAN parameters requested
  // [0-65535]
  if(m0->sz_seq_ran_param != m1->sz_seq_ran_param) 
    return false;

  for(size_t i = 0; i < m0->sz_seq_ran_param; ++i){
    if(eq_seq_ran_param(&m0->seq_ran_param[i], &m1->seq_ran_param[i]) == false){
      assert(0!=0 && "Debug");
      return false;
    }
  }

  return true;
}

e2sm_rc_ind_msg_frmt_5_t cp_e2sm_rc_ind_msg_frmt_5(e2sm_rc_ind_msg_frmt_5_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ind_msg_frmt_5_t dst = {0}; 

  // List of RAN parameters requested
  // [0-65535]
  if(src->sz_seq_ran_param > 0 ){
    dst.sz_seq_ran_param = src->sz_seq_ran_param; 
    dst.seq_ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t) );
    assert(dst.seq_ran_param != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.seq_ran_param[i] = cp_seq_ran_param(&src->seq_ran_param[i]);
  }

  return dst;
}

  

