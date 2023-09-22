#include "rrc_msg_id.h"

#include <assert.h>

void free_rrc_msg_id( rrc_msg_id_t* src)
{
  assert(src != NULL);

 //rrc_message_id_e type;
 // union{
 //   lte_rrc_class_e lte;
 //   nr_rrc_class_e nr;
 // };

  // Mandatory
  // Integer
  // Number starts from 0 from the first
  // entry of a given RRC message class
  // defined in TS 36.331 [14] or TS
  // 38.331 [15].
//  uint64_t rrc_msg_id;

}



bool eq_rrc_msg_id(rrc_msg_id_t const* m0, rrc_msg_id_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  if(m0->type != m1->type)
    return false;

  if(m0->type == LTE_RRC_MESSAGE_ID){
    if(m0->lte != m1->lte)
      return false;
  } else if(m0->type == NR_RRC_MESSAGE_ID){
    if(m0->nr != m1->nr)
      return false;
  } else {
    assert(0!=0 && "Unknown type");
  }

  // Mandatory
  // Integer
  // Number starts from 0 from the first
  // entry of a given RRC message class
  // defined in TS 36.331 [14] or TS
  // 38.331 [15].
  if(m0->rrc_msg_id != m1->rrc_msg_id)
    return false;

  return true;
}

rrc_msg_id_t cp_rrc_msg_id(rrc_msg_id_t const* src)
{
  assert(src != NULL);

  rrc_msg_id_t dst = {0};
  // Mandatory
  dst.type = src->type;
  if(dst.type == LTE_RRC_MESSAGE_ID){
    dst.lte = src->lte;
  } else if(dst.type == NR_RRC_MESSAGE_ID ){
    dst.nr = src->nr;
  } else {
    assert(0 !=0 && "Unknown type");
  }

  // Mandatory
  // Integer
  // Number starts from 0 from the first
  // entry of a given RRC message class
  // defined in TS 36.331 [14] or TS
  // 38.331 [15].
  dst.rrc_msg_id = src->rrc_msg_id; 

  return dst;
}

