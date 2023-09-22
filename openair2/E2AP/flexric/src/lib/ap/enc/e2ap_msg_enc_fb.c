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



#include "e2ap_msg_enc_fb.h"

#include <stdio.h>
#include <assert.h>

#include "../ie/fb/e2ap_builder.h"
#include "e2ap_verifier.h"

#include "util/ngran_types.h"
#include "lib/ap/type_defs.h"


//#include "ngran_types.h"

#include "../global_consts.h"

static inline
e2ap_Cause_union_ref_t copy_cause(flatcc_builder_t* B, cause_t src)
{
  switch (src.present) {
    case CAUSE_NOTHING:
      assert(0 && "not implemented");
      break;
    case CAUSE_RICREQUEST:
      assert(src.ricRequest < 11);
      return e2ap_Cause_as_ricRequest(e2ap_RicRequest_create(B, src.ricRequest));
    case CAUSE_RICSERVICE:
      assert(src.ricService < 3);
      return e2ap_Cause_as_ricService(e2ap_RicService_create(B, src.ricService));
    case CAUSE_TRANSPORT:
      assert(src.transport < 2);
      return e2ap_Cause_as_transportLayer(e2ap_TransportLayer_create(B, src.transport));
    case CAUSE_PROTOCOL:
      assert(src.protocol < 7);
      return e2ap_Cause_as_protocol(e2ap_Protocol_create(B, src.protocol));
    case CAUSE_MISC:
      assert(src.misc < 4);
      return e2ap_Cause_as_misc(e2ap_Misc_create(B, src.misc));
  }
  assert(0 && "illegal code path");
}

static inline
uint32_t get_time_to_wait(uint32_t* ttw)
{
  /* the ASN.1 encoding treats the time_to_wait_ms type as an enum, although it
   * is a uint32_t (in ms). For the time being, we will do the same, but
   * convert it to ms value for the flatbuffers */
  assert(*ttw < 18);
  switch (*ttw) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 2;
    case 3: return 5;
    case 4: return 10;
    case 5: return 20;
    case 6: return 30;
    case 7: return 40;
    case 8: return 50;
    case 9: return 100;
    case 10: return 200;
    case 11: return 500;
    case 12: return 1000;
    case 13: return 2000;
    case 14: return 5000;
    case 15: return 10000;
    case 16: return 20000;
    case 17: return 60000;
    default: assert(0 && "illegal value for time to wait");
  }
  assert(0 && "illegal code path");
}


///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////



byte_array_t e2ap_enc_subscription_request_fb(const ric_subscription_request_t* sr)
{
  assert(sr);
  // action_def is optional, therefore it can be NULL
  assert(sr->event_trigger.buf != NULL && sr->event_trigger.len > 0);
  assert(sr->len_action <= (size_t)MAX_NUM_ACTION_DEF);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicSubscriptionRequest_start(B);
  e2ap_RicSubscriptionRequest_eventTriggerDefinition_create(B, sr->event_trigger.buf, sr->event_trigger.len);

  e2ap_RicSubscriptionRequest_actions_start(B);
  for(size_t i = 0; i < sr->len_action; ++i){
    const ric_action_t* src = &sr->action[i]; 
    e2ap_Action_start(B);
    if (src->definition)
      e2ap_Action_definition_create(B, src->definition->buf, src->definition->len);
    if (src->subseq_action) {
      e2ap_RicSubsequentAction_start(B);
      e2ap_RicSubsequentAction_type_force_add(B, src->subseq_action->type);
      if (src->subseq_action->time_to_wait_ms)
        e2ap_RicSubsequentAction_timeToWait_add(B, get_time_to_wait(src->subseq_action->time_to_wait_ms));
      e2ap_Action_subsequentAction_add(B, e2ap_RicSubsequentAction_end(B));
    }
    e2ap_Action_id_force_add(B, src->id);
    e2ap_Action_type_force_add(B, src->type); /* maps one-to-one */
    e2ap_RicSubscriptionRequest_actions_push(B, e2ap_Action_end(B));
  }
  e2ap_RicSubscriptionRequest_actions_end(B);

  e2ap_RicSubscriptionRequest_requestId_create(B, sr->ric_id.ric_req_id, sr->ric_id.ric_inst_id);
  e2ap_RicSubscriptionRequest_ranFunctionId_force_add(B, sr->ric_id.ran_func_id);

  e2ap_Message_union_ref_t msg = e2ap_Message_as_subscriptionRequest(e2ap_RicSubscriptionRequest_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_subscription_request_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_REQUEST);
  return e2ap_enc_subscription_request_fb(&msg->u_msgs.ric_sub_req);
}

