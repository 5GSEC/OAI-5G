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


#include "e2ap_msg_dec_fb.h"

#include "../free/e2ap_msg_free.h"

#include "../e2ap_ap.h"
//#include "../enc/e2ap_encode.h"
#include "../ie/fb/e2ap_reader.h"

#include "../global_consts.h"

#include <stdio.h>

static inline
cause_t copy_cause(e2ap_Cause_union_t cause)
{
  assert(e2ap_Cause_is_known_type(cause.type));
  cause_t ret;
  switch (cause.type) {
    case e2ap_Cause_ricRequest:
      assert(e2ap_RicRequest_ricRequestCause(cause.value) < 11);
      ret.ricRequest = e2ap_RicRequest_ricRequestCause(cause.value);
      ret.present = CAUSE_RICREQUEST;
      break;
    case e2ap_Cause_ricService:
      assert(e2ap_RicService_ricServiceCause(cause.value) < 3);
      ret.ricService = e2ap_RicService_ricServiceCause(cause.value);
      ret.present = CAUSE_RICSERVICE;
      break;
    case e2ap_Cause_transportLayer:
      assert(e2ap_TransportLayer_transportLayerCause(cause.value) < 2);
      ret.transport = e2ap_TransportLayer_transportLayerCause(cause.value);
      ret.present = CAUSE_TRANSPORT;
      break;
    case e2ap_Cause_protocol:
      assert(e2ap_Protocol_protocolCause(cause.value) < 7);
      ret.protocol = e2ap_Protocol_protocolCause(cause.value);
      ret.present = CAUSE_PROTOCOL;
      break;
    case e2ap_Cause_misc:
      assert(e2ap_Misc_miscellaneousCause(cause.value) < 4);
      ret.misc = e2ap_Misc_miscellaneousCause(cause.value);
      ret.present = CAUSE_MISC;
      break;
    case e2ap_Cause_NONE:
      assert(0 && "not implemented");
      break;
  }
  return ret;
}

static inline
uint32_t get_time_to_wait(uint32_t ttw)
{
  /* the ASN.1 encoding treats the time_to_wait_ms type as an enum, although it
   * is a uint32_t (in ms). For the time being, we will do the same, but
   * convert it from the ms value of the flatbuffers */
  if (ttw == 0)
    return 0;
  else if (ttw == 1)
    return 1;
  else if (ttw == 2)
    return 2;
  else if (ttw <= 5)
    return 3;
  else if (ttw <= 10)
    return 4;
  else if (ttw <= 20)
    return 5;
  else if (ttw <= 30)
    return 6;
  else if (ttw <= 40)
    return 7;
  else if (ttw <= 50)
    return 8;
  else if (ttw <= 100)
    return 9;
  else if (ttw <= 200)
    return 10;
  else if (ttw <= 500)
    return 11;
  else if (ttw <= 1000)
    return 12;
  else if (ttw <= 2000)
    return 13;
  else if (ttw <= 5000)
    return 14;
  else if (ttw <= 10000)
    return 15;
  else if (ttw <= 20000)
    return 16;
  else if (ttw <= 60000)
    return 17;
  else
    assert("illegal value for time to wait, cannot map to enum");
  assert(0 && "illegal code path");
}

static inline
byte_array_t copy_vec_uint8_to_ba(const uint8_t *b, size_t l)
{
  uint8_t* buf = malloc(l);
  assert(buf);
  memcpy(buf, b, l);
  return (byte_array_t) { .buf = buf, .len = l};
}

