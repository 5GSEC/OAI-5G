#ifndef SLICE_ENCRYPTION_PLAIN_H
#define SLICE_ENCRYPTION_PLAIN_H 

#include "../../../util/byte_array.h"
#include "../ie/slice_data_ie.h"


// Used for static polymorphism. 
// View slice_enc_generic file
typedef struct{

} slice_enc_plain_t;


byte_array_t slice_enc_event_trigger_plain(slice_event_trigger_t const* event_trigger);

byte_array_t slice_enc_action_def_plain(slice_action_def_t const*);

byte_array_t slice_enc_ind_hdr_plain(slice_ind_hdr_t const*); 

byte_array_t slice_enc_ind_msg_plain(slice_ind_msg_t const*); 

byte_array_t slice_enc_call_proc_id_plain(slice_call_proc_id_t const*); 

byte_array_t slice_enc_ctrl_hdr_plain(slice_ctrl_hdr_t const*); 

byte_array_t slice_enc_ctrl_msg_plain(slice_ctrl_msg_t const*); 

byte_array_t slice_enc_ctrl_out_plain(slice_ctrl_out_t const*); 

byte_array_t slice_enc_func_def_plain(slice_func_def_t const*);

#endif

