#include <assert.h>

#include "dec_gnb_cu_up.h"

gnb_cu_up_e2sm_t dec_gNB_CU_UP_UE_asn(const UEID_GNB_CU_UP_t * gnb_cu_up_asn)
{
    gnb_cu_up_e2sm_t gnb_cu_up = {0};

    // gNB CU CP UE E1AP
    memcpy(&gnb_cu_up.gnb_cu_cp_ue_e1ap, &gnb_cu_up_asn->gNB_CU_CP_UE_E1AP_ID, 4);


    // RAN UE ID
    // Optional

    if (gnb_cu_up_asn->ran_UEID != NULL)
    {
        gnb_cu_up.ran_ue_id = calloc(1, sizeof(*gnb_cu_up.ran_ue_id));
        assert(gnb_cu_up.ran_ue_id != NULL && "Memory exhausted" );

        assert(gnb_cu_up_asn->ran_UEID->buf != NULL);
        assert(gnb_cu_up_asn->ran_UEID->size == 8);

        memcpy(gnb_cu_up.ran_ue_id, gnb_cu_up_asn->ran_UEID->buf, 8);
    }


    return gnb_cu_up;
}
