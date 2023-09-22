#include <assert.h>

#include "../../../util/conversions.h"


#include "enc_global_gnb_id.h"

GlobalGNB_ID_t* enc_global_gnb_id_asn(const global_gnb_id_t * global_gnb_id)
{
    GlobalGNB_ID_t * global_gnb_id_asn = calloc(1, sizeof(GlobalGNB_ID_t));
    assert (global_gnb_id_asn != NULL && "Memory exhausted");


    // PLMN Identity
    MCC_MNC_TO_PLMNID(global_gnb_id->plmn_id.mcc, global_gnb_id->plmn_id.mnc, global_gnb_id->plmn_id.mnc_digit_len, &global_gnb_id_asn->pLMNIdentity);


    // gNB Type
    if (global_gnb_id->type == GNB_TYPE_ID)
    {
        global_gnb_id_asn->gNB_ID.present = GNB_ID_PR_gNB_ID;
        global_gnb_id_asn->gNB_ID.choice.gNB_ID = cp_macro_gnb_id_to_bit_string(global_gnb_id->gnb_id); // BIT STRING (22..32), ask Mikel; taken as default 32 bits
    }
    else
    {
        assert(false && "Unknown gNB Type");
    }
    

    return global_gnb_id_asn;
}