byte_array_t e2ap_enc_subscription_response_fb(const ric_subscription_response_t* sr)
{
  assert(sr);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicSubscriptionResponse_start(B);

  e2ap_RicSubscriptionResponse_admittedActions_start(B);
  for (size_t i = 0; i < sr->len_admitted; ++i)
    e2ap_RicSubscriptionResponse_admittedActions_push_create(B, sr->admitted[i].ric_act_id);
  e2ap_RicSubscriptionResponse_admittedActions_end(B);

  e2ap_RicSubscriptionResponse_notAdmittedActions_start(B);
  for (size_t i = 0; i < sr->len_na; ++i) {
    const ric_action_not_admitted_t* src = &sr->not_admitted[i];  
    e2ap_RicSubscriptionResponse_notAdmittedActions_push_create(B, src->ric_act_id, copy_cause(B, src->cause));
  }
  e2ap_RicSubscriptionResponse_notAdmittedActions_end(B);

  e2ap_RicSubscriptionResponse_requestId_create(B, sr->ric_id.ric_req_id, sr->ric_id.ric_inst_id);
  e2ap_RicSubscriptionResponse_ranFunctionId_force_add(B, sr->ric_id.ran_func_id);

  e2ap_Message_union_ref_t msg = e2ap_Message_as_subscriptionResponse(e2ap_RicSubscriptionResponse_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_subscription_response_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_RESPONSE);
  return e2ap_enc_subscription_response_fb(&msg->u_msgs.ric_sub_resp);
}

byte_array_t e2ap_enc_subscription_failure_fb(const ric_subscription_failure_t* sf)
{
  assert(sf);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicSubscriptionFailure_start(B);

  e2ap_RicSubscriptionFailure_notAdmittedActions_start(B);
  for (size_t i = 0; i < sf->len_na; ++i) {
    const ric_action_not_admitted_t* src = &sf->not_admitted[i];
    e2ap_RicSubscriptionFailure_notAdmittedActions_push_create(B, src->ric_act_id, copy_cause(B, src->cause));
  }
  e2ap_RicSubscriptionFailure_notAdmittedActions_end(B);

  assert(!sf->crit_diag && "encoding of criticalityDiagnostics is not implemented");

  e2ap_RicSubscriptionFailure_requestId_create(B, sf->ric_id.ric_req_id, sf->ric_id.ric_inst_id);
  e2ap_RicSubscriptionFailure_ranFunctionId_force_add(B, sf->ric_id.ran_func_id);

  e2ap_Message_union_ref_t msg = e2ap_Message_as_subscriptionFailure(e2ap_RicSubscriptionFailure_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_subscription_failure_fb_msg(const e2ap_msg_t* msg )
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_FAILURE);
  return e2ap_enc_subscription_failure_fb(&msg->u_msgs.ric_sub_fail);
}

byte_array_t e2ap_enc_subscription_delete_request_fb(const ric_subscription_delete_request_t* sdr)
{
  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicSubscriptionDeleteRequest_start(B);
  e2ap_RicSubscriptionDeleteRequest_requestId_create(B, sdr->ric_id.ric_req_id, sdr->ric_id.ric_inst_id);
  e2ap_RicSubscriptionDeleteRequest_ranFunctionId_force_add(B, sdr->ric_id.ran_func_id);

  e2ap_Message_union_ref_t msg = e2ap_Message_as_subscriptionDeleteRequest(e2ap_RicSubscriptionDeleteRequest_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_subscription_delete_request_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_REQUEST);
  return e2ap_enc_subscription_delete_request_fb(&msg->u_msgs.ric_sub_del_req);
}

