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



#include <assert.h>
#include <time.h>

#include "../../../src/lib/ap/ie/asn/E2AP-PDU.h"

#include "../src/lib/ap/enc/e2ap_msg_enc_asn.h"
#include "../src/lib/ap/dec/e2ap_msg_dec_asn.h"
#include "../src/lib/ap/free/e2ap_msg_free.h"

void try_encode( E2AP_PDU_t* pdu)
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


void test_subscription_request()
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

 ric_subscription_request_t sr_begin = {
  .ric_id = ric_id,
  .event_trigger = ba_event_trigger,
  .action = action,
  .len_action = len_action
 };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_request_asn_pdu(&sr_begin);   
  e2ap_msg_t msg = e2ap_dec_subscription_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SUBSCRIPTION_REQUEST );
  ric_subscription_request_t* sr_end = &msg.u_msgs.ric_sub_req;
  assert(eq_ric_subscritption_request(&sr_begin, sr_end) == true);
  e2ap_free_subscription_request(&sr_begin);
  e2ap_free_subscription_request(sr_end);
}

void test_subscription_response()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
                          .ric_inst_id = 2,
                          .ran_func_id = 12};


  const size_t len_admitted = 1;;
  ric_action_admitted_t* admitted = calloc(len_admitted, sizeof(ric_action_admitted_t));
  admitted[0].ric_act_id = 42;
  ric_action_not_admitted_t* not_admitted = NULL;
  const size_t len_na = 0;

  ric_subscription_response_t sr_begin = {
    .ric_id = ric_id,
    .admitted = admitted,
    .len_admitted = len_admitted,
    .not_admitted = not_admitted,
    .len_na = len_na
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_response_asn_pdu(&sr_begin);   
  e2ap_msg_t msg = e2ap_dec_subscription_response(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SUBSCRIPTION_RESPONSE);
  ric_subscription_response_t* sr_end = &msg.u_msgs.ric_sub_resp;
  assert(eq_ric_subscritption_response(&sr_begin, sr_end) == true);
  e2ap_free_subscription_response(&sr_begin);
  e2ap_free_subscription_response(sr_end);
}

void test_subscription_failure()
{
    const ric_gen_id_t ric_id = {.ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12};


  ric_action_not_admitted_t* na = calloc(1,sizeof(ric_action_not_admitted_t)); 
  na->ric_act_id = 2;
  na->cause.present = CAUSE_PROTOCOL;
  na->cause.protocol = CAUSE_PROTOCOL_SEMANTIC_ERROR;	

  criticality_diagnostics_t* crit_diag = NULL; 

  ric_subscription_failure_t sf_begin = {
    .ric_id = ric_id,
    . not_admitted = na,
    .len_na = 1,
    .crit_diag = crit_diag, // optional
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_failure_asn_pdu(&sf_begin);   
  e2ap_msg_t msg = e2ap_dec_subscription_failure(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SUBSCRIPTION_FAILURE);
  ric_subscription_failure_t* sf_end = &msg.u_msgs.ric_sub_fail;
  assert(eq_ric_subscritption_failure(&sf_begin, sf_end) == true);
  e2ap_free_subscription_failure(&sf_begin);
  e2ap_free_subscription_failure(sf_end);
}

void test_subscription_delete_request()
{
  const ric_gen_id_t ric_id = {
    .ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12};

  ric_subscription_delete_request_t dr_begin = {
    .ric_id = ric_id,
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_delete_request_asn_pdu(&dr_begin);
  e2ap_msg_t msg = e2ap_dec_subscription_delete_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SUBSCRIPTION_DELETE_REQUEST);
  ric_subscription_delete_request_t* dr_end = &msg.u_msgs.ric_sub_del_req;
  assert(eq_ric_subscription_delete_request(&dr_begin, dr_end) == true);
  e2ap_free_subscription_delete_request(&dr_begin);
  e2ap_free_subscription_delete_request(dr_end);
}

void test_ric_subscription_delete_response()
{
  const ric_gen_id_t ric_id = {
    .ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12
  };

  ric_subscription_delete_response_t dr_begin = {
    .ric_id = ric_id,
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_delete_response_asn_pdu(&dr_begin);
  e2ap_msg_t msg = e2ap_dec_subscription_delete_response(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SUBSCRIPTION_DELETE_RESPONSE);
  ric_subscription_delete_response_t* dr_end = &msg.u_msgs.ric_sub_del_resp;
  assert(eq_ric_subscription_delete_response(&dr_begin, dr_end) == true);
  e2ap_free_subscription_delete_response(&dr_begin);
  e2ap_free_subscription_delete_response(dr_end);
}

void test_subscription_delete_failure()
{
  const ric_gen_id_t ric_id = {
    .ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12
  };

  cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};

  criticality_diagnostics_t* crit_diag = NULL; // optional

  ric_subscription_delete_failure_t dr_begin = {
  .ric_id = ric_id,
  .cause = cause,
  .crit_diag =  crit_diag,
  };

  E2AP_PDU_t* pdu = e2ap_enc_subscription_delete_failure_asn_pdu(&dr_begin);
  e2ap_msg_t msg = e2ap_dec_subscription_delete_failure(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SUBSCRIPTION_DELETE_FAILURE);
  ric_subscription_delete_failure_t* dr_end = &msg.u_msgs.ric_sub_del_fail;
  assert(eq_ric_subscription_delete_failure(&dr_begin, dr_end) == true);
  e2ap_free_subscription_delete_failure(&dr_begin);
  e2ap_free_subscription_delete_failure(dr_end);
}

void test_indication()
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

  ric_indication_t ind_begin = {
  .ric_id = ric_id,
  .action_id = action_id,
  .type = type,
  .hdr = ba_hdr,
  .msg = ba_msg,
  };

  E2AP_PDU_t* pdu = e2ap_enc_indication_asn_pdu(&ind_begin);
  e2ap_msg_t msg = e2ap_dec_indication(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_INDICATION);
  ric_indication_t* ind_end = &msg.u_msgs.ric_ind;
  assert(eq_ric_indication(&ind_begin, ind_end) == true);
  e2ap_free_indication(&ind_begin);
  e2ap_free_indication(ind_end);
}

