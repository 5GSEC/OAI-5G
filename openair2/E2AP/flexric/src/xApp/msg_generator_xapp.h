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




#ifndef MESSAGE_GENERATOR_XAPP_H
#define MESSAGE_GENERATOR_XAPP_H 


#include "../lib/ap/e2ap_types/ric_subscription_request.h"
#include "../lib/ap/e2ap_types/e42_setup_request.h"
#include "../lib/ap/e2ap_types/e42_ric_subscription_request.h"

#include "e42_xapp.h"


ric_subscription_request_t generate_subscription_request(ric_gen_id_t ric_id, sm_ric_t const* sm, void* cmd);

e42_ric_subscription_request_t generate_e42_ric_subscription_request(uint16_t xapp_id, global_e2_node_id_t* id,  ric_subscription_request_t* sr); 

e42_setup_request_t generate_e42_setup_request(e42_xapp_t* xapp);

ric_control_request_t generate_ric_control_request(ric_gen_id_t ric_id, sm_ric_t const* sm, void* ctrl_msg);

//e42_ric_control_request_t generate_e42_ric_control_request(uint16_t xapp_id, global_e2_node_id_t* id,  ric_subscription_request_t* sr);

#endif

