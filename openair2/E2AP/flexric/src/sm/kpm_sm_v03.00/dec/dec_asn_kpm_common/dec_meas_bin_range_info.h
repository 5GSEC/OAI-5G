#ifndef DECRYPTION_MEASUREMENT_BIN_RANGE_INFORMATION_KPM_V3_H
#define DECRYPTION_MEASUREMENT_BIN_RANGE_INFORMATION_KPM_V3_H



#include "../../ie/asn/DistMeasurementBinRangeItem.h"
#include "../../ie/kpm_data_ie/data/meas_bin_range_info_lst.h"

meas_bin_range_info_lst_t kpm_dec_meas_bin_range_item(const DistMeasurementBinRangeItem_t * meas_bin_range_item_asn);


#endif
