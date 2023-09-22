#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "dec_ric_action_def_frm_4.h"
#include "dec_ric_action_def_frm_1.h"
#include "../dec_asn_kpm_common/dec_matching_cond_frm_4.h"

kpm_act_def_format_4_t kpm_dec_action_def_frm_4_asn(const E2SM_KPM_ActionDefinition_Format4_t * act_def_asn)
{
    kpm_act_def_format_4_t act_def = {0};

    // 1. Matching Condition : [1, 32768]
    act_def.matching_cond_lst_len = act_def_asn->matchingUeCondList.list.count;
    assert(act_def.matching_cond_lst_len >= 1 && act_def.matching_cond_lst_len <= maxnoofConditionInfoPerSub);

    act_def.matching_cond_lst = calloc(act_def.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t));
    assert(act_def.matching_cond_lst != NULL && "Memory exhausted");

    for (size_t i = 0; i<act_def.matching_cond_lst_len; i++)
    {
        act_def.matching_cond_lst[i] = kpm_dec_matching_cond_frm_4_asn(act_def_asn->matchingUeCondList.list.array[i]);
    }

    act_def.action_def_format_1 = kpm_dec_action_def_frm_1_asn(&act_def_asn->subscriptionInfo);

    return act_def;
}
