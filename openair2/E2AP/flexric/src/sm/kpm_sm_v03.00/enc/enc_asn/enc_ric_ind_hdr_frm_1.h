#ifndef ENCRYPTION_ASN_RIC_INDICATION_HEADER_FORMAT_1_KPM_V3_H
#define ENCRYPTION_ASN_RIC_INDICATION_HEADER_FORMAT_1_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_hdr_frm_1.h"
#include "../../ie/asn/E2SM-KPM-IndicationHeader-Format1.h"

E2SM_KPM_IndicationHeader_Format1_t * kpm_enc_ind_hdr_frm_1_asn(const kpm_ric_ind_hdr_format_1_t * ind_hdr);

#ifdef __cplusplus
}
#endif

#endif
