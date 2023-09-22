#ifndef DECODE_LABEL_INFO_FORMAT_1_KPM_V3_H
#define DECODE_LABEL_INFO_FORMAT_1_KPM_V3_H

#include "../../ie/asn/LabelInfoItem.h"
#include "../../ie/kpm_data_ie/data/label_info_lst.h"

label_info_lst_t kpm_dec_label_info_asn(const MeasurementLabel_t * meas_label_asn);



#endif
