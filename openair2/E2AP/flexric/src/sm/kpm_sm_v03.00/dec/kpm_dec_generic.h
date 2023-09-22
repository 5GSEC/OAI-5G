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



#ifndef MAC_DECRYPTION_GENERIC
#define MAC_DECRYPTION_GENERIC 

#include "kpm_dec_asn.h"
//#include "kpm_dec_fb.h"
// #include "kpm_dec_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-KPM v.2.0 Technical Specification
/////////////////////////////////////////////////////////////////////


#define kpm_dec_event_trigger(T,U,V) _Generic ((T), \
                           /* kpm_enc_plain_t*: kpm_dec_event_trigger_plain,*/  \
                           kpm_enc_asn_t*: kpm_dec_event_trigger_asn, \
                           default: kpm_dec_event_trigger_asn) (U,V)
                          
#define kpm_dec_action_def(T,U,V) _Generic ((T), \
                           /* kpm_enc_plain_t*: kpm_dec_action_def_plain,*/  \
                           kpm_enc_asn_t*: kpm_dec_action_def_asn, \
                           default: kpm_dec_action_def_asn) (U,V)

#define kpm_dec_ind_hdr(T,U,V) _Generic ((T), \
                           kpm_enc_asn_t*: kpm_dec_ind_hdr_asn, \
                           default:  kpm_dec_ind_hdr_asn) (U,V)

#define kpm_dec_ind_msg(T,U,V) _Generic ((T), \
                           kpm_enc_asn_t*: kpm_dec_ind_msg_asn, \
                           default:  kpm_dec_ind_msg_asn) (U,V)

#define kpm_dec_func_def(T,U,V) _Generic ((T), \
                           kpm_enc_asn_t*: kpm_dec_func_def_asn, \
                           default:  kpm_dec_func_def_asn) (U,V)
                           
#endif

