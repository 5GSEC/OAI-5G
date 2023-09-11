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
 * Author and copyright: Laurent Thomas, open-cells.com
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

#include "e1ap.h"
#include "e1ap_common.h"
#include "e1ap_api.h"
#include "gnb_config.h"
#include "openair2/SDAP/nr_sdap/nr_sdap_entity.h"
#include "openair3/UTILS/conversions.h"
#include "openair2/RRC/NR/MESSAGES/asn1_msg.h"
#include "common/openairinterface5g_limits.h"
#include "common/utils/LOG/log.h"
#include "openair2/F1AP/f1ap_common.h"
#include "e1ap_default_values.h"
#include "gtp_itf.h"

#define E1AP_NUM_MSG_HANDLERS 14
typedef int (*e1ap_message_processing_t)(e1ap_upcp_inst_t *inst, const E1AP_E1AP_PDU_t *message_p);
e1ap_message_processing_t e1ap_message_processing[E1AP_NUM_MSG_HANDLERS][3] = {

  { 0, 0, 0 }, /* Reset */
  { 0, 0, 0 }, /* ErrorIndication */
  { 0, 0, 0 }, /* privateMessage */
  { e1apCUCP_handle_SETUP_REQUEST, e1apCUUP_handle_SETUP_RESPONSE, e1apCUUP_handle_SETUP_FAILURE }, /* gNBCUUPE1Setup */
  { 0, 0, 0 }, /* gNBCUCPE1Setup */
  { 0, 0, 0 }, /* gNBCUUPConfigurationUpdate */
  { 0, 0, 0 }, /* gNBCUCPConfigurationUpdate */
  { 0, 0, 0 }, /* E1Release */
  { e1apCUUP_handle_BEARER_CONTEXT_SETUP_REQUEST, e1apCUCP_handle_BEARER_CONTEXT_SETUP_RESPONSE, e1apCUCP_handle_BEARER_CONTEXT_SETUP_FAILURE }, /* bearerContextSetup */
  { e1apCUUP_handle_BEARER_CONTEXT_MODIFICATION_REQUEST, 0, 0 }, /* bearerContextModification */
  { 0, 0, 0 }, /* bearerContextModificationRequired */
  { e1apCUUP_handle_BEARER_CONTEXT_RELEASE_COMMAND, e1apCUCP_handle_BEARER_CONTEXT_RELEASE_COMPLETE, 0 }, /* bearerContextRelease */
  { 0, 0, 0 }, /* bearerContextReleaseRequired */
  { 0, 0, 0 } /* bearerContextInactivityNotification */
};

const char *e1ap_direction2String(int e1ap_dir) {
  static const char *e1ap_direction_String[] = {
    "", /* Nothing */
    "Initiating message", /* initiating message */
    "Successfull outcome", /* successfull outcome */
    "UnSuccessfull outcome", /* successfull outcome */
  };
  return(e1ap_direction_String[e1ap_dir]);
}

int e1ap_handle_message(instance_t instance, uint32_t assoc_id,
                        const uint8_t *const data, const uint32_t data_length) {
  E1AP_E1AP_PDU_t pdu= {0};
  int ret;
  DevAssert(data != NULL);

  if (e1ap_decode_pdu(&pdu, data, data_length) < 0) {
    LOG_E(E1AP, "Failed to decode PDU\n");
    return -1;
  }
  const E1AP_ProcedureCode_t procedureCode = pdu.choice.initiatingMessage->procedureCode;
  /* Checking procedure Code and direction of message */
  if ((procedureCode >= E1AP_NUM_MSG_HANDLERS) || (pdu.present > E1AP_E1AP_PDU_PR_unsuccessfulOutcome) || (pdu.present <= E1AP_E1AP_PDU_PR_NOTHING)) {
    LOG_E(E1AP, "[SCTP %d] Either procedureCode %ld or direction %d exceed expected\n", assoc_id, procedureCode, pdu.present);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E1AP_E1AP_PDU, &pdu);
    return -1;
  }

  if (e1ap_message_processing[procedureCode][pdu.present - 1] == NULL) {
    // No handler present. This can mean not implemented or no procedure for eNB (wrong direction).
    LOG_E(E1AP, "[SCTP %d] No handler for procedureCode %ld in %s\n", assoc_id, procedureCode, e1ap_direction2String(pdu.present - 1));
    ret=-1;
  } else {
    /* Calling the right handler */
    LOG_I(E1AP, "Calling handler with instance %ld\n",instance);
    ret = (*e1ap_message_processing[procedureCode][pdu.present - 1])(getCxtE1(instance), &pdu);
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E1AP_E1AP_PDU, &pdu);
  return ret;
}

void e1_task_handle_sctp_data_ind(instance_t instance, sctp_data_ind_t *sctp_data_ind)
{
  int result;
  DevAssert(sctp_data_ind != NULL);
  e1ap_handle_message(instance, sctp_data_ind->assoc_id,
                      sctp_data_ind->buffer, sctp_data_ind->buffer_length);
  result = itti_free(TASK_UNKNOWN, sctp_data_ind->buffer);
  AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
}

void e1ap_itti_send_sctp_close_association(bool isCu, instance_t instance) {
  MessageDef *message = itti_alloc_new_message(TASK_S1AP, 0, SCTP_CLOSE_ASSOCIATION);
  sctp_close_association_t *sctp_close_association = &message->ittiMsg.sctp_close_association;
  sctp_close_association->assoc_id      = e1ap_assoc_id(isCu,instance);
  itti_send_msg_to_task(TASK_SCTP, instance, message);
}

int e1ap_send_RESET(bool isCu, e1ap_setup_req_t *setupReq, E1AP_Reset_t *Reset)
{
  AssertFatal(false,"Not implemented yet\n");
  E1AP_E1AP_PDU_t pdu= {0};
  return e1ap_encode_send(isCu, setupReq, &pdu, 0, __func__);
}

