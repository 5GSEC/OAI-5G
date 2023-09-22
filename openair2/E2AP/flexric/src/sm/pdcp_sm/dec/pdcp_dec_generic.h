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



#ifndef PDCP_DECRYPTION_GENERIC
#define PDCP_DECRYPTION_GENERIC 

#include "pdcp_dec_asn.h"
#include "pdcp_dec_fb.h"
#include "pdcp_dec_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define pdcp_dec_event_trigger(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_event_trigger_plain, \
                           pdcp_enc_asn_t*: pdcp_dec_event_trigger_asn,\
                           pdcp_enc_fb_t*: pdcp_dec_event_trigger_fb,\
                           default: pdcp_dec_event_trigger_plain) (U,V)

#define pdcp_dec_action_def(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_action_def_plain, \
                           pdcp_enc_asn_t*: pdcp_dec_action_def_asn, \
                           pdcp_enc_fb_t*: pdcp_dec_action_def_fb, \
                           default:  pdcp_dec_action_def_plain) (U,V)

#define pdcp_dec_ind_hdr(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_ind_hdr_plain , \
                           pdcp_enc_asn_t*: pdcp_dec_ind_hdr_asn, \
                           pdcp_enc_fb_t*: pdcp_dec_ind_hdr_fb, \
                           default:  pdcp_dec_ind_hdr_plain) (U,V)

#define pdcp_dec_ind_msg(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_ind_msg_plain , \
                           pdcp_enc_asn_t*: pdcp_dec_ind_msg_asn, \
                           pdcp_enc_fb_t*: pdcp_dec_ind_msg_fb, \
                           default:  pdcp_dec_ind_msg_plain) (U,V)

#define pdcp_dec_call_proc_id(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_call_proc_id_plain , \
                           pdcp_enc_asn_t*: pdcp_dec_call_proc_id_asn, \
                           pdcp_enc_fb_t*: pdcp_dec_call_proc_id_fb, \
                           default:  pdcp_dec_call_proc_id_plain) (U,V)

#define pdcp_dec_ctrl_hdr(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_ctrl_hdr_plain , \
                           pdcp_enc_asn_t*: pdcp_dec_ctrl_hdr_asn, \
                           pdcp_enc_fb_t*: pdcp_dec_ctrl_hdr_fb, \
                           default: pdcp_dec_ctrl_hdr_plain) (U,V)

#define pdcp_dec_ctrl_msg(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_ctrl_msg_plain , \
                           pdcp_enc_asn_t*: pdcp_dec_ctrl_msg_asn, \
                           pdcp_enc_fb_t*: pdcp_dec_ctrl_msg_fb, \
                           default:  pdcp_dec_ctrl_msg_plain) (U,V)

#define pdcp_dec_ctrl_out(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_ctrl_out_plain , \
                           pdcp_enc_asn_t*: pdcp_dec_ctrl_out_asn, \
                           pdcp_enc_fb_t*: pdcp_dec_ctrl_out_fb, \
                           default:  pdcp_dec_ctrl_out_plain) (U,V)

#define pdcp_dec_func_def(T,U,V) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_dec_func_def_plain, \
                           pdcp_enc_asn_t*: pdcp_dec_func_def_asn, \
                           pdcp_enc_fb_t*:  pdcp_dec_func_def_fb, \
                           default:  pdcp_dec_func_def_plain) (U,V)

#endif

