#ifndef DECRYPTION_ASN_RIC_INDICATION_MESSAGE_FORMAT_1_KPM_V3_H
#define DECRYPTION_ASN_RIC_INDICATION_MESSAGE_FORMAT_1_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_msg_frm_1.h"
#include "../../ie/asn/E2SM-KPM-IndicationMessage-Format1.h"

kpm_ind_msg_format_1_t kpm_dec_ind_msg_frm_1_asn(const E2SM_KPM_IndicationMessage_Format1_t * ind_msg_asn);

#ifdef __cplusplus
}
#endif

#endif