byte_array_t e2ap_enc_subscription_delete_response_fb(const ric_subscription_delete_response_t* sdr)
{
  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicSubscriptionDeleteResponse_start(B);
  e2ap_RicSubscriptionDeleteResponse_requestId_create(B, sdr->ric_id.ric_req_id, sdr->ric_id.ric_inst_id);
  e2ap_RicSubscriptionDeleteResponse_ranFunctionId_force_add(B, sdr->ric_id.ran_func_id);

  e2ap_Message_union_ref_t msg = e2ap_Message_as_subscriptionDeleteResponse(e2ap_RicSubscriptionDeleteResponse_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_subscription_delete_response_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_RESPONSE);

  //return e2ap_enc_subscription_delete_request_fb(&msg->u_msgs.ric_sub_del_req);
  return e2ap_enc_subscription_delete_response_fb(&msg->u_msgs.ric_sub_del_resp);
}

byte_array_t e2ap_enc_subscription_delete_failure_fb(const ric_subscription_delete_failure_t* sdf)
{
  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicSubscriptionDeleteFailure_start(B);

  e2ap_RicSubscriptionDeleteFailure_cause_add(B, copy_cause(B, sdf->cause));

  assert(!sdf->crit_diag && "encoding of criticalityDiagnostics is not implemented");

  e2ap_RicSubscriptionDeleteFailure_requestId_create(B, sdf->ric_id.ric_req_id, sdf->ric_id.ric_inst_id);
  e2ap_RicSubscriptionDeleteFailure_ranFunctionId_force_add(B, sdf->ric_id.ran_func_id);

  e2ap_Message_union_ref_t msg = e2ap_Message_as_subscriptionDeleteFailure(e2ap_RicSubscriptionDeleteFailure_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_subscription_delete_failure_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_FAILURE);
  return e2ap_enc_subscription_delete_failure_fb(&msg->u_msgs.ric_sub_del_fail);
}

byte_array_t e2ap_enc_indication_fb(const ric_indication_t* ind)
{
  e2ap_RicIndicationType_enum_t ric_ind_type = ind->type; 
  assert(ric_ind_type == e2ap_RicIndicationType_Report || ric_ind_type == e2ap_RicIndicationType_Insert);
  assert(ind->hdr.buf && ind->hdr.len > 0);
  assert(ind->msg.buf && ind->msg.len > 0);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  /* flatcc repo: builder.md#packing-tables:
   * "By reordering the fields, the table may be packed better, or be better
   * able to reuse an existing vtable. The create call already does this" -> we
   * use the ordering of e2ap_RicIndication_create(B) to do that */
  e2ap_RicIndication_start(B);
  e2ap_RicIndication_header_create(B, ind->hdr.buf, ind->hdr.len);
  /* alternatively, we might do: */
  //e2ap_RicIndication_header_start(B);
  //uint8_t* h = e2ap_RicIndication_header_extend(B, ind_hdr_len);
  //memcpy(h, ind_hdr, ind_hdr_len);
  //e2ap_RicIndication_header_truncate(B, hdr_len);
  //e2ap_RicIndication_header_end(B);
  e2ap_RicIndication_message_create(B, ind->msg.buf, ind->msg.len);
  if (ind->call_process_id)
    e2ap_RicIndication_callProcessId_create(B, ind->call_process_id->buf, ind->call_process_id->len);
  e2ap_RicIndication_requestId_create(B, ind->ric_id.ric_req_id, ind->ric_id.ric_inst_id);
  e2ap_RicIndication_ranFunctionId_force_add(B, ind->ric_id.ran_func_id);
  if (ind->sn)
    e2ap_RicIndication_sn_force_add(B, *ind->sn);
  e2ap_RicIndication_actionId_force_add(B, ind->action_id);
  e2ap_RicIndication_indicationType_force_add(B, ric_ind_type);
  e2ap_Message_union_ref_t msg = e2ap_Message_as_indication(e2ap_RicIndication_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_indication_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_INDICATION);
  return e2ap_enc_indication_fb(&msg->u_msgs.ric_ind);
}

