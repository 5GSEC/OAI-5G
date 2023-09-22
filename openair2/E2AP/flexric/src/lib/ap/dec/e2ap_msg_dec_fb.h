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


#ifndef E2AP_MSG_DEC_FB_H
#define E2AP_MSG_DEC_FB_H

#include "lib/ap/type_defs.h"

typedef const struct e2ap_E2Message_table *e2ap_E2Message_table_t;
struct e2ap_fb;

e2ap_msg_t e2ap_msg_dec_fb(struct e2ap_fb* enc, byte_array_t ba);

void init_ap_fb(struct e2ap_fb*);

///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// RIC -> E2
e2ap_msg_t e2ap_dec_subscription_request_fb(e2ap_E2Message_table_t);

// E2 -> RIC 
e2ap_msg_t e2ap_dec_subscription_response_fb(e2ap_E2Message_table_t);

//E2 -> RIC 
e2ap_msg_t e2ap_dec_subscription_failure_fb(e2ap_E2Message_table_t);


//RIC -> E2
e2ap_msg_t e2ap_dec_subscription_delete_request_fb(e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_subscription_delete_response_fb(e2ap_E2Message_table_t);


//E2 -> RIC
e2ap_msg_t e2ap_dec_subscription_delete_failure_fb(e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_indication_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_control_request_fb( e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_control_ack_fb(e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_control_failure_fb(e2ap_E2Message_table_t);


///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// RIC <-> E2 
e2ap_msg_t e2ap_dec_error_indication_fb(e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_setup_request_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t  e2ap_dec_setup_response_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_setup_failure_fb(e2ap_E2Message_table_t);


// RIC <-> E2
e2ap_msg_t e2ap_dec_reset_request_fb(e2ap_E2Message_table_t);


// RIC <-> E2
e2ap_msg_t e2ap_dec_reset_response_fb(e2ap_E2Message_table_t );

  
// E2 -> RIC
e2ap_msg_t e2ap_dec_service_update_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_service_update_ack_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_service_update_failure_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_service_query_fb(e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_node_configuration_update_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_node_configuration_update_ack_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_node_configuration_update_failure_fb(e2ap_E2Message_table_t);


// RIC -> E2
e2ap_msg_t e2ap_dec_connection_update_fb(e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_connection_update_ack_fb(e2ap_E2Message_table_t);


// E2 -> RIC
e2ap_msg_t e2ap_dec_connection_update_failure_fb(e2ap_E2Message_table_t);

#endif
