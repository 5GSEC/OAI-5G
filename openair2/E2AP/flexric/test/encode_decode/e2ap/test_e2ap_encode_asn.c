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



#ifndef TEST_E2AP_ENCODE_ASN_H
#define TEST_E2AP_ENCODE_ASN_H

#include <assert.h>
#include <stdio.h>

#include "../src/util/byte_array.h"
#include "../src/lib/ap/type_defs.h"
#include "../src/lib/ap/enc/e2ap_msg_enc_asn.h"
#include "../src/lib/ap/free/e2ap_msg_free.h"

#include "../../../src/lib/ap/ie/asn/E2AP-PDU.h"


#include <stdlib.h>
#include <string.h>

static
void try_encode(E2AP_PDU_t* pdu)
{
  assert(pdu != NULL);
  uint8_t buffer[2048];
  xer_fprint(stderr, &asn_DEF_E2AP_PDU, pdu);
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER; // ATS_BASIC_XER; // ATS_ALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2AP_PDU, pdu, buffer, 2048);
  fprintf(stdout, "er encoded is %ld\n", er.encoded);
  fflush(stdout);
  assert(er.encoded < 2048);
  if(er.encoded == -1){
    printf("Failed the encoding in type %s and xml_type = %s\n", er.failed_type->name, er.failed_type->xml_tag); 
    fflush(stdout);
  }
  assert(er.encoded > -1);
}

static
void free_pdu(E2AP_PDU_t* pdu)
{
  assert(pdu != NULL);
  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_PDU,pdu);
  free(pdu);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Messages for Near-RT RIC Functional Procedures //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void test_ric_subscription_request()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
                          .ric_inst_id = 2,
                          .ran_func_id = 12};

  byte_array_t ba_event_trigger;
  memset(&ba_event_trigger, 0, sizeof(byte_array_t));
  const char* event_trigger = "This is the event trigger";
  ba_event_trigger.buf = malloc(strlen(event_trigger));
  memcpy(ba_event_trigger.buf, event_trigger, strlen(event_trigger));
  ba_event_trigger.len = strlen(event_trigger);

  const size_t len_action = 1;
  ric_action_t* action = calloc(len_action, sizeof(ric_action_t));
  action->id = 0;
  action->type = RIC_ACT_REPORT;  

 ric_subscription_request_t sr_before = {
  .ric_id = ric_id,
  .event_trigger = ba_event_trigger,
  .action = action,
  .len_action = len_action
 };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_request_asn_pdu(&sr_before);   
  e2ap_free_subscription_request(&sr_before);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu); 
  free_byte_array(ba);
}

