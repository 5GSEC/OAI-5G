#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "enc_ric_ind_msg_frm_1.h"
#include "../enc_asn_kpm_common/enc_meas_info_frm_1.h"
#include "../enc_asn_kpm_common/enc_meas_data.h"


E2SM_KPM_IndicationMessage_Format1_t kpm_enc_ind_msg_frm_1_asn(const kpm_ind_msg_format_1_t * ind_msg)
{
    E2SM_KPM_IndicationMessage_Format1_t ind_msg_asn = {0};


    //  1. Measurement Data : [1, 65535]
    ind_msg_asn.measData = kpm_enc_meas_data_asn(ind_msg->meas_data_lst, ind_msg->meas_data_lst_len);


    //  2. Measurement Information List : [0, 65535], OPTIONAL
    if (ind_msg->meas_info_lst_len != 0)
    {
        ind_msg_asn.measInfoList = calloc(ind_msg->meas_info_lst_len, sizeof(MeasurementInfoList_t));
        assert(ind_msg_asn.measInfoList != NULL && "Memory exhausted");
        *ind_msg_asn.measInfoList = kpm_enc_meas_info_asn(ind_msg->meas_info_lst, ind_msg->meas_info_lst_len);
    }
    else
    {
        ind_msg_asn.measInfoList->list.count = 0;  // ask Mikel, maybe there is no need for this
    }


    //  3. Granularity Period  -  OPTIONAL
    if (ind_msg->gran_period_ms != NULL)
    {
        ind_msg_asn.granulPeriod = calloc(1, sizeof(*ind_msg_asn.granulPeriod));
        memcpy(ind_msg_asn.granulPeriod, ind_msg->gran_period_ms, 4);
        assert(*ind_msg_asn.granulPeriod >= min_val_GranularityPeriod && *ind_msg_asn.granulPeriod <= max_val_GranularityPeriod);
    }
        

    return ind_msg_asn;

}