e2ap_msg_t e2ap_dec_subscription_request_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_subscriptionRequest);

  e2ap_RicSubscriptionRequest_table_t fb_rsr = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_SUBSCRIPTION_REQUEST};
  ric_subscription_request_t* sr = &ret.u_msgs.ric_sub_req;

  assert(e2ap_RicSubscriptionRequest_requestId_is_present(fb_rsr));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicSubscriptionRequest_requestId(fb_rsr);
  sr->ric_id.ric_req_id = ricreq->ricRequestorId;
  sr->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicSubscriptionRequest_ranFunctionId_is_present(fb_rsr));
  sr->ric_id.ran_func_id = e2ap_RicSubscriptionRequest_ranFunctionId(fb_rsr);

  const uint8_t* event = e2ap_RicSubscriptionRequest_eventTriggerDefinition(fb_rsr);
  const size_t event_len = flatbuffers_uint8_vec_len(event);
  assert(event && event_len > 0);
  sr->event_trigger.buf = malloc(event_len);
  assert(sr->event_trigger.buf);
  memcpy(sr->event_trigger.buf, event, event_len);
  sr->event_trigger.len = event_len;

  e2ap_Action_vec_t actions = e2ap_RicSubscriptionRequest_actions(fb_rsr);
  const int len_ra = e2ap_Action_vec_len(actions);
  assert(len_ra > 0 && len_ra <= MAX_NUM_ACTION_DEF );
  sr->len_action = len_ra;
  sr->action = calloc(len_ra, sizeof(ric_action_t));
  assert(sr->action);

  for (int i = 0; i < len_ra; ++i) {
    e2ap_Action_table_t action = e2ap_Action_vec_at(actions, i);
    ric_action_t * dst = &sr->action[i];

    dst->id = e2ap_Action_id(action);
    dst->type = e2ap_Action_type(action); /* maps one-to-one */

    const uint8_t* definition = e2ap_Action_definition(action);
    const size_t definition_len = flatbuffers_uint8_vec_len(definition);
    if (definition && definition_len > 0) {
      dst->definition = malloc(sizeof(byte_array_t));
      assert(dst->definition);
      dst->definition->buf = malloc(definition_len);
      assert(dst->definition->buf);
      memcpy(dst->definition->buf, definition, definition_len);
      dst->definition->len = definition_len;
    }

    e2ap_RicSubsequentAction_table_t rsa = e2ap_Action_subsequentAction(action);
    if (rsa) {
      dst->subseq_action = calloc(1, sizeof(ric_subsequent_action_t));
      assert(dst->subseq_action);
      e2ap_RicSubsequentActionType_enum_t rsat = e2ap_RicSubsequentAction_type(rsa);
      assert(rsat);
      dst->subseq_action->type = rsat;
      if (e2ap_RicSubsequentAction_timeToWait(rsa) != 0) {
        dst->subseq_action->time_to_wait_ms = malloc(sizeof(uint32_t));
        assert(dst->subseq_action->time_to_wait_ms);
        *dst->subseq_action->time_to_wait_ms = get_time_to_wait(e2ap_RicSubsequentAction_timeToWait(rsa));
      }
    }
  }

  return ret;
}

e2ap_msg_t e2ap_dec_subscription_response_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_subscriptionResponse);

  e2ap_RicSubscriptionResponse_table_t fb_rsr = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_SUBSCRIPTION_RESPONSE};
  ric_subscription_response_t* sr = &ret.u_msgs.ric_sub_resp;

  assert(e2ap_RicSubscriptionResponse_requestId_is_present(fb_rsr));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicSubscriptionResponse_requestId(fb_rsr);
  sr->ric_id.ric_req_id = ricreq->ricRequestorId;
  sr->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicSubscriptionResponse_ranFunctionId_is_present(fb_rsr));
  sr->ric_id.ran_func_id = e2ap_RicSubscriptionResponse_ranFunctionId(fb_rsr);

  e2ap_RicActionAdmitted_vec_t al = e2ap_RicSubscriptionResponse_admittedActions(fb_rsr);
  sr->len_admitted = e2ap_RicActionAdmitted_vec_len(al);
  assert(sr->len_admitted > 0);
  sr->admitted = calloc(sr->len_admitted, sizeof(*sr->admitted));
  for (size_t i = 0; i < sr->len_admitted; ++i) {
    e2ap_RicActionAdmitted_table_t admitted = e2ap_RicActionAdmitted_vec_at(al, i);
    ric_action_admitted_t* dst = &sr->admitted[i];
    dst->ric_act_id = e2ap_RicActionAdmitted_id(admitted);
  }

  e2ap_RicActionNotAdmitted_vec_t nal = e2ap_RicSubscriptionResponse_notAdmittedActions(fb_rsr);
  sr->len_na = e2ap_RicActionNotAdmitted_vec_len(nal);
  if (sr->len_na > 0) {
    sr->not_admitted = calloc(sr->len_na, sizeof(ric_action_not_admitted_t));
    for (size_t i = 0; i < sr->len_na; ++i) {
      e2ap_RicActionNotAdmitted_table_t not_admitted = e2ap_RicActionNotAdmitted_vec_at(nal, i);
      ric_action_not_admitted_t* dst = &sr->not_admitted[i];
      dst->ric_act_id = e2ap_RicActionNotAdmitted_id(not_admitted);
      dst->cause = copy_cause(e2ap_RicActionNotAdmitted_cause_union(not_admitted));
    }
  }

  return ret;
}

e2ap_msg_t e2ap_dec_subscription_failure_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_subscriptionFailure);

  e2ap_RicSubscriptionFailure_table_t fb_rsf = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_SUBSCRIPTION_FAILURE};
  ric_subscription_failure_t* sf = &ret.u_msgs.ric_sub_fail;

  assert(e2ap_RicSubscriptionFailure_requestId_is_present(fb_rsf));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicSubscriptionFailure_requestId(fb_rsf);
  sf->ric_id.ric_req_id = ricreq->ricRequestorId;
  sf->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicSubscriptionFailure_ranFunctionId_is_present(fb_rsf));
  sf->ric_id.ran_func_id = e2ap_RicSubscriptionFailure_ranFunctionId(fb_rsf);

  e2ap_RicActionNotAdmitted_vec_t naa = e2ap_RicSubscriptionFailure_notAdmittedActions(fb_rsf);
  sf->len_na = e2ap_RicActionNotAdmitted_vec_len(naa);
  if (sf->len_na > 0) {
    sf->not_admitted = calloc(sf->len_na, sizeof(ric_action_not_admitted_t));
    assert(sf->not_admitted);
    for (size_t i = 0; i < sf->len_na; ++i) {
      e2ap_RicActionNotAdmitted_table_t not_admitted = e2ap_RicActionNotAdmitted_vec_at(naa, i);
      ric_action_not_admitted_t* dst = &sf->not_admitted[i];
      dst->ric_act_id = e2ap_RicActionNotAdmitted_id(not_admitted);
      dst->cause = copy_cause(e2ap_RicActionNotAdmitted_cause_union(not_admitted));
    }
  }

  assert(!e2ap_RicSubscriptionFailure_criticalityDiagnostics_is_present(fb_rsf) && "decoding of criticality diagnostics is not implemented");
  return ret;
}

