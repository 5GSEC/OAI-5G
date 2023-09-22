#include <assert.h>

#include "enc_ng_enb_du.h"

UEID_NG_ENB_DU_t * enc_ng_eNB_DU_UE_asn(const ng_enb_du_e2sm_t * ng_enb_du)
{
    UEID_NG_ENB_DU_t * ng_enb_du_asn = calloc(1, sizeof(UEID_NG_ENB_DU_t));
    assert(ng_enb_du_asn != NULL && "Memory exhausted");

    // C-if CU DU separated
    // ng-eNB-CU UE W1AP ID

    memcpy(&ng_enb_du_asn->ng_eNB_CU_UE_W1AP_ID, &ng_enb_du->ng_enb_cu_ue_w1ap_id, 4);


    return ng_enb_du_asn;
}
