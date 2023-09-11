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

/*! \file m3ap_MCE.c
 * \brief m3ap tasks for MCE
 * \author Javier Morgade  <javier.morgade@ieee.org>
 * \date 2019
 * \version 0.1
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "intertask_interface.h"

#include "m3ap_MCE.h"
#include "m3ap_MCE_defs.h"
#include "m3ap_MCE_management_procedures.h"
#include "m3ap_MCE_handler.h"
//#include "m3ap_MCE_generate_messages.h"
#include "m3ap_common.h"
#include "m3ap_MCE_interface_management.h"
#include "m3ap_ids.h"
#include "m3ap_timers.h"

#include "queue.h"
#include "assertions.h"
#include "conversions.h"

struct m3ap_mce_map;
struct m3ap_MCE_data_s;

m3ap_setup_req_t * m3ap_mce_data_g;

RB_PROTOTYPE(m3ap_mce_map, m3ap_MCE_data_s, entry, m3ap_MCE_compare_assoc_id);

static
void m3ap_MCE_handle_sctp_data_ind(instance_t instance, sctp_data_ind_t *sctp_data_ind);

static
void m3ap_MCE_handle_sctp_association_resp(instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp);

static
void m3ap_MCE_handle_sctp_association_ind(instance_t instance, sctp_new_association_ind_t *sctp_new_association_ind);

static
void m3ap_MCE_handle_register_MCE(instance_t instance,
                                  m3ap_register_mce_req_t *m3ap_register_MME);
static
void m3ap_MCE_register_MCE(m3ap_MCE_instance_t *instance_p,
                           net_ip_address_t    *target_MME_ip_addr,
                           net_ip_address_t    *local_ip_addr,
                           uint16_t             in_streams,
                           uint16_t             out_streams,
                           uint32_t             mce_port_for_M3C,
                           int                  multi_sd);

//static
//void m3ap_eNB_handle_handover_req(instance_t instance,
//                                  m3ap_handover_req_t *m3ap_handover_req);
//
//static
//void m3ap_eNB_handle_handover_req_ack(instance_t instance,
//                                      m3ap_handover_req_ack_t *m3ap_handover_req_ack);
//
//static
//void m3ap_eNB_ue_context_release(instance_t instance,
//                                 m3ap_ue_context_release_t *m3ap_ue_context_release);
//

static
void m3ap_MCE_handle_sctp_data_ind(instance_t instance, sctp_data_ind_t *sctp_data_ind) {
  int result;
  DevAssert(sctp_data_ind != NULL);
  m3ap_MCE_handle_message(instance, sctp_data_ind->assoc_id, sctp_data_ind->stream,
                          sctp_data_ind->buffer, sctp_data_ind->buffer_length);
  result = itti_free(TASK_UNKNOWN, sctp_data_ind->buffer);
  AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
}

static
void m3ap_MCE_handle_sctp_association_resp(instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp) {
  m3ap_MCE_instance_t *instance_p;
  m3ap_MCE_data_t *m3ap_mce_data_p;
  DevAssert(sctp_new_association_resp != NULL);
  //printf("m3ap_eNB_handle_sctp_association_resp at 1\n");
  //dump_trees();
  instance_p = m3ap_MCE_get_instance(instance);
  DevAssert(instance_p != NULL);

  /* if the assoc_id is already known, it is certainly because an IND was received
   * before. In this case, just update streams and return
   */
  if (sctp_new_association_resp->assoc_id != -1) {
    m3ap_mce_data_p = m3ap_get_MCE(instance_p, sctp_new_association_resp->assoc_id,
                                   sctp_new_association_resp->ulp_cnx_id);

    if (m3ap_mce_data_p != NULL) {
      /* some sanity check - to be refined at some point */
      if (sctp_new_association_resp->sctp_state != SCTP_STATE_ESTABLISHED) {
        M3AP_ERROR("m3ap_mce_data_p not NULL and sctp state not SCTP_STATE_ESTABLISHED, what to do?\n");
        abort();
      }

      m3ap_mce_data_p->in_streams  = sctp_new_association_resp->in_streams;
      m3ap_mce_data_p->out_streams = sctp_new_association_resp->out_streams;
      return;
    }
  }

  m3ap_mce_data_p = m3ap_get_MCE(instance_p, -1,
                                 sctp_new_association_resp->ulp_cnx_id);
  DevAssert(m3ap_mce_data_p != NULL);
  //printf("m3ap_MCE_handle_sctp_association_resp at 2\n");
  //dump_trees_m3();

  if (sctp_new_association_resp->sctp_state != SCTP_STATE_ESTABLISHED) {
    M3AP_WARN("Received unsuccessful result for SCTP association (%u), instance %ld, cnx_id %u\n",
              sctp_new_association_resp->sctp_state,
              instance,
              sctp_new_association_resp->ulp_cnx_id);
    //m3ap_handle_m3_setup_message(instance_p, m3ap_enb_data_p,
                                 //sctp_new_association_resp->sctp_state == SCTP_STATE_SHUTDOWN);
	  sleep(4);
	  int index;
	  /* Trying to connect to the provided list of eNB ip address */
	  for (index = 0; index < instance_p->nb_m3; index++) {
	    //M2AP_INFO("eNB[%d] eNB id %u acting as an initiator (client)\n",
	     //         instance_id, instance->eNB_id);
	    m3ap_MCE_register_MCE(instance_p,
				  &instance_p->target_mme_m3_ip_address[index],
				  &instance_p->mme_m3_ip_address,
				  instance_p->sctp_in_streams,
				  instance_p->sctp_out_streams,
				  instance_p->mce_port_for_M3C,
				  instance_p->multi_sd);
	  }
    return;
  }

  //printf("m3ap_MCE_handle_sctp_association_resp at 3\n");
  //dump_trees_m3();
  /* Update parameters */
  m3ap_mce_data_p->assoc_id    = sctp_new_association_resp->assoc_id;
  m3ap_mce_data_p->in_streams  = sctp_new_association_resp->in_streams;
  m3ap_mce_data_p->out_streams = sctp_new_association_resp->out_streams;
  //printf("m3ap_MCE_handle_sctp_association_resp at 4\n");
  //dump_trees_m3();

  m3ap_mce_data_g->assoc_id    = sctp_new_association_resp->assoc_id;
  m3ap_mce_data_g->sctp_in_streams  = sctp_new_association_resp->in_streams;
  m3ap_mce_data_g->sctp_out_streams = sctp_new_association_resp->out_streams;

  /* Prepare new m3 Setup Request */
  //m3ap_eNB_generate_m3_setup_request(instance_p, m3ap_enb_data_p);
  MCE_send_M3_SETUP_REQUEST(instance_p, m3ap_mce_data_p);
}

