#ifndef TC_ENCRYPTION_PLAIN_H
#define TC_ENCRYPTION_PLAIN_H 

#include "../../../util/byte_array.h"
#include "../ie/tc_data_ie.h"


// Used for static polymorphism. 
// View tc_enc_generic file
typedef struct{

} tc_enc_plain_t;


byte_array_t tc_enc_event_trigger_plain(tc_event_trigger_t const* event_trigger);

byte_array_t tc_enc_action_def_plain(tc_action_def_t const*);

byte_array_t tc_enc_ind_hdr_plain(tc_ind_hdr_t const*); 

byte_array_t tc_enc_ind_msg_plain(tc_ind_msg_t const*); 

byte_array_t tc_enc_call_proc_id_plain(tc_call_proc_id_t const*); 

byte_array_t tc_enc_ctrl_hdr_plain(tc_ctrl_hdr_t const*); 

byte_array_t tc_enc_ctrl_msg_plain(tc_ctrl_msg_t const*); 

byte_array_t tc_enc_ctrl_out_plain(tc_ctrl_out_t const*); 

byte_array_t tc_enc_func_def_plain(tc_func_def_t const*);

#endif