void test_control_request()
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

  byte_array_t ba_msg;
  memset(&ba_msg, 0, sizeof(byte_array_t));
  const char* str_msg = "Message string ";
  ba_msg.buf = malloc(strlen(str_msg) );
  memcpy(ba_msg.buf, str_msg, strlen(str_msg) );
  ba_msg.len = strlen(str_msg);
  ric_control_ack_req_t* ack_req = NULL; // optional


  ric_control_request_t rcr_begin = {
  .ric_id = ric_id,
  .call_process_id =call_process_id, // optional
  .hdr = hdr,
  .msg = ba_msg,
  .ack_req = ack_req, // optional
};

  E2AP_PDU_t* pdu = e2ap_enc_control_request_asn_pdu(&rcr_begin);
  e2ap_msg_t msg = e2ap_dec_control_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_CONTROL_REQUEST);
  ric_control_request_t* rcr_end = &msg.u_msgs.ric_ctrl_req;
  assert(eq_ric_control_request(&rcr_begin, rcr_end) == true);
  e2ap_free_control_request(&rcr_begin);
  e2ap_free_control_request(rcr_end);
}

void test_control_request_ack()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12};

  byte_array_t* call_process_id = NULL; // optional
  ric_control_status_t status = RIC_CONTROL_STATUS_SUCCESS;
  byte_array_t* control_outcome = NULL; // optional

  ric_control_acknowledge_t c_ack_begin = {
    .ric_id = ric_id,
    .call_process_id = call_process_id,
    .status = status,
    .control_outcome = control_outcome,
  };

  E2AP_PDU_t* pdu = e2ap_enc_control_ack_asn_pdu(&c_ack_begin);
  e2ap_msg_t msg = e2ap_dec_control_ack(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_CONTROL_ACKNOWLEDGE);
  ric_control_acknowledge_t* c_ack_end = &msg.u_msgs.ric_ctrl_ack;
  assert(eq_ric_control_ack_req(&c_ack_begin, c_ack_end) == true);
  e2ap_free_control_ack(&c_ack_begin);
  e2ap_free_control_ack(c_ack_end);
}

