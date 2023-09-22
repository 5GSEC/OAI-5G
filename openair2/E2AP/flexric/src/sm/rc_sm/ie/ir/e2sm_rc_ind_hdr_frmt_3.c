#include "e2sm_rc_ind_hdr_frmt_3.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ind_hdr_frmt_3(e2sm_rc_ind_hdr_frmt_3_t* src)
{
  assert(src != NULL);

  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  if(src->ev_trigger_cond != NULL)
    free(src->ev_trigger_cond );

  // UE ID
  // Optional 
  // 9.3.10
  if(src->ue_id != NULL){
    free_ue_id_e2sm(src->ue_id);
    free(src->ue_id);
  }

}

bool eq_e2sm_rc_ind_hdr_frmt_3(e2sm_rc_ind_hdr_frmt_3_t const* m0, e2sm_rc_ind_hdr_frmt_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  // [1 - 65535]
  if(m0->ev_trigger_cond != m1->ev_trigger_cond){
    if(m0->ev_trigger_cond == NULL || m1->ev_trigger_cond == NULL)
      return false;

    assert(*m0->ev_trigger_cond > 0);
    assert(*m1->ev_trigger_cond > 0);

    if(*m0->ev_trigger_cond != *m1->ev_trigger_cond)
      return false;
  }

  // UE ID
  // Optional 
  // 9.3.10
  if(m0->ue_id != m1->ue_id){
    if(m0->ue_id == NULL || m1->ue_id == NULL)
      return false;

    if(eq_ue_id_e2sm(m0->ue_id, m1->ue_id) == false)
      return false;
  }

  return true;
}

e2sm_rc_ind_hdr_frmt_3_t cp_e2sm_rc_ind_hdr_frmt_3(e2sm_rc_ind_hdr_frmt_3_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ind_hdr_frmt_3_t dst = {0}; 
  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  // [1 - 65535]
  if(src->ev_trigger_cond != NULL){
    assert(*src->ev_trigger_cond > 0); 

    dst.ev_trigger_cond = malloc(sizeof(uint16_t));
    assert(dst.ev_trigger_cond != NULL && "Memory exhausted");
    *dst.ev_trigger_cond = *src->ev_trigger_cond;
  }

  // UE ID
  // Optional 
  // 9.3.10
  if(src->ue_id != NULL){
    dst.ue_id = calloc(1, sizeof(ue_id_e2sm_t ));
    assert(dst.ue_id != NULL && "Memory exhausted");
    *dst.ue_id = cp_ue_id_e2sm(src->ue_id);
  } 

  return dst;
}

