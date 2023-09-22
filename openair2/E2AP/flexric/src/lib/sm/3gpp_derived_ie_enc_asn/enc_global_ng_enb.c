#include <assert.h>

#include "../../../util/conversions.h"

#include "enc_global_ng_enb.h"


GlobalNgENB_ID_t * enc_global_ng_enb_asn(const global_ng_enb_id_t * global_ng_enb_id)
{
    GlobalNgENB_ID_t * global_ng_enb_id_asn = calloc(1, sizeof(GlobalNgENB_ID_t));
    assert (global_ng_enb_id_asn != NULL && "Memory exhausted");


    // PLMN Identity
    MCC_MNC_TO_PLMNID(global_ng_enb_id->plmn_id.mcc, global_ng_enb_id->plmn_id.mnc, global_ng_enb_id->plmn_id.mnc_digit_len, &global_ng_enb_id_asn->pLMNIdentity);

    // NG eNB Type

    switch (global_ng_enb_id->type)
    {
    case MACRO_NG_ENB_TYPE_ID:

        global_ng_enb_id_asn->ngENB_ID.present = NgENB_ID_PR_macroNgENB_ID;
        global_ng_enb_id_asn->ngENB_ID.choice.macroNgENB_ID = cp_macro_ng_enb_id_to_bit_string(global_ng_enb_id->macro_ng_enb_id);
        break;
    
    case SHORT_MACRO_NG_ENB_TYPE_ID:

        global_ng_enb_id_asn->ngENB_ID.present = NgENB_ID_PR_shortMacroNgENB_ID;
        global_ng_enb_id_asn->ngENB_ID.choice.shortMacroNgENB_ID = cp_short_macro_ng_enb_id_to_bit_string(global_ng_enb_id->short_macro_ng_enb_id);
        break;

    case LONG_MACRO_NG_ENB_TYPE_ID:

        global_ng_enb_id_asn->ngENB_ID.present = NgENB_ID_PR_longMacroNgENB_ID;
        global_ng_enb_id_asn->ngENB_ID.choice.longMacroNgENB_ID = cp_long_macro_ng_enb_id_to_bit_string(global_ng_enb_id->long_macro_ng_enb_id);
        break;

    default:
        assert(false && "Unknown NG eNB Type");
    }
    

    return global_ng_enb_id_asn;
}
