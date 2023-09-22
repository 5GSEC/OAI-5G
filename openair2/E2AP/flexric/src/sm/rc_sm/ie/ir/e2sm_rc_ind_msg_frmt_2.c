#include "e2sm_rc_ind_msg_frmt_2.h"

#include <assert.h>

void free_e2sm_rc_ind_msg_frmt_2(e2sm_rc_ind_msg_frmt_2_t const* src)
{
  assert(src != NULL);

  //Sequence of UE Identifier
  //[1-65535]
  assert(src->sz_seq_ue_id > 0 && src->sz_seq_ue_id <  65535+1);
  for(size_t i =0 ; i < src->sz_seq_ue_id; ++i){
    free_seq_ue_id(&src->seq_ue_id[i]);
  }
  assert(src->seq_ue_id != NULL);
  free(src->seq_ue_id);
}

bool eq_e2sm_rc_ind_msg_frmt_2( e2sm_rc_ind_msg_frmt_2_t const* m0,  e2sm_rc_ind_msg_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_seq_ue_id != m1->sz_seq_ue_id)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ue_id; ++i){
    if(eq_seq_ue_id(&m0->seq_ue_id[i], &m1->seq_ue_id[i]) == false){
      assert(0!=0 && "Debug");
      return false;
    }
  }

  return true;
}

e2sm_rc_ind_msg_frmt_2_t cp_e2sm_rc_ind_msg_frmt_2(e2sm_rc_ind_msg_frmt_2_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ind_msg_frmt_2_t dst = {0}; 

  //Sequence of UE Identifier
  //[1-65535]
  assert(src->sz_seq_ue_id > 0 && src->sz_seq_ue_id < 65536);

  dst.sz_seq_ue_id = src->sz_seq_ue_id;

  dst.seq_ue_id = calloc(dst.sz_seq_ue_id, sizeof(seq_ue_id_t));
  assert(dst.seq_ue_id != NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_seq_ue_id; ++i){
    dst.seq_ue_id[i] = cp_seq_ue_id(&src->seq_ue_id[i]);
  }

  return dst;
}


