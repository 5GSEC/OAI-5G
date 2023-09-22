#include "seq_ue_info.h"

#include <assert.h>

void free_seq_ue_info(seq_ue_info_t* src)
{
  assert(src != NULL);

  // UE ID
  // Mandatory
  // 9.3.10
  free_ue_id_e2sm(&src->ue_id);

  // UE Context Information
  // Optiona;
  // OCTET STRING
  assert(src->ue_ctx_info == NULL && "not implemented"); 

  // Cell Global ID
  // Mandatory
  // 9.3.36
  free_cell_global_id(&src->cell_global_id);

}

bool eq_seq_ue_info(seq_ue_info_t const* m0,  seq_ue_info_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // UE ID
  // Mandatory
  // 9.3.10
  if(eq_ue_id_e2sm(&m0->ue_id, &m1->ue_id ) == false)
    return false;

  // UE Context Information
  // Optiona;
  // OCTET STRING
  assert(m0->ue_ctx_info == NULL && "Not implemented"); 
  assert(m1->ue_ctx_info == NULL && "Not implemented"); 

  // Cell Global ID
  // Mandatory
  // 9.3.36
  if(eq_cell_global_id(&m0->cell_global_id, &m1->cell_global_id) == false) 
    return false;

  return true;
}

seq_ue_info_t cp_seq_ue_info(seq_ue_info_t const* src)
{
  assert(src != NULL);
  seq_ue_info_t dst = {0}; 

  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = cp_ue_id_e2sm(&src->ue_id);  

  // UE Context Information
  // Optiona;
  // OCTET STRING
  assert(src->ue_ctx_info == NULL && "Not implemented");

  // Cell Global ID
  // Mandatory
  // 9.3.36
  dst.cell_global_id = cp_cell_global_id(&src->cell_global_id);

  return dst;
}


