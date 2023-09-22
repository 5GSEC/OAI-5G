#include "e2sm_rc_ev_trg_frmt_5.h"

#include <assert.h>

void free_e2sm_rc_ev_trg_frmt_5(e2sm_rc_ev_trg_frmt_5_t* src)
{
  // On Demand
  // Mandatory
  // on_demand_frmt_5_e on_mand;

  // Associated UE Info
  // Optional
  // 9.3.26
  assert(src->assoc_ue_info == NULL && "Not implemented"); 

  // Associated Cell Info
  // Optional
  // 9.3.27
  assert(src->assoc_cell_info == NULL && "Not implemented");

}

bool eq_e2sm_rc_ev_trg_frmt_5(e2sm_rc_ev_trg_frmt_5_t const* m0, e2sm_rc_ev_trg_frmt_5_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  // On Demand
  // Mandatory
  if(m0->on_demand != m1->on_demand)
    return false;

  // Associated UE Info
  // Optional
  // 9.3.26
  assert(m0->assoc_ue_info == NULL && "Not implemented");
  assert(m1->assoc_ue_info == NULL && "Not implemented");

  // Associated Cell Info
  // Optional
  // 9.3.27
  assert(m0->assoc_cell_info == NULL && "Not implemented");
  assert(m1->assoc_cell_info == NULL && "Not implemented");

  return true;
}

e2sm_rc_ev_trg_frmt_5_t cp_e2sm_rc_ev_trg_frmt_5(e2sm_rc_ev_trg_frmt_5_t const* src)
{
  assert(src != NULL);

  e2sm_rc_ev_trg_frmt_5_t dst = {0};
  // On Demand
  // Mandatory
  dst.on_demand = src->on_demand;

  // Associated UE Info
  // Optional
  // 9.3.26
  assert(src->assoc_ue_info == NULL && "Not implemented");

  // Associated Cell Info
  // Optional
  // 9.3.27
  assert(src->assoc_cell_info == NULL && "not implemented");

  return dst;
}

