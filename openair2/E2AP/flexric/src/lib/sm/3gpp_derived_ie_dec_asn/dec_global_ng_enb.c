#include <assert.h>

#include "../../../util/conversions.h"

#include "dec_global_ng_enb.h"

global_ng_enb_id_t dec_global_ng_enb_asn(const GlobalNgENB_ID_t * global_ng_enb_id_asn)
{
    global_ng_enb_id_t global_ng_enb_id = {0};


    // PLMN Identity
    PLMNID_TO_MCC_MNC(&global_ng_enb_id_asn->pLMNIdentity, global_ng_enb_id.plmn_id.mcc, global_ng_enb_id.plmn_id.mnc, global_ng_enb_id.plmn_id.mnc_digit_len);

    // NG eNB Type

    switch (global_ng_enb_id_asn->ngENB_ID.present)
    {
    case NgENB_ID_PR_macroNgENB_ID:
        global_ng_enb_id.type = MACRO_NG_ENB_TYPE_ID;
        global_ng_enb_id.macro_ng_enb_id = cp_macro_ng_enb_id_to_u32(global_ng_enb_id_asn->ngENB_ID.choice.macroNgENB_ID);
        break;
    
    case NgENB_ID_PR_shortMacroNgENB_ID:
        global_ng_enb_id.type = SHORT_MACRO_NG_ENB_TYPE_ID;
        global_ng_enb_id.short_macro_ng_enb_id = cp_short_macro_ng_enb_id_to_u32(global_ng_enb_id_asn->ngENB_ID.choice.shortMacroNgENB_ID);
        break;

    case NgENB_ID_PR_longMacroNgENB_ID:
        global_ng_enb_id.type = LONG_MACRO_NG_ENB_TYPE_ID;
        global_ng_enb_id.long_macro_ng_enb_id = cp_long_macro_ng_enb_id_to_u32(global_ng_enb_id_asn->ngENB_ID.choice.longMacroNgENB_ID);
        break;

    default:
        assert(false && "Unknown NG eNB Type");
    }


    return global_ng_enb_id;
}
