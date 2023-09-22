#ifndef TC_ENCRYPTIOIN_ASN_H
#define TC_ENCRYPTIOIN_ASN_H

#include "../../../util/byte_array.h"
#include "../ie/tc_data_ie.h"

// Used for static polymorphism. 
// See tc_enc_generic.h file
typedef struct{

} tc_enc_asn_t;

byte_array_t tc_enc_event_trigger_asn(tc_event_trigger_t const* event_trigger);

byte_array_t tc_enc_action_def_asn(tc_action_def_t const*);

byte_array_t tc_enc_ind_hdr_asn(tc_ind_hdr_t const*); 

byte_array_t tc_enc_ind_msg_asn(tc_ind_msg_t const*); 

byte_array_t tc_enc_call_proc_id_asn(tc_call_proc_id_t const*); 

byte_array_t tc_enc_ctrl_hdr_asn(tc_ctrl_hdr_t const*); 

byte_array_t tc_enc_ctrl_msg_asn(tc_ctrl_msg_t const*); 

byte_array_t tc_enc_ctrl_out_asn(tc_ctrl_out_t const*); 

byte_array_t tc_enc_func_def_asn(tc_func_def_t const*);

#endif

