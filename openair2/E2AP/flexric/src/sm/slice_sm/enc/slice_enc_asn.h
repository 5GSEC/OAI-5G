#ifndef SLICE_ENCRYPTIOIN_ASN_H
#define SLICE_ENCRYPTIOIN_ASN_H

#include "../../../util/byte_array.h"
#include "../ie/slice_data_ie.h"

// Used for static polymorphism. 
// See slice_enc_generic.h file
typedef struct{

} slice_enc_asn_t;

byte_array_t slice_enc_event_trigger_asn(slice_event_trigger_t const* event_trigger);

byte_array_t slice_enc_action_def_asn(slice_action_def_t const*);

byte_array_t slice_enc_ind_hdr_asn(slice_ind_hdr_t const*); 

byte_array_t slice_enc_ind_msg_asn(slice_ind_msg_t const*); 

byte_array_t slice_enc_call_proc_id_asn(slice_call_proc_id_t const*); 

byte_array_t slice_enc_ctrl_hdr_asn(slice_ctrl_hdr_t const*); 

byte_array_t slice_enc_ctrl_msg_asn(slice_ctrl_msg_t const*); 

byte_array_t slice_enc_ctrl_out_asn(slice_ctrl_out_t const*); 

byte_array_t slice_enc_func_def_asn(slice_func_def_t const*);

#endif

