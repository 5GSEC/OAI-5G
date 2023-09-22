#include <assert.h>

#include "../../../util/conversions.h"
#include "../../../sm/rc_sm/ie/asn/asn_constant.h"

#include "dec_enb.h"
#include "dec_global_enb_id.h"

enb_e2sm_t dec_eNB_UE_asn(const UEID_ENB_t * enb_asn)
{
    enb_e2sm_t enb = {0};


    // Mandatory
    // MME UE S1AP ID
    memcpy(&enb.mme_ue_s1ap_id, &enb_asn->mME_UE_S1AP_ID, 4);


    // Mandatory
    // GUMMEI
    PLMNID_TO_MCC_MNC(&enb_asn->gUMMEI.pLMN_Identity, enb.gummei.plmn_id.mcc, enb.gummei.plmn_id.mnc, enb.gummei.plmn_id.mnc_digit_len);

    OCTET_STRING_TO_INT16(&enb_asn->gUMMEI.mME_Group_ID, enb.gummei.mme_group_id);

    OCTET_STRING_TO_INT8(&enb_asn->gUMMEI.mME_Code, enb.gummei.mme_code);


    // C-ifDCSetup
    // MeNB UE X2AP ID
    if (enb_asn->m_eNB_UE_X2AP_ID != NULL)
    {
        enb.enb_ue_x2ap_id = calloc(1, sizeof(*enb.enb_ue_x2ap_id));
        memcpy(enb.enb_ue_x2ap_id, enb_asn->m_eNB_UE_X2AP_ID, 2);
        assert(*enb.enb_ue_x2ap_id < max_val_ENB_UE_X2AP_ID + 1);
    }


    // C-ifDCSetup
    // MeNB UE X2AP ID Extension
    if (enb_asn->m_eNB_UE_X2AP_ID_Extension != NULL)
    {
        enb.enb_ue_x2ap_id_extension = calloc(1, sizeof(*enb.enb_ue_x2ap_id_extension));
        memcpy(enb.enb_ue_x2ap_id_extension, enb_asn->m_eNB_UE_X2AP_ID_Extension, 2);
        assert(*enb.enb_ue_x2ap_id_extension < max_val_ENB_UE_X2AP_ID + 1);
    }


    // Global eNB ID
    // C-ifDCSetup
    if (enb_asn->globalENB_ID != NULL)
    {
        enb.global_enb_id = calloc(1, sizeof(*enb.global_enb_id));
        assert(enb.global_enb_id != NULL && "Memory exhausted");
        *enb.global_enb_id = dec_global_enb_id_asn(enb_asn->globalENB_ID);
    }
      


    return enb;
}
