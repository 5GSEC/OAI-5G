#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "enb.h"

void free_enb_ue_id_e2sm(enb_e2sm_t * src)
{
    assert(src != NULL);

    if (src->enb_ue_x2ap_id != NULL)
        free(src->enb_ue_x2ap_id);

    if (src->enb_ue_x2ap_id_extension != NULL)
      free(src->enb_ue_x2ap_id_extension);

    if (src->global_enb_id != NULL)
      free(src->global_enb_id);

}

bool eq_enb_ue_id_e2sm(enb_e2sm_t const * m0, enb_e2sm_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    // 6.2.3.26
    // Mandatory
    // MME UE S1AP ID
    if (m0->mme_ue_s1ap_id != m1->mme_ue_s1ap_id)
      return false;

    // 6.2.3.18
    // Mandatory
    // GUMMEI
    if (eq_e2sm_gummei(&m0->gummei, &m1->gummei) != true)
      return false;

    // 6.2.3.23
    // C-ifDCSetup
    // MeNB UE X2AP ID
    if (m0->enb_ue_x2ap_id != NULL && m1->enb_ue_x2ap_id != NULL && *m0->enb_ue_x2ap_id != *m1->enb_ue_x2ap_id)
      return false;

    // 6.2.3.24
    // C-ifDCSetup
    // MeNB UE X2AP ID Extension
    if (m0->enb_ue_x2ap_id_extension != NULL && m1->enb_ue_x2ap_id_extension != NULL && *m0->enb_ue_x2ap_id_extension != *m1->enb_ue_x2ap_id_extension)
      return false;

    // 6.2.3.9
    // C-ifDCSetup
    // Global eNB ID
    if (eq_global_enb_id(m0->global_enb_id, m1->global_enb_id) != true)
      return false;


    return true;
}

enb_e2sm_t cp_enb_ue_id_e2sm(const enb_e2sm_t * src)
{
  assert(src != NULL);

  enb_e2sm_t dst = {0};

  // 6.2.3.26
  // Mandatory
  // MME UE S1AP ID
  memcpy(&dst.mme_ue_s1ap_id, &src->mme_ue_s1ap_id, 4);

  // 6.2.3.18
  // Mandatory
  // GUMMEI
  dst.gummei = cp_e2sm_gummei(&src->gummei);

  // 6.2.3.23
  // C-ifDCSetup
  // MeNB UE X2AP ID
  if (src->enb_ue_x2ap_id != NULL)
  {
    dst.enb_ue_x2ap_id = calloc(1, sizeof(uint16_t));
    memcpy(dst.enb_ue_x2ap_id, src->enb_ue_x2ap_id, 2);
  }

  // 6.2.3.24
  // C-ifDCSetup
  // MeNB UE X2AP ID Extension
  if (src->enb_ue_x2ap_id_extension != NULL)
  {
    dst.enb_ue_x2ap_id_extension = calloc(1, sizeof(uint16_t));
    memcpy(dst.enb_ue_x2ap_id_extension, src->enb_ue_x2ap_id_extension, 2);
  }

  // 6.2.3.9
  // C-ifDCSetup
  // Global eNB ID
  if (src->global_enb_id != NULL)
  {
    dst.global_enb_id = calloc(1, sizeof(global_enb_id_t));
    assert( dst.global_enb_id != NULL && "Memory exhausted");

    *dst.global_enb_id = cp_global_enb_ue_id_e2sm(src->global_enb_id);
  }

  return dst;
}