static
void m3ap_MCE_handle_sctp_association_ind(instance_t instance, sctp_new_association_ind_t *sctp_new_association_ind) {
  m3ap_MCE_instance_t *instance_p;
  m3ap_MCE_data_t *m3ap_mce_data_p;
  //printf("m3ap_MCE_handle_sctp_association_ind at 1 (called for instance %d)\n", instance);
  //dump_trees_m3();
  DevAssert(sctp_new_association_ind != NULL);
  instance_p = m3ap_MCE_get_instance(instance);
  DevAssert(instance_p != NULL);
  m3ap_mce_data_p = m3ap_get_MCE(instance_p, sctp_new_association_ind->assoc_id, -1);

  if (m3ap_mce_data_p != NULL) abort();

  //  DevAssert(m3ap_mce_data_p != NULL);
  if (m3ap_mce_data_p == NULL) {
    /* Create new MCE descriptor */
    m3ap_mce_data_p = calloc(1, sizeof(*m3ap_mce_data_p));
    DevAssert(m3ap_mce_data_p != NULL);
    m3ap_mce_data_p->cnx_id                = m3ap_MCE_fetch_add_global_cnx_id();
    m3ap_mce_data_p->m3ap_MCE_instance = instance_p;
    /* Insert the new descriptor in list of known eNB
     * but not yet associated.
     */
    RB_INSERT(m3ap_mce_map, &instance_p->m3ap_mce_head, m3ap_mce_data_p);
    m3ap_mce_data_p->state = M3AP_MCE_STATE_CONNECTED;
    instance_p->m3_target_mme_nb++;

    if (instance_p->m3_target_mme_pending_nb > 0) {
      instance_p->m3_target_mme_pending_nb--;
    }
  } else {
    M3AP_WARN("m3ap_mce_data_p already exists\n");
  }

  //printf("m3ap_MCE_handle_sctp_association_ind at 2\n");
  //dump_trees_m3();
  /* Update parameters */
  m3ap_mce_data_p->assoc_id    = sctp_new_association_ind->assoc_id;
  m3ap_mce_data_p->in_streams  = sctp_new_association_ind->in_streams;
  m3ap_mce_data_p->out_streams = sctp_new_association_ind->out_streams;
  //printf("m3ap_MCE_handle_sctp_association_ind at 3\n");
  //dump_trees_m3();
}

