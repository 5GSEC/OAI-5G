#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "gnb.h"


void free_gnb_ue_id_e2sm(gnb_e2sm_t * src)
{
    assert(src != NULL);

    if (src->gnb_cu_ue_f1ap_lst_len != 0)
        free(src->gnb_cu_ue_f1ap_lst);

    if (src->gnb_cu_cp_ue_e1ap_lst_len != 0)
      free(src->gnb_cu_cp_ue_e1ap_lst);

    if (src->ran_ue_id != NULL)
        free(src->ran_ue_id);

    if (src->ng_ran_node_ue_xnap_id != NULL)
        free(src->ng_ran_node_ue_xnap_id);

    if (src->global_gnb_id != NULL)
        free(src->global_gnb_id);

    if (src->global_ng_ran_node_id != NULL)
        free(src->global_ng_ran_node_id);

}


bool eq_gnb_ue_id_e2sm(gnb_e2sm_t const * m0, gnb_e2sm_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    // AMF UE NGAP ID
    if (m0->amf_ue_ngap_id != m1->amf_ue_ngap_id)
      return false;
    
    // GUAMI
    if (eq_guami(&m0->guami, &m1->guami) != true)
      return false;
    
    // gNB-CU UE F1AP ID List
    if (m0->gnb_cu_ue_f1ap_lst_len != m1->gnb_cu_ue_f1ap_lst_len)
      return false;
    
    for (size_t i = 0; i < m0->gnb_cu_ue_f1ap_lst_len; i++)
    {
        if (m0->gnb_cu_ue_f1ap_lst[i] != m1->gnb_cu_ue_f1ap_lst[i])
          return false;
    }
    
    // gNB-CU-CP UE E1AP
    if (m0->gnb_cu_cp_ue_e1ap_lst_len != m1->gnb_cu_cp_ue_e1ap_lst_len)
      return false;
    
    for (size_t i = 0; i < m0->gnb_cu_cp_ue_e1ap_lst_len; i++)
    {
        if (m0->gnb_cu_cp_ue_e1ap_lst[i] != m1->gnb_cu_cp_ue_e1ap_lst[i])
          return false;
    }
    
    // RAN UE ID
    if (m0->ran_ue_id != NULL && m1->ran_ue_id != NULL && *m0->ran_ue_id != *m1->ran_ue_id)
      return false;
    
    // M-NG-RAN node UE XnAP ID
    if (m0->ng_ran_node_ue_xnap_id != NULL && m1->ng_ran_node_ue_xnap_id != NULL && *m0->ng_ran_node_ue_xnap_id != *m1->ng_ran_node_ue_xnap_id)
      return false;
    
    // Global gNB ID
    if (m0->global_gnb_id != NULL && m1->global_gnb_id != NULL)
    {
      if (eq_global_gnb_id(m0->global_gnb_id, m1->global_gnb_id) != true)
        return false;
    }
    

    // Global NG-RAN Node ID
    if (m0->global_ng_ran_node_id != NULL && m1->global_ng_ran_node_id != NULL)
      if (eq_global_ng_ran_node_id(m0->global_ng_ran_node_id, m0->global_ng_ran_node_id) != true)
        return false;

    return true;
}


gnb_e2sm_t cp_gnb_ue_id_e2sm(const gnb_e2sm_t * src)
{
  assert(src != NULL);

  gnb_e2sm_t dst = {0};

  // AMF UE NGAP ID
  memcpy(&dst.amf_ue_ngap_id, &src->amf_ue_ngap_id, 8);
    
  // GUAMI
  dst.guami = cp_guami(&src->guami);

  // gNB-CU UE F1AP ID List
  dst.gnb_cu_ue_f1ap_lst_len = src->gnb_cu_ue_f1ap_lst_len;
  if(dst.gnb_cu_ue_f1ap_lst_len > 0) 
    dst.gnb_cu_ue_f1ap_lst = calloc(dst.gnb_cu_ue_f1ap_lst_len, sizeof(uint32_t));

  for (size_t i = 0; i<dst.gnb_cu_ue_f1ap_lst_len; i++)
  {
    memcpy(&dst.gnb_cu_ue_f1ap_lst[i], &src->gnb_cu_ue_f1ap_lst[i], 4);
  }

  // gNB-CU-CP UE E1AP
  dst.gnb_cu_cp_ue_e1ap_lst_len = src->gnb_cu_cp_ue_e1ap_lst_len;
  
  if(dst.gnb_cu_cp_ue_e1ap_lst_len > 0)
    dst.gnb_cu_cp_ue_e1ap_lst = calloc(dst.gnb_cu_cp_ue_e1ap_lst_len, sizeof(uint32_t));

  for (size_t i = 0; i<dst.gnb_cu_cp_ue_e1ap_lst_len; i++)
  {
    memcpy(&dst.gnb_cu_cp_ue_e1ap_lst[i], &src->gnb_cu_cp_ue_e1ap_lst[i], 4);
  }

  // RAN UE ID
  if (src->ran_ue_id) {
    dst.ran_ue_id = calloc(1, sizeof(uint64_t));
    memcpy(dst.ran_ue_id, src->ran_ue_id, 8); 
  }

  // M-NG-RAN node UE XnAP ID
  if (src->ng_ran_node_ue_xnap_id) {
    dst.ng_ran_node_ue_xnap_id = calloc(1, sizeof(uint32_t));
    memcpy(dst.ng_ran_node_ue_xnap_id, src->ng_ran_node_ue_xnap_id, 4); 
  }

  // Global gNB ID
  if (src->global_gnb_id != NULL)
  {
    dst.global_gnb_id = calloc(1, sizeof(global_gnb_id_t));
    assert(dst.global_gnb_id != NULL && "Memory exhausted" );

    *dst.global_gnb_id = cp_global_gnb_id(src->global_gnb_id);
  }

  // Global NG-RAN Node ID
  if (src->global_ng_ran_node_id != NULL)
  {
    dst.global_ng_ran_node_id = calloc(1, sizeof(global_ng_ran_node_id_t));
    *dst.global_ng_ran_node_id = cp_global_ng_ran_node_id(src->global_ng_ran_node_id);
  }

  return dst;
}

