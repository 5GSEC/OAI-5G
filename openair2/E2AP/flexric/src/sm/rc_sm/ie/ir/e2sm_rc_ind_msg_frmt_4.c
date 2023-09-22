#include "e2sm_rc_ind_msg_frmt_4.h"

#include <assert.h>

void free_e2sm_rc_ind_msg_frmt_4(e2sm_rc_ind_msg_frmt_4_t* src)
{
  assert(src != NULL);

  //Sequence of UE Information
  // [0-65535]
  assert(src->sz_seq_ue_info < 65535+1);

  for(size_t i = 0; i < src->sz_seq_ue_info; ++i){
    free_seq_ue_info(&src->seq_ue_info[i]);
  }

  if(src->seq_ue_info != NULL){
    assert(src->sz_seq_ue_info >0);
    free(src->seq_ue_info);
  }

  // Sequence of Cell Information
  // [0-65535]
  for(size_t i = 0; i <  src->sz_seq_cell_info_2; ++i){
    free_seq_cell_info_2(&src->seq_cell_info_2[i]);
  }

  if(src->seq_cell_info_2 != NULL){
    assert(src->sz_seq_cell_info_2 > 0);
    free(src->seq_cell_info_2);
  }

}


bool eq_e2sm_rc_ind_msg_frmt_4( e2sm_rc_ind_msg_frmt_4_t const* m0,  e2sm_rc_ind_msg_frmt_4_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //Sequence of UE Information
  // [0-65535]
  assert(m0->sz_seq_ue_info < 65535+1);
  assert(m1->sz_seq_ue_info < 65535+1);

  if(m0->sz_seq_ue_info != m1->sz_seq_ue_info)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ue_info; ++i){
    if(eq_seq_ue_info(&m0->seq_ue_info[i], &m1->seq_ue_info[i]) == false){
      assert(0!=0 && "Debug");
      return false;
    }
  }

  // Sequence of Cell Information
  // [0-65535]
  assert(m0->sz_seq_cell_info_2 < 65535+1);
  assert(m1->sz_seq_cell_info_2 < 65535+1);

  for(size_t i = 0; i < m0->sz_seq_cell_info_2; ++i){
    if(eq_seq_cell_info_2(&m0->seq_cell_info_2[i], &m1->seq_cell_info_2[i] ) == false){
      assert(0!=0 && "Debug");
      return false;
    }
  }

  return true;
}


e2sm_rc_ind_msg_frmt_4_t cp_e2sm_rc_ind_msg_frmt_4(e2sm_rc_ind_msg_frmt_4_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ind_msg_frmt_4_t dst = {0}; 

  //Sequence of UE Information
  // [0-65535]
  if(src->sz_seq_ue_info > 0){
    dst.sz_seq_ue_info = src->sz_seq_ue_info;
    dst.seq_ue_info = calloc( dst.sz_seq_ue_info, sizeof(seq_ue_info_t));
    assert(dst.seq_ue_info != NULL && "Memory exhausted" );
  }

  for(size_t i = 0; i < dst.sz_seq_ue_info; ++i){
    dst.seq_ue_info[i] = cp_seq_ue_info(&src->seq_ue_info[i]);
  }

  // Sequence of Cell Information
  // [0-65535]
  if(src->sz_seq_cell_info_2 > 0){
    dst.sz_seq_cell_info_2 = src->sz_seq_cell_info_2;

    dst.seq_cell_info_2 = calloc(dst.sz_seq_cell_info_2, sizeof(seq_cell_info_2_t));
    assert(dst.seq_cell_info_2 != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_cell_info_2; ++i){
    dst.seq_cell_info_2[i] = cp_seq_cell_info_2(&src->seq_cell_info_2[i]);
  }

  return dst;
}

