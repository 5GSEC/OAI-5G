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



#ifndef MSG_HANDLER_IAPP
#define MSG_HANDLER_IAPP

#include "e42_iapp.h"
#include "lib/ap/type_defs.h"


void init_handle_msg_iapp(handle_msg_fp_iapp (*handle_msg)[31]);

e2ap_msg_t e2ap_msg_handle_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg);


///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// RIC -> E2
e2ap_msg_t e2ap_handle_subscription_request_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);

//RIC -> E2
e2ap_msg_t e2ap_handle_subscription_delete_request_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC -> E2
e2ap_msg_t e2ap_handle_control_request_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);

// E2 -> RIC
e2ap_msg_t e2ap_handle_subscription_response_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// RIC <-> E2 
e2ap_msg_t e2ap_handle_error_indication_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// xApp -> iApp
e2ap_msg_t e2ap_handle_e42_setup_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg);


// RIC -> E2
e2ap_msg_t e2ap_handle_setup_failure_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC <-> E2
e2ap_msg_t e2ap_handle_reset_request_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC <-> E2
e2ap_msg_t e2ap_handle_reset_response_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);

  
// RIC -> E2
e2ap_msg_t e2ap_handle_service_update_ack_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC -> E2
e2ap_msg_t e2ap_handle_service_update_failure_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC -> E2
e2ap_msg_t e2ap_handle_service_query_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC -> E2
e2ap_msg_t e2ap_handle_node_configuration_update_ack_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC -> E2
e2ap_msg_t e2ap_handle_node_configuration_update_failure_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// RIC -> E2
e2ap_msg_t e2ap_handle_connection_update_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// xApp -> iApp
e2ap_msg_t e2ap_handle_e42_ric_subscription_request_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);

// xApp -> iApp
e2ap_msg_t e2ap_handle_e42_ric_subscription_delete_request_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);


// xApp -> iApp
e2ap_msg_t e2ap_handle_e42_ric_control_request_iapp(e42_iapp_t* ag, const e2ap_msg_t* msg);

// iApp -> xApp
e2ap_msg_t e2ap_handle_ric_indication_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg);

// iApp -> xApp
e2ap_msg_t e2ap_handle_subscription_delete_response_iapp( e42_iapp_t* iapp, const e2ap_msg_t* msg);



// iApp -> xApp 
e2ap_msg_t e2ap_handle_e42_ric_control_ack_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg);

#endif