e2ap_msg_t e2ap_dec_subscription_delete_request_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_subscriptionDeleteRequest);

  e2ap_RicSubscriptionDeleteRequest_table_t fb_sdr = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_SUBSCRIPTION_DELETE_REQUEST};
  ric_subscription_delete_request_t* dr = &ret.u_msgs.ric_sub_del_req;

  assert(e2ap_RicSubscriptionDeleteRequest_requestId_is_present(fb_sdr));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicSubscriptionDeleteRequest_requestId(fb_sdr);
  dr->ric_id.ric_req_id = ricreq->ricRequestorId;
  dr->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicSubscriptionDeleteRequest_ranFunctionId_is_present(fb_sdr));
  dr->ric_id.ran_func_id = e2ap_RicSubscriptionDeleteRequest_ranFunctionId(fb_sdr);


  return ret;
}

e2ap_msg_t e2ap_dec_subscription_delete_response_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_subscriptionDeleteResponse);

  e2ap_RicSubscriptionDeleteResponse_table_t fb_sdr = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_SUBSCRIPTION_DELETE_RESPONSE};
  ric_subscription_delete_response_t* dr = &ret.u_msgs.ric_sub_del_resp;

  assert(e2ap_RicSubscriptionDeleteResponse_requestId_is_present(fb_sdr));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicSubscriptionDeleteResponse_requestId(fb_sdr);
  dr->ric_id.ric_req_id = ricreq->ricRequestorId;
  dr->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicSubscriptionDeleteResponse_ranFunctionId_is_present(fb_sdr));
  dr->ric_id.ran_func_id = e2ap_RicSubscriptionDeleteResponse_ranFunctionId(fb_sdr);

  return ret;
}

e2ap_msg_t e2ap_dec_subscription_delete_failure_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_subscriptionDeleteFailure);

  e2ap_RicSubscriptionDeleteFailure_table_t fb_sdf = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_SUBSCRIPTION_DELETE_FAILURE};
  ric_subscription_delete_failure_t* df = &ret.u_msgs.ric_sub_del_fail;

  assert(e2ap_RicSubscriptionDeleteFailure_requestId_is_present(fb_sdf));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicSubscriptionDeleteFailure_requestId(fb_sdf);
  df->ric_id.ric_req_id = ricreq->ricRequestorId;
  df->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicSubscriptionDeleteFailure_ranFunctionId_is_present(fb_sdf));
  df->ric_id.ran_func_id = e2ap_RicSubscriptionDeleteFailure_ranFunctionId(fb_sdf);

  assert(e2ap_RicSubscriptionDeleteFailure_cause_is_present(fb_sdf));
  df->cause = copy_cause(e2ap_RicSubscriptionDeleteFailure_cause_union(fb_sdf));

  assert(!e2ap_RicSubscriptionDeleteFailure_criticalityDiagnostics_is_present(fb_sdf) && "decoding of criticality diagnostics is not implemented");

  return ret;
}

e2ap_msg_t e2ap_dec_indication_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_indication);

  e2ap_RicIndication_table_t fb_sdr = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_INDICATION};
  ric_indication_t* ind = &ret.u_msgs.ric_ind;

  assert(e2ap_RicIndication_requestId_is_present(fb_sdr));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicIndication_requestId(fb_sdr);
  ind->ric_id.ric_req_id = ricreq->ricRequestorId;
  ind->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicIndication_ranFunctionId_is_present(fb_sdr));
  ind->ric_id.ran_func_id = e2ap_RicIndication_ranFunctionId(fb_sdr);

  assert(e2ap_RicIndication_actionId_is_present(fb_sdr));
  ind->action_id = e2ap_RicIndication_actionId(fb_sdr);

  if (e2ap_RicIndication_sn_is_present(fb_sdr)) {
    ind->sn = malloc(sizeof(*ind->sn));
    assert(ind->sn);
    *ind->sn = e2ap_RicIndication_sn(fb_sdr);
  }

  assert(e2ap_RicIndication_indicationType_is_present(fb_sdr));
  ind->type = e2ap_RicIndication_indicationType(fb_sdr);

  assert(e2ap_RicIndication_header_is_present(fb_sdr));
  const uint8_t* hdr = e2ap_RicIndication_header(fb_sdr);
  const size_t hdr_len = flatbuffers_uint8_vec_len(hdr);
  ind->hdr.buf = malloc(hdr_len);
  assert(ind->hdr.buf);
  memcpy(ind->hdr.buf, hdr, hdr_len);
  ind->hdr.len = hdr_len;

  assert(e2ap_RicIndication_message_is_present(fb_sdr));
  const uint8_t* msg = e2ap_RicIndication_message(fb_sdr);
  const size_t msg_len = flatbuffers_uint8_vec_len(msg);
  ind->msg.buf = malloc(msg_len);
  assert(ind->msg.buf);
  memcpy(ind->msg.buf, msg, msg_len);
  ind->msg.len = msg_len;

  if (e2ap_RicIndication_callProcessId_is_present(fb_sdr)) {
    const uint8_t* cpi = e2ap_RicIndication_callProcessId(fb_sdr);
    const size_t cpi_len = flatbuffers_uint8_vec_len(cpi);
    ind->call_process_id = malloc(cpi_len);
    assert(ind->call_process_id);
    memcpy(ind->call_process_id->buf, cpi, cpi_len);
    ind->call_process_id->len = cpi_len;
  }

  return ret;
}

