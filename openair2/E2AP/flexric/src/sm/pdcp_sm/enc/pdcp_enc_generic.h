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



#ifndef PDCP_ENCRYPTION_GENERIC
#define PDCP_ENCRYPTION_GENERIC 

#include "pdcp_enc_asn.h"
#include "pdcp_enc_fb.h"
#include "pdcp_enc_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define pdcp_enc_event_trigger(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_event_trigger_plain, \
                           pdcp_enc_asn_t*: pdcp_enc_event_trigger_asn,\
                           pdcp_enc_fb_t*: pdcp_enc_event_trigger_fb,\
                           default: pdcp_enc_event_trigger_plain) (U)

#define pdcp_enc_action_def(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_action_def_plain, \
                           pdcp_enc_asn_t*: pdcp_enc_action_def_asn, \
                           pdcp_enc_fb_t*: pdcp_enc_action_def_fb, \
                           default:  pdcp_enc_action_def_plain) (U)

#define pdcp_enc_ind_hdr(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_ind_hdr_plain , \
                           pdcp_enc_asn_t*: pdcp_enc_ind_hdr_asn, \
                           pdcp_enc_fb_t*: pdcp_enc_ind_hdr_fb, \
                           default:  pdcp_enc_ind_hdr_plain) (U)

#define pdcp_enc_ind_msg(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_ind_msg_plain , \
                           pdcp_enc_asn_t*: pdcp_enc_ind_msg_asn, \
                           pdcp_enc_fb_t*: pdcp_enc_ind_msg_fb, \
                           default:  pdcp_enc_ind_msg_plain) (U)

#define pdcp_enc_call_proc_id(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_call_proc_id_plain , \
                           pdcp_enc_asn_t*: pdcp_enc_call_proc_id_asn, \
                           pdcp_enc_fb_t*: pdcp_enc_call_proc_id_fb, \
                           default:  pdcp_enc_call_proc_id_plain) (U)

#define pdcp_enc_ctrl_hdr(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_ctrl_hdr_plain , \
                           pdcp_enc_asn_t*: pdcp_enc_ctrl_hdr_asn, \
                           pdcp_enc_fb_t*: pdcp_enc_ctrl_hdr_fb, \
                           default:  pdcp_enc_ctrl_hdr_plain) (U)

#define pdcp_enc_ctrl_msg(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_ctrl_msg_plain , \
                           pdcp_enc_asn_t*: pdcp_enc_ctrl_msg_asn, \
                           pdcp_enc_fb_t*: pdcp_enc_ctrl_msg_fb, \
                           default:  pdcp_enc_ctrl_msg_plain) (U)

#define pdcp_enc_ctrl_out(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_ctrl_out_plain , \
                           pdcp_enc_asn_t*: pdcp_enc_ctrl_out_asn, \
                           pdcp_enc_fb_t*: pdcp_enc_ctrl_out_fb, \
                           default:  pdcp_enc_ctrl_out_plain) (U)

#define pdcp_enc_func_def(T,U) _Generic ((T), \
                           pdcp_enc_plain_t*: pdcp_enc_func_def_plain, \
                           pdcp_enc_asn_t*: pdcp_enc_func_def_asn, \
                           pdcp_enc_fb_t*:  pdcp_enc_func_def_fb, \
                           default:  pdcp_enc_func_def_plain) (U)

#endif

