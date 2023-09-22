#include <assert.h>

#include "../../../util/conversions.h"

#include "dec_ng_enb.h"
#include "dec_global_ng_enb.h"
#include "dec_global_ng_ran.h"


ng_enb_e2sm_t dec_ng_eNB_UE_asn(const UEID_NG_ENB_t * ng_enb_asn)
{
    ng_enb_e2sm_t ng_enb = {0};

    // AMF UE NGAP ID
    assert(ng_enb_asn->amf_UE_NGAP_ID.buf != NULL);
    memcpy(&ng_enb.amf_ue_ngap_id, ng_enb_asn->amf_UE_NGAP_ID.buf, 5);
    assert(ng_enb.amf_ue_ngap_id < (1UL << 40) );

    // GUAMI

    PLMNID_TO_MCC_MNC(&ng_enb_asn->guami.pLMNIdentity, ng_enb.guami.plmn_id.mcc, ng_enb.guami.plmn_id.mnc, ng_enb.guami.plmn_id.mnc_digit_len);

    ng_enb.guami.amf_region_id = cp_amf_region_id_to_u8(ng_enb_asn->guami.aMFRegionID);

    ng_enb.guami.amf_set_id = cp_amf_set_id_to_u16(ng_enb_asn->guami.aMFSetID);

    ng_enb.guami.amf_ptr = cp_amf_ptr_to_u8(ng_enb_asn->guami.aMFPointer);


    // C-if CU DU separated
    // ng-eNB-CU UE W1AP ID

    if (ng_enb_asn->ng_eNB_CU_UE_W1AP_ID != NULL)
    {
        ng_enb.ng_enb_cu_ue_w1ap_id = calloc(1, sizeof(*ng_enb.ng_enb_cu_ue_w1ap_id));
        memcpy(ng_enb.ng_enb_cu_ue_w1ap_id, ng_enb_asn->ng_eNB_CU_UE_W1AP_ID, 4);
    }


    // C- ifDCSetup
    // M-NG-RAN node UE XnAP ID

    if (ng_enb_asn->m_NG_RAN_UE_XnAP_ID != NULL)
    {
        ng_enb.ng_ran_node_ue_xnap_id = calloc(1, sizeof(*ng_enb.ng_ran_node_ue_xnap_id));
        memcpy(ng_enb.ng_ran_node_ue_xnap_id, ng_enb_asn->m_NG_RAN_UE_XnAP_ID, 4);
    }


    // Global NG eNB
    // Optional
    if (ng_enb_asn->globalNgENB_ID != NULL)
    {
      ng_enb.global_ng_enb_id = calloc(1, sizeof(*ng_enb.global_ng_enb_id));
      *ng_enb.global_ng_enb_id = dec_global_ng_enb_asn(ng_enb_asn->globalNgENB_ID);
    }
      

    // Global NG-RAN Node ID
    // C-ifDCSetup
    if (ng_enb_asn->globalNG_RANNode_ID != NULL)
      ng_enb.global_ng_ran_node_id = dec_global_ng_ran_asn(ng_enb_asn->globalNG_RANNode_ID);

    return ng_enb;
}
