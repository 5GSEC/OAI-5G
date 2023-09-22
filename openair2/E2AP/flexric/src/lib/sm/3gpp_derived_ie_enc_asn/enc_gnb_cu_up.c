#include <assert.h>

#include "enc_gnb_cu_up.h"

UEID_GNB_CU_UP_t * enc_gNB_CU_UP_UE_asn(const gnb_cu_up_e2sm_t * gnb_cu_up)
{
    UEID_GNB_CU_UP_t * gnb_cu_up_asn = calloc(1, sizeof(UEID_GNB_CU_UP_t));
    assert(gnb_cu_up_asn != NULL && "Memory exhausted");

    // 6.2.3.20
    // Mandatory
    // gNB CU CP UE E1AP
    memcpy(&gnb_cu_up_asn->gNB_CU_CP_UE_E1AP_ID, &gnb_cu_up->gnb_cu_cp_ue_e1ap, 4);


    // RAN UE ID
    // Optional
    if (gnb_cu_up->ran_ue_id != NULL)
    {
        gnb_cu_up_asn->ran_UEID = calloc(1, sizeof(*gnb_cu_up_asn->ran_UEID));
        gnb_cu_up_asn->ran_UEID->buf = calloc(8, sizeof(*gnb_cu_up_asn->ran_UEID->buf));
        memcpy(gnb_cu_up_asn->ran_UEID->buf, gnb_cu_up->ran_ue_id, 8);
        gnb_cu_up_asn->ran_UEID->size = 8;
    }

    return gnb_cu_up_asn;
}
