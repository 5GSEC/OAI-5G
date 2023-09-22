#include <assert.h>

#include "enc_ue_id.h"
#include "../3gpp_derived_ie_enc_asn/enc_gnb.h"
#include "../3gpp_derived_ie_enc_asn/enc_gnb_du.h"
#include "../3gpp_derived_ie_enc_asn/enc_gnb_cu_up.h"
#include "../3gpp_derived_ie_enc_asn/enc_ng_enb.h"
#include "../3gpp_derived_ie_enc_asn/enc_ng_enb_du.h"
#include "../3gpp_derived_ie_enc_asn/enc_en_gnb.h"
#include "../3gpp_derived_ie_enc_asn/enc_enb.h"

UEID_t enc_ue_id_asn(const ue_id_e2sm_t * ue_id)
{
    UEID_t ue_id_asn = {0};

    switch (ue_id->type)
    {
    case GNB_UE_ID_E2SM:
        ue_id_asn.present = UEID_PR_gNB_UEID;
        ue_id_asn.choice.gNB_UEID = enc_gNB_UE_asn(&ue_id->gnb);
        break;
    
    case GNB_DU_UE_ID_E2SM:
        ue_id_asn.present = UEID_PR_gNB_DU_UEID;
        ue_id_asn.choice.gNB_DU_UEID = enc_gNB_DU_UE_asn(&ue_id->gnb_du);
        break;

    case GNB_CU_UP_UE_ID_E2SM:
        ue_id_asn.present = UEID_PR_gNB_CU_UP_UEID;
        ue_id_asn.choice.gNB_CU_UP_UEID = enc_gNB_CU_UP_UE_asn(&ue_id->gnb_cu_up);
        break;

    case NG_ENB_UE_ID_E2SM:
        ue_id_asn.present = UEID_PR_ng_eNB_UEID;
        ue_id_asn.choice.ng_eNB_UEID = enc_ng_eNB_UE_asn(&ue_id->ng_enb);
        break;

    case NG_ENB_DU_UE_ID_E2SM:
        ue_id_asn.present = UEID_PR_ng_eNB_DU_UEID;
        ue_id_asn.choice.ng_eNB_DU_UEID = enc_ng_eNB_DU_UE_asn(&ue_id->ng_enb_du);
        break;

    case EN_GNB_UE_ID_E2SM:
        ue_id_asn.present = UEID_PR_en_gNB_UEID;
        ue_id_asn.choice.en_gNB_UEID = enc_en_gNB_UE_asn(&ue_id->en_gnb);
        break;

    case ENB_UE_ID_E2SM:
        ue_id_asn.present = UEID_PR_eNB_UEID;
        ue_id_asn.choice.eNB_UEID = enc_eNB_UE_asn(&ue_id->enb);
        break;


    default:
        assert(false && "Unknown UE ID Type");
    }


    return ue_id_asn;
}
