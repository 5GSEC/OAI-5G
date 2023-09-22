#include <assert.h>
#include "../../../../util/alg_ds/alg/defer.h"
#include "enc_ric_action_def_frm_2.h"
#include "enc_ric_action_def_frm_1.h"
#include "../../../../lib/sm/enc_asn_sm_common/enc_ue_id.h"

E2SM_KPM_ActionDefinition_Format2_t * kpm_enc_action_def_frm_2_asn(const kpm_act_def_format_2_t * act_def_frm_2) 
{
    E2SM_KPM_ActionDefinition_Format2_t * act_def_frm_2_asn = calloc(1, sizeof(E2SM_KPM_ActionDefinition_Format2_t));
    assert (act_def_frm_2_asn != NULL && "Memory exhausted" );

    act_def_frm_2_asn->ueID = enc_ue_id_asn(&act_def_frm_2->ue_id);

    act_def_frm_2_asn->subscriptInfo = kpm_enc_action_def_frm_1_asn(&act_def_frm_2->action_def_format_1);

    return act_def_frm_2_asn;
}
