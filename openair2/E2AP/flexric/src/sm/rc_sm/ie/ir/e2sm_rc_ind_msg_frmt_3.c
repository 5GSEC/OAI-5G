#include "e2sm_rc_ind_msg_frmt_3.h"

#include <assert.h>

void free_e2sm_rc_ind_msg_frmt_3(e2sm_rc_ind_msg_frmt_3_t* src)
{
  assert(src != NULL);

  // Sequence of Cell Information
  // [1 - 65535]
  assert(src->sz_seq_cell_info > 0 && src->sz_seq_cell_info < 65535+1); 

  for(size_t i = 0; i < src->sz_seq_cell_info; ++i){
    free_seq_cell_info(&src->seq_cell_info[i]);
  }
  assert(src->seq_cell_info != NULL);
  free(src->seq_cell_info);

}

bool eq_e2sm_rc_ind_msg_frmt_3(e2sm_rc_ind_msg_frmt_3_t const* m0, e2sm_rc_ind_msg_frmt_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of Cell Information
  // [1 - 65535]
  if(m0->sz_seq_cell_info != m1->sz_seq_cell_info)
    return false;

  for(size_t i = 0; i < m0->sz_seq_cell_info; ++i){
    if(eq_seq_cell_info(&m0->seq_cell_info[i], &m1->seq_cell_info[i]) == false){
      assert(0!=0 && "Debug");
      return false;
    }
  }

  return true;
}

e2sm_rc_ind_msg_frmt_3_t cp_e2sm_rc_ind_msg_frmt_3(e2sm_rc_ind_msg_frmt_3_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ind_msg_frmt_3_t dst = {0}; 

  // Sequence of Cell Information
  // [1 - 65535]
  assert(src->sz_seq_cell_info > 0 && src->sz_seq_cell_info <  65536);
  dst.sz_seq_cell_info = src->sz_seq_cell_info;

  dst.seq_cell_info = calloc(dst.sz_seq_cell_info, sizeof( seq_cell_info_t ));
  assert(dst.seq_cell_info != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_cell_info; ++i){
    dst.seq_cell_info[i] = cp_seq_cell_info(&src->seq_cell_info[i]);
  }

  return dst;
}