e2ap_msg_t e2ap_dec_control_request_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_controlRequest);

  e2ap_RicControlRequest_table_t fb_cr = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_CONTROL_REQUEST };
  ric_control_request_t* ctrl = &ret.u_msgs.ric_ctrl_req;

  assert(e2ap_RicControlRequest_requestId_is_present(fb_cr));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicControlRequest_requestId(fb_cr);
  ctrl->ric_id.ric_req_id = ricreq->ricRequestorId;
  ctrl->ric_id.ric_inst_id = ricreq->ricInstanceId;

  /* the ran Function ID is not written in enc() when it is the default (0), so
   * do not assert on its presence */
  //assert(e2ap_RicControlRequest_ranFunctionId_is_present(fb_cr));
  ctrl->ric_id.ran_func_id = e2ap_RicControlRequest_ranFunctionId(fb_cr);

  if (e2ap_RicControlRequest_callProcessId_is_present(fb_cr)) {
    const uint8_t* cpi = e2ap_RicControlRequest_callProcessId(fb_cr);
    const size_t cpi_len = flatbuffers_uint8_vec_len(cpi);
    ctrl->call_process_id = malloc(cpi_len);
    assert(ctrl->call_process_id);
    *ctrl->call_process_id = copy_vec_uint8_to_ba(cpi, cpi_len);
  }

  assert(e2ap_RicControlRequest_header_is_present(fb_cr));
  const uint8_t* hdr = e2ap_RicControlRequest_header(fb_cr);
  const size_t hdr_len = flatbuffers_uint8_vec_len(hdr);
  ctrl->hdr = copy_vec_uint8_to_ba(hdr, hdr_len);

  assert(e2ap_RicControlRequest_message_is_present(fb_cr));
  const uint8_t* msg = e2ap_RicControlRequest_message(fb_cr);
  const size_t msg_len = flatbuffers_uint8_vec_len(msg);
  ctrl->msg = copy_vec_uint8_to_ba(msg, msg_len);

  if (e2ap_RicControlRequest_ackRequest_is_present(fb_cr)) {
    ctrl->ack_req = malloc(sizeof(*ctrl->ack_req));
    assert(ctrl->ack_req);
    *ctrl->ack_req = e2ap_RicControlRequest_ackRequest(fb_cr);
  }

  return ret;
}

e2ap_msg_t e2ap_dec_control_ack_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_controlAcknowledge);

  e2ap_RicControlAcknowledge_table_t fb_ca = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_CONTROL_ACKNOWLEDGE};
  ric_control_acknowledge_t* ca = &ret.u_msgs.ric_ctrl_ack;

  assert(e2ap_RicControlAcknowledge_requestId_is_present(fb_ca));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicControlAcknowledge_requestId(fb_ca);
  ca->ric_id.ric_req_id = ricreq->ricRequestorId;
  ca->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicControlAcknowledge_ranFunctionId_is_present(fb_ca));
  ca->ric_id.ran_func_id = e2ap_RicControlAcknowledge_ranFunctionId(fb_ca);

  if (e2ap_RicControlAcknowledge_callProcessId_is_present(fb_ca)) {
    const uint8_t* cpi = e2ap_RicControlAcknowledge_callProcessId(fb_ca);
    const size_t cpi_len = flatbuffers_uint8_vec_len(cpi);
    ca->call_process_id = malloc(cpi_len);
    assert(ca->call_process_id);
    *ca->call_process_id = copy_vec_uint8_to_ba(cpi, cpi_len);
  }

  /* the outcome is not written in enc() when it is the default (0), so
   * do not assert on its presence */
  //assert(e2ap_RicControlAcknowledge_outcome_is_present(fb_ca));
  ca->status = e2ap_RicControlAcknowledge_status(fb_ca);

  if (e2ap_RicControlAcknowledge_outcome_is_present(fb_ca)) {
    const uint8_t* oc = e2ap_RicControlAcknowledge_outcome(fb_ca);
    const size_t oc_len = flatbuffers_uint8_vec_len(oc);
    ca->control_outcome = malloc(oc_len);
    assert(ca->control_outcome);
    *ca->control_outcome = copy_vec_uint8_to_ba(oc, oc_len);
  }

  return ret;
}