void test_control_request_failure()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12};

  cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};

  byte_array_t* call_process_id = NULL; // optional
  byte_array_t* control_outcome = NULL;

  ric_control_failure_t cf_begin = {
    .ric_id = ric_id,
    .call_process_id = call_process_id, // optional
    .cause = cause,
    .control_outcome = control_outcome, // optional
  };

  E2AP_PDU_t* pdu = e2ap_enc_control_failure_asn_pdu(&cf_begin);
  e2ap_msg_t msg = e2ap_dec_control_failure(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_CONTROL_FAILURE ); 
  ric_control_failure_t* cf_end = &msg.u_msgs.ric_ctrl_fail;
  assert(eq_control_failure(&cf_begin, cf_end) == true);
  e2ap_free_control_failure(&cf_begin);
  e2ap_free_control_failure(cf_end);
}

void test_error_indication()
{
  ric_gen_id_t* ric_id = calloc(1, sizeof(ric_gen_id_t));
  ric_id->ric_inst_id = 2;
  ric_id->ric_req_id = 3;
  ric_id->ran_func_id = 42;

  cause_t* cause = NULL; // optional
  criticality_diagnostics_t* crit_diag = NULL; // optional

  e2ap_error_indication_t ei_begin = {
  .ric_id = ric_id, // optional
  .cause = cause, // optional
  .crit_diag = crit_diag, // optional
  };
  E2AP_PDU_t* pdu = e2ap_enc_error_indication_asn_pdu(&ei_begin);
  e2ap_msg_t msg = e2ap_dec_error_indication(pdu);
  free_pdu(pdu); 
  assert(msg.type == E2AP_ERROR_INDICATION); 
 e2ap_error_indication_t* ei_end = &msg.u_msgs.err_ind;

  assert(eq_error_indication(&ei_begin, ei_end) == true);
  e2ap_free_error_indication(&ei_begin);
  e2ap_free_error_indication(ei_end);
}

void test_setup_rquest()
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

  e2_setup_request_t e2_stp_req_begin =
  {
    .id = id,
    .ran_func_item = ran_func_item ,
    .len_rf = len_rf, 
    .comp_conf_update = comp_conf_update,
    .len_ccu = len_ccu
  };

  E2AP_PDU_t* pdu = e2ap_enc_setup_request_asn_pdu(&e2_stp_req_begin);
  e2ap_msg_t msg = e2ap_dec_setup_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == E2_SETUP_REQUEST); 
  e2_setup_request_t* e2_stp_req_end = &msg.u_msgs.e2_stp_req;

  assert(eq_e2_setup_request(&e2_stp_req_begin, e2_stp_req_end) == true);
  e2ap_free_setup_request(&e2_stp_req_begin);
  e2ap_free_setup_request(e2_stp_req_end);
}

void test_setup_response()
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

  e2_setup_response_t e2_stp_res_begin = {
    .id = id,
    .accepted = accepted ,
    .len_acc = len_acc ,
    .rejected = rejected ,
    .len_rej = len_rej ,
    .comp_conf_update_ack_list = comp_conf_update_ack_list ,
    .len_ccual = len_ccual ,
  };

  E2AP_PDU_t* pdu = e2ap_enc_setup_response_asn_pdu(&e2_stp_res_begin);
  e2ap_msg_t msg = e2ap_dec_setup_response(pdu);
  free_pdu(pdu); 
  assert(msg.type == E2_SETUP_RESPONSE); 
  e2_setup_response_t* e2_stp_res_end = &msg.u_msgs.e2_stp_resp;

  assert(eq_e2_setup_response(&e2_stp_res_begin, e2_stp_res_end) == true);
  e2ap_free_setup_response(&e2_stp_res_begin);
  e2ap_free_setup_response(e2_stp_res_end);
}

void test_setup_failure()
{
  cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};