int m3ap_MCE_init_sctp (m3ap_MCE_instance_t *instance_p,
                        net_ip_address_t    *local_ip_addr,
                        uint32_t mce_port_for_M3C) {
  // Create and alloc new message
  MessageDef                             *message;
  sctp_init_t                            *sctp_init  = NULL;
  DevAssert(instance_p != NULL);
  DevAssert(local_ip_addr != NULL);
  message = itti_alloc_new_message (TASK_M3AP_MCE, 0, SCTP_INIT_MSG_MULTI_REQ);
  sctp_init = &message->ittiMsg.sctp_init_multi;
  sctp_init->port = mce_port_for_M3C;
  sctp_init->ppid = M3AP_SCTP_PPID;
  sctp_init->ipv4 = 1;
  sctp_init->ipv6 = 0;
  sctp_init->nb_ipv4_addr = 1;
#if 0
  memcpy(&sctp_init->ipv4_address,
         local_ip_addr,
         sizeof(*local_ip_addr));
#endif
  sctp_init->ipv4_address[0] = inet_addr(local_ip_addr->ipv4_address);
  /*
   * SR WARNING: ipv6 multi-homing fails sometimes for localhost.
   * * * * Disable it for now.
   */
  sctp_init->nb_ipv6_addr = 0;
  sctp_init->ipv6_address[0] = "0:0:0:0:0:0:0:1";
  return itti_send_msg_to_task (TASK_SCTP, instance_p->instance, message);
}

static void m3ap_MCE_register_MCE(m3ap_MCE_instance_t *instance_p,
                                  net_ip_address_t    *target_MCE_ip_address,
                                  net_ip_address_t    *local_ip_addr,
                                  uint16_t             in_streams,
                                  uint16_t             out_streams,
                                  uint32_t         mce_port_for_M3C,
                                  int                  multi_sd) {
  MessageDef                       *message                   = NULL;
  sctp_new_association_req_multi_t *sctp_new_association_req  = NULL;
  m3ap_MCE_data_t                  *m3ap_mce_data             = NULL;
  DevAssert(instance_p != NULL);
  DevAssert(target_MCE_ip_address != NULL);
  message = itti_alloc_new_message(TASK_M3AP_MCE, 0, SCTP_NEW_ASSOCIATION_REQ_MULTI);
  sctp_new_association_req = &message->ittiMsg.sctp_new_association_req_multi;
  sctp_new_association_req->port = mce_port_for_M3C;
  sctp_new_association_req->ppid = M3AP_SCTP_PPID;
  sctp_new_association_req->in_streams  = in_streams;
  sctp_new_association_req->out_streams = out_streams;
  sctp_new_association_req->multi_sd = multi_sd;
  memcpy(&sctp_new_association_req->remote_address,
         target_MCE_ip_address,
         sizeof(*target_MCE_ip_address));
  memcpy(&sctp_new_association_req->local_address,
         local_ip_addr,
         sizeof(*local_ip_addr));
  /* Create new MCE descriptor */
  m3ap_mce_data = calloc(1, sizeof(*m3ap_mce_data));
  DevAssert(m3ap_mce_data != NULL);
  m3ap_mce_data->cnx_id                = m3ap_MCE_fetch_add_global_cnx_id();
  sctp_new_association_req->ulp_cnx_id = m3ap_mce_data->cnx_id;
  m3ap_mce_data->assoc_id          = -1;
  m3ap_mce_data->m3ap_MCE_instance = instance_p;
  /* Insert the new descriptor in list of known eNB
   * but not yet associated.
   */

  m3ap_mce_data_g = (m3ap_setup_req_t*)calloc(1,sizeof(m3ap_setup_req_t));



  RB_INSERT(m3ap_mce_map, &instance_p->m3ap_mce_head, m3ap_mce_data);
  m3ap_mce_data->state = M3AP_MCE_STATE_WAITING;
  instance_p->m3_target_mme_nb ++;
  instance_p->m3_target_mme_pending_nb ++;
  itti_send_msg_to_task(TASK_SCTP, instance_p->instance, message);
}

