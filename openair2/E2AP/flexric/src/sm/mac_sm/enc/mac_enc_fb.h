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



#ifndef MAC_ENCRYPTION_FLATBUFFERS_H
#define MAC_ENCRYPTION_FLATBUFFERS_H

#include "../../../util/byte_array.h"
#include "../ie/mac_data_ie.h"


// Used for static polymorphism. 
// View mac_enc_generic file
typedef struct{

} mac_enc_fb_t;

byte_array_t mac_enc_event_trigger_fb(mac_event_trigger_t const* event_trigger);

byte_array_t mac_enc_action_def_fb(mac_action_def_t const*);

byte_array_t mac_enc_ind_hdr_fb(mac_ind_hdr_t const*); 

byte_array_t mac_enc_ind_msg_fb(mac_ind_msg_t const*); 

byte_array_t mac_enc_call_proc_id_fb(mac_call_proc_id_t const*); 

byte_array_t mac_enc_ctrl_hdr_fb(mac_ctrl_hdr_t const*); 

byte_array_t mac_enc_ctrl_msg_fb(mac_ctrl_msg_t const*); 

byte_array_t mac_enc_ctrl_out_fb(mac_ctrl_out_t const*); 

byte_array_t mac_enc_func_def_fb(mac_func_def_t const*);


#endif

