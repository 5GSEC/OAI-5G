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

/*! \file openair2/F1AP/f1ap_du_task.c
* \brief data structures for F1 interface modules
* \author EURECOM/NTUST
* \date 2018
* \version 0.1
* \company Eurecom
* \email: navid.nikaein@eurecom.fr, raymond.knopp@eurecom.fr, bing-kai.hong@eurecom.fr
* \note
* \warning
*/

#include "f1ap_common.h"
#include "f1ap_du_interface_management.h"
#include "f1ap_du_ue_context_management.h"
#include "f1ap_du_rrc_message_transfer.h"
#include "f1ap_du_task.h"
#include <openair3/ocp-gtpu/gtp_itf.h>

//Fixme: Uniq dirty DU instance, by global var, datamodel need better management
instance_t DUuniqInstance=0;

void du_task_send_sctp_association_req(instance_t instance, f1ap_setup_req_t *f1ap_setup_req) {
  DevAssert(f1ap_setup_req != NULL);
  MessageDef                 *message_p                   = NULL;
  sctp_new_association_req_t *sctp_new_association_req_p  = NULL;
  message_p = itti_alloc_new_message(TASK_DU_F1, 0, SCTP_NEW_ASSOCIATION_REQ);
  sctp_new_association_req_p = &message_p->ittiMsg.sctp_new_association_req;
  sctp_new_association_req_p->ulp_cnx_id = instance;
  sctp_new_association_req_p->port = F1AP_PORT_NUMBER;
  sctp_new_association_req_p->ppid = F1AP_SCTP_PPID;
  sctp_new_association_req_p->in_streams  = f1ap_setup_req->sctp_in_streams;
  sctp_new_association_req_p->out_streams = f1ap_setup_req->sctp_out_streams;
  // remote
  memcpy(&sctp_new_association_req_p->remote_address,
         &f1ap_setup_req->CU_f1_ip_address,
         sizeof(f1ap_setup_req->CU_f1_ip_address));
  // local
  memcpy(&sctp_new_association_req_p->local_address,
         &f1ap_setup_req->DU_f1_ip_address,
         sizeof(f1ap_setup_req->DU_f1_ip_address));
  //printf("sib itti message %s\n", f1ap_setup_req_t->sib1[0]);
  //printf("nr_cellid : %llx (%lld)",f1ap_setup_req->nr_cellid[0],f1ap_setup_req->nr_cellid[0]);
  //du_f1ap_register_to_sctp
  itti_send_msg_to_task(TASK_SCTP, instance, message_p);
}

void du_task_handle_sctp_association_resp(instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp) {
  DevAssert(sctp_new_association_resp != NULL);

  if (sctp_new_association_resp->sctp_state != SCTP_STATE_ESTABLISHED) {
    LOG_W(F1AP, "Received unsuccessful result for SCTP association (%u), instance %ld, cnx_id %u\n",
          sctp_new_association_resp->sctp_state,
          instance,
          sctp_new_association_resp->ulp_cnx_id);
    //f1ap_handle_setup_message(instance, sctp_new_association_resp->sctp_state == SCTP_STATE_SHUTDOWN);
    return; // exit -1 for debugging
  }

  // save the assoc id
  f1ap_setup_req_t *f1ap_du_data=f1ap_req(false, instance);
  f1ap_du_data->assoc_id         = sctp_new_association_resp->assoc_id;
  f1ap_du_data->sctp_in_streams  = sctp_new_association_resp->in_streams;
  f1ap_du_data->sctp_out_streams = sctp_new_association_resp->out_streams;
  f1ap_du_data->default_sctp_stream_id = 0;
  /* setup parameters for F1U and start the server */
  DU_send_F1_SETUP_REQUEST(instance);
}

void du_task_handle_sctp_data_ind(instance_t instance, sctp_data_ind_t *sctp_data_ind) {
  int result;
  DevAssert(sctp_data_ind != NULL);
  f1ap_handle_message(instance, sctp_data_ind->assoc_id, sctp_data_ind->stream,
                      sctp_data_ind->buffer, sctp_data_ind->buffer_length);
  result = itti_free(TASK_UNKNOWN, sctp_data_ind->buffer);
  AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
}

