#ifndef DECODE_MEASUREMENT_INFO_FORMAT_3_KPM_V3_H
#define DECODE_MEASUREMENT_INFO_FORMAT_3_KPM_V3_H

#include "../../ie/asn/MeasurementCondList.h"
#include "../../ie/kpm_data_ie/data/meas_info_frm_3_lst.h"

meas_info_format_3_lst_t * kpm_dec_meas_info_cond_asn(const MeasurementCondList_t * meas_info_cond_asn, const size_t meas_info_cond_len);



#endif
