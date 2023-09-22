#ifndef ENCODE_UE_ID_GRANULARITY_PERIOD_LIST_KPM_V3_H
#define ENCODE_UE_ID_GRANULARITY_PERIOD_LIST_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ie/asn/MatchingUEidPerGP-Item.h"
#include "../../ie/kpm_data_ie/data/ue_id_gran_period_lst.h"

MatchingUEidPerGP_Item_t * kpm_enc_ue_id_gran_period_asn(const ue_id_gran_period_lst_t * ue_id_gran_period);

#ifdef __cplusplus
}
#endif

#endif
