#include "e2sm_rc_ev_trg_frmt_3.h"

#include <assert.h>



void free_e2sm_rc_ev_trg_frmt_3(e2sm_rc_ev_trg_frmt_3_t* src)
{
  assert(src != NULL);

  assert(src->sz_e2_node_info_chng > 0 && "Not implemented" ); 
  assert(src-> e2_node_info_chng != NULL);

  for(size_t i = 0; i < src->sz_e2_node_info_chng; ++i){
    free_e2_node_info_chng(&src->e2_node_info_chng[i]);
  }

  free(src->e2_node_info_chng);

}

bool eq_e2sm_rc_ev_trg_frmt_3(e2sm_rc_ev_trg_frmt_3_t const* m0, e2sm_rc_ev_trg_frmt_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of E2 Node
  // Information Change
  // [1 - 65535]
  assert(m0->sz_e2_node_info_chng > 0 && m0->sz_e2_node_info_chng < 65535 + 1 && "Out of range");
  assert(m1->sz_e2_node_info_chng > 0 && m1->sz_e2_node_info_chng < 65535 + 1 && "Out of range");
  assert(m0->e2_node_info_chng != NULL);
  assert(m1->e2_node_info_chng != NULL);

  if(m0->sz_e2_node_info_chng != m1->sz_e2_node_info_chng )
    return false;

  for(size_t i = 0; i < m0->sz_e2_node_info_chng; ++i){
    if(eq_e2_node_info_chng(&m0->e2_node_info_chng[i], &m1->e2_node_info_chng[i] ) == false)
      return false;
  }

  return true;
}

e2sm_rc_ev_trg_frmt_3_t cp_e2sm_rc_ev_trg_frmt_3(e2sm_rc_ev_trg_frmt_3_t const* src)
{
  assert(src != NULL);
  e2sm_rc_ev_trg_frmt_3_t dst = {0};

  // Sequence of E2 Node
  // Information Change
  // [1 - 65535]
  assert(src->sz_e2_node_info_chng > 0 && src->sz_e2_node_info_chng <  65536);
  dst.sz_e2_node_info_chng = src->sz_e2_node_info_chng;

  dst.e2_node_info_chng = calloc(dst.sz_e2_node_info_chng, sizeof(e2_node_info_chng_t) );
  assert(dst.e2_node_info_chng != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_e2_node_info_chng; ++i){
    dst.e2_node_info_chng[i] = cp_e2_node_info_chng(&src->e2_node_info_chng[i]);
  }

  return dst;
}

