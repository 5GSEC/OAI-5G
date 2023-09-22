#include <assert.h>

#include "enc_gnb_du.h"


UEID_GNB_DU_t * enc_gNB_DU_UE_asn(const gnb_du_e2sm_t * gnb_du)
{
    UEID_GNB_DU_t * gnb_du_asn = calloc(1, sizeof(UEID_GNB_DU_t));
    assert(gnb_du_asn != NULL && "Memory exhausted");

    // 6.2.3.21
    // Mandatory
    // gNB CU UE F1AP
    memcpy(&gnb_du_asn->gNB_CU_UE_F1AP_ID, &gnb_du->gnb_cu_ue_f1ap, 4);


    // RAN UE ID
    // Optional
    if (gnb_du->ran_ue_id != NULL)
    {
        gnb_du_asn->ran_UEID = calloc(1, sizeof(*gnb_du_asn->ran_UEID));
        gnb_du_asn->ran_UEID->buf = calloc(8, sizeof(*gnb_du_asn->ran_UEID->buf));
        memcpy(gnb_du_asn->ran_UEID->buf, gnb_du->ran_ue_id, 8);
        gnb_du_asn->ran_UEID->size = 8;
    }
    

    return gnb_du_asn;
}