e2ap_msg_t e2ap_dec_control_failure_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_controlFailure);

  e2ap_RicControlFailure_table_t fb_cf = fb_msg.value;

  e2ap_msg_t ret = {.type = RIC_CONTROL_FAILURE};
  ric_control_failure_t* cf = &ret.u_msgs.ric_ctrl_fail;

  assert(e2ap_RicControlFailure_requestId_is_present(fb_cf));
  e2ap_RicRequestId_struct_t ricreq = e2ap_RicControlFailure_requestId(fb_cf);
  cf->ric_id.ric_req_id = ricreq->ricRequestorId;
  cf->ric_id.ric_inst_id = ricreq->ricInstanceId;

  assert(e2ap_RicControlFailure_ranFunctionId_is_present(fb_cf));
  cf->ric_id.ran_func_id = e2ap_RicControlFailure_ranFunctionId(fb_cf);

  if (e2ap_RicControlFailure_callProcessId_is_present(fb_cf)) {
    const uint8_t* cpi = e2ap_RicControlFailure_callProcessId(fb_cf);
    const size_t cpi_len = flatbuffers_uint8_vec_len(cpi);
    cf->call_process_id = malloc(cpi_len);
    assert(cf->call_process_id);
    *cf->call_process_id = copy_vec_uint8_to_ba(cpi, cpi_len);
  }

  assert(e2ap_RicControlFailure_cause_is_present(fb_cf));
  cf->cause = copy_cause(e2ap_RicControlFailure_cause_union(fb_cf));

  if (e2ap_RicControlFailure_outcome_is_present(fb_cf)) {
    const uint8_t* oc = e2ap_RicControlFailure_outcome(fb_cf);
    const size_t oc_len = flatbuffers_uint8_vec_len(oc);
    cf->control_outcome = malloc(oc_len);
    assert(cf->control_outcome);
    *cf->control_outcome = copy_vec_uint8_to_ba(oc, oc_len);
  }

  return ret;
}


e2ap_msg_t e2ap_dec_error_indication_fb(e2ap_E2Message_table_t e2mt)
{
  assert(0!=0 && "Not implemented");
  (void) e2mt;

  e2ap_msg_t ret = {.type = E2AP_ERROR_INDICATION};
  return ret;
}

