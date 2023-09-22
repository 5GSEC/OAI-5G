#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ng_enb.h"

void free_ng_enb_ue_id_e2sm(ng_enb_e2sm_t * src)
{
    assert(src != NULL);

    if (src->ng_enb_cu_ue_w1ap_id != NULL)
        free(src->ng_enb_cu_ue_w1ap_id);

    if (src->ng_ran_node_ue_xnap_id != NULL)
      free(src->ng_ran_node_ue_xnap_id);

    if (src->global_ng_enb_id != NULL)
          free(src->global_ng_enb_id);

    if (src->global_ng_ran_node_id != NULL)
          free(src->global_ng_ran_node_id);

}

bool eq_ng_enb_ue_id_e2sm(ng_enb_e2sm_t const * m0, ng_enb_e2sm_t const * m1)
{

    // AMF UE NGAP ID
    if (m0->amf_ue_ngap_id != m1->amf_ue_ngap_id)
      return false;
    
    // GUAMI
    if (eq_guami(&m0->guami, &m1->guami) != true)
      return false;
    
    // 6.2.3.22
    // C-if CU DU separated
    // ng-eNB-CU UE W1AP ID
    if (m0->ng_enb_cu_ue_w1ap_id != NULL && m1->ng_enb_cu_ue_w1ap_id != NULL && *m0->ng_enb_cu_ue_w1ap_id != *m1->ng_enb_cu_ue_w1ap_id)
      return false;
    
    // 6.2.3.19
    // C- ifDCSetup
    // M-NG-RAN node UE XnAP ID
    if (m0->ng_ran_node_ue_xnap_id != NULL && m1->ng_ran_node_ue_xnap_id != NULL && *m0->ng_ran_node_ue_xnap_id != *m1->ng_ran_node_ue_xnap_id)
      return false;
    
    // Global NG eNB ID
    if (m0->global_ng_enb_id != NULL && m1->global_ng_enb_id != NULL)
    {
      if (eq_global_ng_enb_id(m0->global_ng_enb_id, m1->global_ng_enb_id) != true)
        return false;
    }

    // Global NG-RAN Node ID
    if (m0->global_ng_ran_node_id != NULL && m1->global_ng_ran_node_id != NULL)
    {
      if (eq_global_ng_ran_node_id(m0->global_ng_ran_node_id, m1->global_ng_ran_node_id) != true)
        return false;
    }


    return true;
}

ng_enb_e2sm_t cp_ng_enb_ue_id_e2sm(const ng_enb_e2sm_t * src)
{
  assert(src != NULL);

  ng_enb_e2sm_t dst = {0};

  // AMF UE NGAP ID
  memcpy(&dst.amf_ue_ngap_id, &src->amf_ue_ngap_id, 8);

  // GUAMI
  dst.guami = cp_guami(&src->guami);

  // 6.2.3.22
  // C-if CU DU separated
  // ng-eNB-CU UE W1AP ID
  if (src->ng_enb_cu_ue_w1ap_id != NULL)
  {
    dst.ng_enb_cu_ue_w1ap_id = calloc(1, sizeof(uint32_t));
    memcpy(dst.ng_enb_cu_ue_w1ap_id, src->ng_enb_cu_ue_w1ap_id, 4);
  }


  // 6.2.3.19
  // C- ifDCSetup
  // M-NG-RAN node UE XnAP ID
  if (src->ng_ran_node_ue_xnap_id != NULL)
  {
    dst.ng_ran_node_ue_xnap_id = calloc(1, sizeof(uint32_t));
    memcpy(dst.ng_ran_node_ue_xnap_id, src->ng_ran_node_ue_xnap_id, 4);
  }

  // Global NG eNB ID
  if (src->global_ng_enb_id != NULL)
  {
    dst.global_ng_enb_id = calloc(1, sizeof(global_ng_enb_id_t));
    *dst.global_ng_enb_id = cp_global_ng_enb_id(src->global_ng_enb_id);
  }

  // Global NG-RAN Node ID
  if (src->global_ng_ran_node_id != NULL)
  {
    dst.global_ng_ran_node_id = calloc(1, sizeof(global_ng_ran_node_id_t));
    *dst.global_ng_ran_node_id = cp_global_ng_ran_node_id(src->global_ng_ran_node_id);
  }


  return dst;
}
