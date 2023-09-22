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


#ifndef MSG_HANDLER_RIC
#define MSG_HANDLER_RIC

#include "near_ric.h"
#include "lib/ap/type_defs.h"


e2ap_msg_t e2ap_msg_handle_ric(near_ric_t* ric, const e2ap_msg_t* msg);

///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// E2 -> RIC 
e2ap_msg_t e2ap_handle_subscription_response_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

//E2 -> RIC 
e2ap_msg_t e2ap_handle_subscription_failure_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_subscription_delete_response_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

//E2 -> RIC
e2ap_msg_t e2ap_handle_subscription_delete_failure_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_indication_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_control_ack_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_control_failure_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// RIC <-> E2 
e2ap_msg_t e2ap_handle_error_indication_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_setup_request_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// RIC <-> E2
e2ap_msg_t e2ap_handle_reset_request_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// RIC <-> E2
e2ap_msg_t e2ap_handle_reset_response_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);
  
// E2 -> RIC
e2ap_msg_t e2ap_handle_service_update_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_node_configuration_update_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_connection_update_ack_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_connection_update_failure_ric(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

#endif

