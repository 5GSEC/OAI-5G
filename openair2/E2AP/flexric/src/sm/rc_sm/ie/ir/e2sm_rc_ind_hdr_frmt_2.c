#include "e2sm_rc_ind_hdr_frmt_2.h"

#include <assert.h>

void free_e2sm_rc_ind_hdr_frmt_2(e2sm_rc_ind_hdr_frmt_2_t* src)
{
  assert(src != NULL);

  // UE ID
  // Mandatory
  // 9.3.10
  free_ue_id_e2sm(&src->ue_id);

  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. From common SM
  // RIC Style Type 
  // Integer
  //uint32_t ric_style_type;

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  //uint16_t ins_ind_id; 

}


bool eq_e2sm_rc_ind_hdr_frmt_2(e2sm_rc_ind_hdr_frmt_2_t const* m0, e2sm_rc_ind_hdr_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // UE ID
  // Mandatory
  // 9.3.10
  if(eq_ue_id_e2sm(&m0->ue_id, &m1->ue_id ) == false ){
    assert(0!=0 && "Debug assert");
    return false;
  }

  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. From common SM
  // RIC Style Type 
  // Integer
  if(m0->ric_style_type != m1->ric_style_type)
    return false;

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  assert(m0->ins_ind_id > 0);
  assert(m1->ins_ind_id > 0);
  
  if(m0->ins_ind_id != m1->ins_ind_id)
    return false;

  return true;
}

e2sm_rc_ind_hdr_frmt_2_t cp_e2sm_rc_ind_hdr_frmt_2(e2sm_rc_ind_hdr_frmt_2_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ind_hdr_frmt_2_t dst = {0}; 

  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = cp_ue_id_e2sm(&src->ue_id);

  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. From common SM
  // RIC Style Type 
  // Integer
  dst.ric_style_type = src->ric_style_type;

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  assert(src->ins_ind_id > 0); 
  dst.ins_ind_id = src->ins_ind_id; 

  return dst;
}

