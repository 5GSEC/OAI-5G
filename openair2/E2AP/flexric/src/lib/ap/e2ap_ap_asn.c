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



#include "e2ap_ap_asn.h"

#include <assert.h>
#include <stdlib.h>

#include "lib/ap/dec/e2ap_msg_dec_generic.h"
#include "lib/ap/enc/e2ap_msg_enc_generic.h"
#include "lib/ap/free/e2ap_msg_free.h"

void init_ap_asn(e2ap_asn_t* asn)
{
  assert(asn != NULL);

  // Encoding functions
  asn->enc_msg[0] =  e2ap_enc_subscription_request_asn_msg;
  asn->enc_msg[1] =  e2ap_enc_subscription_response_asn_msg;
  asn->enc_msg[2] =  e2ap_enc_subscription_failure_asn_msg;
  asn->enc_msg[3] =  e2ap_enc_subscription_delete_request_asn_msg;
  asn->enc_msg[4] =  e2ap_enc_subscription_delete_response_asn_msg;
  asn->enc_msg[5] =  e2ap_enc_subscription_delete_failure_asn_msg;
  asn->enc_msg[6] =  e2ap_enc_indication_asn_msg;
  asn->enc_msg[7] =  e2ap_enc_control_request_asn_msg;
  asn->enc_msg[8] =  e2ap_enc_control_ack_asn_msg;
  asn->enc_msg[9] =  e2ap_enc_control_failure_asn_msg;
  asn->enc_msg[10] =  e2ap_enc_error_indication_asn_msg;
  asn->enc_msg[11] =  e2ap_enc_setup_request_asn_msg;
  asn->enc_msg[12] =  e2ap_enc_setup_response_asn_msg;
  asn->enc_msg[13] =  e2ap_enc_setup_failure_asn_msg;
  asn->enc_msg[14] =  e2ap_enc_reset_request_asn_msg;
  asn->enc_msg[15] =  e2ap_enc_reset_response_asn_msg;
  asn->enc_msg[16] =  e2ap_enc_service_update_asn_msg;
  asn->enc_msg[17] =  e2ap_enc_service_update_ack_asn_msg;
  asn->enc_msg[18] =  e2ap_enc_service_update_failure_asn_msg;
  asn->enc_msg[19] =  e2ap_enc_service_query_asn_msg;
  asn->enc_msg[20] =  e2ap_enc_node_configuration_update_asn_msg;
  asn->enc_msg[21] =  e2ap_enc_node_configuration_update_ack_asn_msg;
  asn->enc_msg[22] =  e2ap_enc_node_configuration_update_failure_asn_msg;
  asn->enc_msg[23] =  e2ap_enc_node_connection_update_asn_msg;
  asn->enc_msg[24] =  e2ap_enc_node_connection_update_ack_asn_msg;
  asn->enc_msg[25] =  e2ap_enc_node_connection_update_failure_asn_msg;
  asn->enc_msg[26] =  e2ap_enc_e42_setup_request_asn_msg;
  asn->enc_msg[27] =  e2ap_enc_e42_setup_response_asn_msg;
                      
  asn->enc_msg[28] = e2ap_enc_e42_subscription_request_asn_msg;
  asn->enc_msg[29] = e2ap_enc_e42_subscription_delete_request_asn_msg;

  asn->enc_msg[30] = e2ap_enc_e42_control_request_asn_msg;

  // Decoding Functions
  asn->dec_msg[0] =  e2ap_dec_subscription_request;
  asn->dec_msg[1] =  e2ap_dec_subscription_response;
  asn->dec_msg[2] =  e2ap_dec_subscription_failure;
  asn->dec_msg[3] =  e2ap_dec_subscription_delete_request;
  asn->dec_msg[4] =  e2ap_dec_subscription_delete_response;
  asn->dec_msg[5] =  e2ap_dec_subscription_delete_failure;
  asn->dec_msg[6] =  e2ap_dec_indication;
  asn->dec_msg[7] =  e2ap_dec_control_request;
  asn->dec_msg[8] =  e2ap_dec_control_ack;
  asn->dec_msg[9] =  e2ap_dec_control_failure;
  asn->dec_msg[10] =  e2ap_dec_error_indication;
  asn->dec_msg[11] =  e2ap_dec_setup_request;
  asn->dec_msg[12] =  e2ap_dec_setup_response;
  asn->dec_msg[13] =  e2ap_dec_setup_failure;
  asn->dec_msg[14] =  e2ap_dec_reset_request;
  asn->dec_msg[15] =  e2ap_dec_reset_response;
  asn->dec_msg[16] =  e2ap_dec_service_update;
  asn->dec_msg[17] =  e2ap_dec_service_update_ack;
  asn->dec_msg[18] =  e2ap_dec_service_update_failure;
  asn->dec_msg[19] =  e2ap_dec_service_query;
  asn->dec_msg[20] =  e2ap_dec_node_configuration_update;
  asn->dec_msg[21] =  e2ap_dec_node_configuration_update_ack;
  asn->dec_msg[22] =  e2ap_dec_node_configuration_update_failure;
  asn->dec_msg[23] =  e2ap_dec_connection_update;
  asn->dec_msg[24] =  e2ap_dec_connection_update_ack;
  asn->dec_msg[25] =  e2ap_dec_connection_update_failure;
  asn->dec_msg[26] =  e2ap_dec_e42_setup_request;
  asn->dec_msg[27] =  e2ap_dec_e42_setup_response;
  asn->dec_msg[28] =  e2ap_dec_e42_subscription_request;
  asn->dec_msg[29] =  e2ap_dec_e42_subscription_delete_request;
  asn->dec_msg[30] =  e2ap_dec_e42_control_request;
 
  // Free Functions
  asn->free_msg[0] =  e2ap_free_subscription_request_msg;
  asn->free_msg[1] =  e2ap_free_subscription_response_msg;
  asn->free_msg[2] =  e2ap_free_subscription_failure_msg;
  asn->free_msg[3] =  e2ap_free_subscription_delete_request_msg;
  asn->free_msg[4] =  e2ap_free_subscription_delete_response_msg;
  asn->free_msg[5] =  e2ap_free_subscription_delete_failure_msg;
  asn->free_msg[6] =  e2ap_free_indication_msg;
  asn->free_msg[7] =  e2ap_free_control_request_msg;
  asn->free_msg[8] =  e2ap_free_control_ack_msg;
  asn->free_msg[9] =  e2ap_free_control_failure_msg;
  asn->free_msg[10] =  e2ap_free_error_indication_msg;
  asn->free_msg[11] =  e2ap_free_setup_request_msg;
  asn->free_msg[12] =  e2ap_free_setup_response_msg;
  asn->free_msg[13] =  e2ap_free_setup_failure_msg;
  asn->free_msg[14] =  e2ap_free_reset_request_msg;
  asn->free_msg[15] =  e2ap_free_reset_response_msg;
  asn->free_msg[16] =  e2ap_free_service_update_msg;
  asn->free_msg[17] =  e2ap_free_service_update_ack_msg;
  asn->free_msg[18] =  e2ap_free_service_update_failure_msg;
  asn->free_msg[19] =  e2ap_free_service_query_msg;
  asn->free_msg[20] =  e2ap_free_node_configuration_update_msg;
  asn->free_msg[21] =  e2ap_free_node_configuration_update_ack_msg;
  asn->free_msg[22] =  e2ap_free_node_configuration_update_failure_msg;
  asn->free_msg[23] =  e2ap_free_node_connection_update_msg;
  asn->free_msg[24] =  e2ap_free_node_connection_update_ack_msg;
  asn->free_msg[25] =  e2ap_free_node_connection_update_failure_msg;
  asn->free_msg[26] =  e2ap_free_e42_setup_request_msg;
  asn->free_msg[27] =  e2ap_free_e42_setup_response_msg;
  asn->free_msg[28] =  e2ap_free_e42_ric_subscription_request_msg;
  asn->free_msg[29] =  e2ap_free_e42_ric_subscription_delete_request_msg;
  asn->free_msg[30] =  e2ap_free_e42_ric_control_request_msg;

}

