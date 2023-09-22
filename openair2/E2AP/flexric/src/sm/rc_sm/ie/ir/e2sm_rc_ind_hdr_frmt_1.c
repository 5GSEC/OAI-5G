#include "e2sm_rc_ind_hdr_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_rc_ind_hdr_frmt_1(e2sm_rc_ind_hdr_frmt_1_t* src)
{
  assert(src != NULL);

  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  // [1 - 65535]
  if(src->ev_trigger_id != NULL){
   assert(*src->ev_trigger_id > 0); 
   free(src->ev_trigger_id);
  }

}

bool eq_e2sm_rc_ind_hdr_frmt_1(e2sm_rc_ind_hdr_frmt_1_t const* m0,e2sm_rc_ind_hdr_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->ev_trigger_id == m1->ev_trigger_id)
    return true;

  if(m0->ev_trigger_id == NULL || m1->ev_trigger_id == NULL)
    return false;

  if(*m0->ev_trigger_id != *m1->ev_trigger_id)
    return false;

  return true;
}

e2sm_rc_ind_hdr_frmt_1_t cp_e2sm_rc_ind_hdr_frmt_1(e2sm_rc_ind_hdr_frmt_1_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ind_hdr_frmt_1_t dst = {0}; 

  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  // [1 - 65535]
  assert(src->ev_trigger_id != NULL && "Optional but not much sense to be void"); 
  assert(*src->ev_trigger_id > 1);

  dst.ev_trigger_id = malloc(sizeof(uint16_t));
  assert(dst.ev_trigger_id != NULL && "Not implemented");

  *dst.ev_trigger_id = *src->ev_trigger_id;

  return dst;
}


