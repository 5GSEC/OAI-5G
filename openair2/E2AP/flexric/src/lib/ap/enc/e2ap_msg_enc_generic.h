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



#ifndef E2AP_MSG_ENC_GENERIC_H
#define E2AP_MSG_ENC_GENERIC_H

#include "e2ap_msg_enc_asn.h" 
#include "e2ap_msg_enc_fb.h" 


#define e2ap_enc_setup_request_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_setup_request_asn, \
                                          const e2ap_asn_t*: e2ap_enc_setup_request_asn, \
                                          e2ap_fb_t*: e2ap_enc_setup_request_fb, \
                                          const e2ap_fb_t*: e2ap_enc_setup_request_fb, \
                                          default:  e2ap_enc_setup_request_asn) (U)

#define e2ap_enc_setup_response_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_setup_response_asn, \
                                          const e2ap_asn_t*: e2ap_enc_setup_response_asn, \
                                          e2ap_fb_t*: e2ap_enc_setup_response_fb, \
                                          const e2ap_fb_t*: e2ap_enc_setup_response_fb, \
                                          default:  e2ap_enc_setup_response_asn) (U)

#define e2ap_enc_subscription_request_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_subscription_request_asn, \
                                          const e2ap_asn_t*:  e2ap_enc_subscription_request_asn, \
                                          e2ap_fb_t*: e2ap_enc_subscription_request_fb, \
                                          const e2ap_fb_t*: e2ap_enc_subscription_request_fb, \
                                          default:  e2ap_enc_subscription_request_asn) (U)

#define e2ap_enc_subscription_response_gen(T,U) _Generic ((T), e2ap_asn_t*:  e2ap_enc_subscription_response_asn, \
                                          const e2ap_asn_t*: e2ap_enc_subscription_response_asn, \
                                          e2ap_fb_t*:  e2ap_enc_subscription_response_fb, \
                                          const e2ap_fb_t*:   e2ap_enc_subscription_response_fb, \
                                          default:   e2ap_enc_subscription_response_asn) (U)

#define e2ap_enc_subscription_failure_gen(T,U) _Generic ((T), e2ap_asn_t*:  e2ap_enc_subscription_failure_asn, \
                                          const e2ap_asn_t*: e2ap_enc_subscription_failure_asn, \
                                          e2ap_fb_t*:  e2ap_enc_subscription_failure_fb, \
                                          const e2ap_fb_t*:   e2ap_enc_subscription_failure_fb, \
                                          default:   e2ap_enc_subscription_failure_asn) (U)

#define e2ap_enc_subscription_delete_request_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_subscription_delete_request_asn, \
                                          const e2ap_asn_t*:  e2ap_enc_subscription_delete_request_asn, \
                                          e2ap_fb_t*: e2ap_enc_subscription_delete_request_fb, \
                                          const e2ap_fb_t*: e2ap_enc_subscription_delete_request_fb, \
                                          default:  e2ap_enc_subscription_delete_request_asn) (U)

#define e2ap_enc_subscription_delete_response_gen(T,U) _Generic ((T), e2ap_asn_t*:  e2ap_enc_subscription_delete_response_asn, \
                                          const e2ap_asn_t*: e2ap_enc_subscription_delete_response_asn, \
                                          e2ap_fb_t*:  e2ap_enc_subscription_delete_response_fb, \
                                          const e2ap_fb_t*:   e2ap_enc_subscription_delete_response_fb, \
                                          default:   e2ap_enc_subscription_delete_response_asn) (U)

#define e2ap_enc_subscription_delete_failure_gen(T,U) _Generic ((T), e2ap_asn_t*:  e2ap_enc_subscription_delete_failure_asn, \
                                          const e2ap_asn_t*: e2ap_enc_subscription_delete_failure_asn, \
                                          e2ap_fb_t*:  e2ap_enc_subscription_delete_failure_fb, \
                                          const e2ap_fb_t*:   e2ap_enc_subscription_delete_failure_fb, \
                                          default:   e2ap_enc_subscription_delete_failure_asn) (U)

#define e2ap_enc_indication_gen(T,U) _Generic ((T), e2ap_asn_t*:  e2ap_enc_indication_asn, \
                                          const e2ap_asn_t*:   e2ap_enc_indication_asn, \
                                          e2ap_fb_t*:    e2ap_enc_indication_fb, \
                                          const e2ap_fb_t*: e2ap_enc_indication_fb, \
                                          default: e2ap_enc_indication_fb) (U)

#define e2ap_enc_control_request_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_control_request_asn, \
                                          const e2ap_asn_t*: e2ap_enc_control_request_asn, \
                                          e2ap_fb_t*:  e2ap_enc_control_request_fb, \
                                          const e2ap_fb_t*:   e2ap_enc_control_request_fb, \
                                          default:  e2ap_enc_control_request_asn) (U)

#define e2ap_enc_control_acknowledge_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_control_ack_asn, \
                                          const e2ap_asn_t*: e2ap_enc_control_ack_asn, \
                                          e2ap_fb_t*:     e2ap_enc_control_ack_fb, \
                                          const e2ap_fb_t*: e2ap_enc_control_ack_fb, \
                                          default:  e2ap_enc_control_ack_asn) (U)

#define e2ap_enc_control_failure_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_control_failure_asn, \
                                          const e2ap_asn_t*: e2ap_enc_control_failure_asn, \
                                          e2ap_fb_t*:     e2ap_enc_control_failure_fb, \
                                          const e2ap_fb_t*: e2ap_enc_control_failure_fb, \
                                          default:  e2ap_enc_control_failure_asn) (U)

#define e2ap_enc_e42_setup_request_gen(T,U) _Generic ((T),    e2ap_asn_t*: e2ap_enc_e42_setup_request_asn, \
                                          const e2ap_asn_t*:  e2ap_enc_e42_setup_request_asn, \
                                          e2ap_fb_t*:         e2ap_enc_e42_setup_request_fb, \
                                          const e2ap_fb_t*:   e2ap_enc_e42_setup_request_fb, \
                                          default:            e2ap_enc_e42_setup_request_asn) (U)

#define e2ap_enc_e42_subscription_request_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_e42_subscription_request_asn, \
                                          const e2ap_asn_t*:    e2ap_enc_e42_subscription_request_asn, \
                                          e2ap_fb_t*:           e2ap_enc_e42_subscription_request_fb, \
                                          const e2ap_fb_t*:     e2ap_enc_e42_subscription_request_fb, \
                                          default:              e2ap_enc_e42_subscription_request_asn) (U)


#define e2ap_enc_e42_subscription_delete_request_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_e42_subscription_delete_request_asn, \
                                          const e2ap_asn_t*:    e2ap_enc_e42_subscription_delete_request_asn, \
                                          e2ap_fb_t*:           e2ap_enc_e42_subscription_delete_request_fb, \
                                          const e2ap_fb_t*:     e2ap_enc_e42_subscription_delete_request_fb, \
                                          default:              e2ap_enc_e42_subscription_delete_request_asn) (U)


#define e2ap_enc_e42_control_request_gen(T,U) _Generic ((T), e2ap_asn_t*: e2ap_enc_e42_control_request_asn, \
                                          const e2ap_asn_t*:  e2ap_enc_e42_control_request_asn, \
                                          e2ap_fb_t*:         e2ap_enc_e42_control_request_fb, \
                                          const e2ap_fb_t*:   e2ap_enc_e42_control_request_fb, \
                                          default:            e2ap_enc_e42_control_request_asn) (U)





#endif

