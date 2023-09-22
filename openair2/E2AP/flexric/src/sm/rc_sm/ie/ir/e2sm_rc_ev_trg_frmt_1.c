#include <assert.h>

#include "e2sm_rc_ev_trg_frmt_1.h"

void free_e2sm_rc_ev_trg_frmt_1(e2sm_rc_ev_trg_frmt_1_t* src)
{
  assert(src != NULL);

  // [1 - 65535]
  
  assert(src->sz_msg_ev_trg > 0 && src->sz_msg_ev_trg < 65535 + 1);
  for(size_t i = 0; i < src->sz_msg_ev_trg; ++i){
    free_msg_ev_trg(&src->msg_ev_trg[i]);
  }

  assert(src->msg_ev_trg != NULL);
  free(src->msg_ev_trg);

  // Global Associated UE Info
  // Optional
  // 9.3.26
  assert(src->global_assoc_ue_info == NULL && "Not implemented");
}

bool eq_e2sm_rc_ev_trg_frmt_1(e2sm_rc_ev_trg_frmt_1_t const* m0, e2sm_rc_ev_trg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // [1 - 65535]
  assert(m0->sz_msg_ev_trg > 0 && m0->sz_msg_ev_trg < 65535 + 1);
  assert(m1->sz_msg_ev_trg > 0 && m1->sz_msg_ev_trg < 65535 + 1);
  assert(m0->msg_ev_trg != NULL);
  assert(m1->msg_ev_trg != NULL);

  if(m0->sz_msg_ev_trg  != m1->sz_msg_ev_trg)
    return false;

  for(size_t i = 0; i < m0->sz_msg_ev_trg; ++i){
    if(eq_msg_ev_trg(&m0->msg_ev_trg[i], &m1->msg_ev_trg[i]) == false)
      return false;
  }

  // Global Associated UE Info
  // Optional
  // 9.3.26
  assert(m0->global_assoc_ue_info == NULL && "Not implemented");
  assert(m1->global_assoc_ue_info == NULL && "Not implemented");

  return true;
}

e2sm_rc_ev_trg_frmt_1_t cp_e2sm_rc_ev_trg_frmt_1(e2sm_rc_ev_trg_frmt_1_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ev_trg_frmt_1_t dst = {0}; 

  // [1 - 65535]
  assert(src->sz_msg_ev_trg > 0 && src->sz_msg_ev_trg <  65536);
  dst.sz_msg_ev_trg = src->sz_msg_ev_trg;

  dst.msg_ev_trg = calloc(dst.sz_msg_ev_trg, sizeof( msg_ev_trg_t) );
  assert(dst.msg_ev_trg != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_msg_ev_trg; ++i){
    dst.msg_ev_trg[i] = cp_msg_ev_trg(&src->msg_ev_trg[i]);
  }

  // Global Associated UE Info
  // Optional
  // 9.3.26
  assert(src->global_assoc_ue_info == NULL && "Not implemeneted"); 

  return dst;
}

