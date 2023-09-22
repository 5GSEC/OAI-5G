#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "dec_ric_ind_msg_frm_2.h"
#include "../dec_asn_kpm_common/dec_meas_data.h"
#include "../dec_asn_kpm_common/dec_meas_info_cond_ue.h"


kpm_ind_msg_format_2_t kpm_dec_ind_msg_frm_2_asn(const E2SM_KPM_IndicationMessage_Format2_t * ind_msg_asn)
{
    kpm_ind_msg_format_2_t ind_msg = {0};


    //  1. Measurement Data : [1, 65535]
    ind_msg.meas_data_lst_len = ind_msg_asn->measData.list.count;
    assert(ind_msg.meas_data_lst_len >= 1 && ind_msg.meas_data_lst_len <= maxnoofMeasurementRecord);
    ind_msg.meas_data_lst = kpm_dec_meas_data_asn(ind_msg_asn->measData, ind_msg.meas_data_lst_len);


    //  2. Measurement Condition UE
    ind_msg.meas_info_cond_ue_lst_len = ind_msg_asn->measCondUEidList.list.count;
    assert(ind_msg.meas_info_cond_ue_lst_len >= 1 && ind_msg.meas_info_cond_ue_lst_len <= maxnoofMeasurementInfo);
    ind_msg.meas_info_cond_ue_lst = kpm_dec_meas_info_cond_ue_asn(ind_msg_asn->measCondUEidList, ind_msg.meas_info_cond_ue_lst_len);

    //  3. Granularity Period  -  OPTIONAL
    if (ind_msg_asn->granulPeriod != NULL)
    {
        ind_msg.gran_period_ms = calloc(1, sizeof(*ind_msg.gran_period_ms));
        memcpy(ind_msg.gran_period_ms, ind_msg_asn->granulPeriod, 4);
        assert(*ind_msg.gran_period_ms >= min_val_GranularityPeriod && *ind_msg.gran_period_ms <= max_val_GranularityPeriod);
    }

    return ind_msg;
}