//  e2ap_time_to_wait_e* time_to_wait_ms = NULL;
  e2ap_time_to_wait_e* time_to_wait_ms = calloc(1, sizeof(e2ap_time_to_wait_e)); 
  *time_to_wait_ms = TIMETOWAIT_V1S; 

  criticality_diagnostics_t* crit_diag = NULL; // optional
  transport_layer_information_t* tl_info = calloc(1,sizeof(transport_layer_information_t)); // optional
  const char* addr = "192.168.1.0";
  tl_info->address.buf = malloc(strlen(addr)); 
  memcpy(tl_info->address.buf, addr, strlen(addr) );
  tl_info->address.len = strlen(addr); 
  tl_info->port = calloc(1,sizeof(byte_array_t));
  const char* port = "1010";
  tl_info->port->buf = malloc(strlen(port));
  memcpy(tl_info->port->buf, port, strlen(port));
  tl_info->port->len = strlen(port);  

  e2_setup_failure_t sf_begin = {
  .cause = cause,
  .time_to_wait_ms = time_to_wait_ms,            // optional
  .crit_diag = crit_diag, // optional
  .tl_info = tl_info, // optional
};

  E2AP_PDU_t* pdu = e2ap_enc_setup_failure_asn_pdu(&sf_begin);
  e2ap_msg_t msg = e2ap_dec_setup_failure(pdu);
  free_pdu(pdu); 
  assert(msg.type == E2_SETUP_FAILURE); 
  e2_setup_failure_t* sf_end  = &msg.u_msgs.e2_stp_fail;

  assert(eq_e2_setup_failure(&sf_begin, sf_end) == true);
  e2ap_free_setup_failure(&sf_begin);
  e2ap_free_setup_failure(sf_end);
}

void test_reset_request()
{
  const cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};
 e2ap_reset_request_t rr_begin = {.cause = cause};

  E2AP_PDU_t* pdu = e2ap_enc_reset_request_asn_pdu(&rr_begin);
  e2ap_msg_t msg = e2ap_dec_reset_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == E2AP_RESET_REQUEST); 
 e2ap_reset_request_t* rr_end  = &msg.u_msgs.rst_req;

  assert(eq_reset_request(&rr_begin, rr_end) == true);
  e2ap_free_reset_request(&rr_begin);
  e2ap_free_reset_request(rr_end);
}

void test_reset_response()
{
  criticality_diagnostics_t* crit_diag = NULL; // optional
  e2ap_reset_response_t rr_begin = {
    .crit_diag = crit_diag, // optional
  };

  E2AP_PDU_t* pdu = e2ap_enc_reset_response_asn_pdu(&rr_begin);
  e2ap_msg_t msg = e2ap_dec_reset_response(pdu);
  free_pdu(pdu); 
  assert(msg.type == E2AP_RESET_RESPONSE); 
  e2ap_reset_response_t* rr_end = &msg.u_msgs.rst_resp;

  assert(eq_reset_response(&rr_begin, rr_end) == true);
  e2ap_free_reset_response(&rr_begin);
  e2ap_free_reset_response(rr_end);
}

void test_service_update()
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

  ric_service_update_t su_begin = {
    .len_added = len_added,
    .added = added,  
    .modified = modified,
    .len_modified = len_modified, 
    .deleted = deleted,
    .len_deleted = len_deleted,
  };

  E2AP_PDU_t* pdu = e2ap_enc_service_update_asn_pdu(&su_begin);
  e2ap_msg_t msg = e2ap_dec_service_update(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SERVICE_UPDATE); 
  ric_service_update_t* su_end = &msg.u_msgs.ric_serv_updt;

  assert(eq_ric_service_update(&su_begin, su_end) == true);
  e2ap_free_service_update(&su_begin);
  e2ap_free_service_update(su_end);
}

void test_service_update_ack()
{
  const size_t len_accepted = 1;
  ran_function_id_t* accepted = calloc(len_accepted, sizeof(ran_function_t));
  accepted->id = 3;
  accepted->rev = 0;

  rejected_ran_function_t* rejected = NULL;
  size_t len_rejected = 0;

  ric_service_update_ack_t su_begin = {
    .accepted = accepted,
    .len_accepted = len_accepted,
    .rejected = rejected,
    .len_rejected = len_rejected,
  };

  E2AP_PDU_t* pdu = e2ap_enc_service_update_ack_asn_pdu(&su_begin);
  e2ap_msg_t msg = e2ap_dec_service_update_ack(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SERVICE_UPDATE_ACKNOWLEDGE); 
  ric_service_update_ack_t* su_end = &msg.u_msgs.ric_serv_updt_ack;

  assert(eq_ric_service_update_ack(&su_begin, su_end) == true);
  e2ap_free_service_update_ack(&su_begin);
  e2ap_free_service_update_ack(su_end);
}

