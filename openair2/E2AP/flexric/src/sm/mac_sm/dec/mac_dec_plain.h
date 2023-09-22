/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */



#ifndef MAC_DECRYPTION_PLAIN_H
#define MAC_DECRYPTION_PLAIN_H

#include <stddef.h>
#include "../ie/mac_data_ie.h"


mac_event_trigger_t mac_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len]);

mac_action_def_t mac_dec_action_def_plain(size_t len, uint8_t const action_def[len]);

mac_ind_hdr_t mac_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len]); 

mac_ind_msg_t mac_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len]); 

mac_call_proc_id_t mac_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len]);

mac_ctrl_hdr_t mac_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len]); 

mac_ctrl_msg_t mac_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len]); 

mac_ctrl_out_t mac_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]); 

mac_func_def_t mac_dec_func_def_plain(size_t len, uint8_t const func_def[len]);


#endif


