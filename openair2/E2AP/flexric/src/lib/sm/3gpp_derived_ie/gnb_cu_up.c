#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "gnb_cu_up.h"

void free_gnb_cu_up_ue_id_e2sm(gnb_cu_up_e2sm_t * src)
{
    assert(src != NULL);

    if (src->ran_ue_id != NULL)
        free(src->ran_ue_id);

}

bool eq_gnb_cu_up_ue_id_e2sm(gnb_cu_up_e2sm_t const * m0, gnb_cu_up_e2sm_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    // gNB CU CP UE E1AP
    if (m0->gnb_cu_cp_ue_e1ap != m1->gnb_cu_cp_ue_e1ap)
      return false;

    // RAN UE ID
    if (m0->ran_ue_id != NULL && m1->ran_ue_id != NULL && *m0->ran_ue_id != *m1->ran_ue_id)
      return false;

    return true;
}

gnb_cu_up_e2sm_t cp_gnb_cu_up_id_e2sm(const gnb_cu_up_e2sm_t * src)
{
  assert(src != NULL);

  gnb_cu_up_e2sm_t dst = {0};

  // gNB CU CP UE E1AP
  memcpy(&dst.gnb_cu_cp_ue_e1ap, &src->gnb_cu_cp_ue_e1ap, 4);

  // RAN UE ID
  if (src->ran_ue_id) {
    dst.ran_ue_id = calloc(1, sizeof(uint64_t));
    memcpy(dst.ran_ue_id, src->ran_ue_id, 8); 
  }


  return dst;
}
