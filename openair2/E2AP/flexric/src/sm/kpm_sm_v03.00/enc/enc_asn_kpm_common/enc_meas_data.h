#ifndef ENCODE_MEASUREMENT_DATA_FORMAT_1_KPM_V3_H
#define ENCODE_MEASUREMENT_DATA_FORMAT_1_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ie/asn/MeasurementData.h"
#include "../../ie/kpm_data_ie/data/meas_data_lst.h"

MeasurementData_t kpm_enc_meas_data_asn(const meas_data_lst_t * meas_data, const size_t meas_data_len);

#ifdef __cplusplus
}
#endif

#endif
