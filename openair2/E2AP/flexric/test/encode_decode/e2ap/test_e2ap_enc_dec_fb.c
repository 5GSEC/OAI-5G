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



#include <stdlib.h>
#include <assert.h>

#include "../src/lib/ap/e2ap_ap.h"
#include "../src/lib/ap/type_defs.h"
#include "../src/lib/ap/enc/e2ap_msg_enc_fb.h"
#include "../src/lib/ap/dec/e2ap_msg_dec_fb.h"
#include "../src/lib/ap/free/e2ap_msg_free.h"


e2ap_fb_t fb_type;

void test_subscription_request()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12};
  const char* event_trigger = "This is the event trigger";
  byte_array_t ba_event_trigger = { .buf = (uint8_t *) event_trigger, .len = strlen(event_trigger) };

  const size_t len_action = 1;
  ric_action_t action[len_action];
  memset(action, 0, sizeof(action));
  action[0].id = 0;
  action[0].type = RIC_ACT_REPORT;

 ric_subscription_request_t sr_begin = {
   .ric_id = ric_id,
   .event_trigger = ba_event_trigger,
   .action = action,
   .len_action = len_action
 };

  byte_array_t ba = e2ap_enc_subscription_request_fb(&sr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == RIC_SUBSCRIPTION_REQUEST);
  ric_subscription_request_t* sr_end = &msg.u_msgs.ric_sub_req;
  assert(eq_ric_subscritption_request(&sr_begin, sr_end));
  e2ap_free_subscription_request(sr_end);
}

void test_subscription_response()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12};
  const size_t len_admitted = 1;
  ric_action_admitted_t admitted[len_admitted];
  admitted[0].ric_act_id = 42;
  const size_t len_not_admitted = 1;
  ric_action_not_admitted_t not_admitted[len_not_admitted];
  not_admitted[0].ric_act_id = 2;
  not_admitted[0].cause.present = CAUSE_PROTOCOL;
  not_admitted[0].cause.protocol = CAUSE_PROTOCOL_SEMANTIC_ERROR;

  ric_subscription_response_t sr_begin = {
    .ric_id = ric_id,
    .admitted = admitted,
    .len_admitted = len_admitted,
    .not_admitted = not_admitted,
    .len_na = len_not_admitted
  };

  byte_array_t ba = e2ap_enc_subscription_response_fb(&sr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == RIC_SUBSCRIPTION_RESPONSE);
  ric_subscription_response_t* sr_end = &msg.u_msgs.ric_sub_resp;
  assert(eq_ric_subscritption_response(&sr_begin, sr_end));
  e2ap_free_subscription_response(sr_end);
}

void test_subscription_failure()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12 };
  const size_t len_not_admitted = 1;
  ric_action_not_admitted_t not_admitted[len_not_admitted];
  not_admitted[0].ric_act_id = 2;
  not_admitted[0].cause.present = CAUSE_PROTOCOL;
  not_admitted[0].cause.protocol = CAUSE_PROTOCOL_SEMANTIC_ERROR;
  criticality_diagnostics_t* crit_diag = NULL;
  ric_subscription_failure_t sf_begin = {
    .ric_id = ric_id,
    .not_admitted = not_admitted,
    .len_na = len_not_admitted,
    .crit_diag = crit_diag, // optional
  };

  byte_array_t ba = e2ap_enc_subscription_failure_fb(&sf_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free_byte_array(ba);
  assert(msg.type == RIC_SUBSCRIPTION_FAILURE);
  ric_subscription_failure_t* sf_end = &msg.u_msgs.ric_sub_fail;
  assert(eq_ric_subscritption_failure(&sf_begin, sf_end));
  e2ap_free_subscription_failure(sf_end);
}

void test_subscription_delete_request()
{
  const ric_gen_id_t ric_id = { .ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12 };
  ric_subscription_delete_request_t dr_begin = { .ric_id = ric_id };

  byte_array_t ba = e2ap_enc_subscription_delete_request_fb(&dr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == RIC_SUBSCRIPTION_DELETE_REQUEST);
  ric_subscription_delete_request_t* dr_end = &msg.u_msgs.ric_sub_del_req;
  assert(eq_ric_subscription_delete_request(&dr_begin, dr_end));
  e2ap_free_subscription_delete_request(dr_end);
}

