#ifndef SLICE_ENCRYPTION_FLATBUFFERS_H
#define SLICE_ENCRYPTION_FLATBUFFERS_H

#include "../../../util/byte_array.h"
#include "../ie/slice_data_ie.h"


// Used for static polymorphism. 
// View slice_enc_generic file
typedef struct{

} slice_enc_fb_t;

byte_array_t slice_enc_event_trigger_fb(slice_event_trigger_t const* event_trigger);

byte_array_t slice_enc_action_def_fb(slice_action_def_t const*);

byte_array_t slice_enc_ind_hdr_fb(slice_ind_hdr_t const*); 

byte_array_t slice_enc_ind_msg_fb(slice_ind_msg_t const*); 

byte_array_t slice_enc_call_proc_id_fb(slice_call_proc_id_t const*); 

byte_array_t slice_enc_ctrl_hdr_fb(slice_ctrl_hdr_t const*); 

byte_array_t slice_enc_ctrl_msg_fb(slice_ctrl_msg_t const*); 

byte_array_t slice_enc_ctrl_out_fb(slice_ctrl_out_t const*); 

byte_array_t slice_enc_func_def_fb(slice_func_def_t const*);

#endif