void test_service_update_failure()
{
  const cause_t cause = {.present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID};
  const size_t len_rej = 1;
  rejected_ran_function_t* rejected = calloc(len_rej, sizeof(rejected_ran_function_t));
   rejected->id = 42;
    rejected->cause = cause;

  e2ap_time_to_wait_e* time_to_wait = NULL;
  criticality_diagnostics_t* crit_diag = NULL;


  ric_service_update_failure_t uf_begin = {
  .rejected = rejected, 
  .len_rej= len_rej,
  .time_to_wait = time_to_wait,
  .crit_diag = crit_diag,
  };

  E2AP_PDU_t* pdu = e2ap_enc_service_update_failure_asn_pdu(&uf_begin);
  e2ap_msg_t msg = e2ap_dec_service_update_failure(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SERVICE_UPDATE_FAILURE); 
  ric_service_update_failure_t* uf_end = &msg.u_msgs.ric_serv_updt_fail;

  assert(eq_ric_service_update_failure(&uf_begin, uf_end) == true);
  e2ap_free_service_update_failure(&uf_begin);
  e2ap_free_service_update_failure(uf_end);
}

void test_service_query()
{
  size_t len_accepted = 1;
  e2ap_ran_function_id_rev_t* accepted = calloc(len_accepted, sizeof(e2ap_ran_function_id_rev_t));

  accepted->id = 42;
  accepted->rev = 5;

  ric_service_query_t sq_begin = {
    .accepted = accepted,
    .len_accepted = len_accepted,
  };

  E2AP_PDU_t* pdu = e2ap_enc_service_query_asn_pdu(&sq_begin);
  e2ap_msg_t msg = e2ap_dec_service_query(pdu);
  free_pdu(pdu); 
  assert(msg.type == RIC_SERVICE_QUERY); 
  ric_service_query_t* sq_end = &msg.u_msgs.ric_serv_query;
  assert(eq_ric_service_query(&sq_begin, sq_end) == true);
  e2ap_free_service_query(&sq_begin);
  e2ap_free_service_query(sq_end);
}

void test_node_configuration_update()
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

 e2_node_configuration_update_t cu_begin = {
   .comp_conf_update_list = cc,
   .len_ccul = len_ccul,
 };

  E2AP_PDU_t* pdu = e2ap_enc_node_configuration_update_asn_pdu(&cu_begin);
  e2ap_msg_t msg = e2ap_dec_node_configuration_update(pdu);
  free_pdu(pdu); 
  assert(msg.type == E2_NODE_CONFIGURATION_UPDATE); 
  e2_node_configuration_update_t* cu_end = &msg.u_msgs.e2_node_conf_updt;
  assert(eq_node_configuration_update(&cu_begin, cu_end) == true);
  e2ap_free_node_configuration_update(&cu_begin);
  e2ap_free_node_configuration_update(cu_end);
}

static inline
void fill_ran_function(ran_function_t* rf)
{
  assert(rf != NULL);

  const char* def = "Definition";
  size_t const sz = strlen(def);

  rf->def.len = sz;
  rf->def.buf = malloc(sz);
  assert(rf->def.buf != NULL && "Memory exhauested"); 
  memcpy(rf->def.buf, def, sz);

  rf->id = rand()%1024;  
  rf->rev = rand()%8; 
  rf->oid = NULL;
}

static
E2AP_PDU_t* e2ap_create_pdu(const uint8_t* buffer, int buffer_len)
{
  assert(buffer != NULL);
  assert(buffer_len > -1);

  E2AP_PDU_t* pdu = calloc(1, sizeof(E2AP_PDU_t));
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2AP_PDU, (void**)&pdu, buffer, buffer_len);
  //printf("rval.code = %d\n", rval.code);
  //fprintf(stdout, "length of data %ld\n", rval.consumed);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGEND_BASIC_PER syntax?");
  return pdu;
}



