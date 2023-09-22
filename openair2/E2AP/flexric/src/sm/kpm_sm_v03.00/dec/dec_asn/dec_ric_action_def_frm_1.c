#include <assert.h>
#include "../../ie/asn/asn_constant.h"
#include "../../ie/asn/DistMeasurementBinRangeList.h"

#include "dec_ric_action_def_frm_1.h"
#include "../dec_asn_kpm_common/dec_meas_bin_range_info.h"
#include "../dec_asn_kpm_common/dec_meas_info_frm_1.h"
#include "../../../../lib/sm/dec_asn_sm_common/dec_cell_global_id.h"

kpm_act_def_format_1_t kpm_dec_action_def_frm_1_asn(const  E2SM_KPM_ActionDefinition_Format1_t * act_def_asn)
{
    kpm_act_def_format_1_t act_def={0};

    // 1. Measurement Information List Format 1 : [1, 65535]
    assert((act_def_asn->measInfoList.list.count <= maxnoofMeasurementInfo && act_def_asn->measInfoList.list.count >= 1) 
              && "Number of measures not allowed");
    
    act_def.meas_info_lst_len = act_def_asn->measInfoList.list.count;
    act_def.meas_info_lst = kpm_dec_meas_info_asn(&act_def_asn->measInfoList);


    // 2. Granularity Period : [1, 4294967295]
    act_def.gran_period_ms = act_def_asn->granulPeriod;
    assert(act_def.gran_period_ms >= min_val_GranularityPeriod && act_def.gran_period_ms <= max_val_GranularityPeriod);

    // 3. Cell Global ID - OPTIONAL
    if (act_def_asn->cellGlobalID != NULL) {
       act_def.cell_global_id = calloc(1, sizeof(cell_global_id_t)); 
       assert(act_def.cell_global_id != NULL && "Memory exhausted");
       *act_def.cell_global_id = dec_cell_global_id_asn(act_def_asn->cellGlobalID);
    }

    // 4. Measurement Bin Range Information List : [0, 65535]
    // not yet implemented in ASN.1 - possible extension
    if (act_def_asn->distMeasBinRangeInfo != NULL) {
        DistMeasurementBinRangeList_t * bin_range_list = act_def_asn->distMeasBinRangeInfo;
        
        act_def.meas_bin_range_info_lst_len = bin_range_list->list.count;
        assert(act_def.meas_bin_range_info_lst_len >= 1 && act_def.meas_bin_range_info_lst_len <= maxnoofBin);

        act_def.meas_bin_info_lst = calloc(act_def.meas_bin_range_info_lst_len, sizeof(meas_bin_range_info_lst_t));
        assert(act_def.meas_bin_info_lst != NULL && "Memory exhausted");

        for (size_t i = 0; i < act_def.meas_bin_range_info_lst_len; i++) {
            act_def.meas_bin_info_lst[i] = kpm_dec_meas_bin_range_item(bin_range_list->list.array[i]);
        }
    }


    return act_def;
}
