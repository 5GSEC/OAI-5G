#ifndef TC_ENCRYPTION_FLATBUFFERS_H
#define TC_ENCRYPTION_FLATBUFFERS_H

#include "../../../util/byte_array.h"
#include "../ie/tc_data_ie.h"


// Used for static polymorphism. 
// View tc_enc_generic file
typedef struct{

} tc_enc_fb_t;

byte_array_t tc_enc_event_trigger_fb(tc_event_trigger_t const* event_trigger);

byte_array_t tc_enc_action_def_fb(tc_action_def_t const*);

byte_array_t tc_enc_ind_hdr_fb(tc_ind_hdr_t const*); 

byte_array_t tc_enc_ind_msg_fb(tc_ind_msg_t const*); 

byte_array_t tc_enc_call_proc_id_fb(tc_call_proc_id_t const*); 

byte_array_t tc_enc_ctrl_hdr_fb(tc_ctrl_hdr_t const*); 

byte_array_t tc_enc_ctrl_msg_fb(tc_ctrl_msg_t const*); 

byte_array_t tc_enc_ctrl_out_fb(tc_ctrl_out_t const*); 

byte_array_t tc_enc_func_def_fb(tc_func_def_t const*);

#endif