void test_subscription_delete_response()
{
  const ric_gen_id_t ric_id = { .ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12 };
  ric_subscription_delete_response_t dr_begin = { .ric_id = ric_id };

  byte_array_t ba = e2ap_enc_subscription_delete_response_fb(&dr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == RIC_SUBSCRIPTION_DELETE_RESPONSE);
  ric_subscription_delete_response_t* dr_end = &msg.u_msgs.ric_sub_del_resp;
  assert(eq_ric_subscription_delete_response(&dr_begin, dr_end));
  e2ap_free_subscription_delete_response(dr_end);
}

void test_subscription_delete_failure()
{
  const ric_gen_id_t ric_id = { .ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12 };
  cause_t cause = { .present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID };
  criticality_diagnostics_t* crit_diag = NULL; // optional
  ric_subscription_delete_failure_t dr_begin = {
    .ric_id = ric_id,
    .cause = cause,
    .crit_diag =  crit_diag,
  };

  byte_array_t ba = e2ap_enc_subscription_delete_failure_fb(&dr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free_byte_array(ba);
  assert(msg.type == RIC_SUBSCRIPTION_DELETE_FAILURE);
  ric_subscription_delete_failure_t* dr_end = &msg.u_msgs.ric_sub_del_fail;
  assert(eq_ric_subscription_delete_failure(&dr_begin, dr_end));
  e2ap_free_subscription_delete_failure(dr_end);
}

void test_indication()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12 }; 
  const uint8_t action_id = 19;
  ric_indication_type_e type = RIC_IND_INSERT;
  const char* hdr_str = "This";
  const size_t hdr_len = strlen(hdr_str);
  byte_array_t ba_hdr = { .buf = (uint8_t*) hdr_str, .len = hdr_len };
  const char* msg_str = "This is the message string";
  const size_t msg_len = strlen(msg_str);
  byte_array_t ba_msg = { .buf = (uint8_t*) msg_str, .len = msg_len };

  ric_indication_t ind_begin = {
    .ric_id = ric_id,
    .action_id = action_id,
    .type = type,
    .hdr = ba_hdr,
    .msg = ba_msg,
  };

  byte_array_t ba = e2ap_enc_indication_fb(&ind_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);

  assert(msg.type == RIC_INDICATION);
  ric_indication_t* ind_end = &msg.u_msgs.ric_ind;
  assert(eq_ric_indication(&ind_begin, ind_end));
  e2ap_free_indication(ind_end);
}

void test_control_request()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12 };
  byte_array_t* call_process_id = NULL; // optional
  const char* hdr_str = "Header string";
  const size_t hdr_len = strlen(hdr_str);
  byte_array_t ba_hdr = { .buf = (uint8_t*) hdr_str, .len = hdr_len };

  const char* msg_str = "Message string ";
  const size_t msg_len = strlen(msg_str);
  byte_array_t ba_msg = { .buf = (uint8_t*) msg_str, .len = msg_len };
  ric_control_ack_req_t* ack_req = NULL; // optional

  ric_control_request_t rcr_begin = {
    .ric_id = ric_id,
    .call_process_id = call_process_id,  // optional
    .hdr = ba_hdr,
    .msg = ba_msg,
    .ack_req = ack_req,  // optional
  };

  byte_array_t ba = e2ap_enc_control_request_fb(&rcr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == RIC_CONTROL_REQUEST);
  ric_control_request_t* rcr_end = &msg.u_msgs.ric_ctrl_req;
  assert(eq_ric_control_request(&rcr_begin, rcr_end));
  e2ap_free_control_request(rcr_end);
}

void test_control_request_ack()
{
  const ric_gen_id_t ric_id = { .ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12 };
  byte_array_t* call_process_id = NULL; // optional
  ric_control_status_t status = RIC_CONTROL_STATUS_SUCCESS;
  byte_array_t* control_outcome = NULL; // optional

  ric_control_acknowledge_t c_ack_begin = {
    .ric_id = ric_id,
    .call_process_id = call_process_id,
    .status = status,
    .control_outcome = control_outcome,
  };

  byte_array_t ba = e2ap_enc_control_ack_fb(&c_ack_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == RIC_CONTROL_ACKNOWLEDGE);
  ric_control_acknowledge_t* c_ack_end = &msg.u_msgs.ric_ctrl_ack;
  assert(eq_ric_control_ack_req(&c_ack_begin, c_ack_end));
  e2ap_free_control_ack(c_ack_end);
}