void test_ric_subscription_response()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
                          .ric_inst_id = 2,
                          .ran_func_id = 12};


  const size_t len_admitted = 1;;
  ric_action_admitted_t* admitted = calloc(len_admitted, sizeof(ric_action_admitted_t));
  admitted[0].ric_act_id = 42;
  ric_action_not_admitted_t* not_admitted = NULL;
  const size_t len_na = 0;

  ric_subscription_response_t sr = {
    .ric_id = ric_id,
    .admitted = admitted,
    .len_admitted = len_admitted,
    .not_admitted = not_admitted,
    .len_na = len_na
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_response_asn_pdu(&sr);
  e2ap_free_subscription_response(&sr);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_subscription_failure()
{

  const ric_gen_id_t ric_id = {.ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12};


  ric_action_not_admitted_t* na = calloc(1,sizeof(ric_action_not_admitted_t)); 
  na->ric_act_id = 2;
  na->cause.present = CAUSE_PROTOCOL;
  na->cause.protocol = CAUSE_PROTOCOL_SEMANTIC_ERROR;	

  criticality_diagnostics_t* crit_diag = NULL; 

  ric_subscription_failure_t sf = {
    .ric_id = ric_id,
    . not_admitted = na,
    .len_na = 1,
    .crit_diag = crit_diag, // optional
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_failure_asn_pdu(&sf);
  e2ap_free_subscription_failure(&sf);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}


void test_ric_subscription_delete_request()
{
  const ric_gen_id_t ric_id = {
    .ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12};

  ric_subscription_delete_request_t dr = {
    .ric_id = ric_id,
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_delete_request_asn_pdu(&dr);
  e2ap_free_subscription_delete_request(&dr);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_subscription_delete_response()
{
  const ric_gen_id_t ric_id = {
    .ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12
  };

  ric_subscription_delete_response_t dr = {
    .ric_id = ric_id,
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_delete_response_asn_pdu(&dr);
  e2ap_free_subscription_delete_response(&dr);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_subscription_delete_failure()
{

  const ric_gen_id_t ric_id = {
    .ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12
  };

  cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};

  criticality_diagnostics_t* crit_diag = NULL; // optional

  ric_subscription_delete_failure_t dr = {
  .ric_id = ric_id,
  .cause = cause,
  .crit_diag =  crit_diag,
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_delete_failure_asn_pdu(&dr);
  e2ap_free_subscription_delete_failure(&dr);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}


void test_ric_indication()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
                          .ric_inst_id = 2,
                          .ran_func_id = 12};

  const uint8_t action_id = 19;
  ric_indication_type_e type = RIC_IND_REPORT ;
  byte_array_t ba_hdr;
  memset(&ba_hdr, 0, sizeof(byte_array_t));
  const char* hdr_str = "This is the header string";
  ba_hdr.buf = malloc(strlen(hdr_str));
  memcpy(ba_hdr.buf, hdr_str, strlen(hdr_str));
  ba_hdr.len = strlen(hdr_str);
  byte_array_t ba_msg;
  memset(&ba_msg, 0, sizeof(byte_array_t));
  const char* msg_str = "This is the message string";
  ba_msg.buf = malloc(strlen(msg_str));
  memcpy(ba_msg.buf, msg_str, strlen(msg_str)); 
  ba_msg.len = strlen(msg_str);

  ric_indication_t ric_ind = {
  .ric_id = ric_id,
  .action_id = action_id,
  .type = type,
  .hdr = ba_hdr,
  .msg = ba_msg,
  };

  E2AP_PDU_t* pdu = e2ap_enc_indication_asn_pdu(&ric_ind);
  e2ap_free_indication(&ric_ind);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_e2_setup_request()
{

  e2ap_plmn_t plmn = {
    .mcc = 10,
    .mnc = 15,
    .mnc_digit_len = 2
  };

  const ngran_node_t type = ngran_gNB; 

  global_e2_node_id_t id = {
    .type = type,
    .plmn = plmn,
    .nb_id = 0,
  };

  const size_t len_rf = 1;
  ran_function_t* ran_func_item = calloc(len_rf, sizeof(ran_function_t));
  ran_func_item[0].id = 32;
  ran_func_item[0].rev = 0;
  const char* def = "This is the possible deficniotn";
  ran_func_item[0].def.buf = malloc(strlen(def));
  memcpy(ran_func_item[0].def.buf, def, strlen(def)); 
  ran_func_item[0].def.len = strlen(def); 


  e2_node_component_config_update_t* comp_conf_update = NULL;
  const size_t len_ccu = 0;

  e2_setup_request_t e2_stp_req =
  {
    .id = id,
    .ran_func_item = ran_func_item ,
    .len_rf = len_rf, 
    .comp_conf_update = comp_conf_update,
    .len_ccu = len_ccu
  };

  E2AP_PDU_t* pdu = e2ap_enc_setup_request_asn_pdu(&e2_stp_req);
  e2ap_free_setup_request(&e2_stp_req);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_e2_setup_response()
{
  e2ap_plmn_t plmn = {
    .mcc = 10,
    .mnc = 15,
    .mnc_digit_len = 2
  };

  global_ric_id_t id = {
    .plmn = plmn,
    .near_ric_id.double_word = 42,
  };

  const size_t len_acc = 1;
  accepted_ran_function_t* accepted = calloc(len_acc, sizeof(accepted_ran_function_t));
  accepted[0] = 45;
  const size_t len_rej = 0;
  rejected_ran_function_t* rejected = NULL;
  e2_node_component_config_update_t* comp_conf_update_ack_list = NULL;
  const size_t len_ccual = 0;

  e2_setup_response_t e2_stp_res = {
    .id = id,
    .accepted = accepted ,
    .len_acc = len_acc ,
    .rejected = rejected ,
    .len_rej = len_rej ,
    .comp_conf_update_ack_list = comp_conf_update_ack_list ,
    .len_ccual = len_ccual ,
  };

  E2AP_PDU_t* pdu = e2ap_enc_setup_response_asn_pdu(&e2_stp_res);
  e2ap_free_setup_response(&e2_stp_res);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_control_request()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
                          .ric_inst_id = 2,
                          .ran_func_id = 12};

  byte_array_t* call_process_id = NULL; // optional
  byte_array_t hdr;
  memset(&hdr, 0, sizeof(byte_array_t));
  const char* str_hdr = "Header string";
  hdr.buf = malloc(strlen(str_hdr)); 
  memcpy(hdr.buf, str_hdr, strlen(str_hdr));
  hdr.len = strlen(str_hdr);

  byte_array_t msg;
  memset(&msg, 0, sizeof(byte_array_t));
  const char* str_msg = "Message string ";
  msg.buf = malloc(strlen(str_msg) );
  memcpy(msg.buf, str_msg, strlen(str_msg) );
  msg.len = strlen(str_msg);
  ric_control_ack_req_t* ack_req = NULL; // optional


  ric_control_request_t rcr = {
  .ric_id = ric_id,
  .call_process_id =call_process_id, // optional
  .hdr = hdr,
  .msg = msg,
  .ack_req = ack_req, // optional
};

  E2AP_PDU_t* pdu = e2ap_enc_control_request_asn_pdu(&rcr);
  e2ap_free_control_request(&rcr);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_control_ack()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
                          .ric_inst_id = 2,
                          .ran_func_id = 12};

  byte_array_t* call_process_id = NULL; // optional
  ric_control_status_t status = RIC_CONTROL_STATUS_SUCCESS;
  byte_array_t* control_outcome = NULL; // optional

 ric_control_acknowledge_t c_ack = {
  .ric_id = ric_id,
  .call_process_id = call_process_id,
  .status = status,
  .control_outcome = control_outcome,
 };

  E2AP_PDU_t* pdu = e2ap_enc_control_ack_asn_pdu(&c_ack);
  e2ap_free_control_ack(&c_ack);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_control_failure()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
                          .ric_inst_id = 2,
                          .ran_func_id = 12};

  cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};

  byte_array_t* call_process_id = NULL; // optional
 byte_array_t* control_outcome = NULL;

  ric_control_failure_t cf = {
  .ric_id = ric_id,
  .call_process_id = call_process_id, // optional
  .cause = cause,
  .control_outcome = control_outcome, // optional
  };

  E2AP_PDU_t* pdu = e2ap_enc_control_failure_asn_pdu(&cf);
  e2ap_free_control_failure(&cf);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_error_indication()
{
  ric_gen_id_t* ric_id = calloc(1, sizeof(ric_gen_id_t));
  ric_id->ric_inst_id = 2;
  ric_id->ric_req_id = 3;
  ric_id->ran_func_id = 42;

  cause_t* cause = NULL; // optional
  criticality_diagnostics_t* crit_diag = NULL; // optional

  e2ap_error_indication_t ei = {
  .ric_id = ric_id, // optional
  .cause = cause, // optional
  .crit_diag = crit_diag, // optional
  };

  E2AP_PDU_t* pdu = e2ap_enc_error_indication_asn_pdu(&ei);
  e2ap_free_error_indication(&ei);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_e2_setup_failure()
{
  cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};

  e2ap_time_to_wait_e* time_to_wait_ms = calloc(1, sizeof(e2ap_time_to_wait_e)); // optional 
  *time_to_wait_ms = TIMETOWAIT_V1S; 

  criticality_diagnostics_t* crit_diag = NULL; // optional

  transport_layer_information_t* tl_info = calloc(1,sizeof(transport_layer_information_t)); // optional
  const char* addr = "192.168.1.0";
  tl_info->address.buf = malloc(strlen(addr)); 
  memcpy(tl_info->address.buf, addr, strlen(addr) );
  tl_info->address.len = strlen(addr); 

  tl_info->port = NULL;
  // Bug. Check why
  /*
  calloc(1,sizeof(byte_array_t));
  const char* port = "1010";
  tl_info->port->buf = calloc(1, strlen(port));
  memcpy(tl_info->port->buf, port, strlen(port));
  tl_info->port->len = strlen(port);  
*/
  e2_setup_failure_t sf = {
  .cause = cause,
  .time_to_wait_ms = time_to_wait_ms,            // optional
  .crit_diag = crit_diag, // optional
  .tl_info = tl_info, // optional
};

  E2AP_PDU_t* pdu = e2ap_enc_setup_failure_asn_pdu(&sf);
  e2ap_free_setup_failure(&sf);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_reset_request()
{
  const cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};
 e2ap_reset_request_t rr = {.cause = cause};

  E2AP_PDU_t* pdu = e2ap_enc_reset_request_asn_pdu(&rr);
  e2ap_free_reset_request(&rr);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_reset_response()
{
  criticality_diagnostics_t* crit_diag = NULL; // optional
  e2ap_reset_response_t rr = {
    .crit_diag = crit_diag, // optional
  };

  E2AP_PDU_t* pdu = e2ap_enc_reset_response_asn_pdu(&rr);
  e2ap_free_reset_response(&rr);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_service_update()
{
  byte_array_t ba;
  memset(&ba, 0, sizeof(byte_array_t));
  const char* def = "This is a dummy definition";
  ba.buf =  malloc(strlen(def));
  memcpy(ba.buf, def, strlen(def));
  ba.len = strlen(def);

  const size_t len_added = 1;
  ran_function_t* added = calloc(len_added, sizeof(ran_function_t ));
  added->id = 42;
  added->rev = 0;
  added->def = ba;

  ran_function_t* modified = NULL;
  const size_t len_modified = 0;
  e2ap_ran_function_id_rev_t* deleted = NULL;
  size_t len_deleted = 0;

 ric_service_update_t su = {
  .len_added = len_added,
  .added = added,  
  .modified = modified,
  .len_modified = len_modified, 
  .deleted = deleted,
  .len_deleted = len_deleted,
 };

  E2AP_PDU_t* pdu = e2ap_enc_service_update_asn_pdu(&su);
  e2ap_free_service_update(&su);
  try_encode(pdu);
  byte_array_t ba2 = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba2);
}

void test_ric_service_update_ack()
{
  const size_t len_accepted = 1;
  ran_function_id_t* accepted = calloc(len_accepted, sizeof(ran_function_t));
  accepted->id = 3;
  accepted->rev = 0;

  rejected_ran_function_t* rejected = NULL;
  size_t len_rejected = 0;

  ric_service_update_ack_t su = {
    .accepted = accepted,
    .len_accepted = len_accepted,
    .rejected = rejected,
    .len_rejected = len_rejected,
  };

  E2AP_PDU_t* pdu = e2ap_enc_service_update_ack_asn_pdu(&su);
  e2ap_free_service_update_ack(&su);
  try_encode(pdu);
  byte_array_t ba2 = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba2);
}

void test_ric_service_update_failure()
{
  const cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};
  const size_t len_rej = 1;
  rejected_ran_function_t* rejected = calloc(len_rej, sizeof(rejected_ran_function_t));
   rejected->id = 42;
    rejected->cause = cause;

  e2ap_time_to_wait_e* time_to_wait = NULL;
  criticality_diagnostics_t* crit_diag = NULL;


  ric_service_update_failure_t uf = {
  .rejected = rejected, 
  .len_rej= len_rej,
  .time_to_wait = time_to_wait,
  .crit_diag = crit_diag,
  };

  E2AP_PDU_t* pdu = e2ap_enc_service_update_failure_asn_pdu(&uf);
  e2ap_free_service_update_failure(&uf);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_ric_service_query()
{
  size_t len_accepted = 1;
  e2ap_ran_function_id_rev_t*  accepted = calloc(len_accepted, sizeof(e2ap_ran_function_id_rev_t));

  accepted->id = 42;
  accepted->rev = 5;

  ric_service_query_t sq = {
    .accepted = accepted,
    .len_accepted = len_accepted,
  };

  E2AP_PDU_t* pdu = e2ap_enc_service_query_asn_pdu(&sq);
  e2ap_free_service_query(&sq);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_e2_node_configuration_update()
{
  const char* str_conf = "Configuration Update";
  byte_array_t* ba2 = malloc(sizeof(byte_array_t));
  ba2->buf = malloc(strlen(str_conf));
  ba2->len = strlen(str_conf);
  memcpy(ba2->buf, str_conf, ba2->len);

  const size_t len_ccul = 1;
  e2_node_component_config_update_t* cc = calloc(1, sizeof(e2_node_component_config_update_t));

  cc->e2_node_component_type = E2_NODE_COMPONENT_TYPE_EN_GNB;
  cc->id_present = NULL; 
  cc->update_present = E2_NODE_COMPONENT_CONFIG_UPDATE_EN_GNB_CONFIG_UPDATE;
  cc->en_gnb.x2ap_en_gnb = ba2;

 e2_node_configuration_update_t cu = {
   .comp_conf_update_list = cc,
   .len_ccul = len_ccul,
 };

  E2AP_PDU_t* pdu = e2ap_enc_node_configuration_update_asn_pdu(&cu);
  e2ap_free_node_configuration_update(&cu);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_e2_node_configuration_update_ack()
{
  const size_t len_ccual = 1;
  e2_node_component_config_update_ack_item_t* ccua = calloc(1, sizeof(e2_node_component_config_update_ack_item_t));

  ccua->e2_node_component_type = E2_NODE_COMPONENT_TYPE_EN_GNB;
  ccua->id_present = NULL; 
  ccua->failure_cause = NULL;
  ccua->update_outcome = 2;

  e2_node_configuration_update_ack_t cua = {
  .comp_conf_update_ack_list = ccua,
  .len_ccual = len_ccual,
  };

  E2AP_PDU_t* pdu = e2ap_enc_node_configuration_update_ack_asn_pdu(&cua);
  e2ap_free_node_configuration_update_ack(&cua);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_e2_node_configuration_update_failure()
{
  const cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};
  e2ap_time_to_wait_e* time_to_wait = calloc(1, sizeof(e2ap_time_to_wait_e));
  *time_to_wait = TIMETOWAIT_V10S;	
  criticality_diagnostics_t* crit_diag = NULL;

 e2_node_configuration_update_failure_t cuf = {
  .cause = cause,
  .time_wait = time_to_wait,
  .crit_diag =  crit_diag,
 };
 
  E2AP_PDU_t* pdu = e2ap_enc_node_configuration_update_failure_asn_pdu(&cuf);
  e2ap_free_node_configuration_update_failure(&cuf);
  try_encode(pdu);
  byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba);
}

void test_e2_node_connection_update()
{
  const char* addr = "192.168.0.1";
  byte_array_t ba =  {.len = strlen(addr), .buf = malloc(strlen(addr))};
  memcpy(ba.buf, addr, strlen(addr));

  const size_t len_add = 1;
  e2_connection_update_item_t* add = calloc(len_add, sizeof(e2_connection_update_item_t));
  add->usage = TNL_USAGE_RIC_SERVICE;	
  add->info.tnl_addr = ba;
  add->info.tnl_port = NULL;

  e2_connection_update_remove_item_t* rem = NULL;
  size_t len_rem = 0;
  e2_connection_update_item_t* mod = NULL;
  size_t len_mod = 0;

  e2_node_connection_update_t cu = {
    .add = add,
    .len_add = len_add,
    .rem = rem,
    .len_rem = len_rem,
    .mod = mod,
    .len_mod = len_mod,
  };

  E2AP_PDU_t* pdu = e2ap_enc_node_connection_update_asn_pdu(&cu);
  e2ap_free_node_connection_update(&cu); 
  try_encode(pdu);
  byte_array_t ba2 = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba2);

}

void test_e2_node_connection_update_ack()
{
  const char* addr = "192.168.0.1";
  byte_array_t ba =  {.len = strlen(addr), .buf = malloc(strlen(addr))};
  memcpy(ba.buf, addr, strlen(addr));

  const size_t len_setup = 1;
 e2_connection_update_item_t* setup = calloc(len_setup, sizeof(e2_connection_update_item_t));
 setup->usage = TNL_USAGE_RIC_SERVICE;	
 setup->info.tnl_addr = ba;
 setup->info.tnl_port = NULL;

 e2_connection_setup_failed_t* failed = NULL;
 const size_t len_failed = 0;

  e2_node_connection_update_ack_t ca = {
    .setup = setup,
    .len_setup = len_setup,
    .failed = failed,
    .len_failed = len_failed,
  };

  E2AP_PDU_t* pdu = e2ap_enc_node_connection_update_ack_asn_pdu(&ca);
  e2ap_free_node_connection_update_ack(&ca); 
  try_encode(pdu);
  byte_array_t ba2 = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba2);
}

void test_e2_node_connection_update_failure()
{

  const cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};
  e2ap_time_to_wait_e* time_wait = NULL; 
  criticality_diagnostics_t* crit_diag = NULL; 

  e2_node_connection_update_failure_t cf = {
    .cause = cause,
    .time_wait = time_wait,
    .crit_diag = crit_diag,
  };

  E2AP_PDU_t* pdu = e2ap_enc_node_connection_update_failure_asn_pdu(&cf);
  e2ap_free_node_connection_update_failure(&cf); 
  try_encode(pdu);
  byte_array_t ba2 = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu);
  free_byte_array(ba2);
}

int main()
{
  test_ric_subscription_request();
  test_ric_subscription_response();
  test_ric_subscription_failure();
  test_ric_subscription_delete_request();
  test_ric_subscription_delete_response();
  test_ric_subscription_delete_failure();
  test_ric_indication();
  test_ric_control_request();
  test_ric_control_ack();
  test_ric_control_failure();
  test_error_indication();
  test_e2_setup_request();
  test_e2_setup_response();
  test_e2_setup_failure();
  test_reset_request();
  test_reset_response(); 
  test_ric_service_update(); 
  test_ric_service_update_ack(); 
  test_ric_service_update_failure(); 
  test_ric_service_query(); 
  test_e2_node_configuration_update();  
  test_e2_node_configuration_update_ack();  
  test_e2_node_configuration_update_failure();  
  test_e2_node_connection_update();
  test_e2_node_connection_update_ack(); 
  test_e2_node_connection_update_failure(); 

  return 0;
}

#endif