static
void m3ap_MCE_handle_register_MCE(instance_t instance,
                                  m3ap_register_mce_req_t *m3ap_register_MCE) {
  m3ap_MCE_instance_t *new_instance;
  DevAssert(m3ap_register_MCE != NULL);
  /* Look if the provided instance already exists */
  new_instance = m3ap_MCE_get_instance(instance);

  if (new_instance != NULL) {
    /* Checks if it is a retry on the same MCE */
    DevCheck(new_instance->MCE_id == m3ap_register_MCE->MCE_id, new_instance->MCE_id, m3ap_register_MCE->MCE_id, 0);
    DevCheck(new_instance->cell_type == m3ap_register_MCE->cell_type, new_instance->cell_type, m3ap_register_MCE->cell_type, 0);
    DevCheck(new_instance->tac == m3ap_register_MCE->tac, new_instance->tac, m3ap_register_MCE->tac, 0);
    DevCheck(new_instance->mcc == m3ap_register_MCE->mcc, new_instance->mcc, m3ap_register_MCE->mcc, 0);
    DevCheck(new_instance->mnc == m3ap_register_MCE->mnc, new_instance->mnc, m3ap_register_MCE->mnc, 0);
    M3AP_WARN("MCE[%ld] already registered\n", instance);
  } else {
    new_instance = calloc(1, sizeof(m3ap_MCE_instance_t));
    DevAssert(new_instance != NULL);
    RB_INIT(&new_instance->m3ap_mce_head);
    /* Copy usefull parameters */
    new_instance->instance         = instance;
    new_instance->MCE_name         = m3ap_register_MCE->MCE_name;
    new_instance->MCE_id           = m3ap_register_MCE->MCE_id;
    new_instance->cell_type        = m3ap_register_MCE->cell_type;
    new_instance->tac              = m3ap_register_MCE->tac;
    new_instance->mcc              = m3ap_register_MCE->mcc;
    new_instance->mnc              = m3ap_register_MCE->mnc;
    new_instance->mnc_digit_length = m3ap_register_MCE->mnc_digit_length;
    new_instance->num_cc           = m3ap_register_MCE->num_cc;

    m3ap_id_manager_init(&new_instance->id_manager);
    m3ap_timers_init(&new_instance->timers,
                     m3ap_register_MCE->t_reloc_prep,
                     m3ap_register_MCE->tm3_reloc_overall);

    for (int i = 0; i< m3ap_register_MCE->num_cc; i++) {
      new_instance->eutra_band[i]              = m3ap_register_MCE->eutra_band[i];
      new_instance->downlink_frequency[i]      = m3ap_register_MCE->downlink_frequency[i];
      new_instance->uplink_frequency_offset[i] = m3ap_register_MCE->uplink_frequency_offset[i];
      new_instance->Nid_cell[i]                = m3ap_register_MCE->Nid_cell[i];
      new_instance->N_RB_DL[i]                 = m3ap_register_MCE->N_RB_DL[i];
      new_instance->frame_type[i]              = m3ap_register_MCE->frame_type[i];
      new_instance->fdd_earfcn_DL[i]           = m3ap_register_MCE->fdd_earfcn_DL[i];
      new_instance->fdd_earfcn_UL[i]           = m3ap_register_MCE->fdd_earfcn_UL[i];
    }

    DevCheck(m3ap_register_MCE->nb_m3 <= M3AP_MAX_NB_MCE_IP_ADDRESS,
             M3AP_MAX_NB_MCE_IP_ADDRESS, m3ap_register_MCE->nb_m3, 0);
    memcpy(new_instance->target_mme_m3_ip_address,
           m3ap_register_MCE->target_mme_m3_ip_address,
           m3ap_register_MCE->nb_m3 * sizeof(net_ip_address_t));
    new_instance->nb_m3             = m3ap_register_MCE->nb_m3;
    new_instance->mme_m3_ip_address = m3ap_register_MCE->mme_m3_ip_address;
    new_instance->sctp_in_streams   = m3ap_register_MCE->sctp_in_streams;
    new_instance->sctp_out_streams  = m3ap_register_MCE->sctp_out_streams;
    new_instance->mce_port_for_M3C  = m3ap_register_MCE->mme_port_for_M3C;
    /* Add the new instance to the list of MCE (meaningfull in virtual mode) */
    m3ap_MCE_insert_new_instance(new_instance);
    M3AP_INFO("Registered new MCE[%ld] and %s MCE id %u\n",
              instance,
              m3ap_register_MCE->cell_type == CELL_MACRO_ENB ? "macro" : "home",
              m3ap_register_MCE->MCE_id);

    
    printf("ipv4_address %s\n",m3ap_register_MCE->mme_m3_ip_address.ipv4_address);
    /* initiate the SCTP listener */
    if (m3ap_MCE_init_sctp(new_instance,&m3ap_register_MCE->mme_m3_ip_address,m3ap_register_MCE->mme_port_for_M3C) <  0 ) {
      M3AP_ERROR ("Error while sending SCTP_INIT_MSG to SCTP \n");
      return;
    }

    M3AP_INFO("MCE[%ld] MCE id %u acting as a listner (server)\n",
              instance, m3ap_register_MCE->MCE_id);
  }
}

