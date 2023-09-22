#include <assert.h>

#include "../../../sm/rc_sm/ie/asn/asn_constant.h"
#include "../../../sm/rc_sm/ie/asn/UEID-GNB-CU-CP-E1AP-ID-List.h"

#include "dec_en_gnb.h"
#include "dec_global_enb_id.h"

en_gnb_e2sm_t dec_en_gNB_UE_asn(const UEID_EN_GNB_t * en_gnb_asn)
{
    en_gnb_e2sm_t en_gnb = {0};

    // Mandatory
    // MeNB UE X2AP ID
    memcpy(&en_gnb.enb_ue_x2ap_id, &en_gnb_asn->m_eNB_UE_X2AP_ID, 2);
    assert(en_gnb.enb_ue_x2ap_id < max_val_ENB_UE_X2AP_ID + 1);

    // OPTIONAL
    // MeNB UE X2AP ID Extension
    if (en_gnb_asn->m_eNB_UE_X2AP_ID_Extension != NULL)
    {
        en_gnb.enb_ue_x2ap_id_extension = calloc(1, sizeof(*en_gnb.enb_ue_x2ap_id_extension));
        memcpy(en_gnb.enb_ue_x2ap_id_extension, en_gnb_asn->m_eNB_UE_X2AP_ID_Extension, 2);
        assert(*en_gnb.enb_ue_x2ap_id_extension < max_val_ENB_UE_X2AP_ID + 1);
    }


    // Mandatory
    // Global eNB ID
    en_gnb.global_enb_id = dec_global_enb_id_asn(&en_gnb_asn->globalENB_ID);


    // gNB-CU UE F1AP ID
    // C-ifCUDUseparated 
    if (en_gnb_asn->gNB_CU_UE_F1AP_ID != NULL)
    {
        en_gnb.gnb_cu_ue_f1ap_lst = calloc(1, sizeof(*en_gnb.gnb_cu_ue_f1ap_lst));
        memcpy(en_gnb.gnb_cu_ue_f1ap_lst, en_gnb_asn->gNB_CU_UE_F1AP_ID, 4);
    }


    // gNB-CU-CP UE E1AP ID List
    // C-ifCPUPseparated 

    if (en_gnb_asn->gNB_CU_CP_UE_E1AP_ID_List != NULL)
    {
        assert(en_gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.count >= 1 && en_gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.count <= maxE1APid);
        en_gnb.gnb_cu_cp_ue_e1ap_lst_len = en_gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.count;
        en_gnb.gnb_cu_cp_ue_e1ap_lst = calloc(en_gnb.gnb_cu_cp_ue_e1ap_lst_len, sizeof(*en_gnb.gnb_cu_cp_ue_e1ap_lst));

        for (size_t i = 0; i < en_gnb.gnb_cu_cp_ue_e1ap_lst_len; i++)
        {
            memcpy(&en_gnb.gnb_cu_cp_ue_e1ap_lst[i], en_gnb_asn->gNB_CU_CP_UE_E1AP_ID_List->list.array[i], 4);
        }
    }
    else
    {
        en_gnb.gnb_cu_cp_ue_e1ap_lst_len = 0;
    }


    // RAN UE ID
    // Optional

    if (en_gnb_asn->ran_UEID != NULL)
    {
        en_gnb.ran_ue_id = calloc(1, sizeof(*en_gnb.ran_ue_id));
        assert(en_gnb.ran_ue_id != NULL && "Memory exhausted" );
        
        assert(en_gnb_asn->ran_UEID->buf != NULL);
        assert(en_gnb_asn->ran_UEID->size == 8);

        memcpy(en_gnb.ran_ue_id, en_gnb_asn->ran_UEID->buf, 8);
    }


    return en_gnb;
}