void test_e42_setup_request()
{
  time_t t;
  srand((unsigned) time(&t));
  uint32_t r = rand()%8 + 1;

  e42_setup_request_t sr_begin = {
    .len_rf = r
  };

  if(r > 0){
    sr_begin.ran_func_item = calloc(r, sizeof(ran_function_t));
    assert(sr_begin.ran_func_item != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < sr_begin.len_rf; ++i){
    ran_function_t* rf = &sr_begin.ran_func_item[i];
    fill_ran_function(rf);
  }


/*
  byte_array_t ba = e2ap_enc_e42_setup_request_asn(&sr_begin);
  assert(ba.buf != NULL && ba.len > 0);
  E2AP_PDU_t* pdu = e2ap_create_pdu(ba.buf, ba.len);
  assert(pdu != NULL);
//  const e2_msg_type_t msg_type =  e2ap_get_msg_type(pdu);  
//  printf("Decoding message type = %d \n", msg_type);
  e2ap_msg_t msg =  e2ap_dec_e42_setup_request(pdu);
*/




  E2AP_PDU_t* pdu = e2ap_enc_e42_setup_request_asn_pdu(&sr_begin);
  e2ap_msg_t msg = e2ap_dec_e42_setup_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == E42_SETUP_REQUEST); 
  e42_setup_request_t * sr_end = &msg.u_msgs.e42_stp_req;
  assert(eq_e42_setup_request(&sr_begin, sr_end) == true);
  e2ap_free_e42_setup_request(&sr_begin);
  e2ap_free_e42_setup_request(sr_end);
}


void test_e42_setup_response()
{
  time_t t;
  srand((unsigned) time(&t));

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


  e42_setup_response_t sr_begin = {
    .xapp_id = rand()%1024,
    .len_e2_nodes_conn = rand()%4+1,
  };

  if(sr_begin.len_e2_nodes_conn > 0 ){
    sr_begin.nodes = calloc(sr_begin.len_e2_nodes_conn, sizeof( e2_node_connected_t ) );
    assert(sr_begin.nodes != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < sr_begin.len_e2_nodes_conn; ++i){
    e2_node_connected_t* n = &sr_begin.nodes[i];
    n->id = id; 

    uint32_t r = rand()%8;

    n->len_rf = r;
    if(r > 0){
      n->ack_rf = calloc(r, sizeof(ran_function_t));
      assert(n->ack_rf != NULL && "Memory exhausted");
    }

    for(size_t i = 0; i < r; ++i){
      ran_function_t* rf = &n->ack_rf[i];
      fill_ran_function(rf);
    }
  }

  byte_array_t ba = e2ap_enc_e42_setup_response_asn(&sr_begin);
  assert(ba.buf != NULL && ba.len > 0);
  E2AP_PDU_t* pdu = e2ap_create_pdu(ba.buf, ba.len);
  free_byte_array(ba);
  assert(pdu != NULL);
  e2ap_msg_t msg =  e2ap_dec_e42_setup_response(pdu);
  free_pdu(pdu); 
  assert(msg.type == E42_SETUP_RESPONSE); 
  e42_setup_response_t * sr_end = &msg.u_msgs.e42_stp_resp;
  assert(eq_e42_setup_response(&sr_begin, sr_end) == true);
  e2ap_free_e42_setup_response(&sr_begin);
  e2ap_free_e42_setup_response(sr_end);
}

static
void test_e42_subscription_request()
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

 ric_subscription_request_t sr = {
  .ric_id = ric_id,
  .event_trigger = ba_event_trigger,
  .action = action,
  .len_action = len_action
 };

  e42_ric_subscription_request_t e42_sr_begin = {
    .xapp_id = 13,
    .id = id,
    .sr = sr
  }; 

  E2AP_PDU_t* pdu = e2ap_enc_e42_subscription_request_asn_pdu(&e42_sr_begin);   
  e2ap_msg_t msg = e2ap_dec_e42_subscription_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == E42_RIC_SUBSCRIPTION_REQUEST );
  e42_ric_subscription_request_t* e42_sr_end = &msg.u_msgs.e42_ric_sub_req;
  assert(eq_e42_ric_subscritption_request(&e42_sr_begin, e42_sr_end) == true);
  e2ap_free_e42_ric_subscription_request(&e42_sr_begin);
  e2ap_free_e42_ric_subscription_request(e42_sr_end);

}

