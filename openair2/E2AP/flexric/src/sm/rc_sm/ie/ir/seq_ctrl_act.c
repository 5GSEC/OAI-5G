#include "seq_ctrl_act.h"

#include <assert.h>

void free_seq_ctrl_act(seq_ctrl_act_t* src)
{
  assert(src != NULL);
  //Control Action ID
  //Mandatory
  //9.3.6
  // [1 - 65535]
  assert(src->ctrl_act_id > 0);

  // Control Action Parameters
  // Optional
  // 9.2.1.7.1 E2SM-RC Control Message Format 1
  if(src->ctrl_msg_frmt_1 != NULL){
    free_e2sm_rc_ctrl_msg_frmt_1(src->ctrl_msg_frmt_1);
    free(src->ctrl_msg_frmt_1);
  }

}

bool eq_seq_ctrl_act(seq_ctrl_act_t const* m0,  seq_ctrl_act_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //Control Action ID
  //Mandatory
  //9.3.6
  // [1 - 65535]
  assert(m0->ctrl_act_id > 0);
  assert(m1->ctrl_act_id > 0);
  if(m0->ctrl_act_id != m1->ctrl_act_id)
    return false;

  // Control Action Parameters
  // Optional
  // 9.2.1.7.1 E2SM-RC Control Message Format 1
  if(eq_e2sm_rc_ctrl_msg_frmt_1(m0->ctrl_msg_frmt_1, m1->ctrl_msg_frmt_1) == false)
    return false;

  return true;
}

seq_ctrl_act_t cp_seq_ctrl_act(seq_ctrl_act_t const* src)
{
  assert(src != NULL);
  seq_ctrl_act_t dst = {0}; 

  //Control Action ID
  //Mandatory
  //9.3.6
  // [1 - 65535]
  assert(src->ctrl_act_id > 0);
  dst.ctrl_act_id = src->ctrl_act_id;

  // Control Action Parameters
  // BUG, BUG, BUG in the standard 
  // Optional in english. Mandatory in ASN. 
  // 9.2.1.7.1 E2SM-RC Control Message Format 1
  if(src->ctrl_msg_frmt_1 != NULL){
    dst.ctrl_msg_frmt_1 = calloc(1, sizeof(e2sm_rc_ctrl_msg_frmt_1_t));
    assert(dst.ctrl_msg_frmt_1 != NULL && "Memory exhausted");

    *dst.ctrl_msg_frmt_1 = cp_e2sm_rc_ctrl_msg_frmt_1(src->ctrl_msg_frmt_1); 
  }

  return dst;
}

