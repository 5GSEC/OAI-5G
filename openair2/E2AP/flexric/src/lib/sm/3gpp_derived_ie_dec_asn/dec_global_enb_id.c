#include <assert.h>

#include "../../../util/conversions.h"

#include "dec_global_enb_id.h"

global_enb_id_t dec_global_enb_id_asn(const GlobalENB_ID_t * global_enb_id_asn)
{
    global_enb_id_t global_enb_id = {0};

    // PLMN Identity
    PLMNID_TO_MCC_MNC(&global_enb_id_asn->pLMNIdentity, global_enb_id.plmn_id.mcc, global_enb_id.plmn_id.mnc, global_enb_id.plmn_id.mnc_digit_len);

    // eNB Type

    switch (global_enb_id_asn->eNB_ID.present)
    {
    case ENB_ID_PR_macro_eNB_ID:
        global_enb_id.type = MACRO_ENB_TYPE_ID;
        global_enb_id.macro_enb_id = cp_macro_enb_id_to_u32(global_enb_id_asn->eNB_ID.choice.macro_eNB_ID);
        break;
    
    case ENB_ID_PR_home_eNB_ID:
        global_enb_id.type = HOME_ENB_TYPE_ID;
        global_enb_id.home_enb_id = cp_home_enb_id_to_u32(global_enb_id_asn->eNB_ID.choice.home_eNB_ID);
        break;

    case ENB_ID_PR_short_Macro_eNB_ID:
        global_enb_id.type = SHORT_MACRO_ENB_TYPE_ID;
        global_enb_id.short_macro_enb_id = cp_short_macro_enb_id_to_u32(global_enb_id_asn->eNB_ID.choice.short_Macro_eNB_ID);
        break;

    case ENB_ID_PR_long_Macro_eNB_ID:
        global_enb_id.type = LONG_MACRO_ENB_TYPE_ID;
        global_enb_id.long_macro_enb_id = cp_long_macro_enb_id_to_u32(global_enb_id_asn->eNB_ID.choice.long_Macro_eNB_ID);
        break;

    default:
        assert(false && "Unknown eNB Type");
    }

    return global_enb_id;
}
