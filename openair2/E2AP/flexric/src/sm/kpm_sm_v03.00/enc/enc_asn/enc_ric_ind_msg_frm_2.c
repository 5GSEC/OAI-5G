#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "enc_ric_ind_msg_frm_2.h"
#include "../enc_asn_kpm_common/enc_meas_data.h"
#include "../enc_asn_kpm_common/enc_meas_info_cond_ue.h"

E2SM_KPM_IndicationMessage_Format2_t * kpm_enc_ind_msg_frm_2_asn(const kpm_ind_msg_format_2_t * ind_msg)
{
    E2SM_KPM_IndicationMessage_Format2_t * ind_msg_asn = calloc(1, sizeof(E2SM_KPM_IndicationMessage_Format2_t));
    assert(ind_msg_asn != NULL && "Memory exhausted");


    //  1. Measurement Data : [1, 65535]
    ind_msg_asn->measData = kpm_enc_meas_data_asn(ind_msg->meas_data_lst, ind_msg->meas_data_lst_len);


    //  2. Measurement Condition UE : [1, 65535]
    ind_msg_asn->measCondUEidList = kpm_enc_meas_info_cond_ue_asn(ind_msg->meas_info_cond_ue_lst, ind_msg->meas_info_cond_ue_lst_len);


    //  3. Granularity Period  -  OPTIONAL
    if (ind_msg->gran_period_ms != NULL)
    {
        ind_msg_asn->granulPeriod = calloc(1, sizeof(*ind_msg_asn->granulPeriod));
        memcpy(ind_msg_asn->granulPeriod, ind_msg->gran_period_ms, 4);
        assert(*ind_msg_asn->granulPeriod >= min_val_GranularityPeriod && *ind_msg_asn->granulPeriod <= max_val_GranularityPeriod);
    }
        

    return ind_msg_asn;
}