void test_control_request_failure()
{
  const ric_gen_id_t ric_id = {.ric_req_id = 0, .ric_inst_id = 2, .ran_func_id = 12};
  cause_t cause = { .present = CAUSE_RICREQUEST, .ricRequest = CAUSE_RIC_RAN_FUNCTION_ID_INVALID };
  byte_array_t* call_process_id = NULL; // optional
  byte_array_t* control_outcome = NULL;
  ric_control_failure_t cf_begin = {
    .ric_id = ric_id,
    .call_process_id = call_process_id, // optional
    .cause = cause,
    .control_outcome = control_outcome, // optional
  };

  byte_array_t ba = e2ap_enc_control_failure_fb(&cf_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free_byte_array(ba);
  assert(msg.type == RIC_CONTROL_FAILURE );
  ric_control_failure_t* cf_end = &msg.u_msgs.ric_ctrl_fail;
  assert(eq_control_failure(&cf_begin, cf_end));
  e2ap_free_control_failure(cf_end);
}

void test_setup_request()
{
  const plmn_t plmn = {.mcc = 10, .mnc = 15, .mnc_digit_len = 2};
  const global_e2_node_id_t ge2ni = {.type = ngran_gNB, .plmn = plmn, .nb_id = 5555};
  const char* func_def = "PDCP_stats";
  const size_t len_rf = 1;
  ran_function_t r[len_rf];
  memset(&r, 0, sizeof(r));
  r[0].id = 1;
  r[0].rev = 0;
  r[0].def.buf = (uint8_t*) func_def;
  r[0].def.len = strlen(func_def);

  e2_setup_request_t sr_begin = {
    .id = ge2ni,
    .ran_func_item = r,
    .len_rf = len_rf
  };

  byte_array_t ba = e2ap_enc_setup_request_fb(&sr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == E2_SETUP_REQUEST);
  e2_setup_request_t* sr_end = &msg.u_msgs.e2_stp_req;
  //assert(eq_e2_setup_request(&sr_begin, sr_end));
  e2ap_free_setup_request(sr_end);
}

void test_setup_response()
{
  const plmn_t plmn = {.mcc = 10, .mnc = 15, .mnc_digit_len = 2};
  const global_ric_id_t ric_id = { .plmn = plmn, .near_ric_id.double_word = 1337 };
  const size_t len_acc = 3;
  accepted_ran_function_t acc[len_acc];
  acc[0] = 1;
  acc[1] = 2;
  acc[2] = 3;
  const size_t len_rej = 2;
  rejected_ran_function_t rej[len_rej];
  rej[0] = (rejected_ran_function_t) {
    .id = 4,
    .cause = {
      .present = CAUSE_TRANSPORT,
      .transport = CAUSE_TRANSPORT_TRANSPORT_RESOURCE_UNAVAILABLE
    }
  };
  rej[1] = (rejected_ran_function_t) {
    .id = 5,
    .cause = {
      .present = CAUSE_PROTOCOL,
      .protocol = CAUSE_PROTOCOL_SEMANTIC_ERROR
    }
  };

  e2_setup_response_t sr_begin = {
    .id = ric_id,
    .accepted = acc,
    .len_acc = len_acc,
    .rejected = rej,
    .len_rej = len_rej
  };

  byte_array_t ba = e2ap_enc_setup_response_fb(&sr_begin);
  e2ap_msg_t msg = e2ap_msg_dec_fb(&fb_type, ba);
  free(ba.buf);
  assert(msg.type == E2_SETUP_RESPONSE);
  e2_setup_response_t* sr_end = &msg.u_msgs.e2_stp_resp;
  assert(eq_e2_setup_response(&sr_begin, sr_end));
  //e2ap_free_setup_response(sr_end); NOT IMPLEMENTED, free manually:
  if (sr_end->len_acc > 0)
    free(sr_end->accepted);
  if (sr_end->len_rej > 0)
    free(sr_end->rejected);
  if (sr_end->len_ccual > 0)
    assert(0 && "free() for len_ccual > 0 not implemented");
}

int main()
{
  init_ap_fb(&fb_type);

  test_subscription_request();
  test_subscription_response();
  test_subscription_failure();
  test_subscription_delete_response();
  test_subscription_delete_request();
  test_subscription_delete_failure();
  test_indication();
  test_control_request();
  test_control_request_ack();
  test_control_request_failure();

  test_setup_request();
  test_setup_response();

  return 0;
}
