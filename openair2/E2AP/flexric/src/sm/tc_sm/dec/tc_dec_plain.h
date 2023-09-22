#ifndef TC_DECRYPTION_PLAIN_H
#define TC_DECRYPTION_PLAIN_H

#include <stddef.h>
#include "../ie/tc_data_ie.h"


tc_event_trigger_t tc_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len]);

tc_action_def_t tc_dec_action_def_plain(size_t len, uint8_t const action_def[len]);

tc_ind_hdr_t tc_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len]); 

tc_ind_msg_t tc_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len]); 

tc_call_proc_id_t tc_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len]);

tc_ctrl_hdr_t tc_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len]); 

tc_ctrl_msg_t tc_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len]); 

tc_ctrl_out_t tc_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]); 

tc_func_def_t tc_dec_func_def_plain(size_t len, uint8_t const func_def[len]);

#endif


