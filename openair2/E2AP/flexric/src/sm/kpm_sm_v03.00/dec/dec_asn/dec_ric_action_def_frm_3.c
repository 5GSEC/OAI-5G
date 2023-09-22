#include <assert.h>

#include "../../../../lib/sm/dec_asn_sm_common/dec_cell_global_id.h"
#include "../../ie/asn/asn_constant.h"
#include "../dec_asn_kpm_common/dec_meas_info_frm_3.h"
#include "dec_ric_action_def_frm_3.h"

kpm_act_def_format_3_t kpm_dec_action_def_frm_3_asn(const E2SM_KPM_ActionDefinition_Format3_t * act_def_asn)
{
    kpm_act_def_format_3_t act_def={0};

    // 1. Measurement Information List Format 3 : [1, 65535]
    
    assert((act_def_asn->measCondList.list.count <= maxnoofMeasurementInfo && act_def_asn->measCondList.list.count >= 1) 
              && "Number of measures not allowed");
    
    act_def.meas_info_lst_len = act_def_asn->measCondList.list.count;
    act_def.meas_info_lst = kpm_dec_meas_info_cond_asn(&act_def_asn->measCondList, act_def.meas_info_lst_len);
    
    // 2. Granularity Period : [1, 4294967295]
    
    act_def.gran_period_ms = act_def_asn->granulPeriod;
    assert(act_def.gran_period_ms >= min_val_GranularityPeriod && act_def.gran_period_ms <= max_val_GranularityPeriod);
     

    // 3. Cell Global ID - OPTIONAL
    if (act_def_asn->cellGlobalID != NULL){
      act_def.cell_global_id = calloc(1, sizeof(cell_global_id_t));
      assert(act_def.cell_global_id != NULL && "Memory exhausted");
      *act_def.cell_global_id = dec_cell_global_id_asn(act_def_asn->cellGlobalID);
    }

    return act_def;
}