static
void m3ap_MCE_handle_sctp_init_msg_multi_cnf(
  instance_t instance_id,
  sctp_init_msg_multi_cnf_t *m) {
  m3ap_MCE_instance_t *instance;
  int index;
  DevAssert(m != NULL);
  instance = m3ap_MCE_get_instance(instance_id);
  DevAssert(instance != NULL);
  instance->multi_sd = m->multi_sd;

  /* Exit if CNF message reports failure.
   * Failure means multi_sd < 0.
   */
  if (instance->multi_sd < 0) {
    M3AP_ERROR("Error: be sure to properly configure M3 in your configuration file.\n");
    DevAssert(instance->multi_sd >= 0);
  }

  /* Trying to connect to the provided list of MCE ip address */

  for (index = 0; index < instance->nb_m3; index++) {
    M3AP_INFO("MCE[%ld] MCE id %u acting as an initiator (client)\n",
              instance_id, instance->MCE_id);
    m3ap_MCE_register_MCE(instance,
                          &instance->target_mme_m3_ip_address[index],
                          &instance->mme_m3_ip_address,
                          instance->sctp_in_streams,
                          instance->sctp_out_streams,
                          instance->mce_port_for_M3C,
                          instance->multi_sd);
  }
}

//static
//void m3ap_eNB_handle_handover_req(instance_t instance,
//                                  m3ap_handover_req_t *m3ap_handover_req)
//{
//  m3ap_eNB_instance_t *instance_p;
//  m3ap_eNB_data_t     *target;
//  m3ap_id_manager     *id_manager;
//  int                 ue_id;
//
//  int target_pci = m3ap_handover_req->target_physCellId;
//
//  instance_p = m3ap_eNB_get_instance(instance);
//  DevAssert(instance_p != NULL);
//
//  target = m3ap_is_eNB_pci_in_list(target_pci);
//  DevAssert(target != NULL);
//
//  /* allocate m3ap ID */
//  id_manager = &instance_p->id_manager;
//  ue_id = m3ap_allocate_new_id(id_manager);
//  if (ue_id == -1) {
//    M3AP_ERROR("could not allocate a new M3AP UE ID\n");
//    /* TODO: cancel handover: send (to be defined) message to RRC */
//    exit(1);
//  }
//  /* id_source is ue_id, id_target is unknown yet */
//  m3ap_set_ids(id_manager, ue_id, m3ap_handover_req->rnti, ue_id, -1);
//  m3ap_id_set_state(id_manager, ue_id, M2ID_STATE_SOURCE_PREPARE);
//  m3ap_set_reloc_prep_timer(id_manager, ue_id,
//                            m3ap_timer_get_tti(&instance_p->timers));
//  m3ap_id_set_target(id_manager, ue_id, target);
//
//  m3ap_eNB_generate_m3_handover_request(instance_p, target, m3ap_handover_req, ue_id);
//}