byte_array_t e2ap_enc_control_request_fb(const ric_control_request_t* cr)
{
  assert(cr);
  assert(cr->hdr.buf && cr->hdr.len > 0);
  assert(cr->msg.buf && cr->msg.len > 0);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicControlRequest_start(B);

  if (cr->call_process_id)
    e2ap_RicControlRequest_callProcessId_create(B, cr->call_process_id->buf, cr->call_process_id->len);
  e2ap_RicControlRequest_header_create(B, cr->hdr.buf, cr->hdr.len);
  e2ap_RicControlRequest_message_create(B, cr->msg.buf, cr->msg.len);
  e2ap_RicControlRequest_requestId_create(B, cr->ric_id.ric_req_id, cr->ric_id.ric_inst_id);
  e2ap_RicControlRequest_ranFunctionId_force_add(B, cr->ric_id.ran_func_id);
  if (cr->ack_req)
    e2ap_RicControlRequest_ackRequest_force_add(B, *cr->ack_req); /* maps one-to-one */

  e2ap_Message_union_ref_t msg = e2ap_Message_as_controlRequest(e2ap_RicControlRequest_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_control_request_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL );
  assert(msg->type == RIC_CONTROL_REQUEST);
  return e2ap_enc_control_request_fb(&msg->u_msgs.ric_ctrl_req);
}

byte_array_t e2ap_enc_control_ack_fb(const ric_control_acknowledge_t* ca)
{
  e2ap_RicControlStatus_enum_t ctrl_status = ca->status;
  assert(ctrl_status == e2ap_RicControlStatus_Success
      || ctrl_status == e2ap_RicControlStatus_Rejected
      || ctrl_status == e2ap_RicControlStatus_Failed);  

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicControlAcknowledge_start(B);

  if (ca->call_process_id)
    e2ap_RicControlAcknowledge_callProcessId_create(B, ca->call_process_id->buf, ca->call_process_id->len);
  if (ca->control_outcome)
    e2ap_RicControlAcknowledge_outcome_create(B, ca->control_outcome->buf, ca->control_outcome->len);

  e2ap_RicControlAcknowledge_requestId_create(B, ca->ric_id.ric_req_id, ca->ric_id.ric_inst_id);
  e2ap_RicControlAcknowledge_ranFunctionId_force_add(B, ca->ric_id.ran_func_id);

  e2ap_RicControlAcknowledge_status_force_add(B, ctrl_status); /* maps one-to-one */

  e2ap_Message_union_ref_t msg = e2ap_Message_as_controlAcknowledge(e2ap_RicControlAcknowledge_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_control_ack_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_ACKNOWLEDGE);
  return e2ap_enc_control_ack_fb(&msg->u_msgs.ric_ctrl_ack);
}

byte_array_t e2ap_enc_control_failure_fb(const ric_control_failure_t* cf)
{
  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_RicControlFailure_start(B);

  if (cf->call_process_id)
    e2ap_RicControlFailure_callProcessId_create(B, cf->call_process_id->buf, cf->call_process_id->len);

  e2ap_RicControlFailure_cause_add(B, copy_cause(B, cf->cause));

  if (cf->control_outcome)
    e2ap_RicControlFailure_outcome_create(B, cf->control_outcome->buf, cf->control_outcome->len);

  e2ap_RicControlFailure_requestId_create(B, cf->ric_id.ric_req_id, cf->ric_id.ric_inst_id);
  e2ap_RicControlFailure_ranFunctionId_force_add(B, cf->ric_id.ran_func_id);

  e2ap_Message_union_ref_t msg = e2ap_Message_as_controlFailure(e2ap_RicControlFailure_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_control_failure_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_ACKNOWLEDGE);
  return e2ap_enc_control_failure_fb(&msg->u_msgs.ric_ctrl_fail); 
}

byte_array_t e2ap_enc_error_indication_fb(const e2ap_error_indication_t* ei)
{
  assert(0 && "not implemented");
}

byte_array_t e2ap_enc_error_indication_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2AP_ERROR_INDICATION);
  return  e2ap_enc_error_indication_fb(&msg->u_msgs.err_ind );
}

