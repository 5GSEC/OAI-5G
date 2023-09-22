#include <assert.h>

#include "dec_ue_id.h"
#include "../3gpp_derived_ie_dec_asn/dec_gnb.h"
#include "../3gpp_derived_ie_dec_asn/dec_gnb_du.h"
#include "../3gpp_derived_ie_dec_asn/dec_gnb_cu_up.h"
#include "../3gpp_derived_ie_dec_asn/dec_ng_enb.h"
#include "../3gpp_derived_ie_dec_asn/dec_ng_enb_du.h"
#include "../3gpp_derived_ie_dec_asn/dec_en_gnb.h"
#include "../3gpp_derived_ie_dec_asn/dec_enb.h"

ue_id_e2sm_t dec_ue_id_asn(const UEID_t * ue_id_asn)
{
    ue_id_e2sm_t ue_id = {0};


    switch (ue_id_asn->present)
    {
    case UEID_PR_gNB_UEID:
        ue_id.type = GNB_UE_ID_E2SM;
        ue_id.gnb = dec_gNB_UE_asn(ue_id_asn->choice.gNB_UEID);
        break;

    case UEID_PR_gNB_DU_UEID:
        ue_id.type = GNB_DU_UE_ID_E2SM;
        ue_id.gnb_du = dec_gNB_DU_UE_asn(ue_id_asn->choice.gNB_DU_UEID);
        break;

    case UEID_PR_gNB_CU_UP_UEID:
        ue_id.type = GNB_CU_UP_UE_ID_E2SM;
        ue_id.gnb_cu_up = dec_gNB_CU_UP_UE_asn(ue_id_asn->choice.gNB_CU_UP_UEID);
        break;

    case UEID_PR_ng_eNB_UEID:
        ue_id.type = NG_ENB_UE_ID_E2SM;
        ue_id.ng_enb = dec_ng_eNB_UE_asn(ue_id_asn->choice.ng_eNB_UEID);
        break;

    case UEID_PR_ng_eNB_DU_UEID:
        ue_id.type = NG_ENB_DU_UE_ID_E2SM;
        ue_id.ng_enb_du = dec_ng_eNB_DU_UE_asn(ue_id_asn->choice.ng_eNB_DU_UEID);
        break;

    case UEID_PR_en_gNB_UEID:
        ue_id.type = EN_GNB_UE_ID_E2SM;
        ue_id.en_gnb = dec_en_gNB_UE_asn(ue_id_asn->choice.en_gNB_UEID);
        break;

    case UEID_PR_eNB_UEID:
        ue_id.type = ENB_UE_ID_E2SM;
        ue_id.enb = dec_eNB_UE_asn(ue_id_asn->choice.eNB_UEID);
        break;

    default:
        assert(false && "Unknown UE ID Type");
    }

    return ue_id;
}
