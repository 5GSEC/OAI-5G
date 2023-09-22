#ifndef ENCRYPTION_ASN_ACTION_DEFINITION_FORMAT_4_KPM_V3_H
#define ENCRYPTION_ASN_ACTION_DEFINITION_FORMAT_4_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ie/kpm_data_ie/kpm_ric_info/kpm_ric_action_def_frm_4.h"
#include "../../ie/asn/E2SM-KPM-ActionDefinition-Format4.h"

E2SM_KPM_ActionDefinition_Format4_t * kpm_enc_action_def_frm_4_asn(const kpm_act_def_format_4_t * act_def_frm_4);

#ifdef __cplusplus
}
#endif

#endif