static
void test_e42_subscription_delete_request()
{

  const ric_gen_id_t ric_id = {
    .ric_req_id = 0,
    .ric_inst_id = 2,
    .ran_func_id = 12};

  ric_subscription_delete_request_t dr_begin = {
    .ric_id = ric_id,
  };


  e42_ric_subscription_delete_request_t e42_sdr_begin = {
    .xapp_id = 13,
    .sdr = dr_begin 
  }; 

  E2AP_PDU_t* pdu = e2ap_enc_e42_subscription_delete_request_asn_pdu(&e42_sdr_begin);
  e2ap_msg_t msg = e2ap_dec_e42_subscription_delete_request(pdu);
  free_pdu(pdu); 
  assert(msg.type == E42_RIC_SUBSCRIPTION_DELETE_REQUEST);
  e42_ric_subscription_delete_request_t* e42_sdr_end = &msg.u_msgs.e42_ric_sub_del_req;
  assert(eq_e42_ric_subscription_delete_request(&e42_sdr_begin, e42_sdr_end) == true);
  e2ap_free_e42_ric_subscription_delete_request(&e42_sdr_begin);
  e2ap_free_e42_ric_subscription_delete_request(e42_sdr_end);
}

static
void test_e42_control_request()
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

  byte_array_t ba_msg;
  memset(&ba_msg, 0, sizeof(byte_array_t));
  const char* str_msg = "Message string ";
  ba_msg.buf = malloc(strlen(str_msg) );
  memcpy(ba_msg.buf, str_msg, strlen(str_msg) );
  ba_msg.len = strlen(str_msg);
  ric_control_ack_req_t* ack_req = NULL; // optional

  ric_control_request_t rcr = {
  .ric_id = ric_id,
  .call_process_id =call_process_id, // optional
  .hdr = hdr,
  .msg = ba_msg,
  .ack_req = ack_req, // optional
  };


  e42_ric_control_request_t rcr_begin = {
    .xapp_id = 7,
    .id = id,
    .ctrl_req = rcr
  }; 

  E2AP_PDU_t* pdu = e2ap_enc_e42_control_request_asn_pdu(&rcr_begin);

  const byte_array_t ba = e2ap_enc_asn_pdu_ba(pdu);
  free_pdu(pdu); 

  E2AP_PDU_t* pdu2 = e2ap_create_pdu(ba.buf, ba.len);
  free_byte_array(ba);

  e2ap_msg_t msg = e2ap_dec_e42_control_request(pdu2);
  free_pdu(pdu2); 

  assert(msg.type == E42_RIC_CONTROL_REQUEST);
  e42_ric_control_request_t* rcr_end = &msg.u_msgs.e42_ric_ctrl_req;
  assert(eq_e42_ric_control_request(&rcr_begin, rcr_end) == true);
  e2ap_free_e42_ric_control_request(&rcr_begin);
  e2ap_free_e42_ric_control_request(rcr_end);

}


int main()
{

  for(size_t i =0 ; i < 1024; ++i){
    test_subscription_request();
    test_subscription_response();
    test_subscription_failure();
    test_subscription_delete_request();
    test_ric_subscription_delete_response();
    test_subscription_delete_failure();
    test_indication();
    test_control_request(); 
    test_control_request_ack(); 
    test_control_request_failure(); 
    test_error_indication();
    test_setup_rquest();
    test_setup_response();
    test_setup_failure();
    test_reset_request(); 
    test_reset_response();
    test_service_update();
    test_service_update_ack();
    test_service_update_failure();
    test_service_query();

    // ToDO:
    //  test_node_configuration_update();
    //  test_node_configuration_update_ack();
    //  test_node_configuration_update_failure();
    //  test_connection_update();
    //  test_connection_update_ack();
    //  test_connection_update_failure();

    // E42
    test_e42_setup_request();
    test_e42_setup_response();
    test_e42_subscription_request();
    test_e42_subscription_delete_request();     

    test_e42_control_request();

  }
  puts("Sucess running the encoding/decoding test");
  return 0;
}

