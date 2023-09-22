#include "e2sm_rc_ctrl_hdr_frmt_1.h"

#include <assert.h>

void free_e2sm_rc_ctrl_hdr_frmt_1(e2sm_rc_ctrl_hdr_frmt_1_t* src)
{
  assert(src != NULL);

  // UE ID
  // Mandatory
  // 9.3.10
  free_ue_id_e2sm(&src->ue_id);

  // RIC Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. 
  // INTEGER
  // uint32_t ric_style_type;

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1- 65535]
  assert(src->ctrl_act_id > 0); 
  // uint16_t ctrl_act_id;

  // RIC Control decision
  // Optional
  assert(src->ric_ctrl_decision == NULL && "Not implemented"); 

}

bool eq_e2sm_rc_ctrl_hdr_frmt_1(e2sm_rc_ctrl_hdr_frmt_1_t const* m0, e2sm_rc_ctrl_hdr_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

    // UE ID
  // Mandatory
  // 9.3.10
  if(eq_ue_id_e2sm(&m0->ue_id, &m1->ue_id) == false) 
    return false;

  // RIC Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. 
  // INTEGER
  if(m0->ric_style_type != m1->ric_style_type)
    return false;

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1- 65535]
  assert(m0->ctrl_act_id > 0);
  assert(m1->ctrl_act_id > 0);
  if(m0->ctrl_act_id != m1->ctrl_act_id)
    return false;

  // RIC Control decision
  // Optional
  assert(m0->ric_ctrl_decision == NULL && "Not implemented");
  assert(m1->ric_ctrl_decision == NULL && "Not implemented");

  return true;
}

e2sm_rc_ctrl_hdr_frmt_1_t cp_e2sm_rc_ctrl_hdr_frmt_1( e2sm_rc_ctrl_hdr_frmt_1_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ctrl_hdr_frmt_1_t dst = {0}; 
  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = cp_ue_id_e2sm(&src->ue_id);   

  // RIC Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. 
  // INTEGER
  dst.ric_style_type = src->ric_style_type;

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1- 65535]
  assert(src->ctrl_act_id > 0); 
  dst.ctrl_act_id = src->ctrl_act_id;

  // RIC Control decision
  // Optional
  assert(src->ric_ctrl_decision == NULL && "Not implemented");

  return dst;
}

