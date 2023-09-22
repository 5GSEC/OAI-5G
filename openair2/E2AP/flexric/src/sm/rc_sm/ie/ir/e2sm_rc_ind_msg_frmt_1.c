#include "e2sm_rc_ind_msg_frmt_1.h"

#include <assert.h>


void free_e2sm_rc_ind_msg_frmt_1(e2sm_rc_ind_msg_frmt_1_t const* src)
{
  assert(src != NULL);

  //  Sequence of RAN
  //  Parameters
  //  [1 - 65535]
  assert(src->sz_seq_ran_param > 0 && src->sz_seq_ran_param < 65535+1);
  assert(src->seq_ran_param != NULL);

  for(size_t i = 0; i < src->sz_seq_ran_param; ++i){
    free_seq_ran_param(&src->seq_ran_param[i]);
  }
  free(src->seq_ran_param);

}


bool eq_e2sm_rc_ind_msg_frmt_1( e2sm_rc_ind_msg_frmt_1_t const* m0,  e2sm_rc_ind_msg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_seq_ran_param != m1->sz_seq_ran_param)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ran_param; ++i){
    if(eq_seq_ran_param(&m0->seq_ran_param[i], &m1->seq_ran_param[i] ) == false){
      assert(0!=0 && "Debug");
      return false;
    }
  }

  return true;
}


e2sm_rc_ind_msg_frmt_1_t cp_e2sm_rc_ind_msg_frmt_1(e2sm_rc_ind_msg_frmt_1_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ind_msg_frmt_1_t dst = {0}; 

  //  Sequence of RAN
  //  Parameters
  //  [1 - 65535]
  assert(src->sz_seq_ran_param > 0);
  
  dst.seq_ran_param = calloc(src->sz_seq_ran_param, sizeof(seq_ran_param_t) );
  assert(dst.seq_ran_param != NULL && "Memory exhausted");

  dst.sz_seq_ran_param = src->sz_seq_ran_param;

  for(size_t i = 0; i < src->sz_seq_ran_param; ++i){
    dst.seq_ran_param[i] = cp_seq_ran_param(&src->seq_ran_param[i]);
  }

  return dst;
}

