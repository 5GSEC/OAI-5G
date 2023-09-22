#ifndef ENCRYPTION_MEASUREMENT_BIN_RANGE_INFORMATION_KPM_V3_H
#define ENCRYPTION_MEASUREMENT_BIN_RANGE_INFORMATION_KPM_V3_H



#include "../../ie/asn/DistMeasurementBinRangeItem.h"
#include "../../ie/kpm_data_ie/data/meas_bin_range_info_lst.h"


DistMeasurementBinRangeItem_t * kpm_enc_meas_bin_range_item(const meas_bin_range_info_lst_t * meas_bin_range_item);


#endif