byte_array_t e2ap_enc_setup_request_fb(const e2_setup_request_t* sr)
{
  assert(sr->id.type == ngran_gNB); // only this type supported
  assert(sr->len_rf <= (size_t)MAX_NUM_RAN_FUNC_ID);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_E2SetupRequest_start(B);

  e2ap_GNB_start(B);
  const plmn_t* plmn = &sr->id.plmn;
  e2ap_GNB_plmn_create(B, plmn->mcc, plmn->mnc, plmn->mnc_digit_len);
  e2ap_GNB_id_add(B, sr->id.nb_id);
  //e2ap_GNB_cu_up_id_add(B, 1234567);
  e2ap_GNB_ref_t gnb = e2ap_GNB_end(B);
  e2ap_E2SetupRequest_id_add(B, e2ap_GlobalE2NodeId_as_gNB(gnb));

  e2ap_E2SetupRequest_ranFunctions_start(B);
  for (size_t i = 0; i < sr->len_rf; ++i) {
    const ran_function_t* rf = &sr->ran_func_item[i];
    e2ap_RanFunction_start(B);
    e2ap_RanFunction_definition_create(B, rf->def.buf, rf->def.len);
    if (rf->oid)
      e2ap_RanFunction_oid_create(B, (const char*) rf->oid->buf, rf->oid->len);
    e2ap_RanFunction_id_force_add(B, rf->id);
    e2ap_RanFunction_revision_force_add(B, rf->rev);
    e2ap_E2SetupRequest_ranFunctions_push(B, e2ap_RanFunction_end(B));
  }
  e2ap_E2SetupRequest_ranFunctions_end(B);

  /* E2NodeComponentConfigurationUpdate: TODO */
  assert(sr->len_ccu == 0 && "E2NodeComponentConfigurationUpdate handling not implemented");

  e2ap_Message_union_ref_t msg = e2ap_Message_as_setupRequest(e2ap_E2SetupRequest_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_setup_request_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_REQUEST);
  return e2ap_enc_setup_request_fb(&msg->u_msgs.e2_stp_req);
}

byte_array_t e2ap_enc_setup_response_fb(const e2_setup_response_t* sr)
{
  assert(sr);
  assert(sr->len_acc <= (size_t)MAX_NUM_RAN_FUNC_ID);
  assert(sr->len_rej <= (size_t)MAX_NUM_RAN_FUNC_ID);
  assert(sr->len_ccual <= (size_t)MAX_NUM_E2_NODE_COMPONENTS);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  e2ap_E2SetupResponse_start(B);

  e2ap_GlobalRicId_start(B);
  const plmn_t* plmn = &sr->id.plmn;
  e2ap_GlobalRicId_plmn_create(B, plmn->mcc, plmn->mnc, plmn->mnc_digit_len);
  e2ap_GlobalRicId_nearRtRicId_add(B, sr->id.near_ric_id.double_word);
  e2ap_E2SetupResponse_globalRicId_add(B, e2ap_GlobalRicId_end(B));

  /*
  e2ap_Protocol_ref_t prot = e2ap_Protocol_create(B, e2ap_ProtocolCause_AbstractSyntaxErrorReject);
  e2ap_Cause_union_ref_t f1 = e2ap_Cause_as_protocol(prot);

  e2ap_RicServices_start(B);
  e2ap_RicServices_ricRequest_force_add(B, e2ap_RicRequest_RanFunctionIdInvalid);
  e2ap_RicServices_ref_t ricserv = e2ap_RicServices_end(B);//e2ap_RicServices_create(B, e2ap_RicRequest_RanFunctionIdInvalid, e2ap_RicService_FunctionNotRequired); //
  e2ap_Cause_union_ref_t r1 = e2ap_Cause_as_ricServices(ricserv);
  */

  e2ap_E2SetupResponse_acceptedRanFunctions_start(B);
  uint16_t* v_acc = e2ap_E2SetupResponse_acceptedRanFunctions_extend(B, sr->len_acc);
  for (size_t i = 0; i < sr->len_acc; ++i)
    v_acc[i] = sr->accepted[i];
  e2ap_E2SetupResponse_acceptedRanFunctions_end(B);

  e2ap_E2SetupResponse_rejectedRanFunctions_start(B);
  e2ap_RejectedRanFunction_ref_t* rrf = e2ap_E2SetupResponse_rejectedRanFunctions_extend(B, sr->len_rej);
  for (size_t i = 0; i < sr->len_rej; ++i) {
    const uint16_t id = sr->rejected[i].id;
    const e2ap_Cause_union_ref_t cause = copy_cause(B, sr->rejected[i].cause);
    rrf[i] = e2ap_RejectedRanFunction_create(B, id, cause);

    /*
    e2ap_RejectedRanFunction_vec_push_start(B);
      e2ap_RejectedRanFunction_ranFunctionId_force_add(B, 102);
      e2ap_RejectedRanFunction_cause_add(B, e2ap_Cause_as_misc(e2ap_Misc_create(B, e2ap_MiscellaneousCause_OMIntervention)));
    e2ap_RejectedRanFunction_vec_push_end(B);

    e2ap_E2SetupResponse_rejectedRanFunctions_push_create(B, 103, r1);
    //e2ap_RejectedRanFunction_vec_push_start(B);
    //  e2ap_RejectedRanFunction_ranFunctionId_force_add(B, 103);
    //  e2ap_RejectedRanFunction_cause_add(B, e2ap_Cause_as_ricServices(r1));
    //e2ap_RejectedRanFunction_vec_push_end(B);
    */
  }
  e2ap_E2SetupResponse_rejectedRanFunctions_end(B);

  /*
  e2ap_E2SetupResponse_updateAck_start(B);
    e2ap_E2NodeCompConfUpdateAckItem_vec_push_start(B);
    e2ap_E2NodeCompConfUpdateAckItem_type_add(B, e2ap_E2NodeCompType_gNB_DU);
    e2ap_E2NodeCompConfUpdateAck_start(B);
    e2ap_E2NodeCompConfUpdateAck_outcome_add(B, e2ap_E2NodeCompConfUpdateAckOutcome_Success);
    e2ap_E2NodeCompConfUpdateAckItem_updateAck_add(B, e2ap_E2NodeCompConfUpdateAck_end(B));
    e2ap_E2NodeCompConfUpdateAckItem_vec_push_end(B);
  e2ap_E2SetupResponse_updateAck_end(B);
  */

  e2ap_Message_union_ref_t msg = e2ap_Message_as_setupResponse(e2ap_E2SetupResponse_end(B));
  e2ap_E2Message_create_as_root(B, msg);

  size_t size;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = e2ap_E2Message_verify_as_root(buf, size))) {
    printf("E2Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t e2ap_enc_setup_response_fb_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_RESPONSE);
  return e2ap_enc_setup_response_fb(&msg->u_msgs.e2_stp_resp);
}

byte_array_t e2ap_enc_setup_failure_fb(const e2_setup_failure_t* sr)
{
  assert(0 && "not implemented");
}

byte_array_t e2ap_enc_setup_failure_msg(const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_FAILURE);
  e2ap_enc_setup_failure_fb(&msg->u_msgs.e2_stp_fail);

  byte_array_t ba = {0};
  return ba;
}

