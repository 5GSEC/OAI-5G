#include "e2sm_rc_ev_trg_frmt_4.h"

#include <assert.h>


void free_e2sm_rc_ev_trg_frmt_4(e2sm_rc_ev_trg_frmt_4_t* src)
{
  assert(src != NULL);

  assert(src->sz_ue_info_chng > 0 && src->sz_ue_info_chng <  65535 + 1);

  for(size_t i = 0; i < src->sz_ue_info_chng; ++i){
    free_ue_info_chng(&src->ue_info_chng[i]);
  }

  free(src->ue_info_chng); 
 
}

bool eq_e2sm_rc_ev_trg_frmt_4(e2sm_rc_ev_trg_frmt_4_t const* m0, e2sm_rc_ev_trg_frmt_4_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

 // Sequence of UE Information Change
 // [ 1 - 65535]
  assert(m0->sz_ue_info_chng > 0 && m0->sz_ue_info_chng < 65535 + 1); 
  assert(m1->sz_ue_info_chng > 0 && m1->sz_ue_info_chng < 65535 + 1); 
  
  if(m0->sz_ue_info_chng != m1->sz_ue_info_chng)
    return false;

  for(size_t i = 0; i < m0->sz_ue_info_chng; ++i){
    if(eq_ue_info_chng(&m0->ue_info_chng[i], &m1->ue_info_chng[i]) == false)
      return false;
  }

  return true;
}


e2sm_rc_ev_trg_frmt_4_t cp_e2sm_rc_ev_trg_frmt_4(e2sm_rc_ev_trg_frmt_4_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ev_trg_frmt_4_t dst = {0};

  // Sequence of UE Information Change
  // [ 1 - 65535]
  assert(src->sz_ue_info_chng > 0 && src->sz_ue_info_chng < 65536); 
  dst.sz_ue_info_chng = src->sz_ue_info_chng;
  dst.ue_info_chng = calloc(dst.sz_ue_info_chng, sizeof(ue_info_chng_t));
  assert(dst.ue_info_chng != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ue_info_chng; ++i){
    dst.ue_info_chng[i] = cp_ue_info_chng(&src->ue_info_chng[i]);
  }

  return dst;
}

