#ifndef DECODE_MEASUREMENT_INFO_FORMAT_1_KPM_V3_H
#define DECODE_MEASUREMENT_INFO_FORMAT_1_KPM_V3_H

#include "../../ie/asn/MeasurementInfoList.h"
#include "../../ie/kpm_data_ie/data/meas_info_frm_1_lst.h"

meas_info_format_1_lst_t* kpm_dec_meas_info_asn(const MeasurementInfoList_t * meas_info_asn );


#endif