e2ap_msg_t e2ap_dec_setup_request_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_setupRequest);

  e2ap_E2SetupRequest_table_t fb_sr = fb_msg.value;

  e2ap_msg_t ret = {.type = E2_SETUP_REQUEST};
  e2_setup_request_t* sr = &ret.u_msgs.e2_stp_req;

  e2ap_GlobalE2NodeId_union_t id  = e2ap_E2SetupRequest_id_union(fb_sr);
  switch (id.type) {
    case e2ap_GlobalE2NodeId_gNB:
      sr->id.type = ngran_gNB;
      sr->id.plmn.mcc = e2ap_GNB_plmn(id.value)->mcc;
      sr->id.plmn.mnc = e2ap_GNB_plmn(id.value)->mnc;
      sr->id.plmn.mnc_digit_len = e2ap_GNB_plmn(id.value)->mnc_digit_length;
      sr->id.nb_id = e2ap_GNB_id(id.value);
      //if (e2ap_GNB_cu_up_id_is_present(id.value)) e2ap_GNB_cu_up_id(id.value)
      //if (e2ap_GNB_du_id_is_present(id.value)) e2ap_GNB_du_id(id.value)
      break;
    case e2ap_GlobalE2NodeId_engNB:
      //sr->id.type = ngran_ng_eNB_CU??;
      //e2ap_EngNB_plmn(id.value)->mcc, e2ap_EngNB_plmn(id.value)->mnc_digit_length,
      //e2ap_EngNB_plmn(id.value)->mnc, e2ap_EngNB_id(id.value)
      assert(0 && "en-gNB not implemented");
      break;
    case e2ap_GlobalE2NodeId_ngeNB:
      sr->id.type = ngran_ng_eNB;
      sr->id.plmn.mcc = e2ap_NgeNB_plmn(id.value)->mcc;
      sr->id.plmn.mnc = e2ap_NgeNB_plmn(id.value)->mnc;
      sr->id.plmn.mnc_digit_len = e2ap_NgeNB_plmn(id.value)->mnc_digit_length;
      sr->id.nb_id = e2ap_NgeNB_id(id.value);
      break;
    case e2ap_GlobalE2NodeId_eNB:
      sr->id.type = ngran_eNB;
      sr->id.plmn.mcc = e2ap_ENB_plmn(id.value)->mcc;
      sr->id.plmn.mnc = e2ap_ENB_plmn(id.value)->mnc;
      sr->id.plmn.mnc_digit_len = e2ap_ENB_plmn(id.value)->mnc_digit_length;
      sr->id.nb_id = e2ap_ENB_id(id.value);
      break;
    default:
      assert(0 && "no ID present\n");
      break;
  }

  e2ap_RanFunction_vec_t ranFunctions = e2ap_E2SetupRequest_ranFunctions(fb_sr);
  const size_t len_rf = e2ap_RanFunction_vec_len(ranFunctions);
  sr->len_rf = len_rf;
  if (sr->len_rf > 0) {
    sr->ran_func_item = calloc(len_rf, sizeof(*sr->ran_func_item));
    assert(sr->ran_func_item);
    for (size_t i = 0; i < len_rf; ++i) {
      e2ap_RanFunction_table_t rf = e2ap_RanFunction_vec_at(ranFunctions, i);
      ran_function_t* dst = &sr->ran_func_item[i];

      assert(e2ap_RanFunction_id(rf) <= MAX_RAN_FUNC_ID);
      dst->id = e2ap_RanFunction_id(rf);

      assert(e2ap_RanFunction_revision(rf) > -1 && e2ap_RanFunction_revision(rf) <= MAX_RAN_FUNC_REV);
      dst->rev = e2ap_RanFunction_revision(rf);

      const uint8_t* definition = e2ap_RanFunction_definition(rf);
      const size_t definition_len = flatbuffers_uint8_vec_len(definition);
      assert(definition && definition_len > 0);
      dst->def = copy_vec_uint8_to_ba(definition, definition_len);

      if (e2ap_RanFunction_oid_is_present(rf)) {
        dst->oid = malloc(sizeof(*dst->oid));
        assert(dst->oid);
        const char* oid = e2ap_RanFunction_oid(rf);
        const size_t oid_len = flatbuffers_string_len(oid);
        *dst->oid = copy_vec_uint8_to_ba((uint8_t*) oid, oid_len);
      }
    }
  }

  e2ap_E2NodeCompConfUpdateItem_vec_t e2nccuis = e2ap_E2SetupRequest_e2NodeComponentConfigurationUpdateList(fb_sr);
  const size_t len_ccu = e2ap_RanFunction_vec_len(e2nccuis);
  sr->len_ccu = len_ccu;
  if (sr->len_ccu > 0) {
    sr->comp_conf_update = calloc(len_ccu, sizeof(*sr->ran_func_item));
    assert(sr->comp_conf_update);
    for (size_t i = 0; i < len_ccu; ++i) {
      e2ap_E2NodeCompConfUpdateItem_table_t e2nccui = e2ap_E2NodeCompConfUpdateItem_vec_at(e2nccuis, i);
      e2_node_component_config_update_t* dst = &sr->comp_conf_update[i];

      dst->e2_node_component_type = e2ap_E2NodeCompConfUpdateItem_type(e2nccui);

      if (e2ap_E2NodeCompConfUpdateItem_id_is_present(e2nccui)) {
        dst->id_present = malloc(sizeof(*dst->id_present));
        assert(dst->id_present);
        e2ap_E2NodeCompId_union_t id = e2ap_E2NodeCompConfUpdateItem_id_union(e2nccui);
        switch (id.type) {
          case e2ap_E2NodeCompId_gnb_cu_up:
            *dst->id_present = E2_NODE_COMPONENT_ID_E2_NODE_COMPONENT_TYPE_GNB_CU_UP;
            dst->gnb_cu_up_id = e2ap_E2NCId_id(id.value);
          break;
          case e2ap_E2NodeCompId_gnb_du:
            *dst->id_present = E2_NODE_COMPONENT_ID_E2_NODE_COMPONENT_TYPE_GNB_DU;
            dst->gnb_du_id = e2ap_E2NCId_id(id.value);
            break;
          default:
            assert(0 && "unhandled E2NodeCompId type");
            break;
        }
      }

      e2ap_E2NodeCompConfUpdate_union_t ccu = e2ap_E2NodeCompConfUpdateItem_update_union(e2nccui);
      switch (ccu.type) {
        case e2ap_E2NodeCompConfUpdate_gnb:
          assert(0 && "not implemented yet");
          break;
        case e2ap_E2NodeCompConfUpdate_en_gnb:
          dst->update_present = E2_NODE_COMPONENT_CONFIG_UPDATE_EN_GNB_CONFIG_UPDATE;
          const uint8_t* x2ap_en_gnb = e2ap_en_gnbE2NodeCompConfUpdate_x2ap_en_gnb(ccu.value);
          const size_t x2ap_en_gnb_len = flatbuffers_uint8_vec_len(x2ap_en_gnb);
          if (x2ap_en_gnb && x2ap_en_gnb_len > 0) {
            dst->en_gnb.x2ap_en_gnb = malloc(sizeof(byte_array_t) );
            assert(dst->en_gnb.x2ap_en_gnb);
            *dst->en_gnb.x2ap_en_gnb = copy_vec_uint8_to_ba(x2ap_en_gnb, x2ap_en_gnb_len);
          }
          break;
        case e2ap_E2NodeCompConfUpdate_ng_enb:
          assert(0 && "not implemented yet");
          break;
        case e2ap_E2NodeCompConfUpdate_enb:
          assert(0 && "not implemented yet");
          break;
        default:
          assert(0 && "unhandled E2NodeCompConfUpdate");
          break;
      }
    }
  }

  return ret;
}

