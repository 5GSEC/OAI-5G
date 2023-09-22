#ifndef ENCRYPTION_MATCHING_CONDITION_KPM_V3_H
#define ENCRYPTION_MATCHING_CONDITION_KPM_V3_H



#include "../../ie/asn/MatchingCondItem.h"
#include "../../ie/kpm_data_ie/data/matching_cond_frm_3_lst.h"

MatchingCondItem_t * kpm_enc_matching_cond_asn(const matching_condition_format_3_lst_t * matching_cond);


#endif
