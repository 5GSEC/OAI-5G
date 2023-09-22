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


#ifndef PDCP_DECRYPTION_ASN_H
#define PDCP_DECRYPTION_ASN_H

#include <stddef.h>
#include "../ie/pdcp_data_ie.h"


pdcp_event_trigger_t pdcp_dec_event_trigger_asn(size_t len, uint8_t const ev_tr[len]);

pdcp_action_def_t pdcp_dec_action_def_asn(size_t len, uint8_t const action_def[len]);

pdcp_ind_hdr_t pdcp_dec_ind_hdr_asn(size_t len, uint8_t const ind_hdr[len]); 

pdcp_ind_msg_t pdcp_dec_ind_msg_asn(size_t len, uint8_t const ind_msg[len]); 

pdcp_call_proc_id_t pdcp_dec_call_proc_id_asn(size_t len, uint8_t const call_proc_id[len]);

pdcp_ctrl_hdr_t pdcp_dec_ctrl_hdr_asn(size_t len, uint8_t const ctrl_hdr[len]); 

pdcp_ctrl_msg_t pdcp_dec_ctrl_msg_asn(size_t len, uint8_t const ctrl_msg[len]); 

pdcp_ctrl_out_t pdcp_dec_ctrl_out_asn(size_t len, uint8_t const ctrl_out[len]); 

pdcp_func_def_t pdcp_dec_func_def_asn(size_t len, uint8_t const func[len]);

#endif

