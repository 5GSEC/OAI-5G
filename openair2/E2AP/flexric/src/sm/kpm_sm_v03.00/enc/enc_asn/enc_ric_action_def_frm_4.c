#include <assert.h>
#include "../../ie/asn/asn_constant.h"


#include "enc_ric_action_def_frm_4.h"
#include "enc_ric_action_def_frm_1.h"
#include "../../ie/asn/MatchingUeCondPerSubList.h"
#include "../../ie/asn/MatchingUeCondPerSubItem.h"

#include "../enc_asn_kpm_common/enc_matching_cond_frm_4.h"



E2SM_KPM_ActionDefinition_Format4_t * kpm_enc_action_def_frm_4_asn(const kpm_act_def_format_4_t * act_def_frm_4)
{
    E2SM_KPM_ActionDefinition_Format4_t * act_def_frm_4_asn = calloc(1, sizeof(E2SM_KPM_ActionDefinition_Format4_t));
    assert (act_def_frm_4_asn != NULL && "Memory exhausted" );


    // 1. Matching Condition : [1, 32768]

    assert(act_def_frm_4->matching_cond_lst_len >= 1 && act_def_frm_4->matching_cond_lst_len <= maxnoofConditionInfoPerSub);
    
    for (size_t i = 0; i < act_def_frm_4->matching_cond_lst_len; i++)
    {
        MatchingUeCondPerSubItem_t * matching_cond = kpm_enc_matching_cond_frm_4_asn(&act_def_frm_4->matching_cond_lst[i]);
        int rc1 = ASN_SEQUENCE_ADD(&act_def_frm_4_asn->matchingUeCondList.list, matching_cond);
        assert(rc1 == 0);
    }

    act_def_frm_4_asn->subscriptionInfo = kpm_enc_action_def_frm_1_asn(&act_def_frm_4->action_def_format_1);

    return act_def_frm_4_asn;
}
