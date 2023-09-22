#include "seq_cell_info_2.h"

#include <assert.h>

void free_seq_cell_info_2(seq_cell_info_2_t* src)
{
  assert(src != NULL);

  // Cell Global ID
  // Mandatory
  // 9.3.36
  // 6.2.2.5. 
  free_cell_global_id(&src->cell_global_id);

  // Cell Context Information
  // Optional
  // OCTET STRING
  assert(src->cell_ctx_info == NULL && "Not implemented");

  // Neighbour Relation Table
  // Optional
  // 9.3.38
  assert(src->neighbour_rela_tbl == NULL && "not impemented");

}

bool eq_seq_cell_info_2(seq_cell_info_2_t const* m0, seq_cell_info_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Cell Global ID
  // Mandatory
  // 9.3.36
  // 6.2.2.5. 
  if(eq_cell_global_id(&m0->cell_global_id, &m1->cell_global_id) == false) 
    return false;

  // Cell Context Information
  // Optional
  // OCTET STRING
  assert(m0->cell_ctx_info == NULL && "Not implemented");
  assert(m1->cell_ctx_info == NULL && "Not implemented");

  // Neighbour Relation Table
  // Optional
  // 9.3.38
  assert(m0->neighbour_rela_tbl == NULL && "not implemented"); 
  assert(m1->neighbour_rela_tbl == NULL && "not implemented"); 

  return true;
}


seq_cell_info_2_t cp_seq_cell_info_2(seq_cell_info_2_t const* src)
{
  assert(src != NULL);
  seq_cell_info_2_t dst = {0}; 

  // Cell Global ID
  // Mandatory
  // 9.3.36
  // 6.2.2.5. 
  dst.cell_global_id = cp_cell_global_id(&src->cell_global_id);

  // Cell Context Information
  // Optional
  // OCTET STRING
  assert(src->cell_ctx_info == NULL && "Not implemented");

  // Neighbour Relation Table
  // Optional
  // 9.3.38
  assert(src->neighbour_rela_tbl == NULL && "Not implemented");

  return dst;
}

