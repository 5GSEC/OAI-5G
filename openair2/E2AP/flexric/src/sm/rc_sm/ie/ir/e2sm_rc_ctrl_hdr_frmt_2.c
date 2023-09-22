#include "e2sm_rc_ctrl_hdr_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ctrl_hdr_frmt_2( e2sm_rc_ctrl_hdr_frmt_2_t* src)
{
  assert(src != NULL);

 // UE ID
 // Optional
 // 9.3.10
  if(src->ue_id != NULL){
    free_ue_id_e2sm(src->ue_id);
    free(src->ue_id);
  }

  // RIC Control decision
  // Optional
  if(src->ric_ctrl_dec != NULL){
    free(src->ric_ctrl_dec);
  }

}
     
bool eq_e2sm_rc_ctrl_hdr_frmt_2( e2sm_rc_ctrl_hdr_frmt_2_t const* m0, e2sm_rc_ctrl_hdr_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_ue_id_e2sm(m0->ue_id, m1->ue_id) == false)
    return false;

  if(m0->ric_ctrl_dec != m1->ric_ctrl_dec){
    if(m0->ue_id == NULL || m1->ue_id == NULL)
      return false;

    if(*m0->ric_ctrl_dec != *m1->ric_ctrl_dec)
      return false;
  }

  return true;
}

e2sm_rc_ctrl_hdr_frmt_2_t cp_e2sm_rc_ctrl_hdr_frmt_2( e2sm_rc_ctrl_hdr_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ctrl_hdr_frmt_2_t dst = {0}; 

  // UE ID
  // Optional
  // 9.3.10
  assert(src->ue_id != NULL && "Optional, but not much sense to be void");
  dst.ue_id = calloc(1, sizeof(ue_id_e2sm_t));
  assert(dst.ue_id != NULL && "Memory exhausted");

  *dst.ue_id = cp_ue_id_e2sm(src->ue_id);

  // RIC Control decision
  // Optional
  assert(src->ric_ctrl_dec == NULL && "Not implemented"); 

  return dst;
}

