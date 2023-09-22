#include <assert.h>

#include "enc_global_ng_ran.h"
#include "enc_global_gnb_id.h"
#include "enc_global_ng_enb.h"

GlobalNGRANNodeID_t * enc_global_ng_ran_asn(const global_ng_ran_node_id_t * global_ng_ran)
{
    GlobalNGRANNodeID_t * global_ng_ran_asn = calloc(1, sizeof(GlobalNGRANNodeID_t));
    assert (global_ng_ran_asn != NULL && "Memory exhausted");

    if (global_ng_ran->type == GNB_GLOBAL_TYPE_ID)
    {
        global_ng_ran_asn->present = GlobalNGRANNodeID_PR_gNB;
        global_ng_ran_asn->choice.gNB = enc_global_gnb_id_asn(&global_ng_ran->global_gnb_id);

    }
    else if (global_ng_ran->type == NG_ENB_GLOBAL_TYPE_ID)
    {
        global_ng_ran_asn->present = GlobalNGRANNodeID_PR_ng_eNB;
        global_ng_ran_asn->choice.ng_eNB = enc_global_ng_enb_asn(&global_ng_ran->global_ng_enb_id);
    }
    else
    {
        assert(false && "Unknown NG RAN Node Type");
    }


    return global_ng_ran_asn;
}