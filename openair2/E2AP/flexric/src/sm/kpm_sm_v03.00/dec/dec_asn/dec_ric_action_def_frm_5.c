#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "dec_ric_action_def_frm_5.h"
#include "dec_ric_action_def_frm_1.h"
#include "../../../../lib/sm/dec_asn_sm_common/dec_ue_id.h"
#include "../../ie/asn/MatchingUEidPerSubItem.h"

kpm_act_def_format_5_t kpm_dec_action_def_frm_5_asn(const E2SM_KPM_ActionDefinition_Format5_t * act_def_asn)
{
    kpm_act_def_format_5_t act_def = {0};

    act_def.ue_id_lst_len = act_def_asn->matchingUEidList.list.count;
    assert(act_def.ue_id_lst_len >= 2 && act_def.ue_id_lst_len <= maxnoofUEIDPerSub);

    act_def.ue_id_lst = calloc(act_def.ue_id_lst_len, sizeof(*act_def.ue_id_lst));
    assert(act_def.ue_id_lst != NULL && "Memory exhausted");

    for (size_t i = 0; i<act_def.ue_id_lst_len; i++)
    {
        MatchingUEidPerSubItem_t * ue_item = act_def_asn->matchingUEidList.list.array[i];
        act_def.ue_id_lst[i] = dec_ue_id_asn(&ue_item->ueID);
    }

    act_def.action_def_format_1 = kpm_dec_action_def_frm_1_asn(&act_def_asn->subscriptionInfo);

    return act_def;
}