//static
//void m3ap_eNB_handle_handover_req_ack(instance_t instance,
//                                      m3ap_handover_req_ack_t *m3ap_handover_req_ack)
//{
//  /* TODO: remove this hack (the goal is to find the correct
//   * eNodeB structure for the other end) - we need a proper way for RRC
//   * and M3AP to identify eNodeBs
//   * RRC knows about mod_id and M3AP knows about eNB_id (eNB_ID in
//   * the configuration file)
//   * as far as I understand.. CROUX
//   */
//  m3ap_eNB_instance_t *instance_p;
//  m3ap_eNB_data_t     *target;
//  int source_assoc_id = m3ap_handover_req_ack->source_assoc_id;
//  int                 ue_id;
//  int                 id_source;
//  int                 id_target;
//
//  instance_p = m3ap_eNB_get_instance(instance);
//  DevAssert(instance_p != NULL);
//
//  target = m3ap_get_eNB(NULL, source_assoc_id, 0);
//  DevAssert(target != NULL);
//
//  /* rnti is a new information, save it */
//  ue_id     = m3ap_handover_req_ack->m3_id_target;
//  id_source = m3ap_id_get_id_source(&instance_p->id_manager, ue_id);
//  id_target = ue_id;
//  m3ap_set_ids(&instance_p->id_manager, ue_id, m3ap_handover_req_ack->rnti, id_source, id_target);
//
//  m3ap_eNB_generate_m3_handover_request_ack(instance_p, target, m3ap_handover_req_ack);
//}
//
//static
//void m3ap_eNB_ue_context_release(instance_t instance,
//                                 m3ap_ue_context_release_t *m3ap_ue_context_release)
//{
//  m3ap_eNB_instance_t *instance_p;
//  m3ap_eNB_data_t     *target;
//  int source_assoc_id = m3ap_ue_context_release->source_assoc_id;
//  int ue_id;
//  instance_p = m3ap_eNB_get_instance(instance);
//  DevAssert(instance_p != NULL);
//
//  target = m3ap_get_eNB(NULL, source_assoc_id, 0);
//  DevAssert(target != NULL);
//
//  m3ap_eNB_generate_m3_ue_context_release(instance_p, target, m3ap_ue_context_release);
//
//  /* free the M3AP UE ID */
//  ue_id = m3ap_find_id_from_rnti(&instance_p->id_manager, m3ap_ue_context_release->rnti);
//  if (ue_id == -1) {
//    M3AP_ERROR("could not find UE %x\n", m3ap_ue_context_release->rnti);
//    exit(1);
//  }
//  m3ap_release_id(&instance_p->id_manager, ue_id);
//}

