#include <assert.h>

#include "../../../util/conversions.h"
#include "../../../sm/rc_sm/ie/asn/asn_constant.h"

#include "enc_enb.h"
#include "enc_global_enb_id.h"

UEID_ENB_t * enc_eNB_UE_asn(const enb_e2sm_t * enb)
{
    UEID_ENB_t * enb_asn = calloc(1, sizeof(UEID_ENB_t));
    assert(enb_asn != NULL && "Memory exhausted");

    // Mandatory
    // MME UE S1AP ID
    memcpy(&enb_asn->mME_UE_S1AP_ID, &enb->mme_ue_s1ap_id, 4);

    // Mandatory
    // GUMMEI
    MCC_MNC_TO_PLMNID(enb->gummei.plmn_id.mcc, enb->gummei.plmn_id.mnc, enb->gummei.plmn_id.mnc_digit_len, &enb_asn->gUMMEI.pLMN_Identity);

    MME_GID_TO_OCTET_STRING(enb->gummei.mme_group_id, &enb_asn->gUMMEI.mME_Group_ID);

    MME_CODE_TO_OCTET_STRING(enb->gummei.mme_code, &enb_asn->gUMMEI.mME_Code);


    // C-ifDCSetup
    // MeNB UE X2AP ID
    if (enb->enb_ue_x2ap_id != NULL)
    {
        enb_asn->m_eNB_UE_X2AP_ID = calloc(1, sizeof(*enb_asn->m_eNB_UE_X2AP_ID));
        memcpy(enb_asn->m_eNB_UE_X2AP_ID, enb->enb_ue_x2ap_id, 2);
        assert(*enb_asn->m_eNB_UE_X2AP_ID < max_val_ENB_UE_X2AP_ID + 1);
    }


    // C-ifDCSetup
    // MeNB UE X2AP ID Extension
    if (enb->enb_ue_x2ap_id_extension != NULL)
    {
        enb_asn->m_eNB_UE_X2AP_ID_Extension = calloc(1, sizeof(*enb_asn->m_eNB_UE_X2AP_ID_Extension));
        memcpy(enb_asn->m_eNB_UE_X2AP_ID_Extension, enb->enb_ue_x2ap_id_extension, 2);
        assert(*enb_asn->m_eNB_UE_X2AP_ID_Extension < max_val_ENB_UE_X2AP_ID +1);
    }


    // Global eNB ID
    // C-ifDCSetup
    if (enb->global_enb_id != NULL)
    {
        enb_asn->globalENB_ID = calloc(1, sizeof(GlobalENB_ID_t));
        assert (enb_asn->globalENB_ID != NULL && "Memory exhausted" );
        *enb_asn->globalENB_ID = enc_global_enb_id_asn(enb->global_enb_id);
    }
      

    return enb_asn;
}
