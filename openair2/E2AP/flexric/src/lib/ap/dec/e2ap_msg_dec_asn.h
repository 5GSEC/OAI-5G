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


#ifndef E2AP_MSG_DEC_ASN_H
#define E2AP_MSG_DEC_ASN_H

#include "lib/ap/type_defs.h"

struct E2AP_PDU;
typedef struct e2ap_asn e2ap_asn_t;

e2ap_msg_t e2ap_msg_dec_asn(e2ap_asn_t* asn, byte_array_t ba);

void e2ap_msg_free_asn(struct e2ap_asn* enc, e2ap_msg_t* msg);

void init_ap_asn(struct e2ap_asn*);

///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// RIC -> E2
e2ap_msg_t e2ap_dec_subscription_request(const struct E2AP_PDU* pdu);

// E2 -> RIC 
e2ap_msg_t e2ap_dec_subscription_response(const struct E2AP_PDU* pdu);

//E2 -> RIC 
e2ap_msg_t e2ap_dec_subscription_failure(const struct E2AP_PDU* pdu);


//RIC -> E2
e2ap_msg_t e2ap_dec_subscription_delete_request(const struct E2AP_PDU* pdu);


// E2 -> RIC
e2ap_msg_t e2ap_dec_subscription_delete_response(const struct E2AP_PDU* pdu);


//E2 -> RIC
e2ap_msg_t e2ap_dec_subscription_delete_failure(const struct E2AP_PDU* pdu);


// E2 -> RIC
e2ap_msg_t e2ap_dec_indication(const struct E2AP_PDU* pdu);


// RIC -> E2
e2ap_msg_t e2ap_dec_control_request(const struct E2AP_PDU* pdu);


// E2 -> RIC
e2ap_msg_t e2ap_dec_control_ack(const struct E2AP_PDU* pdu);


// E2 -> RIC
e2ap_msg_t e2ap_dec_control_failure(const struct E2AP_PDU* pdu);

  

///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// RIC <-> E2 
e2ap_msg_t e2ap_dec_error_indication(const struct E2AP_PDU* pdu);


// E2 -> RIC
e2ap_msg_t e2ap_dec_setup_request(const struct E2AP_PDU* pdu);


// RIC -> E2
e2ap_msg_t  e2ap_dec_setup_response(const struct E2AP_PDU* pdu);


// RIC -> E2
e2ap_msg_t e2ap_dec_setup_failure(const struct E2AP_PDU* pdu);


// RIC <-> E2
e2ap_msg_t e2ap_dec_reset_request(const struct E2AP_PDU* pdu);


// RIC <-> E2
e2ap_msg_t e2ap_dec_reset_response(const struct E2AP_PDU* pdu);

  

// E2 -> RIC
e2ap_msg_t e2ap_dec_service_update(const struct E2AP_PDU* pdu);


// RIC -> E2
e2ap_msg_t e2ap_dec_service_update_ack(const struct E2AP_PDU* pdu);


// RIC -> E2
 e2ap_msg_t e2ap_dec_service_update_failure(const struct E2AP_PDU* pdu);


// RIC -> E2
  e2ap_msg_t e2ap_dec_service_query(const struct E2AP_PDU* pdu);


// E2 -> RIC
e2ap_msg_t e2ap_dec_node_configuration_update(const struct E2AP_PDU* pdu);


// RIC -> E2
e2ap_msg_t e2ap_dec_node_configuration_update_ack(const struct E2AP_PDU* pdu);


// RIC -> E2
 e2ap_msg_t e2ap_dec_node_configuration_update_failure(const struct E2AP_PDU* pdu);


// RIC -> E2
 e2ap_msg_t e2ap_dec_connection_update(const struct E2AP_PDU* pdu);


// E2 -> RIC
 e2ap_msg_t e2ap_dec_connection_update_ack(const struct E2AP_PDU* pdu);


// E2 -> RIC
e2ap_msg_t e2ap_dec_connection_update_failure(const struct E2AP_PDU* pdu);


// xApp -> iApp
e2ap_msg_t e2ap_dec_e42_setup_request(const struct E2AP_PDU* pdu);

// iApp -> xApp
e2ap_msg_t e2ap_dec_e42_setup_response(const struct E2AP_PDU* pdu);

// xApp -> iApp 
e2ap_msg_t e2ap_dec_e42_subscription_request(const struct E2AP_PDU* pdu);

// xApp -> iApp 
e2ap_msg_t e2ap_dec_e42_subscription_delete_request(const struct E2AP_PDU* pdu);

// xApp -> iApp 
e2ap_msg_t e2ap_dec_e42_control_request(const struct E2AP_PDU* pdu);


#endif

