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

#ifndef IAPP_INTERFACE_H
#define IAPP_INTERFACE_H 

#include "near_ric_if.h"
//##include "../../../test/iapp-xapp/near_ric_emulator.h"

/*
#define start_near_ric_iapp_gen(T) _Generic ((T), \
                          near_ric_if_emulator_t*:       start_near_ric_iapp_emulator, \
                          const near_ric_if_emulator_t*:  start_near_ric_iapp_emulator, \
                          near_ric_t*:                 start_near_ric_iapp, \
                          const near_ric_t*:           start_near_ric_iapp, \
                          default:                     start_near_ric_iapp_emulator) (T)

#define stop_near_ric_iapp_gen(T) _Generic ((T), \
                          near_ric_if_emulator_t*:        stop_near_ric_iapp_emulator, \
                          const near_ric_if_emulator_t*:  stop_near_ric_iapp_emulator, \
                          near_ric_t*:                 stop_near_ric_iapp, \
                          const  near_ric_t*:          stop_near_ric_iapp, \
                          default:                     stop_near_ric_iapp) ()


#define fwd_ric_subscription_request_gen(T,U,V,W) _Generic ((T), \
                                          near_ric_if_emulator_t*:        fwd_ric_subscription_request_emulator, \
                                          const near_ric_if_emulator_t*:  fwd_ric_subscription_request_emulator, \
                                          near_ric_t*:                 fwd_ric_subscription_request, \
                                          const near_ric_t*:           fwd_ric_subscription_request, \
                                          default:                     fwd_ric_subscription_request_emulator) (T,U,V,W)

#define fwd_ric_subscription_request_delete_gen(T,U,V,W) _Generic ((T), \
                                          near_ric_if_emulator_t*:        fwd_ric_subscription_request_delete_emulator, \
                                          const near_ric_if_emulator_t*:  fwd_ric_subscription_request_delete_emulator, \
                                          near_ric_t*:                 fwd_ric_subscription_request_delete, \
                                          const near_ric_t*:           fwd_ric_subscription_request_delete, \
                                          default:                     fwd_ric_subscription_request_delete_emulator) (T,U,V,W)

#define fwd_ric_control_request_gen(T,U,V,W) _Generic ((T), \
                                          near_ric_if_emulator_t*:        fwd_ric_control_request_emulator, \
                                          const  near_ric_if_emulator_t*: fwd_ric_control_request_emulator, \
                                          near_ric_t*:                 fwd_ric_control_request, \
                                          const   near_ric_t*:         fwd_ric_control_request, \
                                          default:                     fwd_ric_control_request_emulator) (T,U,V,W)
*/

#define start_near_ric_iapp_gen(T) _Generic ((T), \
                          near_ric_t*:                 start_near_ric_iapp, \
                          const near_ric_t*:           start_near_ric_iapp, \
                          default:                     start_near_ric_iapp) (T)

#define stop_near_ric_iapp_gen(T) _Generic ((T), \
                          near_ric_t*:                 stop_near_ric_iapp, \
                          const  near_ric_t*:          stop_near_ric_iapp, \
                          default:                     stop_near_ric_iapp) ()


#define fwd_ric_subscription_request_gen(T,U,V,W) _Generic ((T), \
                                          near_ric_t*:                 fwd_ric_subscription_request, \
                                          const near_ric_t*:           fwd_ric_subscription_request, \
                                          default:                     fwd_ric_subscription_request) (T,U,V,W)

#define fwd_ric_subscription_request_delete_gen(T,U,V,W) _Generic ((T), \
                                          near_ric_t*:                 fwd_ric_subscription_request_delete, \
                                          const near_ric_t*:           fwd_ric_subscription_request_delete, \
                                          default:                     fwd_ric_subscription_request_delete) (T,U,V,W)

#define fwd_ric_control_request_gen(T,U,V,W) _Generic ((T), \
                                          near_ric_t*:                 fwd_ric_control_request, \
                                          const   near_ric_t*:         fwd_ric_control_request, \
                                          default:                     fwd_ric_control_request) (T,U,V,W)

#endif

