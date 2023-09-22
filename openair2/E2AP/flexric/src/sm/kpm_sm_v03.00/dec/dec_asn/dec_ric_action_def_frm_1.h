#ifndef DECRYPTION_ASN_ACTION_DEFINITION_FORMAT_1_KPM_V3_H
#define DECRYPTION_ASN_ACTION_DEFINITION_FORMAT_1_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ie/kpm_data_ie/kpm_ric_info/kpm_ric_action_def_frm_1.h"
#include "../../ie/asn/E2SM-KPM-ActionDefinition-Format1.h"

kpm_act_def_format_1_t kpm_dec_action_def_frm_1_asn(const  E2SM_KPM_ActionDefinition_Format1_t * act_def_asn);

#ifdef __cplusplus
}
#endif

#endif
