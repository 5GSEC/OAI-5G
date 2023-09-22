#include <assert.h>

#include "../../../util/conversions.h"
#include "../../../sm/rc_sm/ie/asn/asn_constant.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-F1AP-ID-List.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"

#include "dec_gnb.h"
#include "dec_global_gnb_id.h"
#include "dec_global_ng_ran.h"


gnb_e2sm_t dec_gNB_UE_asn(const UEID_GNB_t * gnb_asn)
{
    gnb_e2sm_t gnb = {0};

    // 6.2.3.16
    // Mandatory
    // AMF UE NGAP ID
    assert(gnb_asn->amf_UE_NGAP_ID.buf != NULL);
    assert(gnb_asn->amf_UE_NGAP_ID.size < 6); // 2^40 max -> 2^(8*5) max 

    asn_INTEGER2ulong(&gnb_asn->amf_UE_NGAP_ID, &gnb.amf_ue_ngap_id );
//    memcpy(&gnb.amf_ue_ngap_id, gnb_asn->amf_UE_NGAP_ID.buf, gnb_asn->amf_UE_NGAP_ID.size);
    assert(gnb.amf_ue_ngap_id < (1UL << 40) );


    // GUAMI

    PLMNID_TO_MCC_MNC(&gnb_asn->guami.pLMNIdentity, gnb.guami.plmn_id.mcc, gnb.guami.plmn_id.mnc, gnb.guami.plmn_id.mnc_digit_len);

    gnb.guami.amf_region_id = cp_amf_region_id_to_u8(gnb_asn->guami.aMFRegionID);

    gnb.guami.amf_set_id = cp_amf_set_id_to_u16(gnb_asn->guami.aMFSetID);

    gnb.guami.amf_ptr = cp_amf_ptr_to_u8(gnb_asn->guami.aMFPointer);


    // gNB-CU UE F1AP ID List
    // C-ifCUDUseparated 
    if (gnb_asn->gNB_CU_UE_F1AP_ID_List != NULL)
    {
        assert(gnb_asn->gNB_CU_UE_F1AP_ID_List->list.count >= 1 && gnb_asn->gNB_CU_UE_F1AP_ID_List->list.count <= maxF1APid);
        gnb.gnb_cu_ue_f1ap_lst_len = gnb_asn->gNB_CU_UE_F1AP_ID_List->list.count;
        gnb.gnb_cu_ue_f1ap_lst = calloc(gnb.gnb_cu_ue_f1ap_lst_len, sizeof(*gnb.gnb_cu_ue_f1ap_lst));

        for (size_t i = 0; i < gnb.gnb_cu_ue_f1ap_lst_len; i++)
        {
            memcpy(&gnb.gnb_cu_ue_f1ap_lst[i], gnb_asn->gNB_CU_UE_F1AP_ID_List->list.array[i], 4);
        }
    }
    else
    {
        gnb.gnb_cu_ue_f1ap_lst_len = 0;
    }

    
    //gNB-CU-CP UE E1AP ID List
    //C-ifCPUPseparated 

    if (gnb_asn->gNB_CU_CP_UE_E1AP_ID_List != NULL)
    {
        assert(gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.count >= 1 && gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.count <= maxE1APid);
        gnb.gnb_cu_cp_ue_e1ap_lst_len = gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.count;
        gnb.gnb_cu_cp_ue_e1ap_lst = calloc(gnb.gnb_cu_cp_ue_e1ap_lst_len, sizeof(*gnb.gnb_cu_cp_ue_e1ap_lst));

        for (size_t i = 0; i < gnb.gnb_cu_cp_ue_e1ap_lst_len; i++)
        {
            memcpy(&gnb.gnb_cu_cp_ue_e1ap_lst[i], gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.array[i], 4);
        }
    }
    else
    {
        gnb.gnb_cu_cp_ue_e1ap_lst_len = 0;
    }


    // RAN UE ID
    // Optional
    if (gnb_asn->ran_UEID != NULL)
    {
        gnb.ran_ue_id = calloc(1, sizeof(*gnb.ran_ue_id));
        assert(gnb.ran_ue_id != NULL && "Memory exhausted" );
        
        assert(gnb_asn->ran_UEID->buf != NULL);
        assert(gnb_asn->ran_UEID->size == 8);

        memcpy(gnb.ran_ue_id, gnb_asn->ran_UEID->buf, 8);
    }
    


    //  M-NG-RAN node UE XnAP ID
    // C- ifDCSetup
    if (gnb_asn->m_NG_RAN_UE_XnAP_ID != NULL)
    {
        gnb.ng_ran_node_ue_xnap_id = calloc(1, sizeof(*gnb.ng_ran_node_ue_xnap_id));
        memcpy(gnb.ng_ran_node_ue_xnap_id, gnb_asn->m_NG_RAN_UE_XnAP_ID, 4);
    }
    

    // Global gNB ID
    // 6.2.3.3
    // Optional
    if(gnb_asn->globalGNB_ID != NULL)
    {
        gnb.global_gnb_id = calloc(1, sizeof(*gnb.global_gnb_id));
        *gnb.global_gnb_id = dec_global_gnb_id_asn(gnb_asn->globalGNB_ID);
    }
      

    // Global NG-RAN Node ID
    // C-ifDCSetup
    if(gnb_asn->globalNG_RANNode_ID != NULL)
      gnb.global_ng_ran_node_id = dec_global_ng_ran_asn(gnb_asn->globalNG_RANNode_ID);


    return gnb;
}
