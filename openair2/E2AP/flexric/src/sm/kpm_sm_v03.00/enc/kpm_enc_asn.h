#ifndef KPM_ENCRYPTION_ASN_H
#define KPM_ENCRYPTION_ASN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ran_function_def.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_event_trigger_def.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_action_def.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_hdr.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_msg.h"



typedef struct { } kpm_enc_asn_t;

byte_array_t kpm_enc_event_trigger_asn(kpm_event_trigger_def_t const* event_trigger);

byte_array_t kpm_enc_action_def_asn(kpm_act_def_t const* action_def);

byte_array_t kpm_enc_ind_hdr_asn(kpm_ind_hdr_t const* ind_hdr);

byte_array_t kpm_enc_ind_msg_asn(kpm_ind_msg_t const* ind_msg);

byte_array_t kpm_enc_func_def_asn(kpm_ran_function_def_t const* func_def);

#ifdef __cplusplus
}
#endif

#endif

// done