int e1ap_send_RESET_ACKNOWLEDGE(instance_t instance, E1AP_Reset_t *Reset) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1ap_handle_RESET(instance_t instance,
                      uint32_t assoc_id,
                      uint32_t stream,
                      E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1ap_handle_RESET_ACKNOWLEDGE(instance_t instance,
                                  uint32_t assoc_id,
                                  uint32_t stream,
                                  E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

/*
    Error Indication
*/
int e1ap_handle_ERROR_INDICATION(instance_t instance,
                                 uint32_t assoc_id,
                                 uint32_t stream,
                                 E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1ap_send_ERROR_INDICATION(instance_t instance, E1AP_ErrorIndication_t *ErrorIndication) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}


/*
    E1 Setup: can be sent on both ways, to be refined
*/

static void fill_SETUP_REQUEST(e1ap_setup_req_t *setup, E1AP_E1AP_PDU_t *pdu)
{
  /* Create */
  /* 0. pdu Type */
  pdu->present = E1AP_E1AP_PDU_PR_initiatingMessage;
  asn1cCalloc(pdu->choice.initiatingMessage, initMsg);
  initMsg->procedureCode = E1AP_ProcedureCode_id_gNB_CU_UP_E1Setup;
  initMsg->criticality   = E1AP_Criticality_reject;
  initMsg->value.present       = E1AP_InitiatingMessage__value_PR_GNB_CU_UP_E1SetupRequest;
  E1AP_GNB_CU_UP_E1SetupRequest_t *e1SetupUP = &initMsg->value.choice.GNB_CU_UP_E1SetupRequest;
  /* mandatory */
  /* c1. Transaction ID (integer value) */
  asn1cSequenceAdd(e1SetupUP->protocolIEs.list, E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_TransactionID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_GNB_CU_UP_E1SetupRequestIEs__value_PR_TransactionID;
  setup->transac_id = E1AP_get_next_transaction_identifier();
  ieC1->value.choice.TransactionID = setup->transac_id;
  LOG_I(E1AP, "Transaction ID of setup request %ld\n", setup->transac_id);
  /* mandatory */
  /* c2. GNB_CU_ID (integer value) */
  asn1cSequenceAdd(e1SetupUP->protocolIEs.list, E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ieC2);
  ieC2->id                       = E1AP_ProtocolIE_ID_id_gNB_CU_UP_ID;
  ieC2->criticality              = E1AP_Criticality_reject;
  ieC2->value.present            = E1AP_GNB_CU_UP_E1SetupRequestIEs__value_PR_GNB_CU_UP_ID;
  asn_int642INTEGER(&ieC2->value.choice.GNB_CU_UP_ID, setup->gNB_cu_up_id);
  /* mandatory */
  /* c4. CN Support */
  asn1cSequenceAdd(e1SetupUP->protocolIEs.list, E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ieC4);
  ieC4->id = E1AP_ProtocolIE_ID_id_CNSupport;
  ieC4->criticality = E1AP_Criticality_reject;
  ieC4->value.present = E1AP_GNB_CU_UP_E1SetupRequestIEs__value_PR_CNSupport;
  ieC4->value.choice.CNSupport = setup->cn_support;

  /* mandatory */
  /* c5. Supported PLMNs */
  asn1cSequenceAdd(e1SetupUP->protocolIEs.list, E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ieC5);
  ieC5->id = E1AP_ProtocolIE_ID_id_SupportedPLMNs;
  ieC5->criticality = E1AP_Criticality_reject;
  ieC5->value.present = E1AP_GNB_CU_UP_E1SetupRequestIEs__value_PR_SupportedPLMNs_List;

  int numSupportedPLMNs = setup->supported_plmns;

  for (int i=0; i < numSupportedPLMNs; i++) {
    asn1cSequenceAdd(ieC5->value.choice.SupportedPLMNs_List.list, E1AP_SupportedPLMNs_Item_t, supportedPLMN);
    /* 5.1 PLMN Identity */
    MCC_MNC_TO_PLMNID(setup->plmns[i].mcc, setup->plmns[i].mnc, setup->plmns[i].mnc_digit_length, &supportedPLMN->pLMN_Identity);
  }
}

static void fill_SETUP_RESPONSE(const e1ap_setup_resp_t *e1ap_setup_resp, E1AP_E1AP_PDU_t *pdu)
{
  /* Create */
  /* 0. pdu Type */
  pdu->present = E1AP_E1AP_PDU_PR_successfulOutcome;
  asn1cCalloc(pdu->choice.successfulOutcome, initMsg);
  initMsg->procedureCode = E1AP_ProcedureCode_id_gNB_CU_UP_E1Setup;
  initMsg->criticality = E1AP_Criticality_reject;
  initMsg->value.present = E1AP_SuccessfulOutcome__value_PR_GNB_CU_UP_E1SetupResponse;
  E1AP_GNB_CU_UP_E1SetupResponse_t *out = &pdu->choice.successfulOutcome->value.choice.GNB_CU_UP_E1SetupResponse;
  /* mandatory */
  /* c1. Transaction ID (integer value) */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_GNB_CU_UP_E1SetupResponseIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_TransactionID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_GNB_CU_UP_E1SetupResponseIEs__value_PR_TransactionID;
  ieC1->value.choice.TransactionID = e1ap_setup_resp->transac_id;
}

void e1ap_send_SETUP_RESPONSE(instance_t inst, const e1ap_setup_resp_t *e1ap_setup_resp)
{
  AssertFatal(getCxtE1(inst), "");
  e1ap_setup_req_t *setupReq = &getCxtE1(inst)->setupReq;
  E1AP_E1AP_PDU_t pdu = {0};
  fill_SETUP_RESPONSE(e1ap_setup_resp, &pdu);
  e1ap_encode_send(getCxtE1(inst)->type, setupReq, &pdu, 0, __func__);
}

static void fill_SETUP_FAILURE(long transac_id, E1AP_E1AP_PDU_t *pdu)
{
  /* Create */
  /* 0. pdu Type */
  pdu->present = E1AP_E1AP_PDU_PR_unsuccessfulOutcome;
  asn1cCalloc(pdu->choice.unsuccessfulOutcome, initMsg);
  initMsg->procedureCode = E1AP_ProcedureCode_id_gNB_CU_UP_E1Setup;
  initMsg->criticality = E1AP_Criticality_reject;
  initMsg->value.present = E1AP_UnsuccessfulOutcome__value_PR_GNB_CU_UP_E1SetupFailure;
  E1AP_GNB_CU_UP_E1SetupFailure_t *out = &pdu->choice.unsuccessfulOutcome->value.choice.GNB_CU_UP_E1SetupFailure;
  /* mandatory */
  /* c1. Transaction ID (integer value) */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_GNB_CU_UP_E1SetupFailureIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_TransactionID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_GNB_CU_UP_E1SetupFailureIEs__value_PR_TransactionID;
  ieC1->value.choice.TransactionID = transac_id;
  /* mandatory */
  /* c2. cause (integer value) */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_GNB_CU_UP_E1SetupFailureIEs_t, ieC2);
  ieC2->id                         = E1AP_ProtocolIE_ID_id_Cause;
  ieC2->criticality                = E1AP_Criticality_ignore;
  ieC2->value.present              = E1AP_GNB_CU_UP_E1SetupFailureIEs__value_PR_Cause;
  ieC2->value.choice.Cause.present = E1AP_Cause_PR_radioNetwork; //choose this accordingly
  ieC2->value.choice.Cause.choice.radioNetwork = E1AP_CauseRadioNetwork_unspecified;
}

void e1apCUCP_send_SETUP_FAILURE(e1ap_setup_req_t *setupReq, long transac_id)
{
  E1AP_E1AP_PDU_t pdu = {0};
  fill_SETUP_FAILURE(transac_id, &pdu);
  e1ap_encode_send(CPtype, setupReq, &pdu, 0, __func__);
}

void extract_SETUP_REQUEST(const E1AP_E1AP_PDU_t *pdu,
                           e1ap_setup_req_t *req) {
  E1AP_GNB_CU_UP_E1SetupRequestIEs_t *ie;
  E1AP_GNB_CU_UP_E1SetupRequest_t *in = &pdu->choice.initiatingMessage->value.choice.GNB_CU_UP_E1SetupRequest;
  /* assoc_id */
  /* req->assoc_id = assoc_id; */

  /* transac_id */
  F1AP_FIND_PROTOCOLIE_BY_ID(E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ie, in,
                             E1AP_ProtocolIE_ID_id_TransactionID, true);
  req->transac_id = ie->value.choice.TransactionID;
  LOG_D(E1AP, "gNB CU UP E1 setup request transaction ID: %ld\n", req->transac_id);

  /* gNB CU UP ID */
  F1AP_FIND_PROTOCOLIE_BY_ID(E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ie, in,
                             E1AP_ProtocolIE_ID_id_gNB_CU_UP_ID, true);
  asn_INTEGER2ulong(&ie->value.choice.GNB_CU_UP_ID, &req->gNB_cu_up_id);
  LOG_D(E1AP, "gNB CU UP ID: %ld\n", req->gNB_cu_up_id);

  /* CN Support */
  F1AP_FIND_PROTOCOLIE_BY_ID(E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ie, in,
                             E1AP_ProtocolIE_ID_id_CNSupport, true);
  req->cn_support = ie->value.choice.CNSupport;
  LOG_D(E1AP, "E1ap CN support: %ld\n", req->cn_support);

  /* Supported PLMNs */
  F1AP_FIND_PROTOCOLIE_BY_ID(E1AP_GNB_CU_UP_E1SetupRequestIEs_t, ie, in,
                             E1AP_ProtocolIE_ID_id_SupportedPLMNs, true);
  req->supported_plmns = ie->value.choice.SupportedPLMNs_List.list.count;
  LOG_D(E1AP, "Number of supported PLMNs: %d\n", req->supported_plmns);

  for (int i=0; i < req->supported_plmns; i++) {
    E1AP_SupportedPLMNs_Item_t *supported_plmn_item = (E1AP_SupportedPLMNs_Item_t *)(ie->value.choice.SupportedPLMNs_List.list.array[i]);

    /* PLMN Identity */
    PLMNID_TO_MCC_MNC(&supported_plmn_item->pLMN_Identity,
                    req->plmns[i].mcc,
                    req->plmns[i].mnc,
                    req->plmns[i].mnc_digit_length);
    LOG_D(E1AP, "MCC: %d\nMNC: %d\n", req->plmns[i].mcc, req->plmns[i].mnc);
  }
}

int e1apCUCP_handle_SETUP_REQUEST(e1ap_upcp_inst_t *inst, const E1AP_E1AP_PDU_t *pdu)
{
  DevAssert(pdu != NULL);
  extract_SETUP_REQUEST(pdu, &inst->setupReq);
  /* Create ITTI message and send to queue */
  MessageDef *msg_p = itti_alloc_new_message(TASK_CUCP_E1, 0 /*unused by callee*/, E1AP_SETUP_REQ);
  memcpy(&E1AP_SETUP_REQ(msg_p), &inst->setupReq, sizeof(e1ap_setup_req_t));

  if (inst->setupReq.supported_plmns > 0) {
    itti_send_msg_to_task(TASK_RRC_GNB, 0 /*unused by callee*/, msg_p);
  } else {
    e1apCUCP_send_SETUP_FAILURE(&inst->setupReq, inst->setupReq.transac_id);
    itti_free(TASK_CUCP_E1, msg_p);
    return -1;
  }

  return 0;
}

int e1apCUUP_handle_SETUP_RESPONSE(e1ap_upcp_inst_t *inst, const E1AP_E1AP_PDU_t *pdu)
{
  LOG_D(E1AP, "%s\n", __func__);
  DevAssert(pdu->present == E1AP_E1AP_PDU_PR_successfulOutcome);
  DevAssert(pdu->choice.successfulOutcome->procedureCode  == E1AP_ProcedureCode_id_gNB_CU_UP_E1Setup);
  DevAssert(pdu->choice.successfulOutcome->criticality  == E1AP_Criticality_reject);
  DevAssert(pdu->choice.successfulOutcome->value.present  == E1AP_SuccessfulOutcome__value_PR_GNB_CU_UP_E1SetupResponse);
  const E1AP_GNB_CU_UP_E1SetupResponse_t  *in = &pdu->choice.successfulOutcome->value.choice.GNB_CU_UP_E1SetupResponse;
  E1AP_GNB_CU_UP_E1SetupResponseIEs_t *ie;

  /* transac_id */
  long transaction_id;
  long old_transaction_id = inst->setupReq.transac_id;
  F1AP_FIND_PROTOCOLIE_BY_ID(E1AP_GNB_CU_UP_E1SetupResponseIEs_t, ie, in,
                             E1AP_ProtocolIE_ID_id_TransactionID, true);
  transaction_id = ie->value.choice.TransactionID;
  LOG_D(E1AP, "gNB CU UP E1 setup response transaction ID: %ld\n", transaction_id);

  if (old_transaction_id != transaction_id)
    LOG_E(E1AP, "Transaction IDs do not match %ld != %ld\n", old_transaction_id, transaction_id);

  E1AP_free_transaction_identifier(transaction_id);

  /* do the required processing */

  return 0;
}

int e1apCUUP_handle_SETUP_FAILURE(e1ap_upcp_inst_t *inst, const E1AP_E1AP_PDU_t *pdu)
{
  E1AP_GNB_CU_UP_E1SetupFailureIEs_t *ie;
  DevAssert(pdu != NULL);
  E1AP_GNB_CU_UP_E1SetupFailure_t *in = &pdu->choice.unsuccessfulOutcome->value.choice.GNB_CU_UP_E1SetupFailure;
  /* Transaction ID */
  F1AP_FIND_PROTOCOLIE_BY_ID(E1AP_GNB_CU_UP_E1SetupFailureIEs_t, ie, in,
                             E1AP_ProtocolIE_ID_id_TransactionID, true);
  /* Cause */
  F1AP_FIND_PROTOCOLIE_BY_ID(E1AP_GNB_CU_UP_E1SetupFailureIEs_t, ie, in,
                             E1AP_ProtocolIE_ID_id_Cause, true);

  return 0;
}

static void fill_CONFIGURATION_UPDATE(E1AP_E1AP_PDU_t *pdu)
{
  /* Create */
  /* 0. pdu Type */
  pdu->present = E1AP_E1AP_PDU_PR_initiatingMessage;
  asn1cCalloc(pdu->choice.initiatingMessage, msg);
  msg->procedureCode = E1AP_ProcedureCode_id_gNB_CU_UP_ConfigurationUpdate;
  msg->criticality   = E1AP_Criticality_reject;
  msg->value.present = E1AP_InitiatingMessage__value_PR_GNB_CU_UP_ConfigurationUpdate;
  E1AP_GNB_CU_UP_ConfigurationUpdate_t *out = &pdu->choice.initiatingMessage->value.choice.GNB_CU_UP_ConfigurationUpdate;
  /* mandatory */
  /* c1. Transaction ID (integer value) */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_GNB_CU_UP_ConfigurationUpdateIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_TransactionID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_GNB_CU_UP_ConfigurationUpdateIEs__value_PR_TransactionID;
  ieC1->value.choice.TransactionID = 0;//get this from stored transaction IDs in CU
  /* mandatory */
  /* c2. Supported PLMNs */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_GNB_CU_UP_ConfigurationUpdateIEs_t, ieC2);
  ieC2->id = E1AP_ProtocolIE_ID_id_SupportedPLMNs;
  ieC2->criticality = E1AP_Criticality_reject;
  ieC2->value.present = E1AP_GNB_CU_UP_ConfigurationUpdateIEs__value_PR_SupportedPLMNs_List;

  int numSupportedPLMNs = 1;

  for (int i=0; i < numSupportedPLMNs; i++) {
    asn1cSequenceAdd(ieC2->value.choice.SupportedPLMNs_List.list, E1AP_SupportedPLMNs_Item_t, supportedPLMN);
    /* 5.1 PLMN Identity */
    OCTET_STRING_fromBuf(&supportedPLMN->pLMN_Identity, "OAI", strlen("OAI"));
  }

  /* mandatory */
  /* c3. TNLA to remove list */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_GNB_CU_UP_ConfigurationUpdateIEs_t, ieC3);
  ieC3->id = E1AP_ProtocolIE_ID_id_GNB_CU_UP_TNLA_To_Remove_List;
  ieC3->criticality = E1AP_Criticality_reject;
  ieC3->value.present = E1AP_GNB_CU_UP_ConfigurationUpdateIEs__value_PR_GNB_CU_UP_TNLA_To_Remove_List;

  int numTNLAtoRemoveList = 1;

  for (int i=0; i < numTNLAtoRemoveList; i++) {
    asn1cSequenceAdd(ieC2->value.choice.GNB_CU_UP_TNLA_To_Remove_List.list, E1AP_GNB_CU_UP_TNLA_To_Remove_Item_t, TNLAtoRemove);
    TNLAtoRemove->tNLAssociationTransportLayerAddress.present = E1AP_CP_TNL_Information_PR_endpoint_IP_Address;
    TRANSPORT_LAYER_ADDRESS_IPv4_TO_BIT_STRING(1234, &TNLAtoRemove->tNLAssociationTransportLayerAddress.choice.endpoint_IP_Address); // TODO: correct me
  }
}

/*
  E1 configuration update: can be sent in both ways, to be refined
*/

void e1apCUUP_send_CONFIGURATION_UPDATE(e1ap_setup_req_t *setupReq)
{
  E1AP_E1AP_PDU_t pdu = {0};
  fill_CONFIGURATION_UPDATE(&pdu);
  e1ap_encode_send(UPtype, setupReq, &pdu, 0, __func__);
}

