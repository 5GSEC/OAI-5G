#ifndef ENCODE_LABEL_INFO_FORMAT_1_KPM_V3_H
#define ENCODE_LABEL_INFO_FORMAT_1_KPM_V3_H

#include "../../ie/asn/LabelInfoItem.h"
#include "../../ie/kpm_data_ie/data/label_info_lst.h"

LabelInfoItem_t * kpm_enc_label_info_asn(const label_info_lst_t * label_info);



#endif
