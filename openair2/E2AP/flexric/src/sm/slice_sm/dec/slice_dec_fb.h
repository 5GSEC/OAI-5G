#ifndef SLICE_DECRYPTION_FLATBUFFERS_H
#define SLICE_DECRYPTION_FLATBUFFERS_H

#include <stddef.h>
#include "../ie/slice_data_ie.h"


slice_event_trigger_t slice_dec_event_trigger_fb(size_t len, uint8_t const ev_tr[len]);

slice_action_def_t slice_dec_action_def_fb(size_t len, uint8_t const action_def[len]);

slice_ind_hdr_t slice_dec_ind_hdr_fb(size_t len, uint8_t const ind_hdr[len]); 

slice_ind_msg_t slice_dec_ind_msg_fb(size_t len, uint8_t const ind_msg[len]); 

slice_call_proc_id_t slice_dec_call_proc_id_fb(size_t len, uint8_t const call_proc_id[len]);

slice_ctrl_hdr_t slice_dec_ctrl_hdr_fb(size_t len, uint8_t const ctrl_hdr[len]); 

slice_ctrl_msg_t slice_dec_ctrl_msg_fb(size_t len, uint8_t const ctrl_msg[len]); 

slice_ctrl_out_t slice_dec_ctrl_out_fb(size_t len, uint8_t const ctrl_out[len]); 

slice_func_def_t slice_dec_func_def_fb(size_t len, uint8_t const func_def[len]);

#endif

