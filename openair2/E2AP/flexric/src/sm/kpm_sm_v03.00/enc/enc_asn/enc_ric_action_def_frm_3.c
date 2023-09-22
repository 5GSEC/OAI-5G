#include <assert.h>

#include "../../../../lib/sm/enc_asn_sm_common/enc_cell_global_id.h"
#include "../../ie/asn/asn_constant.h"
#include "../enc_asn_kpm_common/enc_meas_info_frm_3.h"
#include "enc_ric_action_def_frm_3.h"

E2SM_KPM_ActionDefinition_Format3_t * kpm_enc_action_def_frm_3_asn(const kpm_act_def_format_3_t * act_def_frm_3)
{
    E2SM_KPM_ActionDefinition_Format3_t * act_def_frm_3_asn = calloc(1, sizeof(E2SM_KPM_ActionDefinition_Format3_t));
    assert (act_def_frm_3_asn != NULL && "Memory exhausted" );

    // 1. Measurement Information List Format 3 : [1, 65535]
    act_def_frm_3_asn->measCondList = kpm_enc_meas_info_cond_asn(act_def_frm_3->meas_info_lst, act_def_frm_3->meas_info_lst_len);

    // 2. Granularity Period : [1, 4294967295]
    act_def_frm_3_asn->granulPeriod = act_def_frm_3->gran_period_ms;
    assert(act_def_frm_3->gran_period_ms >= min_val_GranularityPeriod && act_def_frm_3->gran_period_ms <= max_val_GranularityPeriod);
     
    // 3. Cell Global ID - OPTIONAL
    if (act_def_frm_3->cell_global_id != NULL){
      act_def_frm_3_asn->cellGlobalID = calloc(1, sizeof(CGI_t));
      assert(act_def_frm_3_asn->cellGlobalID != NULL && "Memory exhausted");
      *act_def_frm_3_asn->cellGlobalID = enc_cell_global_id_asn(act_def_frm_3->cell_global_id);
    }

    return act_def_frm_3_asn;   
}