uint8_t m3ap_m3setup[] = {
  0x00, 0x07, 0x00, 0x23, 0x00, 0x00, 0x03, 0x00,
  0x12, 0x00, 0x07, 0x40, 0x55, 0xf5, 0x01, 0x00,
  0x25, 0x00, 0x00, 0x13, 0x40, 0x0a, 0x03, 0x80,
  0x4d, 0x33, 0x2d, 0x65, 0x4e, 0x42, 0x33, 0x37,
  0x00, 0x14, 0x00, 0x03, 0x01, 0x00, 0x01
};

uint8_t m3ap_start[] = {
  0x00, 0x00, 0x00, 0x4b, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02, 0x00,
  0x07, 0x00, 0x55, 0xf5, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x04, 0x00, 0x0d, 0x60, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x40, 0x01,
  0x7a, 0x00, 0x05, 0x00, 0x03, 0x07, 0x08, 0x00,
  0x00, 0x06, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x07, 0x00,
  0x0e, 0x00, 0xe8, 0x0a, 0x0a, 0x0a, 0x00, 0x0a,
  0xc8, 0x0a, 0xfe, 0x00, 0x00, 0x00, 0x01
};


void *m3ap_MCE_task(void *arg) {
  MessageDef *received_msg = NULL;
  int         result;
  M3AP_DEBUG("Starting M3AP MCE layer\n");
  m3ap_MCE_prepare_internal_data();
  itti_mark_task_ready(TASK_M3AP_MCE);

  while (1) {
    itti_receive_msg(TASK_M3AP_MCE, &received_msg);

    switch (ITTI_MSG_ID(received_msg)) {
      case MESSAGE_TEST:
//	LOG_W(M3AP,"MCE Received Test Message ... TODO\n");
//	//MessageDef * message_p = itti_alloc_new_message(TASK_M3AP, 0, MESSAGE_TEST);
//	//itti_send_msg_to_task(TASK_M2AP_ENB, 1/*ctxt_pP->module_id*/, message_p);
// 
//        asn_dec_rval_t dec_ret;
//
//	
//	uint8_t  *buffer;
//  	uint32_t  len;
//	M3AP_M3AP_PDU_t          pdu;
//	memset(&pdu, 0, sizeof(pdu));
//
//	buffer = &m3ap_m3setup[0];
//	//buffer = &m3ap_start[0];
//  	len=8*4+7;
//  	//len=8*9+7;
//
//	//if (m3ap_decode_pdu(&pdu, buffer, len) < 0) {
//    	//	LOG_E(M3AP, "Failed to decode PDU\n");
//    //re//turn -1;
//  	//}
//	M3AP_M3AP_PDU_t *pdu2 = &pdu;
//	dec_ret = aper_decode(NULL,
//                        &asn_DEF_M3AP_M3AP_PDU,
//                        (void **)&pdu2,
//                        buffer,
//                        len,
//                        0,
//                        0);
//
//	LOG_W(M3AP,"Trying to print %d\n",(dec_ret.code == RC_OK));
//	xer_fprint(stdout, &asn_DEF_M3AP_M3AP_PDU, &pdu);
//	LOG_W(M3AP,"Trying to print out \n");

        break;
      case TERMINATE_MESSAGE:
        M3AP_WARN(" *** Exiting M3AP thread\n");
        itti_exit_task();
        break;

      case M3AP_SUBFRAME_PROCESS:
        m3ap_check_timers(ITTI_MSG_DESTINATION_INSTANCE(received_msg));
        break;

      case M3AP_REGISTER_MCE_REQ:
	        LOG_I(M3AP,"MCE Received M3AP_REGISTER_MCE_REQ Message\n");

        m3ap_MCE_handle_register_MCE(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                     &M3AP_REGISTER_MCE_REQ(received_msg));
        break;

      case M3AP_MBMS_SESSION_START_RESP:
	        LOG_I(M3AP,"MCE M3AP_MBMS_SESSION_START_RESP Message\n");
	MCE_send_MBMS_SESSION_START_RESPONSE(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                     &M3AP_MBMS_SESSION_START_RESP(received_msg));

	break;
		
      case M3AP_MBMS_SESSION_STOP_RESP:
	        LOG_I(M3AP,"MCE M3AP_MBMS_SESSION_STOP_RESP Message\n");
	MCE_send_MBMS_SESSION_STOP_RESPONSE(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                     &M3AP_MBMS_SESSION_START_RESP(received_msg));

	break;

      case M3AP_MBMS_SESSION_UPDATE_RESP:
	        LOG_I(M3AP,"MCE M3AP_MBMS_SESSION_UPDATE_RESP Message\n");
	//MCE_send_MBMS_SESSION_UPDATE_RESPONSE(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                     //&M3AP_MBMS_SESSION_START_RESP(received_msg));

	break;

	
//      case M3AP_HANDOVER_REQ:
//        m3ap_eNB_handle_handover_req(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
//                                     &M3AP_HANDOVER_REQ(received_msg));
//        break;
//
//      case M3AP_HANDOVER_REQ_ACK:
//        m3ap_eNB_handle_handover_req_ack(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
//                                         &M3AP_HANDOVER_REQ_ACK(received_msg));
//        break;
//
//      case M3AP_UE_CONTEXT_RELEASE:
//        m3ap_eNB_ue_context_release(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
//                                                &M3AP_UE_CONTEXT_RELEASE(received_msg));
//        break;
//
      case SCTP_INIT_MSG_MULTI_CNF:
	        LOG_D(M3AP,"MCE Received SCTP_INIT_MSG_MULTI_CNF Message\n");

        m3ap_MCE_handle_sctp_init_msg_multi_cnf(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                                &received_msg->ittiMsg.sctp_init_msg_multi_cnf);
        break;

      case SCTP_NEW_ASSOCIATION_RESP:
	        LOG_D(M3AP,"MCE Received SCTP_NEW_ASSOCIATION_RESP Message\n");

        m3ap_MCE_handle_sctp_association_resp(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                              &received_msg->ittiMsg.sctp_new_association_resp);
        break;

      case SCTP_NEW_ASSOCIATION_IND:
	        LOG_D(M3AP,"MCE Received SCTP_NEW_ASSOCIATION Message\n");

        m3ap_MCE_handle_sctp_association_ind(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                             &received_msg->ittiMsg.sctp_new_association_ind);
        break;

      case SCTP_DATA_IND:
	        LOG_D(M3AP,"MCE Received SCTP_DATA_IND Message\n");

        m3ap_MCE_handle_sctp_data_ind(ITTI_MSG_DESTINATION_INSTANCE(received_msg),
                                      &received_msg->ittiMsg.sctp_data_ind);
        break;

      default:
        M3AP_ERROR("Received unhandled message: %d:%s\n",
                   ITTI_MSG_ID(received_msg), ITTI_MSG_NAME(received_msg));
        break;
    }

    result = itti_free (ITTI_MSG_ORIGIN_ID(received_msg), received_msg);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
    received_msg = NULL;
  }

  return NULL;
}

#include "common/config/config_userapi.h"

int is_m3ap_MCE_enabled(void)
{
  static volatile int config_loaded = 0;
  static volatile int enabled = 0;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  if (pthread_mutex_lock(&mutex)) goto mutex_error;

  if (config_loaded) {
    if (pthread_mutex_unlock(&mutex)) goto mutex_error;
    return enabled;
  }

  char *enable_m3 = NULL;
  paramdef_t p[] = {
   { "enable_mce_m3", "yes/no", 0, .strptr=&enable_m3, .defstrval="", TYPE_STRING, 0 }
  };

  /* TODO: do it per module - we check only first MCE */
  config_get(p, sizeof(p)/sizeof(paramdef_t), "MCEs.[0]");
  if (enable_m3 != NULL && strcmp(enable_m3, "yes") == 0)
    enabled = 1;

  config_loaded = 1;

  if (pthread_mutex_unlock(&mutex)) goto mutex_error;

  return enabled;

mutex_error:
  LOG_E(M3AP, "mutex error\n");
  exit(1);
}
