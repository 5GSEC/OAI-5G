#ifndef ENCRYPTION_ASN_RIC_INDICATION_MESSAGE_FORMAT_1_KPM_V3_H
#define ENCRYPTION_ASN_RIC_INDICATION_MESSAGE_FORMAT_1_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_msg_frm_1.h"
#include "../../ie/asn/E2SM-KPM-IndicationMessage-Format1.h"

E2SM_KPM_IndicationMessage_Format1_t kpm_enc_ind_msg_frm_1_asn(const kpm_ind_msg_format_1_t * ind_msg);

#ifdef __cplusplus
}
#endif

#endif