e2ap_msg_t e2ap_dec_setup_response_fb(e2ap_E2Message_table_t e2mt)
{
  assert(e2mt);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2mt);
  assert(fb_msg.type == e2ap_Message_setupResponse);

  e2ap_E2SetupResponse_table_t fb_sr = fb_msg.value;

  e2ap_msg_t ret = {.type = E2_SETUP_RESPONSE };
  e2_setup_response_t* sr = &ret.u_msgs.e2_stp_resp;

  assert(e2ap_E2SetupResponse_globalRicId_is_present(fb_sr));
  e2ap_GlobalRicId_table_t id = e2ap_E2SetupResponse_globalRicId(fb_sr);
  sr->id.plmn.mcc = e2ap_GlobalRicId_plmn(id)->mcc;
  sr->id.plmn.mnc = e2ap_GlobalRicId_plmn(id)->mnc;
  sr->id.plmn.mnc_digit_len = e2ap_GlobalRicId_plmn(id)->mnc_digit_length;
  sr->id.near_ric_id.double_word = e2ap_GlobalRicId_nearRtRicId(id);

  flatbuffers_uint16_vec_t accepted = e2ap_E2SetupResponse_acceptedRanFunctions(fb_sr);
  const size_t len_acc = flatbuffers_uint16_vec_len(accepted);
  sr->len_acc = len_acc;
  if (len_acc > 0) {
    sr->accepted = malloc(sr->len_acc * sizeof(*sr->accepted));
      assert(sr->accepted);
    for (size_t i = 0; i < sr->len_acc; ++i)
      sr->accepted[i] = flatbuffers_uint16_vec_at(accepted, i);
  }

  e2ap_RejectedRanFunction_vec_t rejected = e2ap_E2SetupResponse_rejectedRanFunctions(fb_sr);
  const size_t len_rej = e2ap_RejectedRanFunction_vec_len(rejected);
  sr->len_rej = len_rej;
  if (len_rej > 0) {
    sr->rejected = malloc(sr->len_rej * sizeof(*sr->rejected));
    assert(sr->rejected);
    for (size_t i = 0; i < sr->len_rej; ++i) {
      e2ap_RejectedRanFunction_table_t rej = e2ap_RejectedRanFunction_vec_at(rejected, i);
      rejected_ran_function_t* dst = &sr->rejected[i];
      dst->id = e2ap_RejectedRanFunction_ranFunctionId(rej);
      dst->cause = copy_cause(e2ap_RejectedRanFunction_cause_union(rej));
    }
  }

  e2ap_E2NodeCompConfUpdateAckItem_vec_t updateAcks = e2ap_E2SetupResponse_updateAck(fb_sr);
  const size_t len_ccual = e2ap_E2NodeCompConfUpdateAckItem_vec_len(updateAcks);
  sr->len_ccual = len_ccual;
  if (len_ccual > 0) {
    sr->comp_conf_update_ack_list = malloc(sr->len_ccual * sizeof(*sr->comp_conf_update_ack_list));
    assert(sr->comp_conf_update_ack_list);
    for (size_t i = 0; i < sr->len_ccual; ++i) {
      e2ap_E2NodeCompConfUpdateAckItem_table_t ua = e2ap_E2NodeCompConfUpdateAckItem_vec_at(updateAcks, i);
      assert(0 && "not implemented");
      /*
      printf("  [%d] acked Update Item type %s",
             i, e2ap_E2NodeCompType_name(e2ap_E2NodeCompConfUpdateAckItem_type(ua)));
      if (e2ap_E2NodeCompConfUpdateAckItem_id_is_present(ua)) {
        e2ap_E2NodeCompId_union_t id = e2ap_E2NodeCompConfUpdateAckItem_id_union(ua);
        switch (id.type) {
          case e2ap_E2NodeCompId_gnb_cu_up:
            printf(" ID gNB CU-UP %ld", ((e2ap_E2NCId_t *)id.value)->id);
            break;
          case e2ap_E2NodeCompId_gnb_du:
            printf(" ID gNB DU %ld", ((e2ap_E2NCId_t *)id.value)->id);
            break;
        }
      }
      e2ap_E2NodeCompConfUpdateAck_table_t ack = e2ap_E2NodeCompConfUpdateAckItem_updateAck(ua);
      if (e2ap_E2NodeCompConfUpdateAck_outcome(ack) == e2ap_E2NodeCompConfUpdateAckOutcome_Success)
        printf(" outcome SUCCESS\n");
      else
        printf(" outcome Failure Cause %s\n", get_e2ap_Cause_name(e2ap_E2NodeCompConfUpdateAck_cause_union(ack)));
      */
    }
  }
  return ret;
}

