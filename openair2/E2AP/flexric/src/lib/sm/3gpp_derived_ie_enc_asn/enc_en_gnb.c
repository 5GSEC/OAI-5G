#include <assert.h>

#include "../../../sm/rc_sm/ie/asn/asn_constant.h"
#include "../../../sm/rc_sm/ie/asn/asn_SEQUENCE_OF.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-Item.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"

#include "enc_en_gnb.h"
#include "enc_global_enb_id.h"

UEID_EN_GNB_t* enc_en_gNB_UE_asn(const en_gnb_e2sm_t * en_gnb)
{
    UEID_EN_GNB_t * en_gnb_asn = calloc(1, sizeof(UEID_EN_GNB_t));
    assert(en_gnb_asn != NULL && "Memory exhausted");

    // Mandatory
    // MeNB UE X2AP ID
    memcpy(&en_gnb_asn->m_eNB_UE_X2AP_ID, &en_gnb->enb_ue_x2ap_id, 2);
    assert(en_gnb_asn->m_eNB_UE_X2AP_ID < max_val_ENB_UE_X2AP_ID + 1);


    // OPTIONAL
    // MeNB UE X2AP ID Extension
    if (en_gnb->enb_ue_x2ap_id_extension != NULL)
    {
        en_gnb_asn->m_eNB_UE_X2AP_ID_Extension = calloc(1, sizeof(*en_gnb_asn->m_eNB_UE_X2AP_ID_Extension));
        memcpy(en_gnb_asn->m_eNB_UE_X2AP_ID_Extension, en_gnb->enb_ue_x2ap_id_extension, 2);
        assert(*en_gnb_asn->m_eNB_UE_X2AP_ID_Extension < max_val_ENB_UE_X2AP_ID + 1);
    }


    // Mandatory
    // Global eNB ID
    en_gnb_asn->globalENB_ID = enc_global_enb_id_asn(&en_gnb->global_enb_id);

    // gNB-CU UE F1AP ID
    // C-ifCUDUseparated 
    if (en_gnb->gnb_cu_ue_f1ap_lst != NULL)
    {
        en_gnb_asn->gNB_CU_UE_F1AP_ID = calloc(1, sizeof(*en_gnb_asn->gNB_CU_UE_F1AP_ID));
        memcpy(en_gnb_asn->gNB_CU_UE_F1AP_ID, en_gnb->gnb_cu_ue_f1ap_lst, 4);
    }


    // gNB-CU-CP UE E1AP ID List
    // C-ifCPUPseparated 
    if (en_gnb->gnb_cu_cp_ue_e1ap_lst != NULL)
    {
        assert(en_gnb->gnb_cu_cp_ue_e1ap_lst_len >= 1 && en_gnb->gnb_cu_cp_ue_e1ap_lst_len <= maxE1APid);

        en_gnb_asn->gNB_CU_CP_UE_E1AP_ID_List = calloc(en_gnb->gnb_cu_cp_ue_e1ap_lst_len, sizeof(UEID_GNB_CU_CP_E1AP_ID_List_t));

        for (size_t i = 0; i < en_gnb->gnb_cu_cp_ue_e1ap_lst_len; i++)
        {
            UEID_GNB_CU_CP_E1AP_ID_Item_t * e1_item = calloc(1, sizeof(UEID_GNB_CU_CP_E1AP_ID_Item_t));
            memcpy(&e1_item->gNB_CU_CP_UE_E1AP_ID, &en_gnb->gnb_cu_cp_ue_e1ap_lst[i], 4);
            int rc1 = ASN_SEQUENCE_ADD(&en_gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list, e1_item);
            assert(rc1 == 0);
        }

    }


    // RAN UE ID
    // Optional

    if (en_gnb->ran_ue_id != NULL)
    {
        en_gnb_asn->ran_UEID = calloc(1, sizeof(*en_gnb_asn->ran_UEID));
        en_gnb_asn->ran_UEID->buf = calloc(8, sizeof(*en_gnb_asn->ran_UEID->buf));
        memcpy(en_gnb_asn->ran_UEID->buf, en_gnb->ran_ue_id, 8);
        en_gnb_asn->ran_UEID->size = 8;
    }
    
    return en_gnb_asn;
}
