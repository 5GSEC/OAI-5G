#include <assert.h>

#include "dec_global_ng_ran.h"
#include "dec_global_gnb_id.h"
#include "dec_global_ng_enb.h"

global_ng_ran_node_id_t * dec_global_ng_ran_asn(const GlobalNGRANNodeID_t * global_ng_ran_asn)
{
  assert(global_ng_ran_asn != NULL);

    global_ng_ran_node_id_t * global_ng_ran = calloc(1, sizeof(global_ng_ran_node_id_t));
    assert (global_ng_ran != NULL && "Memory exhausted");


    if (global_ng_ran_asn->present == GlobalNGRANNodeID_PR_gNB)
    {
        global_ng_ran->type = GNB_GLOBAL_TYPE_ID;
        global_ng_ran->global_gnb_id = dec_global_gnb_id_asn(global_ng_ran_asn->choice.gNB);
    }
    else if (global_ng_ran_asn->present == GlobalNGRANNodeID_PR_ng_eNB)
    {
        global_ng_ran->type = NG_ENB_GLOBAL_TYPE_ID;
        global_ng_ran->global_ng_enb_id = dec_global_ng_enb_asn(global_ng_ran_asn->choice.ng_eNB);
    }
    else
    {
        assert(false && "Unknown NG RAN Node Type");
    }

    return global_ng_ran;
}
