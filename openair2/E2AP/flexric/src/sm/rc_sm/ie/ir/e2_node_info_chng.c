#include "e2_node_info_chng.h"

#include <assert.h>
#include <stdlib.h>

void free_e2_node_info_chng(e2_node_info_chng_t* src)
{
  assert(src != NULL);

  //Event Trigger Condition ID
  //Mandatory
  //9.3.21
  // INTEGER (1..65535, …)
  // uint16_t ev_trigger_cond_id;

  // E2 Node Information Change ID
  // Mandatory
  // INTEGER (1..512, ...)
  // uint16_t e2_node_info_chng_id;

  // Associated Cell Info
  // Optional
  // 9.3.27
  assert(src->ev_trigger_cell_info == NULL && "Not implemented"); 

  // Logical OR
  // Optional
  // 9.3.25
  assert(src-> log_or == NULL && "Not implemented");

}

bool eq_e2_node_info_chng(e2_node_info_chng_t const* m0, e2_node_info_chng_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //Event Trigger Condition ID
  //Mandatory
  //9.3.21
  // INTEGER (1..65535, …)
  assert(m0->ev_trigger_cond_id);
  assert(m1->ev_trigger_cond_id);

  // E2 Node Information Change ID
  // Mandatory
  // INTEGER (1..512, ...)
  assert(m0->e2_node_info_chng_id > 0 &&  m0->e2_node_info_chng_id < 513);
  assert(m1->e2_node_info_chng_id > 0 &&  m1->e2_node_info_chng_id < 513);

  // Associated Cell Info
  // Optional
  // 9.3.27
  assert(m0->ev_trigger_cell_info == NULL && "Not implemented");
  assert(m1->ev_trigger_cell_info == NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(m0->log_or == NULL && "Not implemented");
  assert(m1->log_or == NULL && "Not implemented");

  return true;
}

e2_node_info_chng_t cp_e2_node_info_chng( e2_node_info_chng_t const* src)
{
  assert(src != NULL);

  e2_node_info_chng_t dst = {0}; 

  //Event Trigger Condition ID
  //Mandatory
  //9.3.21
  // INTEGER (1..65535, …)
  assert(src->ev_trigger_cond_id > 0);  
  dst.ev_trigger_cond_id = src->ev_trigger_cond_id;

  // E2 Node Information Change ID
  // Mandatory
  // INTEGER (1..512, ...)
  assert(src->e2_node_info_chng_id > 0 && src->e2_node_info_chng_id < 513);
  dst.e2_node_info_chng_id = src->e2_node_info_chng_id;

  // Associated Cell Info
  // Optional
  // 9.3.27
  assert(src->ev_trigger_cell_info == NULL && "Not implemented");

  // Logical OR
  // Optional
  // 9.3.25
  assert(src->log_or == NULL && "Not implemeneted");

  return dst;
}

