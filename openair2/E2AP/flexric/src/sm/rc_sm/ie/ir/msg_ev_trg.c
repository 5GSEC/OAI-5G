#include "msg_ev_trg.h"

#include <assert.h>

void free_msg_ev_trg( msg_ev_trg_t* src)
{
  assert(src != NULL);

  // Event Trigger Condition ID
  // Mandatory
  // 9.3.21
  // [1- 65535]
  // uint16_t ev_trigger_cond_id;

  // CHOICE Message Type
  // Mandatory
  if(src->msg_type == NETWORK_INTERFACE_MSG_TYPE_EV_TRG){
    free_network_interface_e2rc(&src->net);
  } else if(src->msg_type == RRC_MSG_MSG_TYPE_EV_TRG){
    free_rrc_msg_id(&src->rrc_msg);
  } else {
    assert(0!=0 && "Unknown type");
  }

  //Message Direction
  //Optinonal
  assert(src->msg_dir == NULL && "Not implemented");

  //  Associated UE Info
  //  Optional
  //  9.3.26
  assert(src-> assoc_ue_info == NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(src-> log_or == NULL && "Not implemented");

}

bool eq_msg_ev_trg(msg_ev_trg_t const* m0, msg_ev_trg_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Event Trigger Condition ID
  // Mandatory
  // 9.3.21
  // [1- 65535]
  if(m0->ev_trigger_cond_id != m1->ev_trigger_cond_id)
    return false;

  // CHOICE Message Type
  // Mandatory
  if(m0->msg_type != m1->msg_type) 
    return false;

  if(m0->msg_type == NETWORK_INTERFACE_MSG_TYPE_EV_TRG){
    if(eq_network_interface_e2rc(&m0->net, &m1->net ) == false )
      return false;

    // 9.3.35
  } else if(m0->msg_type == RRC_MSG_MSG_TYPE_EV_TRG){
    if(eq_rrc_msg_id(&m0->rrc_msg, &m1->rrc_msg) == false)
      return false;

  } else {
    assert(0 != 0 && "Unknown type");
  }

  //Message Direction
  //Optional
  assert(m0->msg_dir == NULL && "Not implemented");
  assert(m1->msg_dir == NULL && "Not implemented");

  //  Associated UE Info
  //  Optional
  //  9.3.26
  assert(m0->assoc_ue_info == NULL && "Not implemented");
  assert(m1->assoc_ue_info == NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(m0->log_or == NULL && "Not implemented");
  assert(m1->log_or == NULL && "Not implemented");

  return true;
}


msg_ev_trg_t cp_msg_ev_trg( msg_ev_trg_t const* src)
{
  assert(src != NULL);
  msg_ev_trg_t dst = {0};

  // Event Trigger Condition ID
  // Mandatory
  // 9.3.21
  // [1- 65535]
  assert(src->ev_trigger_cond_id > 0 );
  dst.ev_trigger_cond_id = src->ev_trigger_cond_id; 

  // CHOICE Message Type
  // Mandatory
  dst.msg_type = src->msg_type;
  if(dst.msg_type == NETWORK_INTERFACE_MSG_TYPE_EV_TRG){
    dst.net = cp_network_interface_e2rc(&src->net);  
  } else if(dst.msg_type == RRC_MSG_MSG_TYPE_EV_TRG){
    // 9.3.35
    dst.rrc_msg = cp_rrc_msg_id(&src->rrc_msg);  
  } else {
    assert(0 !=0 && "Unknown type");
  }

  //Message Direction
  //Optinonal
  assert(src->msg_dir == NULL && "Not implemented"); 

  //  Associated UE Info
  //  Optional
  //  9.3.26
  assert(src->assoc_ue_info == NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(src->log_or == NULL && "Not implemented");

  return dst;
}

