#include "ue_info_chng.h"

#include <assert.h>

void free_ue_info_chng(ue_info_chng_t* src)
{
  assert(src != NULL);


  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  // INTEGER (1..65535, …)
  // uint16_t ev_trig_cond_id;

  // CHOICE Trigger Type
  //ue_info_chng_trigger_type_e type;
  if(src->type == RRC_STATE_UE_INFO_CHNG_TRIGGER_TYPE ){
    // RRC State
    free_rrc_state_lst(&src->rrc_state); 

  } else if(src->type == UE_ID_CHANGE_UE_INFO_CHNG_TRIGGER_TYPE ){
      // UE Identifier Change
      // [1 - 512]
      // uint16_t ue_id_chng;
      assert(src-> ue_id_chng > 0 && src->ue_id_chng < 513);
  } else if(src->type == L2_STATE_UE_INFO_CHNG_TRIGGER_TYPE){
    // L2 State
    // 9.3.29
    // RAN Parameter Conditional Criteria Definition
    free_ran_param_cond_crit_def(&src->l2_state);

  } else {
    assert(0!=0 && "Unknown type" );
  }

  // Associated UE Info 
  // Optional
  // 9.3.26
  assert(src->assoc_ue_info == NULL && "Not implemented"); 

  // Logical OR
  // Optional
  // 9.3.25
  assert(src->log_or == NULL && "Not implemented"); 

}

bool eq_ue_info_chng(ue_info_chng_t const* m0, ue_info_chng_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  // INTEGER (1..65535, …)
  assert(m0->ev_trig_cond_id > 0);
  assert(m1->ev_trig_cond_id > 0);
  if(m0->ev_trig_cond_id != m1->ev_trig_cond_id)
    return false;

  // CHOICE Trigger Type
  if(m0->type != m1->type)
    return false;

  if(m0->type == RRC_STATE_UE_INFO_CHNG_TRIGGER_TYPE){
    // RRC State
    if(eq_rrc_state_lst(&m0->rrc_state, &m1->rrc_state) == false){

      assert(0!=0);
      return false;
    }

  } else if(m0->type == UE_ID_CHANGE_UE_INFO_CHNG_TRIGGER_TYPE){
    // UE Identifier Change
    // [1 - 512]
    if(m0->ue_id_chng != m1->ue_id_chng){
      assert(0!=0);
      return false;
    }

  } else if(m0->type == L2_STATE_UE_INFO_CHNG_TRIGGER_TYPE){
    // L2 State
    // 9.3.29
    // RAN Parameter Conditional Criteria Definition
    if(eq_ran_param_cond_crit_def(&m0->l2_state, &m1->l2_state) == false){

      assert(0!=0);
      return false;
    }
  } else{
    assert(0!=0 && "Unknown type");
  }

  // Associated UE Info 
  // Optional
  // 9.3.26
  assert(m0->assoc_ue_info == NULL && "not implemented");
  assert(m1->assoc_ue_info == NULL && "not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(m0->log_or == NULL && "Not implemented");
  assert(m1->log_or == NULL && "Not implemented");

  return true;
}


ue_info_chng_t cp_ue_info_chng(ue_info_chng_t const* src)
{
  assert(src != NULL);
ue_info_chng_t dst = {0}; 
  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  // INTEGER (1..65535, …)
  assert(src->ev_trig_cond_id > 0);
  dst.ev_trig_cond_id = src->ev_trig_cond_id;

  // CHOICE Trigger Type
  dst.type = src->type;
  if(dst.type == RRC_STATE_UE_INFO_CHNG_TRIGGER_TYPE){
    // RRC State
    dst.rrc_state = cp_rrc_state_lst(&src->rrc_state);  
  } else if(dst.type == UE_ID_CHANGE_UE_INFO_CHNG_TRIGGER_TYPE){
    // UE Identifier Change
    // [1 - 512]
    assert(src->ue_id_chng > 0 && src->ue_id_chng < 513);
    dst.ue_id_chng = src->ue_id_chng;
  } else if(dst.type == L2_STATE_UE_INFO_CHNG_TRIGGER_TYPE ){
     // L2 State
    // 9.3.29
    // RAN Parameter Conditional Criteria Definition
    dst.l2_state = cp_ran_param_cond_crit_def(&src->l2_state);
  } else {
    assert(0!=0 && "Not implemented");
  } 

  // Associated UE Info 
  // Optional
  // 9.3.26
  assert(src->assoc_ue_info == NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(src->log_or == NULL && "Not implemented");

return dst;
}