void *F1AP_DU_task(void *arg) {
  //sctp_cu_init();
  LOG_I(F1AP, "Starting F1AP at DU\n");
  itti_mark_task_ready(TASK_DU_F1);

  // SCTP
  while (1) {
    MessageDef *msg = NULL;
    itti_receive_msg(TASK_DU_F1, &msg);
    instance_t myInstance=ITTI_MSG_DESTINATION_INSTANCE(msg);
    LOG_D(F1AP, "DU Task Received %s for instance %ld\n",
          ITTI_MSG_NAME(msg),myInstance);
    switch (ITTI_MSG_ID(msg)) {
      case F1AP_SETUP_REQ: {
        // this is not a true F1 message, but rather an ITTI message sent by gnb_app
        // 1. save the itti msg so that you can use it to sen f1ap_setup_req, fill the f1ap_setup_req message,
        // 2. store the message in f1ap context, that is also stored in RC
        // 2. send a sctp_association req
        f1ap_setup_req_t *msgSetup = &F1AP_SETUP_REQ(msg);
        createF1inst(false, myInstance, msgSetup);
        du_task_send_sctp_association_req(myInstance, msgSetup);
      } break;

      case F1AP_GNB_CU_CONFIGURATION_UPDATE_ACKNOWLEDGE:
        DU_send_gNB_CU_CONFIGURATION_UPDATE_ACKNOWLEDGE(ITTI_MSG_ORIGIN_INSTANCE(msg),
            &F1AP_GNB_CU_CONFIGURATION_UPDATE_ACKNOWLEDGE(msg));
        break;

      case F1AP_GNB_CU_CONFIGURATION_UPDATE_FAILURE:
        DU_send_gNB_CU_CONFIGURATION_UPDATE_FAILURE(myInstance,
            &F1AP_GNB_CU_CONFIGURATION_UPDATE_FAILURE(msg));
        break;

      case SCTP_NEW_ASSOCIATION_RESP:
        // 1. store the respon
        // 2. send the f1setup_req
        du_task_handle_sctp_association_resp(myInstance,
                                             &msg->ittiMsg.sctp_new_association_resp);
        break;

      case SCTP_DATA_IND:
        // ex: any F1 incoming message for DU ends here
        du_task_handle_sctp_data_ind(myInstance,
                                     &msg->ittiMsg.sctp_data_ind);
        break;

      case F1AP_INITIAL_UL_RRC_MESSAGE: // from rrc
      {
        f1ap_initial_ul_rrc_message_t *msgRrc = &F1AP_INITIAL_UL_RRC_MESSAGE(msg);
        DU_send_INITIAL_UL_RRC_MESSAGE_TRANSFER(myInstance, msgRrc);
      } break;

      case F1AP_UL_RRC_MESSAGE: // to rrc
        DU_send_UL_NR_RRC_MESSAGE_TRANSFER(myInstance, &F1AP_UL_RRC_MESSAGE(msg));
        break;

      case F1AP_UE_CONTEXT_SETUP_RESP:
        DU_send_UE_CONTEXT_SETUP_RESPONSE(myInstance, &F1AP_UE_CONTEXT_SETUP_RESP(msg));
        break;

      case F1AP_UE_CONTEXT_MODIFICATION_RESP:
        DU_send_UE_CONTEXT_MODIFICATION_RESPONSE(myInstance, &F1AP_UE_CONTEXT_MODIFICATION_RESP(msg));
        break;

      case F1AP_UE_CONTEXT_RELEASE_REQ: // from MAC
        DU_send_UE_CONTEXT_RELEASE_REQUEST(myInstance,
                                           &F1AP_UE_CONTEXT_RELEASE_REQ(msg));
        break;

      case F1AP_UE_CONTEXT_RELEASE_COMPLETE:
        DU_send_UE_CONTEXT_RELEASE_COMPLETE(myInstance, &F1AP_UE_CONTEXT_RELEASE_COMPLETE(msg));
        break;

      case F1AP_UE_CONTEXT_MODIFICATION_REQUIRED:
        DU_send_UE_CONTEXT_MODIFICATION_REQUIRED(myInstance, &F1AP_UE_CONTEXT_MODIFICATION_REQUIRED(msg));
        break;

      case TERMINATE_MESSAGE:
        LOG_W(F1AP, " *** Exiting F1AP thread\n");
        itti_exit_task();
        break;

      default:
        LOG_E(F1AP, "DU Received unhandled message: %d:%s\n",
              ITTI_MSG_ID(msg), ITTI_MSG_NAME(msg));
        break;
    } // switch

    int result = itti_free (ITTI_MSG_ORIGIN_ID(msg), msg);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
  } // while

  return NULL;
}