int e1apCUCP_send_gNB_DU_CONFIGURATION_FAILURE(e1ap_setup_req_t *setupReq)
{
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_send_gNB_DU_CONFIGURATION_UPDATE_ACKNOWLEDGE(e1ap_setup_req_t *setupReq)
{
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_handle_CONFIGURATION_UPDATE(e1ap_setup_req_t *setupReq, E1AP_E1AP_PDU_t *pdu)
{
  /*
  E1AP_GNB_CU_UP_E1SetupRequestIEs_t *ie;
  DevAssert(pdu != NULL);
  E1AP_GNB_CU_UP_E1SetupRequest_t *in = &pdu->choice.initiatingMessage->value.choice.GNB_CU_UP_E1SetupRequest;
  */

  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUUP_handle_gNB_DU_CONFIGURATION_UPDATE_ACKNOWLEDGE(e1ap_setup_req_t *setupReq, uint32_t assoc_id, uint32_t stream, E1AP_E1AP_PDU_t *pdu)
{
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUUP_handle_gNB_DU_CONFIGURATION_FAILURE(e1ap_setup_req_t *setupReq, uint32_t assoc_id, uint32_t stream, E1AP_E1AP_PDU_t *pdu)
{
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

/*
  E1 release
*/

int e1ap_send_RELEASE_REQUEST(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1ap_send_RELEASE_ACKNOWLEDGE(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1ap_handle_RELEASE_REQUEST(instance_t instance,
                                uint32_t assoc_id,
                                uint32_t stream,
                                E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1ap_handle_RELEASE_ACKNOWLEDGE(instance_t instance,
                                    uint32_t assoc_id,
                                    uint32_t stream,
                                    E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

/*
  BEARER CONTEXT SETUP REQUEST
*/

static int fill_BEARER_CONTEXT_SETUP_REQUEST(e1ap_setup_req_t *setup, e1ap_bearer_setup_req_t *const bearerCxt, E1AP_E1AP_PDU_t *pdu)
{
  pdu->present = E1AP_E1AP_PDU_PR_initiatingMessage;
  asn1cCalloc(pdu->choice.initiatingMessage, msg);
  msg->procedureCode = E1AP_ProcedureCode_id_bearerContextSetup;
  msg->criticality   = E1AP_Criticality_reject;
  msg->value.present = E1AP_InitiatingMessage__value_PR_BearerContextSetupRequest;
  E1AP_BearerContextSetupRequest_t *out = &pdu->choice.initiatingMessage->value.choice.BearerContextSetupRequest;
  /* mandatory */
  /* c1. gNB-CU-UP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupRequestIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_BearerContextSetupRequestIEs__value_PR_GNB_CU_CP_UE_E1AP_ID;
  ieC1->value.choice.GNB_CU_CP_UE_E1AP_ID = bearerCxt->gNB_cu_cp_ue_id;
  /* mandatory */
  /* c2. Security Information */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupRequestIEs_t, ieC2);
  ieC2->id                         = E1AP_ProtocolIE_ID_id_SecurityInformation;
  ieC2->criticality                = E1AP_Criticality_reject;
  ieC2->value.present              = E1AP_BearerContextSetupRequestIEs__value_PR_SecurityInformation;
  ieC2->value.choice.SecurityInformation.securityAlgorithm.cipheringAlgorithm = bearerCxt->cipheringAlgorithm;
  OCTET_STRING_fromBuf(&ieC2->value.choice.SecurityInformation.uPSecuritykey.encryptionKey,
                       bearerCxt->encryptionKey, strlen(bearerCxt->encryptionKey));

  asn1cCallocOne(ieC2->value.choice.SecurityInformation.securityAlgorithm.integrityProtectionAlgorithm, bearerCxt->integrityProtectionAlgorithm);
  asn1cCalloc(ieC2->value.choice.SecurityInformation.uPSecuritykey.integrityProtectionKey, protKey);
  OCTET_STRING_fromBuf(protKey, bearerCxt->integrityProtectionKey, strlen(bearerCxt->integrityProtectionKey));
  /* mandatory */
  /* c3. UE DL Aggregate Maximum Bit Rate */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupRequestIEs_t, ieC3);
  ieC3->id                         = E1AP_ProtocolIE_ID_id_UEDLAggregateMaximumBitRate;
  ieC3->criticality                = E1AP_Criticality_reject;
  ieC3->value.present              = E1AP_BearerContextSetupRequestIEs__value_PR_BitRate;
  asn_long2INTEGER(&ieC3->value.choice.BitRate, bearerCxt->ueDlAggMaxBitRate);
  /* mandatory */
  /* c4. Serving PLMN */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupRequestIEs_t, ieC4);
  ieC4->id                         = E1AP_ProtocolIE_ID_id_Serving_PLMN;
  ieC4->criticality                = E1AP_Criticality_ignore;
  ieC4->value.present              = E1AP_BearerContextSetupRequestIEs__value_PR_PLMN_Identity;
  PLMN_ID_t *servingPLMN = setup->plmns; // First PLMN is serving PLMN. TODO: Remove hard coding here
  MCC_MNC_TO_PLMNID(servingPLMN->mcc, servingPLMN->mnc, servingPLMN->mnc_digit_length, &ieC4->value.choice.PLMN_Identity);
  /* mandatory */
  /* Activity Notification Level */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupRequestIEs_t, ieC5);
  ieC5->id                         = E1AP_ProtocolIE_ID_id_ActivityNotificationLevel;
  ieC5->criticality                = E1AP_Criticality_reject;
  ieC5->value.present              = E1AP_BearerContextSetupRequestIEs__value_PR_ActivityNotificationLevel;
  ieC5->value.choice.ActivityNotificationLevel = E1AP_ActivityNotificationLevel_pdu_session;// TODO: Remove hard coding
  /* mandatory */
  /*  */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupRequestIEs_t, ieC6);
  ieC6->id            = E1AP_ProtocolIE_ID_id_System_BearerContextSetupRequest;
  ieC6->criticality   = E1AP_Criticality_reject;
  ieC6->value.present = E1AP_BearerContextSetupRequestIEs__value_PR_System_BearerContextSetupRequest;
  ieC6->value.choice.System_BearerContextSetupRequest.present = E1AP_System_BearerContextSetupRequest_PR_nG_RAN_BearerContextSetupRequest;
  E1AP_ProtocolIE_Container_4932P19_t *msgNGRAN_list = calloc(1, sizeof(E1AP_ProtocolIE_Container_4932P19_t));
  ieC6->value.choice.System_BearerContextSetupRequest.choice.nG_RAN_BearerContextSetupRequest = (struct E1AP_ProtocolIE_Container *) msgNGRAN_list;
  asn1cSequenceAdd(msgNGRAN_list->list, E1AP_NG_RAN_BearerContextSetupRequest_t, msgNGRAN);
  msgNGRAN->id = E1AP_ProtocolIE_ID_id_PDU_Session_Resource_To_Setup_List;
  msgNGRAN->criticality = E1AP_Criticality_reject;
  msgNGRAN->value.present = E1AP_NG_RAN_BearerContextSetupRequest__value_PR_PDU_Session_Resource_To_Setup_List;
  E1AP_PDU_Session_Resource_To_Setup_List_t *pdu2Setup = &msgNGRAN->value.choice.PDU_Session_Resource_To_Setup_List;
  for(pdu_session_to_setup_t *i=bearerCxt->pduSession; i < bearerCxt->pduSession+bearerCxt->numPDUSessions; i++) {
    asn1cSequenceAdd(pdu2Setup->list, E1AP_PDU_Session_Resource_To_Setup_Item_t, ieC6_1);
    ieC6_1->pDU_Session_ID = i->sessionId;
    ieC6_1->pDU_Session_Type = i->sessionType;

    INT8_TO_OCTET_STRING(i->sst, &ieC6_1->sNSSAI.sST);

    ieC6_1->securityIndication.integrityProtectionIndication = i->integrityProtectionIndication;
    ieC6_1->securityIndication.confidentialityProtectionIndication = i->confidentialityProtectionIndication;

    ieC6_1->nG_UL_UP_TNL_Information.present = E1AP_UP_TNL_Information_PR_gTPTunnel;
    asn1cCalloc(ieC6_1->nG_UL_UP_TNL_Information.choice.gTPTunnel, gTPTunnel);
    TRANSPORT_LAYER_ADDRESS_IPv4_TO_BIT_STRING(i->tlAddress, &gTPTunnel->transportLayerAddress);
    INT32_TO_OCTET_STRING(i->teId, &gTPTunnel->gTP_TEID);

    for (DRB_nGRAN_to_setup_t *j=i->DRBnGRanList; j < i->DRBnGRanList+i->numDRB2Setup; j++) {
      asn1cSequenceAdd(ieC6_1->dRB_To_Setup_List_NG_RAN.list, E1AP_DRB_To_Setup_Item_NG_RAN_t, ieC6_1_1);
      ieC6_1_1->dRB_ID = j->id;

      ieC6_1_1->sDAP_Configuration.defaultDRB = j->defaultDRB;
      ieC6_1_1->sDAP_Configuration.sDAP_Header_UL = j->sDAP_Header_UL;
      ieC6_1_1->sDAP_Configuration.sDAP_Header_DL = j->sDAP_Header_DL;

      ieC6_1_1->pDCP_Configuration.pDCP_SN_Size_UL = j->pDCP_SN_Size_UL;
      ieC6_1_1->pDCP_Configuration.pDCP_SN_Size_DL = j->pDCP_SN_Size_DL;
      asn1cCallocOne(ieC6_1_1->pDCP_Configuration.discardTimer, j->discardTimer);
      E1AP_T_ReorderingTimer_t *roTimer = calloc(1, sizeof(E1AP_T_ReorderingTimer_t));
      ieC6_1_1->pDCP_Configuration.t_ReorderingTimer = roTimer;
      roTimer->t_Reordering = j->reorderingTimer;
      ieC6_1_1->pDCP_Configuration.rLC_Mode        = j->rLC_Mode;

      for (cell_group_t *k=j->cellGroupList; k < j->cellGroupList+j->numCellGroups; k++) {
        asn1cSequenceAdd(ieC6_1_1->cell_Group_Information.list, E1AP_Cell_Group_Information_Item_t, ieC6_1_1_1);
        ieC6_1_1_1->cell_Group_ID = k->id;
      }

      for (qos_flow_to_setup_t *k=j->qosFlows; k < j->qosFlows+j->numQosFlow2Setup; k++) {
        asn1cSequenceAdd(ieC6_1_1->qos_flow_Information_To_Be_Setup, E1AP_QoS_Flow_QoS_Parameter_Item_t, ieC6_1_1_1);
        ieC6_1_1_1->qoS_Flow_Identifier = k->id;

        if (k->fiveQI_type == non_dynamic) { // non Dynamic 5QI
          ieC6_1_1_1->qoSFlowLevelQoSParameters.qoS_Characteristics.present = E1AP_QoS_Characteristics_PR_non_Dynamic_5QI;
          asn1cCalloc(ieC6_1_1_1->qoSFlowLevelQoSParameters.qoS_Characteristics.choice.non_Dynamic_5QI, non_Dynamic_5QI);
          non_Dynamic_5QI->fiveQI = k->fiveQI;
        } else { // dynamic 5QI
          ieC6_1_1_1->qoSFlowLevelQoSParameters.qoS_Characteristics.present = E1AP_QoS_Characteristics_PR_dynamic_5QI;
          asn1cCalloc(ieC6_1_1_1->qoSFlowLevelQoSParameters.qoS_Characteristics.choice.dynamic_5QI, dynamic_5QI);
          dynamic_5QI->qoSPriorityLevel = k->qoSPriorityLevel;
          dynamic_5QI->packetDelayBudget = k->packetDelayBudget;
          dynamic_5QI->packetErrorRate.pER_Scalar = k->packetError_scalar;
          dynamic_5QI->packetErrorRate.pER_Exponent = k->packetError_exponent;
        }

        ieC6_1_1_1->qoSFlowLevelQoSParameters.nGRANallocationRetentionPriority.priorityLevel = k->priorityLevel;
        ieC6_1_1_1->qoSFlowLevelQoSParameters.nGRANallocationRetentionPriority.pre_emptionCapability = k->pre_emptionCapability;
        ieC6_1_1_1->qoSFlowLevelQoSParameters.nGRANallocationRetentionPriority.pre_emptionVulnerability = k->pre_emptionVulnerability;
      }
    }
  }
  return 0;
}

void e1apCUCP_send_BEARER_CONTEXT_SETUP_REQUEST(instance_t instance, e1ap_bearer_setup_req_t *const bearerCxt)
{
  if (!getCxtE1(instance)) {
    LOG_E(E1AP, "Received a UE bearer to establish while no CU-UP is connected, response on NGAP to send failure is not developped\n");
    // Fixme: add response on NGAP to send failure
    return;
  }

  E1AP_E1AP_PDU_t pdu = {0};
  e1ap_setup_req_t *setupReq = &getCxtE1(instance)->setupReq;
  fill_BEARER_CONTEXT_SETUP_REQUEST(setupReq, bearerCxt, &pdu);
  e1ap_encode_send(CPtype, setupReq, &pdu, 0, __func__);
}

static void fill_BEARER_CONTEXT_SETUP_RESPONSE(e1ap_bearer_setup_resp_t *const resp, E1AP_E1AP_PDU_t *pdu)
{
  /* Create */
  /* 0. pdu Type */
  pdu->present = E1AP_E1AP_PDU_PR_successfulOutcome;
  asn1cCalloc(pdu->choice.successfulOutcome, msg);
  msg->procedureCode = E1AP_ProcedureCode_id_bearerContextSetup;
  msg->criticality   = E1AP_Criticality_reject;
  msg->value.present = E1AP_SuccessfulOutcome__value_PR_BearerContextSetupResponse;
  E1AP_BearerContextSetupResponse_t *out = &pdu->choice.successfulOutcome->value.choice.BearerContextSetupResponse;
  /* mandatory */
  /* c1. gNB-CU-CP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupResponseIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_BearerContextSetupResponseIEs__value_PR_GNB_CU_CP_UE_E1AP_ID;
  ieC1->value.choice.GNB_CU_CP_UE_E1AP_ID = resp->gNB_cu_cp_ue_id;
  /* mandatory */
  /* c2. gNB-CU-UP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupResponseIEs_t, ieC2);
  ieC2->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID;
  ieC2->criticality                = E1AP_Criticality_reject;
  ieC2->value.present              = E1AP_BearerContextSetupResponseIEs__value_PR_GNB_CU_UP_UE_E1AP_ID;
  ieC2->value.choice.GNB_CU_CP_UE_E1AP_ID = resp->gNB_cu_up_ue_id;

  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextSetupResponseIEs_t, ieC3);
  ieC3->id            = E1AP_ProtocolIE_ID_id_System_BearerContextSetupResponse;
  ieC3->criticality   = E1AP_Criticality_reject;
  ieC3->value.present = E1AP_BearerContextSetupResponseIEs__value_PR_System_BearerContextSetupResponse;
  if (0) { // EUTRAN
    ieC3->value.choice.System_BearerContextSetupResponse.present = E1AP_System_BearerContextSetupResponse_PR_e_UTRAN_BearerContextSetupResponse;
    E1AP_ProtocolIE_Container_4932P21_t *msgEUTRAN_list = calloc(1, sizeof(E1AP_ProtocolIE_Container_4932P21_t));
    ieC3->value.choice.System_BearerContextSetupResponse.choice.e_UTRAN_BearerContextSetupResponse = (struct E1AP_ProtocolIE_Container *) msgEUTRAN_list;
    asn1cSequenceAdd(msgEUTRAN_list->list, E1AP_EUTRAN_BearerContextSetupResponse_t, msgEUTRAN);
    msgEUTRAN->id = E1AP_ProtocolIE_ID_id_DRB_Setup_List_EUTRAN;
    msgEUTRAN->criticality = E1AP_Criticality_reject;
    msgEUTRAN->value.present = E1AP_EUTRAN_BearerContextSetupResponse__value_PR_DRB_Setup_List_EUTRAN;
    E1AP_DRB_Setup_List_EUTRAN_t *drbSetup = &msgEUTRAN->value.choice.DRB_Setup_List_EUTRAN;

    for (drb_setup_t *i=resp->DRBList; i < resp->DRBList+resp->numDRBs; i++) {
      asn1cSequenceAdd(drbSetup->list, E1AP_DRB_Setup_Item_EUTRAN_t, ieC3_1);
      ieC3_1->dRB_ID = i->drbId;

      ieC3_1->s1_DL_UP_TNL_Information.present = E1AP_UP_TNL_Information_PR_gTPTunnel;
      asn1cCalloc(ieC3_1->s1_DL_UP_TNL_Information.choice.gTPTunnel, gTPTunnel);
      TRANSPORT_LAYER_ADDRESS_IPv4_TO_BIT_STRING(i->tlAddress, &gTPTunnel->transportLayerAddress);
      INT32_TO_OCTET_STRING(i->teId, &gTPTunnel->gTP_TEID);

      for (up_params_t *j=i->UpParamList; j < i->UpParamList+i->numUpParam; j++) {
        asn1cSequenceAdd(ieC3_1->uL_UP_Transport_Parameters.list, E1AP_UP_Parameters_Item_t, ieC3_1_1);
        ieC3_1_1->uP_TNL_Information.present = E1AP_UP_TNL_Information_PR_gTPTunnel;
        asn1cCalloc(ieC3_1_1->uP_TNL_Information.choice.gTPTunnel, gTPTunnel);
        TRANSPORT_LAYER_ADDRESS_IPv4_TO_BIT_STRING(j->tlAddress, &gTPTunnel->transportLayerAddress);
        INT32_TO_OCTET_STRING(j->teId, &gTPTunnel->gTP_TEID);
      }
    }
  } else {
    ieC3->value.choice.System_BearerContextSetupResponse.present = E1AP_System_BearerContextSetupResponse_PR_nG_RAN_BearerContextSetupResponse;
    E1AP_ProtocolIE_Container_4932P22_t *msgNGRAN_list = calloc(1, sizeof(E1AP_ProtocolIE_Container_4932P22_t));
    ieC3->value.choice.System_BearerContextSetupResponse.choice.nG_RAN_BearerContextSetupResponse = (struct E1AP_ProtocolIE_Container *) msgNGRAN_list;
    asn1cSequenceAdd(msgNGRAN_list->list, E1AP_NG_RAN_BearerContextSetupResponse_t, msgNGRAN);
    msgNGRAN->id = E1AP_ProtocolIE_ID_id_PDU_Session_Resource_Setup_List;
    msgNGRAN->criticality = E1AP_Criticality_reject;
    msgNGRAN->value.present = E1AP_NG_RAN_BearerContextSetupResponse__value_PR_PDU_Session_Resource_Setup_List;
    E1AP_PDU_Session_Resource_Setup_List_t *pduSetup = &msgNGRAN->value.choice.PDU_Session_Resource_Setup_List;

    for (pdu_session_setup_t *i=resp->pduSession; i < resp->pduSession+resp->numPDUSessions; i++) {
      asn1cSequenceAdd(pduSetup->list, E1AP_PDU_Session_Resource_Setup_Item_t, ieC3_1);
      ieC3_1->pDU_Session_ID = i->id;

      ieC3_1->nG_DL_UP_TNL_Information.present = E1AP_UP_TNL_Information_PR_gTPTunnel;
      asn1cCalloc(ieC3_1->nG_DL_UP_TNL_Information.choice.gTPTunnel, gTPTunnel);
      TRANSPORT_LAYER_ADDRESS_IPv4_TO_BIT_STRING(i->tlAddress, &gTPTunnel->transportLayerAddress);
      INT32_TO_OCTET_STRING(i->teId, &gTPTunnel->gTP_TEID);

      for (DRB_nGRAN_setup_t *j=i->DRBnGRanList; j < i->DRBnGRanList+i->numDRBSetup; j++) {
        asn1cSequenceAdd(ieC3_1->dRB_Setup_List_NG_RAN.list, E1AP_DRB_Setup_Item_NG_RAN_t, ieC3_1_1);
        ieC3_1_1->dRB_ID = j->id;

        for (up_params_t *k=j->UpParamList; k < j->UpParamList+j->numUpParam; k++) {
          asn1cSequenceAdd(ieC3_1_1->uL_UP_Transport_Parameters.list, E1AP_UP_Parameters_Item_t, ieC3_1_1_1);
          ieC3_1_1_1->uP_TNL_Information.present = E1AP_UP_TNL_Information_PR_gTPTunnel;
          asn1cCalloc(ieC3_1_1_1->uP_TNL_Information.choice.gTPTunnel, gTPTunnel);
          TRANSPORT_LAYER_ADDRESS_IPv4_TO_BIT_STRING(k->tlAddress, &gTPTunnel->transportLayerAddress);
          INT32_TO_OCTET_STRING(k->teId, &gTPTunnel->gTP_TEID);
        }

        for (qos_flow_setup_t *k=j->qosFlows; k < j->qosFlows+j->numQosFlowSetup; k++) {
          asn1cSequenceAdd(ieC3_1_1->flow_Setup_List.list, E1AP_QoS_Flow_Item_t, ieC3_1_1_1);
          ieC3_1_1_1->qoS_Flow_Identifier = k->id;
        }
      }

      if (i->numDRBFailed > 0)
        ieC3_1->dRB_Failed_List_NG_RAN = calloc(1, sizeof(E1AP_DRB_Failed_List_NG_RAN_t));

      for (DRB_nGRAN_failed_t *j=i->DRBnGRanFailedList; j < i->DRBnGRanFailedList+i->numDRBFailed; j++) {
        asn1cSequenceAdd(ieC3_1->dRB_Failed_List_NG_RAN->list, E1AP_DRB_Failed_Item_NG_RAN_t, ieC3_1_1);
        ieC3_1_1->dRB_ID = j->id;

        ieC3_1_1->cause.present = j->cause_type;
        switch (ieC3_1_1->cause.present) {
          case E1AP_Cause_PR_radioNetwork:
            ieC3_1_1->cause.choice.radioNetwork = j->cause;
            break;

          case E1AP_Cause_PR_transport:
            ieC3_1_1->cause.choice.transport = j->cause;
            break;

          case E1AP_Cause_PR_protocol:
            ieC3_1_1->cause.choice.protocol = j->cause;
            break;

          case E1AP_Cause_PR_misc:
            ieC3_1_1->cause.choice.misc = j->cause;
            break;

          default:
            LOG_E(E1AP, "DRB setup failure cause out of expected range\n");
            break;
        }
      }
    }
  }
}

void e1apCUUP_send_BEARER_CONTEXT_SETUP_RESPONSE(e1ap_upcp_inst_t *inst, e1ap_bearer_setup_resp_t *const resp)
{
  E1AP_E1AP_PDU_t pdu = {0};
  fill_BEARER_CONTEXT_SETUP_RESPONSE(resp, &pdu);
  e1ap_encode_send(UPtype, &inst->setupReq, &pdu, 0, __func__);
}

int e1apCUUP_send_BEARER_CONTEXT_SETUP_FAILURE(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

void extract_BEARER_CONTEXT_SETUP_REQUEST(const E1AP_E1AP_PDU_t *pdu,
                                          e1ap_bearer_setup_req_t *bearerCxt) {
  const E1AP_BearerContextSetupRequest_t *in = &pdu->choice.initiatingMessage->value.choice.BearerContextSetupRequest;
  E1AP_BearerContextSetupRequestIEs_t *ie;

  LOG_I(E1AP, "Bearer context setup number of IEs %d\n", in->protocolIEs.list.count);

  for (int i=0; i < in->protocolIEs.list.count; i++) {
    ie = in->protocolIEs.list.array[i];

    switch(ie->id) {
      case E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupRequestIEs__value_PR_GNB_CU_CP_UE_E1AP_ID);
        bearerCxt->gNB_cu_cp_ue_id = ie->value.choice.GNB_CU_CP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_SecurityInformation:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupRequestIEs__value_PR_SecurityInformation);
        bearerCxt->cipheringAlgorithm = ie->value.choice.SecurityInformation.securityAlgorithm.cipheringAlgorithm;
        memcpy(bearerCxt->encryptionKey,
               ie->value.choice.SecurityInformation.uPSecuritykey.encryptionKey.buf,
               ie->value.choice.SecurityInformation.uPSecuritykey.encryptionKey.size);
        if (ie->value.choice.SecurityInformation.securityAlgorithm.integrityProtectionAlgorithm) {
          bearerCxt->integrityProtectionAlgorithm = *ie->value.choice.SecurityInformation.securityAlgorithm.integrityProtectionAlgorithm;
        }
        if (ie->value.choice.SecurityInformation.uPSecuritykey.integrityProtectionKey) {
          memcpy(bearerCxt->integrityProtectionKey,
                 ie->value.choice.SecurityInformation.uPSecuritykey.integrityProtectionKey->buf,
                 ie->value.choice.SecurityInformation.uPSecuritykey.integrityProtectionKey->size);
        }
        break;

      case E1AP_ProtocolIE_ID_id_UEDLAggregateMaximumBitRate:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupRequestIEs__value_PR_BitRate);
        asn_INTEGER2long(&ie->value.choice.BitRate, &bearerCxt->ueDlAggMaxBitRate);
        break;

      case E1AP_ProtocolIE_ID_id_Serving_PLMN:
        DevAssert(ie->criticality == E1AP_Criticality_ignore);
        DevAssert(ie->value.present == E1AP_BearerContextSetupRequestIEs__value_PR_PLMN_Identity);
        PLMNID_TO_MCC_MNC(&ie->value.choice.PLMN_Identity,
                          bearerCxt->servingPLMNid.mcc,
                          bearerCxt->servingPLMNid.mnc,
                          bearerCxt->servingPLMNid.mnc_digit_length);
        break;

      case E1AP_ProtocolIE_ID_id_ActivityNotificationLevel:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupRequestIEs__value_PR_ActivityNotificationLevel);
        bearerCxt->activityNotificationLevel = ie->value.choice.ActivityNotificationLevel;
        break;

      case E1AP_ProtocolIE_ID_id_System_BearerContextSetupRequest:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupRequestIEs__value_PR_System_BearerContextSetupRequest);
        DevAssert(ie->value.choice.System_BearerContextSetupRequest.present ==
                    E1AP_System_BearerContextSetupRequest_PR_nG_RAN_BearerContextSetupRequest);
        DevAssert(ie->value.choice.System_BearerContextSetupRequest.choice.nG_RAN_BearerContextSetupRequest);
        E1AP_ProtocolIE_Container_4932P19_t *msgNGRAN_list = (E1AP_ProtocolIE_Container_4932P19_t *) ie->value.choice.System_BearerContextSetupRequest.choice.nG_RAN_BearerContextSetupRequest;
        E1AP_NG_RAN_BearerContextSetupRequest_t *msgNGRAN = msgNGRAN_list->list.array[0];
        DevAssert(msgNGRAN_list->list.count == 1);
        DevAssert(msgNGRAN->id == E1AP_ProtocolIE_ID_id_PDU_Session_Resource_To_Setup_List);
        DevAssert(msgNGRAN->value.present == E1AP_NG_RAN_BearerContextSetupRequest__value_PR_PDU_Session_Resource_To_Setup_List);

        E1AP_PDU_Session_Resource_To_Setup_List_t *pdu2SetupList = &msgNGRAN->value.choice.PDU_Session_Resource_To_Setup_List;
        bearerCxt->numPDUSessions = pdu2SetupList->list.count;
        for (int i=0; i < pdu2SetupList->list.count; i++) {
          pdu_session_to_setup_t *pdu_session = bearerCxt->pduSession + i;
          E1AP_PDU_Session_Resource_To_Setup_Item_t *pdu2Setup = pdu2SetupList->list.array[i];

          pdu_session->sessionId = pdu2Setup->pDU_Session_ID;
          pdu_session->sessionType = pdu2Setup->pDU_Session_Type;

          OCTET_STRING_TO_INT8(&pdu2Setup->sNSSAI.sST, pdu_session->sst);

          pdu_session->integrityProtectionIndication = pdu2Setup->securityIndication.integrityProtectionIndication;
          pdu_session->confidentialityProtectionIndication = pdu2Setup->securityIndication.confidentialityProtectionIndication;

          if (pdu2Setup->nG_UL_UP_TNL_Information.choice.gTPTunnel) { // Optional IE
            DevAssert(pdu2Setup->nG_UL_UP_TNL_Information.present ==
                      E1AP_UP_TNL_Information_PR_gTPTunnel);
            BIT_STRING_TO_TRANSPORT_LAYER_ADDRESS_IPv4(&pdu2Setup->nG_UL_UP_TNL_Information.choice.gTPTunnel->transportLayerAddress,
                                                       pdu_session->tlAddress);
            OCTET_STRING_TO_INT32(&pdu2Setup->nG_UL_UP_TNL_Information.choice.gTPTunnel->gTP_TEID, pdu_session->teId);
          }

          E1AP_DRB_To_Setup_List_NG_RAN_t *drb2SetupList = &pdu2Setup->dRB_To_Setup_List_NG_RAN;
          pdu_session->numDRB2Setup = drb2SetupList->list.count;
          for (int j=0; j < drb2SetupList->list.count; j++) {
            DRB_nGRAN_to_setup_t *drb = pdu_session->DRBnGRanList + j;
            E1AP_DRB_To_Setup_Item_NG_RAN_t *drb2Setup = drb2SetupList->list.array[j];

            drb->id = drb2Setup->dRB_ID;

            drb->defaultDRB = drb2Setup->sDAP_Configuration.defaultDRB;
            drb->sDAP_Header_UL = drb2Setup->sDAP_Configuration.sDAP_Header_UL;
            drb->sDAP_Header_DL = drb2Setup->sDAP_Configuration.sDAP_Header_DL;

            drb->pDCP_SN_Size_UL = drb2Setup->pDCP_Configuration.pDCP_SN_Size_UL;
            drb->pDCP_SN_Size_DL = drb2Setup->pDCP_Configuration.pDCP_SN_Size_DL;

            if (drb2Setup->pDCP_Configuration.discardTimer) {
              drb->discardTimer = *drb2Setup->pDCP_Configuration.discardTimer;
            }

            if (drb2Setup->pDCP_Configuration.t_ReorderingTimer) {
              drb->reorderingTimer = drb2Setup->pDCP_Configuration.t_ReorderingTimer->t_Reordering;
            }

            drb->rLC_Mode = drb2Setup->pDCP_Configuration.rLC_Mode;

            E1AP_Cell_Group_Information_t *cellGroupList = &drb2Setup->cell_Group_Information;
            drb->numCellGroups = cellGroupList->list.count;
            for (int k=0; k < cellGroupList->list.count; k++) {
              E1AP_Cell_Group_Information_Item_t *cg2Setup = cellGroupList->list.array[k];

              drb->cellGroupList[k].id = cg2Setup->cell_Group_ID;
            }

            E1AP_QoS_Flow_QoS_Parameter_List_t *qos2SetupList = &drb2Setup->qos_flow_Information_To_Be_Setup;
            drb->numQosFlow2Setup = qos2SetupList->list.count;
            for (int k=0; k < qos2SetupList->list.count; k++) {
              qos_flow_to_setup_t *qos = drb->qosFlows + k;
              E1AP_QoS_Flow_QoS_Parameter_Item_t *qos2Setup = qos2SetupList->list.array[k];

              qos->id = qos2Setup->qoS_Flow_Identifier;

              if (qos2Setup->qoSFlowLevelQoSParameters.qoS_Characteristics.present ==
                  E1AP_QoS_Characteristics_PR_non_Dynamic_5QI) {
                qos->fiveQI_type = non_dynamic;
                qos->fiveQI = qos2Setup->qoSFlowLevelQoSParameters.qoS_Characteristics.choice.non_Dynamic_5QI->fiveQI;
              } else {
                E1AP_Dynamic5QIDescriptor_t *dynamic5QI = qos2Setup->qoSFlowLevelQoSParameters.qoS_Characteristics.choice.dynamic_5QI;
                qos->fiveQI_type = dynamic;
                qos->qoSPriorityLevel = dynamic5QI->qoSPriorityLevel;
                qos->packetDelayBudget = dynamic5QI->packetDelayBudget;
                qos->packetError_scalar = dynamic5QI->packetErrorRate.pER_Scalar;
                qos->packetError_exponent = dynamic5QI->packetErrorRate.pER_Exponent;
              }

              qos->priorityLevel = qos2Setup->qoSFlowLevelQoSParameters.nGRANallocationRetentionPriority.priorityLevel;
              qos->pre_emptionCapability = qos2Setup->qoSFlowLevelQoSParameters.nGRANallocationRetentionPriority.pre_emptionCapability;
              qos->pre_emptionVulnerability = qos2Setup->qoSFlowLevelQoSParameters.nGRANallocationRetentionPriority.pre_emptionVulnerability;
            }
          }
        }
        break;

      default:
        LOG_E(E1AP, "Handle for this IE is not implemented (or) invalid IE detected\n");
        break;
    }
  }


}

int e1apCUUP_handle_BEARER_CONTEXT_SETUP_REQUEST(e1ap_upcp_inst_t *e1_inst, const E1AP_E1AP_PDU_t *pdu)
{
  DevAssert(pdu != NULL);
  DevAssert(pdu->present == E1AP_E1AP_PDU_PR_initiatingMessage);
  DevAssert(pdu->choice.initiatingMessage->procedureCode == E1AP_ProcedureCode_id_bearerContextSetup);
  DevAssert(pdu->choice.initiatingMessage->criticality == E1AP_Criticality_reject);
  DevAssert(pdu->choice.initiatingMessage->value.present == E1AP_InitiatingMessage__value_PR_BearerContextSetupRequest);

  e1ap_bearer_setup_req_t bearerCxt = {0};
  extract_BEARER_CONTEXT_SETUP_REQUEST(pdu, &bearerCxt);
  process_e1_bearer_context_setup_req(e1_inst->instance, &bearerCxt);
  return 0;
}

void extract_BEARER_CONTEXT_SETUP_RESPONSE(const E1AP_E1AP_PDU_t *pdu,
                                           e1ap_bearer_setup_resp_t *bearerCxt) {
  const E1AP_BearerContextSetupResponse_t *in = &pdu->choice.successfulOutcome->value.choice.BearerContextSetupResponse;
  E1AP_BearerContextSetupResponseIEs_t *ie;

  LOG_I(E1AP, "Bearer context setup response number of IEs %d\n", in->protocolIEs.list.count);

  for (int i=0; i < in->protocolIEs.list.count; i++) {
    ie = in->protocolIEs.list.array[i];

    switch(ie->id) {
      case E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupResponseIEs__value_PR_GNB_CU_CP_UE_E1AP_ID);
        bearerCxt->gNB_cu_cp_ue_id = ie->value.choice.GNB_CU_CP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupResponseIEs__value_PR_GNB_CU_UP_UE_E1AP_ID);
        bearerCxt->gNB_cu_up_ue_id = ie->value.choice.GNB_CU_UP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_System_BearerContextSetupResponse:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextSetupResponseIEs__value_PR_System_BearerContextSetupResponse);
        DevAssert(ie->value.choice.System_BearerContextSetupResponse.present ==
                    E1AP_System_BearerContextSetupResponse_PR_nG_RAN_BearerContextSetupResponse);
        E1AP_ProtocolIE_Container_4932P22_t *msgNGRAN_list = (E1AP_ProtocolIE_Container_4932P22_t *) ie->value.choice.System_BearerContextSetupResponse.choice.nG_RAN_BearerContextSetupResponse;
        DevAssert(msgNGRAN_list->list.count == 1);
        E1AP_NG_RAN_BearerContextSetupResponse_t *msgNGRAN = msgNGRAN_list->list.array[0];
        DevAssert(msgNGRAN->id == E1AP_ProtocolIE_ID_id_PDU_Session_Resource_Setup_List);
        DevAssert(msgNGRAN->criticality == E1AP_Criticality_reject);
        DevAssert(msgNGRAN->value.present == E1AP_NG_RAN_BearerContextSetupResponse__value_PR_PDU_Session_Resource_Setup_List);
        E1AP_PDU_Session_Resource_Setup_List_t *pduSetupList = &msgNGRAN->value.choice.PDU_Session_Resource_Setup_List;
        bearerCxt->numPDUSessions = pduSetupList->list.count;

        for (int i=0; i < pduSetupList->list.count; i++) {
          pdu_session_setup_t *pduSetup = bearerCxt->pduSession + i;
          E1AP_PDU_Session_Resource_Setup_Item_t *pdu_session = pduSetupList->list.array[i];
          pduSetup->id = pdu_session->pDU_Session_ID;

          if (pdu_session->nG_DL_UP_TNL_Information.choice.gTPTunnel) {
            DevAssert(pdu_session->nG_DL_UP_TNL_Information.present == E1AP_UP_TNL_Information_PR_gTPTunnel);
            BIT_STRING_TO_TRANSPORT_LAYER_ADDRESS_IPv4(&pdu_session->nG_DL_UP_TNL_Information.choice.gTPTunnel->transportLayerAddress,
                                                       pduSetup->tlAddress);
            OCTET_STRING_TO_INT32(&pdu_session->nG_DL_UP_TNL_Information.choice.gTPTunnel->gTP_TEID,
                                  pduSetup->teId);
          }

          pduSetup->numDRBSetup = pdu_session->dRB_Setup_List_NG_RAN.list.count;
          for (int j=0; j < pdu_session->dRB_Setup_List_NG_RAN.list.count; j++) {
            DRB_nGRAN_setup_t *drbSetup = pduSetup->DRBnGRanList + j;
            E1AP_DRB_Setup_Item_NG_RAN_t *drb = pdu_session->dRB_Setup_List_NG_RAN.list.array[j];

            drbSetup->id = drb->dRB_ID;

            drbSetup->numUpParam = drb->uL_UP_Transport_Parameters.list.count;
            for (int k=0; k < drb->uL_UP_Transport_Parameters.list.count; k++) {
              up_params_t *UL_UP_param = drbSetup->UpParamList + k;
              E1AP_UP_Parameters_Item_t *in_UL_UP_param = drb->uL_UP_Transport_Parameters.list.array[k];

              DevAssert(in_UL_UP_param->uP_TNL_Information.present == E1AP_UP_TNL_Information_PR_gTPTunnel);
              E1AP_GTPTunnel_t *gTPTunnel = in_UL_UP_param->uP_TNL_Information.choice.gTPTunnel;
              if (gTPTunnel) {
                BIT_STRING_TO_TRANSPORT_LAYER_ADDRESS_IPv4(&gTPTunnel->transportLayerAddress,
                                                           UL_UP_param->tlAddress);
                OCTET_STRING_TO_INT32(&gTPTunnel->gTP_TEID,
                                      UL_UP_param->teId);
              } else {
                AssertFatal(false, "gTPTunnel information in required\n");
              }
            }
          }
        }
        break;

      // TODO: remaining IE handlers

      default:
        LOG_E(E1AP, "Handle for this IE is not implemented (or) invalid IE detected\n");
        break;
    }
  }
}

int e1apCUCP_handle_BEARER_CONTEXT_SETUP_RESPONSE(e1ap_upcp_inst_t *inst, const E1AP_E1AP_PDU_t *pdu)
{
  DevAssert(pdu->present == E1AP_E1AP_PDU_PR_successfulOutcome);
  DevAssert(pdu->choice.successfulOutcome->procedureCode == E1AP_ProcedureCode_id_bearerContextSetup);
  DevAssert(pdu->choice.successfulOutcome->criticality == E1AP_Criticality_reject);
  DevAssert(pdu->choice.successfulOutcome->value.present == E1AP_SuccessfulOutcome__value_PR_BearerContextSetupResponse);

  MessageDef *msg = itti_alloc_new_message(TASK_CUCP_E1, 0, E1AP_BEARER_CONTEXT_SETUP_RESP);
  e1ap_bearer_setup_resp_t *bearerCxt = &E1AP_BEARER_CONTEXT_SETUP_RESP(msg);
  extract_BEARER_CONTEXT_SETUP_RESPONSE(pdu, bearerCxt);
  // Fixme: instance is the NGAP instance, no good way to set it here
  instance_t instance = 0;
  itti_send_msg_to_task(TASK_RRC_GNB, instance, msg);

  return 0;
}

int e1apCUCP_handle_BEARER_CONTEXT_SETUP_FAILURE(e1ap_upcp_inst_t *inst, const E1AP_E1AP_PDU_t *pdu)
{
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

/*
  BEARER CONTEXT MODIFICATION REQUEST
*/

static int fill_BEARER_CONTEXT_MODIFICATION_REQUEST(e1ap_setup_req_t *setupReq, e1ap_bearer_setup_req_t *const bearerCxt, E1AP_E1AP_PDU_t *pdu)
{
  pdu->present = E1AP_E1AP_PDU_PR_initiatingMessage;
  asn1cCalloc(pdu->choice.initiatingMessage, msg);
  msg->procedureCode = E1AP_ProcedureCode_id_bearerContextModification;
  msg->criticality   = E1AP_Criticality_reject;
  msg->value.present = E1AP_InitiatingMessage__value_PR_BearerContextModificationRequest;
  E1AP_BearerContextModificationRequest_t *out = &pdu->choice.initiatingMessage->value.choice.BearerContextModificationRequest;
  /* mandatory */
  /* c1. gNB-CU-CP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextModificationRequestIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_BearerContextModificationRequestIEs__value_PR_GNB_CU_CP_UE_E1AP_ID;
  ieC1->value.choice.GNB_CU_CP_UE_E1AP_ID = bearerCxt->gNB_cu_cp_ue_id;
  /* mandatory */
  /* c2. gNB-CU-UP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextModificationRequestIEs_t, ieC2);
  ieC2->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID;
  ieC2->criticality                = E1AP_Criticality_reject;
  ieC2->value.present              = E1AP_BearerContextModificationRequestIEs__value_PR_GNB_CU_UP_UE_E1AP_ID;
  ieC2->value.choice.GNB_CU_UP_UE_E1AP_ID = bearerCxt->gNB_cu_cp_ue_id;
  /* optional */
  /*  */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextModificationRequestIEs_t, ieC3);
  ieC3->id            = E1AP_ProtocolIE_ID_id_System_BearerContextModificationRequest;
  ieC3->criticality   = E1AP_Criticality_reject;
  ieC3->value.present = E1AP_BearerContextModificationRequestIEs__value_PR_System_BearerContextModificationRequest;
  ieC3->value.choice.System_BearerContextModificationRequest.present = E1AP_System_BearerContextModificationRequest_PR_nG_RAN_BearerContextModificationRequest;
  E1AP_ProtocolIE_Container_4932P26_t *msgNGRAN_list = calloc(1, sizeof(E1AP_ProtocolIE_Container_4932P26_t));
  ieC3->value.choice.System_BearerContextModificationRequest.choice.nG_RAN_BearerContextModificationRequest = (struct E1AP_ProtocolIE_Container *) msgNGRAN_list;
  asn1cSequenceAdd(msgNGRAN_list->list, E1AP_NG_RAN_BearerContextModificationRequest_t, msgNGRAN);
  msgNGRAN->id = E1AP_ProtocolIE_ID_id_PDU_Session_Resource_To_Modify_List;
  msgNGRAN->criticality = E1AP_Criticality_reject;
  msgNGRAN->value.present = E1AP_NG_RAN_BearerContextModificationRequest__value_PR_PDU_Session_Resource_To_Modify_List;
  E1AP_PDU_Session_Resource_To_Modify_List_t *pdu2Setup = &msgNGRAN->value.choice.PDU_Session_Resource_To_Modify_List;
  for(pdu_session_to_setup_t *i=bearerCxt->pduSessionMod; i < bearerCxt->pduSessionMod+bearerCxt->numPDUSessionsMod; i++) {
    asn1cSequenceAdd(pdu2Setup->list, E1AP_PDU_Session_Resource_To_Modify_Item_t, ieC3_1);
    ieC3_1->pDU_Session_ID = i->sessionId;

    for (DRB_nGRAN_to_setup_t *j=i->DRBnGRanModList; j < i->DRBnGRanModList+i->numDRB2Modify; j++) {
      asn1cCalloc(ieC3_1->dRB_To_Modify_List_NG_RAN, drb2Mod_List);
      asn1cSequenceAdd(drb2Mod_List->list, E1AP_DRB_To_Modify_Item_NG_RAN_t, drb2Mod);
      drb2Mod->dRB_ID = j->id;

      if (j->numDlUpParam > 0) {
        asn1cCalloc(drb2Mod->dL_UP_Parameters, DL_UP_Param_List);
        for (up_params_t *k=j->DlUpParamList; k < j->DlUpParamList+j->numDlUpParam; k++) {
          asn1cSequenceAdd(DL_UP_Param_List->list, E1AP_UP_Parameters_Item_t, DL_UP_Param);
          DL_UP_Param->uP_TNL_Information.present = E1AP_UP_TNL_Information_PR_gTPTunnel;
          asn1cCalloc(DL_UP_Param->uP_TNL_Information.choice.gTPTunnel, gTPTunnel);
          TRANSPORT_LAYER_ADDRESS_IPv4_TO_BIT_STRING(k->tlAddress, &gTPTunnel->transportLayerAddress);
          INT32_TO_OCTET_STRING(k->teId, &gTPTunnel->gTP_TEID);

          DL_UP_Param->cell_Group_ID = k->cell_group_id;
        }
      }
    }
  }
  return 0;
}

static void e1apCUCP_send_BEARER_CONTEXT_MODIFICATION_REQUEST(instance_t inst, e1ap_bearer_setup_req_t *const bearerCxt)
{
  AssertFatal(getCxtE1(inst), "");
  e1ap_setup_req_t *setupReq = &getCxtE1(inst)->setupReq;
  E1AP_E1AP_PDU_t pdu = {0};
  fill_BEARER_CONTEXT_MODIFICATION_REQUEST(setupReq, bearerCxt, &pdu);
  e1ap_encode_send(CPtype, setupReq, &pdu, 0, __func__);
}

int e1apCUUP_send_BEARER_CONTEXT_MODIFICATION_RESPONSE(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUUP_send_BEARER_CONTEXT_MODIFICATION_FAILURE(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

void extract_BEARER_CONTEXT_MODIFICATION_REQUEST(const E1AP_E1AP_PDU_t *pdu,
                                                 e1ap_bearer_setup_req_t *bearerCxt) {
  const E1AP_BearerContextModificationRequest_t *in = &pdu->choice.initiatingMessage->value.choice.BearerContextModificationRequest;
  E1AP_BearerContextModificationRequestIEs_t *ie;

  LOG_I(E1AP, "Bearer context setup number of IEs %d\n", in->protocolIEs.list.count);

  for (int i=0; i < in->protocolIEs.list.count; i++) {
    ie = in->protocolIEs.list.array[i];

    switch(ie->id) {
      case E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextModificationRequestIEs__value_PR_GNB_CU_CP_UE_E1AP_ID);
        bearerCxt->gNB_cu_cp_ue_id = ie->value.choice.GNB_CU_CP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextModificationRequestIEs__value_PR_GNB_CU_UP_UE_E1AP_ID);
        bearerCxt->gNB_cu_up_ue_id = ie->value.choice.GNB_CU_UP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_System_BearerContextModificationRequest:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextModificationRequestIEs__value_PR_System_BearerContextModificationRequest);
        DevAssert(ie->value.choice.System_BearerContextModificationRequest.present ==
                    E1AP_System_BearerContextModificationRequest_PR_nG_RAN_BearerContextModificationRequest);
        DevAssert(ie->value.choice.System_BearerContextModificationRequest.choice.nG_RAN_BearerContextModificationRequest != NULL);
        E1AP_ProtocolIE_Container_4932P26_t *msgNGRAN_list = (E1AP_ProtocolIE_Container_4932P26_t *) ie->value.choice.System_BearerContextModificationRequest.choice.nG_RAN_BearerContextModificationRequest;
        E1AP_NG_RAN_BearerContextModificationRequest_t *msgNGRAN = msgNGRAN_list->list.array[0];
        DevAssert(msgNGRAN_list->list.count == 1);
        DevAssert(msgNGRAN->id == E1AP_ProtocolIE_ID_id_PDU_Session_Resource_To_Modify_List);
        DevAssert(msgNGRAN->value.present =
                    E1AP_NG_RAN_BearerContextModificationRequest__value_PR_PDU_Session_Resource_To_Modify_List);

        E1AP_PDU_Session_Resource_To_Modify_List_t *pdu2ModList = &msgNGRAN->value.choice.PDU_Session_Resource_To_Modify_List;
        bearerCxt->numPDUSessionsMod = pdu2ModList->list.count;
        for (int i=0; i < pdu2ModList->list.count; i++) {
          pdu_session_to_setup_t *pdu_session = bearerCxt->pduSessionMod + i;
          E1AP_PDU_Session_Resource_To_Modify_Item_t *pdu2Mod = pdu2ModList->list.array[i];

          pdu_session->sessionId = pdu2Mod->pDU_Session_ID;

          E1AP_DRB_To_Modify_List_NG_RAN_t *drb2ModList = pdu2Mod->dRB_To_Modify_List_NG_RAN;
          pdu_session->numDRB2Modify = drb2ModList->list.count;
          for (int j=0; j < drb2ModList->list.count; j++) {
            DRB_nGRAN_to_setup_t *drb = pdu_session->DRBnGRanModList + j;
            E1AP_DRB_To_Modify_Item_NG_RAN_t *drb2Mod = drb2ModList->list.array[j];

            drb->id = drb2Mod->dRB_ID;

            E1AP_UP_Parameters_t *dl_up_paramList = drb2Mod->dL_UP_Parameters;
            drb->numDlUpParam = dl_up_paramList->list.count;
            for (int k=0; k < dl_up_paramList->list.count; k++) {
              up_params_t *dl_up_param = drb->DlUpParamList + k;
              E1AP_UP_Parameters_Item_t *dl_up_param_in = dl_up_paramList->list.array[k]; 
              if (dl_up_param_in->uP_TNL_Information.choice.gTPTunnel) { // Optional IE
                DevAssert(dl_up_param_in->uP_TNL_Information.present = E1AP_UP_TNL_Information_PR_gTPTunnel);
                BIT_STRING_TO_TRANSPORT_LAYER_ADDRESS_IPv4(&dl_up_param_in->uP_TNL_Information.choice.gTPTunnel->transportLayerAddress,
                                                           dl_up_param->tlAddress);
                OCTET_STRING_TO_INT32(&dl_up_param_in->uP_TNL_Information.choice.gTPTunnel->gTP_TEID, dl_up_param->teId);
              } else {
                AssertFatal(false, "gTPTunnel IE is missing. It is mandatory at this point\n");
              }
              dl_up_param->cell_group_id = dl_up_param_in->cell_Group_ID;
            }
          }
        }
        break;

      default:
        LOG_E(E1AP, "Handle for this IE is not implemented (or) invalid IE detected\n");
        break;
    }
  }
}

int e1apCUUP_handle_BEARER_CONTEXT_MODIFICATION_REQUEST(e1ap_upcp_inst_t *e1_inst, const E1AP_E1AP_PDU_t *pdu)
{
  DevAssert(pdu != NULL);
  DevAssert(pdu->present == E1AP_E1AP_PDU_PR_initiatingMessage);
  DevAssert(pdu->choice.initiatingMessage->procedureCode == E1AP_ProcedureCode_id_bearerContextModification);
  DevAssert(pdu->choice.initiatingMessage->criticality == E1AP_Criticality_reject);
  DevAssert(pdu->choice.initiatingMessage->value.present == E1AP_InitiatingMessage__value_PR_BearerContextModificationRequest);

  e1ap_bearer_setup_req_t bearerCxt = {0};
  extract_BEARER_CONTEXT_MODIFICATION_REQUEST(pdu, &bearerCxt);
  CUUP_process_bearer_context_mod_req(e1_inst->instance, &bearerCxt);
  return 0;
}

int e1apCUCP_handle_BEARER_CONTEXT_MODIFICATION_RESPONSE(instance_t instance,
                                                         uint32_t assoc_id,
                                                         uint32_t stream,
                                                         E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_handle_BEARER_CONTEXT_MODIFICATION_FAILURE(instance_t instance,
                                                        uint32_t assoc_id,
                                                        uint32_t stream,
                                                        E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUUP_send_BEARER_CONTEXT_MODIFICATION_REQUIRED(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_send_BEARER_CONTEXT_MODIFICATION_CONFIRM(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_handle_BEARER_CONTEXT_MODIFICATION_REQUIRED(instance_t instance,
                                                         uint32_t assoc_id,
                                                         uint32_t stream,
                                                         E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUUP_handle_BEARER_CONTEXT_MODIFICATION_CONFIRM(instance_t instance,
                                                        uint32_t assoc_id,
                                                        uint32_t stream,
                                                        E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}
/*
  BEARER CONTEXT RELEASE
*/

static int fill_BEARER_CONTEXT_RELEASE_COMMAND(e1ap_setup_req_t *setupReq, e1ap_bearer_release_cmd_t *const cmd, E1AP_E1AP_PDU_t *pdu)
{
  pdu->present = E1AP_E1AP_PDU_PR_initiatingMessage;
  asn1cCalloc(pdu->choice.initiatingMessage, msg);
  msg->procedureCode = E1AP_ProcedureCode_id_bearerContextRelease;
  msg->criticality   = E1AP_Criticality_reject;
  msg->value.present = E1AP_InitiatingMessage__value_PR_BearerContextReleaseCommand;
  E1AP_BearerContextReleaseCommand_t *out = &pdu->choice.initiatingMessage->value.choice.BearerContextReleaseCommand;
  /* mandatory */
  /* c1. gNB-CU-CP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextReleaseCommandIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_BearerContextReleaseCommandIEs__value_PR_GNB_CU_CP_UE_E1AP_ID;
  ieC1->value.choice.GNB_CU_CP_UE_E1AP_ID = cmd->gNB_cu_cp_ue_id;
  /* mandatory */
  /* c2. gNB-CU-UP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextReleaseCommandIEs_t, ieC2);
  ieC2->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID;
  ieC2->criticality                = E1AP_Criticality_reject;
  ieC2->value.present              = E1AP_BearerContextReleaseCommandIEs__value_PR_GNB_CU_UP_UE_E1AP_ID;
  ieC2->value.choice.GNB_CU_UP_UE_E1AP_ID = cmd->gNB_cu_cp_ue_id;

  return 0;
}

int e1apCUCP_send_BEARER_CONTEXT_RELEASE_COMMAND(e1ap_setup_req_t *setupReq, e1ap_bearer_release_cmd_t *const cmd)
{
  E1AP_E1AP_PDU_t pdu = {0};
  fill_BEARER_CONTEXT_RELEASE_COMMAND(setupReq, cmd, &pdu);
  return e1ap_encode_send(CPtype, setupReq, &pdu, 0, __func__);
}

int fill_BEARER_CONTEXT_RELEASE_COMPLETE(e1ap_setup_req_t *setupReq, e1ap_bearer_release_cmd_t *const cmd, E1AP_E1AP_PDU_t *pdu)
{
  pdu->present = E1AP_E1AP_PDU_PR_successfulOutcome;
  asn1cCalloc(pdu->choice.successfulOutcome, msg);
  msg->procedureCode = E1AP_ProcedureCode_id_bearerContextRelease;
  msg->criticality   = E1AP_Criticality_reject;
  msg->value.present = E1AP_SuccessfulOutcome__value_PR_BearerContextReleaseComplete;
  E1AP_BearerContextReleaseComplete_t *out = &pdu->choice.successfulOutcome->value.choice.BearerContextReleaseComplete;
  /* mandatory */
  /* c1. gNB-CU-CP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextReleaseCompleteIEs_t, ieC1);
  ieC1->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID;
  ieC1->criticality                = E1AP_Criticality_reject;
  ieC1->value.present              = E1AP_BearerContextReleaseCompleteIEs__value_PR_GNB_CU_CP_UE_E1AP_ID;
  ieC1->value.choice.GNB_CU_CP_UE_E1AP_ID = cmd->gNB_cu_cp_ue_id;
  /* mandatory */
  /* c2. gNB-CU-UP UE E1AP ID */
  asn1cSequenceAdd(out->protocolIEs.list, E1AP_BearerContextReleaseCompleteIEs_t, ieC2);
  ieC2->id                         = E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID;
  ieC2->criticality                = E1AP_Criticality_reject;
  ieC2->value.present              = E1AP_BearerContextReleaseCompleteIEs__value_PR_GNB_CU_UP_UE_E1AP_ID;
  ieC2->value.choice.GNB_CU_UP_UE_E1AP_ID = cmd->gNB_cu_cp_ue_id;

  return 0;
}

int e1apCUUP_send_BEARER_CONTEXT_RELEASE_COMPLETE(e1ap_upcp_inst_t *inst, e1ap_bearer_release_cmd_t *const cmd)
{
  E1AP_E1AP_PDU_t pdu = {0};
  fill_BEARER_CONTEXT_RELEASE_COMPLETE(&inst->setupReq, cmd, &pdu);
  return e1ap_encode_send(CPtype, &inst->setupReq, &pdu, 0, __func__);
}

int e1apCUUP_send_BEARER_CONTEXT_RELEASE_REQUEST(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

void extract_BEARER_CONTEXT_RELEASE_COMMAND(const E1AP_E1AP_PDU_t *pdu,
                                            e1ap_bearer_release_cmd_t *bearerCxt) {
  const E1AP_BearerContextReleaseCommand_t *in = &pdu->choice.initiatingMessage->value.choice.BearerContextReleaseCommand;
  E1AP_BearerContextReleaseCommandIEs_t *ie;

  LOG_I(E1AP, "Bearer context setup number of IEs %d\n", in->protocolIEs.list.count);

  for (int i=0; i < in->protocolIEs.list.count; i++) {
    ie = in->protocolIEs.list.array[i];

    switch(ie->id) {
      case E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextReleaseCommandIEs__value_PR_GNB_CU_CP_UE_E1AP_ID);
        bearerCxt->gNB_cu_cp_ue_id = ie->value.choice.GNB_CU_CP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextReleaseCommandIEs__value_PR_GNB_CU_UP_UE_E1AP_ID);
        bearerCxt->gNB_cu_up_ue_id = ie->value.choice.GNB_CU_UP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_Cause:
        DevAssert(ie->criticality == E1AP_Criticality_ignore);
        DevAssert(ie->value.present == E1AP_BearerContextReleaseCommandIEs__value_PR_Cause);
        bearerCxt->cause_type = ie->value.choice.Cause.present;
        if ((ie->value.choice.Cause.present != E1AP_Cause_PR_NOTHING) &&
            (ie->value.choice.Cause.present != E1AP_Cause_PR_choice_extension))
          bearerCxt->cause = ie->value.choice.Cause.choice.radioNetwork;

                                                 
      default:
        LOG_E(E1AP, "Handle for this IE is not implemented (or) invalid IE detected\n");
        break;
    }
  }
}

int e1apCUUP_handle_BEARER_CONTEXT_RELEASE_COMMAND(e1ap_upcp_inst_t *e1_inst, const E1AP_E1AP_PDU_t *pdu)
{
  DevAssert(pdu != NULL);
  DevAssert(pdu->present == E1AP_E1AP_PDU_PR_initiatingMessage);
  DevAssert(pdu->choice.initiatingMessage->procedureCode == E1AP_ProcedureCode_id_bearerContextRelease);
  DevAssert(pdu->choice.initiatingMessage->criticality == E1AP_Criticality_reject);
  DevAssert(pdu->choice.initiatingMessage->value.present == E1AP_InitiatingMessage__value_PR_BearerContextReleaseCommand);

  e1ap_bearer_release_cmd_t bearerCxt = {0};
  extract_BEARER_CONTEXT_RELEASE_COMMAND(pdu, &bearerCxt);
  CUUP_process_bearer_release_command(e1_inst->instance, &bearerCxt);
  return 0;
}

void extract_BEARER_CONTEXT_RELEASE_COMPLETE(const E1AP_E1AP_PDU_t *pdu,
                                             e1ap_bearer_release_cmd_t *bearerCxt) {
  const E1AP_BearerContextReleaseComplete_t *in = &pdu->choice.successfulOutcome->value.choice.BearerContextReleaseComplete;
  E1AP_BearerContextReleaseCompleteIEs_t *ie;

  LOG_I(E1AP, "Bearer context setup number of IEs %d\n", in->protocolIEs.list.count);

  for (int i=0; i < in->protocolIEs.list.count; i++) {
    ie = in->protocolIEs.list.array[i];

    switch(ie->id) {
      case E1AP_ProtocolIE_ID_id_gNB_CU_CP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextReleaseCompleteIEs__value_PR_GNB_CU_CP_UE_E1AP_ID);
        bearerCxt->gNB_cu_cp_ue_id = ie->value.choice.GNB_CU_CP_UE_E1AP_ID;
        break;

      case E1AP_ProtocolIE_ID_id_gNB_CU_UP_UE_E1AP_ID:
        DevAssert(ie->criticality == E1AP_Criticality_reject);
        DevAssert(ie->value.present == E1AP_BearerContextReleaseCompleteIEs__value_PR_GNB_CU_UP_UE_E1AP_ID);
        bearerCxt->gNB_cu_up_ue_id = ie->value.choice.GNB_CU_UP_UE_E1AP_ID;
        break;

      default:
        LOG_E(E1AP, "Handle for this IE is not implemented (or) invalid IE detected\n");
        break;
    }
  }
}

int e1apCUCP_handle_BEARER_CONTEXT_RELEASE_COMPLETE(e1ap_upcp_inst_t *e1_inst, const E1AP_E1AP_PDU_t *pdu)
{
  DevAssert(pdu != NULL);
  DevAssert(pdu->present == E1AP_E1AP_PDU_PR_successfulOutcome);
  DevAssert(pdu->choice.successfulOutcome->procedureCode == E1AP_ProcedureCode_id_bearerContextRelease);
  DevAssert(pdu->choice.successfulOutcome->criticality == E1AP_Criticality_reject);
  DevAssert(pdu->choice.successfulOutcome->value.present == E1AP_SuccessfulOutcome__value_PR_BearerContextReleaseComplete);

  e1ap_bearer_release_cmd_t bearerCxt = {0};
  extract_BEARER_CONTEXT_RELEASE_COMPLETE(pdu, &bearerCxt);
  //TODO: CUCP_process_bearer_release_complete(&beareCxt, instance);
  return 0;
}

int e1apCUCP_handle_BEARER_CONTEXT_RELEASE_REQUEST(instance_t instance,
                                                   uint32_t assoc_id,
                                                   uint32_t stream,
                                                   E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

/*
BEARER CONTEXT INACTIVITY NOTIFICATION
 */

int e1apCUUP_send_BEARER_CONTEXT_INACTIVITY_NOTIFICATION(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_handle_BEARER_CONTEXT_INACTIVITY_NOTIFICATION(instance_t instance,
                                                           uint32_t assoc_id,
                                                           uint32_t stream,
                                                           E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}
/*
  DL DATA
*/

int e1apCUUP_send_DL_DATA_NOTIFICATION(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUUP_send_DATA_USAGE_REPORT(instance_t instance) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_handle_DL_DATA_NOTIFICATION(instance_t instance,
                                         uint32_t assoc_id,
                                         uint32_t stream,
                                         E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

int e1apCUCP_handle_send_DATA_USAGE_REPORT(instance_t instance,
                                           uint32_t assoc_id,
                                           uint32_t stream,
                                           E1AP_E1AP_PDU_t *pdu) {
  AssertFatal(false,"Not implemented yet\n");
  return -1;
}

static instance_t cuup_task_create_gtpu_instance_to_du(eth_params_t *IPaddrs) {
  openAddr_t tmp= {0};
  strncpy(tmp.originHost, IPaddrs->my_addr, sizeof(tmp.originHost)-1);
  sprintf(tmp.originService, "%d",  IPaddrs->my_portd);
  return gtpv1Init(tmp);
}

static void e1_task_send_sctp_association_req(long task_id, instance_t instance, e1ap_setup_req_t *e1ap_setup_req)
{
  DevAssert(e1ap_setup_req != NULL);
  getCxtE1(instance)->sockState = SCTP_STATE_CLOSED;
  MessageDef *message_p = itti_alloc_new_message(task_id, 0, SCTP_NEW_ASSOCIATION_REQ);
  sctp_new_association_req_t *sctp_new_req = &message_p->ittiMsg.sctp_new_association_req;
  sctp_new_req->ulp_cnx_id = instance;
  sctp_new_req->port = E1AP_PORT_NUMBER;
  sctp_new_req->ppid = E1AP_SCTP_PPID;
  sctp_new_req->in_streams = e1ap_setup_req->sctp_in_streams;
  sctp_new_req->out_streams = e1ap_setup_req->sctp_out_streams;
  // remote
  sctp_new_req->remote_address = e1ap_setup_req->CUCP_e1_ip_address;
  // local
  sctp_new_req->local_address = e1ap_setup_req->CUUP_e1_ip_address;
  itti_send_msg_to_task(TASK_SCTP, instance, message_p);
}

static void e1apCUUP_send_SETUP_REQUEST(e1ap_setup_req_t *setup)
{
  E1AP_E1AP_PDU_t pdu = {0};
  fill_SETUP_REQUEST(setup, &pdu);
  e1ap_encode_send(UPtype, setup, &pdu, 0, __func__);
}

static void e1_task_handle_sctp_association_resp(E1_t type, instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp)
{
  DevAssert(sctp_new_association_resp != NULL);
  getCxtE1(instance)->sockState = sctp_new_association_resp->sctp_state;
  if (sctp_new_association_resp->sctp_state != SCTP_STATE_ESTABLISHED) {
    LOG_W(E1AP, "Received unsuccessful result for SCTP association (%u), instance %ld, cnx_id %u\n",
          sctp_new_association_resp->sctp_state,
          instance,
          sctp_new_association_resp->ulp_cnx_id);
    long timer_id; // if we want to cancel timer
    timer_setup(1, 0, TASK_CUUP_E1, 0, TIMER_ONE_SHOT, NULL, &timer_id);
    return;
  }

  if (type == UPtype) {
    e1ap_setup_req_t *e1ap_cuup_setup_req = &getCxtE1(instance)->setupReq;
    e1ap_cuup_setup_req->assoc_id = sctp_new_association_resp->assoc_id;
    e1ap_cuup_setup_req->sctp_in_streams = sctp_new_association_resp->in_streams;
    e1ap_cuup_setup_req->sctp_out_streams = sctp_new_association_resp->out_streams;
    e1ap_cuup_setup_req->default_sctp_stream_id = 0;

    eth_params_t IPaddr;
    IPaddr.my_addr = e1ap_cuup_setup_req->localAddressF1U;
    IPaddr.my_portd = e1ap_cuup_setup_req->localPortF1U;
    if (getCxtE1(instance)->gtpInstF1U < 0)
      getCxtE1(instance)->gtpInstF1U = cuup_task_create_gtpu_instance_to_du(&IPaddr);
    if (getCxtE1(instance)->gtpInstF1U < 0)
      LOG_E(E1AP, "Failed to create CUUP F1-U UDP listener");
    extern instance_t CUuniqInstance;
    CUuniqInstance = getCxtE1(instance)->gtpInstF1U;
    cuup_init_n3(instance);
    e1apCUUP_send_SETUP_REQUEST(&getCxtE1(instance)->setupReq);
  }
}

void cuup_init_n3(instance_t instance)
{
  if (getCxtE1(instance)->gtpInstN3 < 0) {
    e1ap_setup_req_t *setup = &getCxtE1(instance)->setupReq;
    openAddr_t tmp = {0};
    strcpy(tmp.originHost, setup->localAddressN3);
    sprintf(tmp.originService, "%d", setup->localPortN3);
    sprintf(tmp.destinationService, "%d", setup->remotePortN3);
    LOG_I(GTPU, "Configuring GTPu address : %s, port : %s\n", tmp.originHost, tmp.originService);
    // Fixme: fully inconsistent instances management
    // dirty global var is a bad fix
    extern instance_t legacyInstanceMapping;
    legacyInstanceMapping = getCxtE1(instance)->gtpInstN3 = gtpv1Init(tmp);
  }
  if (getCxtE1(instance)->gtpInstN3 < 0)
    LOG_E(E1AP, "Failed to create CUUP N3 UDP listener");
  extern instance_t *N3GTPUInst;
  N3GTPUInst = &getCxtE1(instance)->gtpInstN3;
}

void cucp_task_send_sctp_init_req(instance_t instance, char *my_addr) {
  LOG_I(E1AP, "E1AP_CUCP_SCTP_REQ(create socket)\n");
  MessageDef  *message_p = NULL;
  message_p = itti_alloc_new_message (TASK_CUCP_E1, 0, SCTP_INIT_MSG);
  message_p->ittiMsg.sctp_init.port = E1AP_PORT_NUMBER;
  message_p->ittiMsg.sctp_init.ppid = E1AP_SCTP_PPID;
  message_p->ittiMsg.sctp_init.ipv4 = 1;
  message_p->ittiMsg.sctp_init.ipv6 = 0;
  message_p->ittiMsg.sctp_init.nb_ipv4_addr = 1;
  message_p->ittiMsg.sctp_init.ipv4_address[0] = inet_addr(my_addr);
  /*
   * SR WARNING: ipv6 multi-homing fails sometimes for localhost.
   * * * * Disable it for now.
   */
  message_p->ittiMsg.sctp_init.nb_ipv6_addr = 0;
  message_p->ittiMsg.sctp_init.ipv6_address[0] = "0:0:0:0:0:0:0:1";
  itti_send_msg_to_task(TASK_SCTP, instance, message_p);
}

void e1_task_handle_sctp_association_ind(E1_t type, instance_t instance, sctp_new_association_ind_t *sctp_new_ind)
{
  if (getCxtE1(instance))
    LOG_W(E1AP, "CUCP incoming call, re-use older socket context, finish implementation required\n");
  else
    createE1inst(type, instance, NULL);
  getCxtE1(instance)->sockState = SCTP_STATE_ESTABLISHED;
  getCxtE1(instance)->incoming_sock = true;
  e1ap_setup_req_t *setup_req = &getCxtE1(instance)->setupReq;
  setup_req->assoc_id = sctp_new_ind->assoc_id;
  setup_req->sctp_in_streams = sctp_new_ind->in_streams;
  setup_req->sctp_out_streams = sctp_new_ind->out_streams;
  setup_req->default_sctp_stream_id = 0;
}

void e1apHandleTimer(instance_t myInstance)
{
  LOG_W(E1AP, "Try to reconnect to CP\n");
  if (getCxtE1(myInstance)->sockState != SCTP_STATE_ESTABLISHED)
    e1_task_send_sctp_association_req(TASK_CUUP_E1, myInstance, &getCxtE1(myInstance)->setupReq);
}

void *E1AP_CUCP_task(void *arg) {
  LOG_I(E1AP, "Starting E1AP at CU CP\n");
  MessageDef *msg = NULL;
  e1ap_common_init();
  int result;

  while (1) {
    itti_receive_msg(TASK_CUCP_E1, &msg);
    instance_t myInstance=ITTI_MSG_DESTINATION_INSTANCE(msg);
    const int msgType = ITTI_MSG_ID(msg);
    LOG_I(E1AP, "CUCP received %s for instance %ld\n", messages_info[msgType].name, myInstance);

    switch (ITTI_MSG_ID(msg)) {
      case SCTP_NEW_ASSOCIATION_IND:
        e1_task_handle_sctp_association_ind(CPtype, ITTI_MSG_ORIGIN_INSTANCE(msg), &msg->ittiMsg.sctp_new_association_ind);
        break;

      case SCTP_NEW_ASSOCIATION_RESP:
        e1_task_handle_sctp_association_resp(CPtype, ITTI_MSG_ORIGIN_INSTANCE(msg), &msg->ittiMsg.sctp_new_association_resp);
        break;

      case E1AP_SETUP_REQ: {
        e1ap_setup_req_t *req = &E1AP_SETUP_REQ(msg);
        if (req->CUCP_e1_ip_address.ipv4 == 0) {
          LOG_E(E1AP, "No IPv4 address configured\n");
          return NULL;
        }
        cucp_task_send_sctp_init_req(0, req->CUCP_e1_ip_address.ipv4_address);
      } break;

      case SCTP_DATA_IND:
        e1_task_handle_sctp_data_ind(myInstance, &msg->ittiMsg.sctp_data_ind);
        break;

      case E1AP_SETUP_RESP:
        e1ap_send_SETUP_RESPONSE(myInstance, &E1AP_SETUP_RESP(msg));
        break;

      case E1AP_BEARER_CONTEXT_SETUP_REQ:
        e1apCUCP_send_BEARER_CONTEXT_SETUP_REQUEST(myInstance, &E1AP_BEARER_CONTEXT_SETUP_REQ(msg));
        break;

      case E1AP_BEARER_CONTEXT_MODIFICATION_REQ:
        e1apCUCP_send_BEARER_CONTEXT_MODIFICATION_REQUEST(myInstance, &E1AP_BEARER_CONTEXT_SETUP_REQ(msg));
        break;

      default:
        LOG_E(E1AP, "Unknown message received in TASK_CUCP_E1\n");
        break;
    }

    result = itti_free(ITTI_MSG_ORIGIN_ID(msg), msg);
    AssertFatal(result == EXIT_SUCCESS, "Failed to free memory (%d) in E1AP_CUCP_task!\n", result);
    msg = NULL;

  }
}

void *E1AP_CUUP_task(void *arg) {
  LOG_I(E1AP, "Starting E1AP at CU UP\n");
  e1ap_common_init();
  int result;

  // SCTP
  while (1) {
    MessageDef *msg = NULL;
    itti_receive_msg(TASK_CUUP_E1, &msg);
    const instance_t myInstance = ITTI_MSG_DESTINATION_INSTANCE(msg);
    const int msgType = ITTI_MSG_ID(msg);
    LOG_I(E1AP, "CUUP received %s for instance %ld\n", messages_info[msgType].name, myInstance);
    switch (msgType) {
      case E1AP_SETUP_REQ: {
        e1ap_setup_req_t *msgSetup = &E1AP_SETUP_REQ(msg);
        createE1inst(UPtype, myInstance, msgSetup);

        e1_task_send_sctp_association_req(TASK_CUUP_E1, myInstance, msgSetup);
      } break;

      case SCTP_NEW_ASSOCIATION_RESP:
        e1_task_handle_sctp_association_resp(UPtype, myInstance, &msg->ittiMsg.sctp_new_association_resp);
        break;

      case SCTP_DATA_IND:
        e1_task_handle_sctp_data_ind(myInstance, &msg->ittiMsg.sctp_data_ind);
        break;

      case TIMER_HAS_EXPIRED:
        e1apHandleTimer(myInstance);
        break;

      default:
        LOG_E(E1AP, "Unknown message received in TASK_CUUP_E1\n");
        break;
    }

    result = itti_free(ITTI_MSG_ORIGIN_ID(msg), msg);
    AssertFatal(result == EXIT_SUCCESS, "Failed to free memory (%d) in E1AP_CUUP_task!\n", result);
    msg = NULL;

  }
}