// RIC -> E2
e2ap_msg_t e2ap_dec_setup_failure_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");
  e2ap_msg_t ans = {0};
  return ans;
}

// RIC <-> E2
e2ap_msg_t e2ap_dec_reset_request_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");
  e2ap_msg_t ans = {0};
  return ans;

}


// RIC <-> E2
e2ap_msg_t e2ap_dec_reset_response_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}

  
// E2 -> RIC
e2ap_msg_t e2ap_dec_service_update_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// RIC -> E2
e2ap_msg_t e2ap_dec_service_update_ack_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// RIC -> E2
e2ap_msg_t e2ap_dec_service_update_failure_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// RIC -> E2
e2ap_msg_t e2ap_dec_service_query_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// E2 -> RIC
e2ap_msg_t e2ap_dec_node_configuration_update_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// RIC -> E2
e2ap_msg_t e2ap_dec_node_configuration_update_ack_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// RIC -> E2
e2ap_msg_t e2ap_dec_node_configuration_update_failure_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;

}


// RIC -> E2
e2ap_msg_t e2ap_dec_connection_update_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// E2 -> RIC
e2ap_msg_t e2ap_dec_connection_update_ack_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}


// E2 -> RIC
e2ap_msg_t e2ap_dec_connection_update_failure_fb(e2ap_E2Message_table_t t)
{
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans;
}



static
e2_msg_type_t e2ap_get_msg_type(e2ap_Message_union_type_t e2mut)
{
  switch (e2mut) {
    case e2ap_Message_setupRequest:       return E2_SETUP_REQUEST;
    case e2ap_Message_setupResponse:   return E2_SETUP_RESPONSE;
    //case e2ap_Message_setupFailure: return E2_SETUP_FAILURE;
    //case e2ap_Message_errorIndication: return E2AP_ERROR_INDICATION;
    //case e2ap_Message_resetRequest:   return E2AP_RESET_REQUEST;
    //case e2ap_Message_resetResponse:   return E2AP_RESET_RESPONSE;
    case e2ap_Message_controlRequest:   return RIC_CONTROL_REQUEST;
    case e2ap_Message_controlAcknowledge:   return RIC_CONTROL_ACKNOWLEDGE;
    case e2ap_Message_controlFailure: return RIC_CONTROL_FAILURE;
    case e2ap_Message_indication: return RIC_INDICATION;
    //case e2ap_Message_serviceQuery: return RIC_SERVICE_QUERY;
    //case e2ap_Message_serviceUpdate:   return RIC_SERVICE_UPDATE;
    //case e2ap_Message_serviceUpdateAcknowledge:   return RIC_SERVICE_UPDATE_ACKNOWLEDGE;
    //case e2ap_Message_serviceUpdateFailure: return RIC_SERVICE_UPDATE_FAILURE;
    case e2ap_Message_subscriptionRequest:   return RIC_SUBSCRIPTION_REQUEST;
    case e2ap_Message_subscriptionResponse:   return RIC_SUBSCRIPTION_RESPONSE;
    case e2ap_Message_subscriptionFailure: return RIC_SUBSCRIPTION_FAILURE;
    case e2ap_Message_subscriptionDeleteRequest:   return RIC_SUBSCRIPTION_DELETE_REQUEST;
    case e2ap_Message_subscriptionDeleteResponse:   return RIC_SUBSCRIPTION_DELETE_RESPONSE;
    case e2ap_Message_subscriptionDeleteFailure: return RIC_SUBSCRIPTION_DELETE_FAILURE;
    //case e2ap_Message_nodeConfigurationUpdate:   return E2_NODE_CONFIGURATION_UPDATE;
    //case e2ap_Message_nodeConfigurationUpdateAcknowledge:   return E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE;
    //case e2ap_Message_nodeConfigurationUpdateFailure: return E2_NODE_CONFIGURATION_UPDATE_FAILURE;
    //case e2ap_Message_connectionUpdate:   return E2_CONNECTION_UPDATE;
    //case e2ap_Message_connectionUpdateAcknowledge:   return E2_CONNECTION_UPDATE_ACKNOWLEDGE;
    //case e2ap_Message_connectionUpdateFailure: return E2_CONNECTION_UPDATE_FAILURE;
    default: assert(0 && "unhandled message");
  }
  assert(0 && "invalid path");
}

e2ap_msg_t e2ap_msg_dec_fb(e2ap_fb_t* fb, byte_array_t ba)
{
  assert(ba.buf != NULL && ba.len > 0);
  e2ap_E2Message_table_t e2m = e2ap_E2Message_as_root(ba.buf);
  assert(e2m);
  e2ap_Message_union_t fb_msg = e2ap_E2Message_msg_union(e2m);
  const e2_msg_type_t msg_type = e2ap_get_msg_type(fb_msg.type);

  assert(fb->dec_msg[msg_type] && "unhandled message");
  e2ap_msg_t msg = fb->dec_msg[msg_type](e2m);
  return msg;
}