byte_array_t e2ap_enc_e42_setup_request_fb(const e42_setup_request* sr) 
{
    assert(0 != 0 && "Not implemented");
    byte_array_t ba = {0};
    return ba; 
}
  
byte_array_t e2ap_enc_e42_setup_request_msg(const  e2ap_msg_t* msg)
{
    assert(msg != NULL);
    assert(msg->type == E42_SETUP_REQUEST);
    byte_array_t ba = e2ap_enc_e42_setup_request_fb(&msg->u_msgs.e42_setup_request);
    return ba;
}
   
byte_array_t e2ap_enc_e42_subscription_request_fb(const e42_setup_request_t* sr)
{
    assert(0 != 0 && "Not implemented");
    byte_array_t ba = {0};
    return ba;
}
  
byte_array_t e2ap_enc_e42_subscription_request_msg(const  e2ap_msg_t* msg)
{
    assert(msg != NULL);
    assert(msg->type == E42_RIC_SUBSCRIPTION_REQUEST);
    byte_array_t ba = e2ap_enc_e42_subscription_request_fb(&msg->u_msgs.e42_ric_sub_r  eq);  
    return ba;
}
  
byte_array_t e2ap_enc_e42_control_request_fb(const e42_ric_control_request_t* cr)
{
    assert(0 != 0 && "Not implemented");
    byte_array_t ba = {0};
    return ba;
}
  
byte_array_t e2ap_enc_e42_control_request_msg(const  e2ap_msg_t* msg)
{
    assert(msg != NULL);
    assert(msg->type == E42_RIC_CONTROL_REQUEST);
    byte_array_t ba = e2ap_enc_e42_control_request_fb(&msg->u_msgs.e42_ric_ctrl_req);
    return ba;
}

