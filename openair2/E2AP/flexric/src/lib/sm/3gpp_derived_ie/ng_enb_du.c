#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ng_enb_du.h"

void free_ng_enb_du_ue_id_e2sm(ng_enb_du_e2sm_t * src)
{
    assert(src != NULL);

    // No memory allocated in the heap
    (void)src;

}

bool eq_ng_enb_du_ue_id_e2sm(ng_enb_du_e2sm_t const * m0, ng_enb_du_e2sm_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (m0->ng_enb_cu_ue_w1ap_id != m1->ng_enb_cu_ue_w1ap_id)
        return false;

    return true;
}


ng_enb_du_e2sm_t cp_ng_enb_du_ue_id_e2sm(const ng_enb_du_e2sm_t * src)
{
  assert(src != NULL);

  ng_enb_du_e2sm_t dst = {0};

  memcpy(&dst.ng_enb_cu_ue_w1ap_id, &src->ng_enb_cu_ue_w1ap_id, 4);

  return dst;
}
