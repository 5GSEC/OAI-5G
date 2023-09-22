#include "e2sm_rc_ev_trg_frmt_2.h"

#include <assert.h>


void free_e2sm_rc_ev_trg_frmt_2(e2sm_rc_ev_trg_frmt_2_t const* src)
{
  assert(src != NULL);

  //  Call Process Type ID
  //  Mandatory
  //  9.3.15
  //  INTEGER (1.. 65535, …)
  assert(src->call_proc_type_id > 0);

  // Call Breakpoint ID
  // Mandatory
  // 9.3.49
  // INTEGER (1.. 65535, …)
  assert(src->call_break_id > 0);

  // Associated E2 Node Info
  // Optional
  // 9.3.29
  assert(src->assoc_e2_node_info == NULL && "Not implemented");

  // Associated UE Info
  // Optional
  // 9.3.26
  assert(src->assoc_ue_info == NULL && "Not implemented");

} 

bool eq_e2sm_rc_ev_trg_frmt_2(e2sm_rc_ev_trg_frmt_2_t const* m0, e2sm_rc_ev_trg_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //  Call Process Type ID
  //  Mandatory
  //  9.3.15
  //  INTEGER (1.. 65535, …)
  assert(m0->call_proc_type_id > 0 && "Out of range");
  assert(m1->call_proc_type_id > 0 && "Out of range");
  if(m0->call_proc_type_id != m1-> call_proc_type_id)
    return false;

  // Call Breakpoint ID
  // Mandatory
  // 9.3.49
  // INTEGER (1.. 65535, …)
  assert(m0->call_break_id > 0 && "Out of range");
  assert(m1->call_break_id > 0 && "Out of range");
  if(m0-> call_break_id != m1->call_break_id )
    return false;

  // Associated E2 Node Info
  // Optional
  // 9.3.29
  assert(m0->assoc_e2_node_info == NULL && "Not implemented");
  assert(m1->assoc_e2_node_info == NULL && "Not implemented");

  // Associated UE Info
  // Optional
  // 9.3.26
 assert(m0->assoc_ue_info == NULL && "Not implemented");
 assert(m1->assoc_ue_info == NULL && "Not implemented");

  return true;
}

e2sm_rc_ev_trg_frmt_2_t cp_e2sm_rc_ev_trg_frmt_2(e2sm_rc_ev_trg_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ev_trg_frmt_2_t dst = {0}; 

  //  Call Process Type ID
  //  Mandatory
  //  9.3.15
  //  INTEGER (1.. 65535, …)
  assert(src->call_proc_type_id > 0); 
  dst.call_proc_type_id = src->call_proc_type_id;

  // Call Breakpoint ID
  // Mandatory
  // 9.3.49
  // INTEGER (1.. 65535, …)
  assert(src->call_break_id > 0);
  dst.call_break_id = src->call_break_id;

  // Associated E2 Node Info
  // Optional
  // 9.3.29
  assert(src->assoc_e2_node_info == NULL && "Not implemented");

  // Associated UE Info
  // Optional
  // 9.3.26
  assert(src->assoc_ue_info == NULL && "Not implemented");

  return dst;
}

