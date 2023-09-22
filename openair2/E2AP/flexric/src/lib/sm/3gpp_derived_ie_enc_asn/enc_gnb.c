#include <assert.h>
#include <stdio.h>

#include "../../../util/conversions.h"
#include "../../../sm/rc_sm/ie/asn/asn_constant.h"
#include "../../../sm/rc_sm/ie/asn/asn_SEQUENCE_OF.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-F1AP-ID-Item.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-F1AP-ID-List.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-Item.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"

#include "enc_global_gnb_id.h"
#include "enc_global_ng_ran.h"

#include "enc_gnb.h"


UEID_GNB_t* enc_gNB_UE_asn(const gnb_e2sm_t* gnb)
{
  UEID_GNB_t * gnb_asn = calloc(1, sizeof(UEID_GNB_t));
  assert(gnb_asn != NULL && "Memory exhausted");

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  // INTEGER (0..2^40 -1)
  assert(gnb->amf_ue_ngap_id < 1UL << 40); 

  gnb_asn->amf_UE_NGAP_ID.buf = calloc(5, sizeof(uint8_t));
  assert(gnb_asn->amf_UE_NGAP_ID.buf != NULL && "Memory exhausted");

  //memcpy(gnb_asn->amf_UE_NGAP_ID.buf, &gnb->amf_ue_ngap_id, 5);
  //gnb_asn->amf_UE_NGAP_ID.size = 5;
  asn_ulong2INTEGER(&gnb_asn->amf_UE_NGAP_ID, gnb->amf_ue_ngap_id);

  // GUAMI
  MCC_MNC_TO_PLMNID(gnb->guami.plmn_id.mcc, gnb->guami.plmn_id.mnc, gnb->guami.plmn_id.mnc_digit_len, &gnb_asn->guami.pLMNIdentity);
  
  gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string(gnb->guami.amf_region_id);

  gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string(gnb->guami.amf_set_id);

  gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string(gnb->guami.amf_ptr);

  // gNB-CU UE F1AP ID List
  // C-ifCUDUseparated 
  if (gnb->gnb_cu_ue_f1ap_lst != NULL)
  {
    assert(gnb->gnb_cu_ue_f1ap_lst_len >=1 && gnb->gnb_cu_ue_f1ap_lst_len <= maxF1APid);

    gnb_asn->gNB_CU_UE_F1AP_ID_List = calloc(gnb->gnb_cu_ue_f1ap_lst_len, sizeof(UEID_GNB_CU_F1AP_ID_List_t));

    for (size_t i = 0; i < gnb->gnb_cu_ue_f1ap_lst_len; i++)
    {
      UEID_GNB_CU_CP_F1AP_ID_Item_t * f1_item = calloc(1, sizeof(UEID_GNB_CU_CP_F1AP_ID_Item_t));
      memcpy(&f1_item->gNB_CU_UE_F1AP_ID, &gnb->gnb_cu_ue_f1ap_lst[i], 4);
      int rc1 = ASN_SEQUENCE_ADD(&gnb_asn->gNB_CU_UE_F1AP_ID_List->list, f1_item);
      assert(rc1 == 0);
    }
  }


  //gNB-CU-CP UE E1AP ID List
  //C-ifCPUPseparated 

  if (gnb->gnb_cu_cp_ue_e1ap_lst != NULL)
  {
    assert(gnb->gnb_cu_cp_ue_e1ap_lst_len >= 1 && gnb->gnb_cu_cp_ue_e1ap_lst_len <= maxE1APid);

    gnb_asn->gNB_CU_CP_UE_E1AP_ID_List = calloc(gnb->gnb_cu_cp_ue_e1ap_lst_len, sizeof(UEID_GNB_CU_CP_E1AP_ID_List_t));

    for (size_t i = 0; i < gnb->gnb_cu_cp_ue_e1ap_lst_len; i++)
    {
      UEID_GNB_CU_CP_E1AP_ID_Item_t * e1_item = calloc(1, sizeof(UEID_GNB_CU_CP_E1AP_ID_Item_t));
      memcpy(&e1_item->gNB_CU_CP_UE_E1AP_ID, &gnb->gnb_cu_cp_ue_e1ap_lst[i], 4);
      int rc1 = ASN_SEQUENCE_ADD(&gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list, e1_item);
      assert(rc1 == 0);
    }

  }

  // RAN UE ID
  // Optional

  if (gnb->ran_ue_id != NULL)
  {
    gnb_asn->ran_UEID = calloc(1, sizeof(*gnb_asn->ran_UEID));
    gnb_asn->ran_UEID->buf = calloc(8, sizeof(*gnb_asn->ran_UEID->buf));
    memcpy(gnb_asn->ran_UEID->buf, gnb->ran_ue_id, 8);
    gnb_asn->ran_UEID->size = 8;
  }



  //  M-NG-RAN node UE XnAP ID
  // C- ifDCSetup

  if (gnb->ng_ran_node_ue_xnap_id != NULL)
  {
    gnb_asn->m_NG_RAN_UE_XnAP_ID = calloc(1, sizeof(*gnb_asn->m_NG_RAN_UE_XnAP_ID));
    memcpy(gnb_asn->m_NG_RAN_UE_XnAP_ID, gnb->ng_ran_node_ue_xnap_id, 4);
  }


  // Global gNB ID
  // 6.2.3.3
  // Optional

  if (gnb->global_gnb_id != NULL)
    gnb_asn->globalGNB_ID = enc_global_gnb_id_asn(gnb->global_gnb_id);


  // Global NG-RAN Node ID
  // C-ifDCSetup
  if (gnb->global_ng_ran_node_id != NULL)
    gnb_asn->globalNG_RANNode_ID = enc_global_ng_ran_asn(gnb->global_ng_ran_node_id);


  return gnb_asn;
}
