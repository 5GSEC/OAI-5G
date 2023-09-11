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

/*! \file m3ap_MCE_defs.h
 * \brief m3ap struct definitions for MCE
 * \author Javier Morgade  <javier.morgade@ieee.org>
 * \date 2019
 * \version 0.1
 */

#include <stdint.h>

#include "queue.h"
#include "tree.h"

#include "sctp_eNB_defs.h"

#include "m3ap_ids.h" //looks X2AP specific for HO
#include "m3ap_timers.h"

#ifndef M3AP_MCE_DEFS_H_
#define M3AP_MCE_DEFS_H_

#define M3AP_MCE_NAME_LENGTH_MAX    (150)

typedef enum {
  /* Disconnected state: initial state for any association. */
  M3AP_MCE_STATE_DISCONNECTED = 0x0,

  /* State waiting for m2 Setup response message if the target MCE accepts or
   * M3 Setup failure if rejects the MCE.
   */
  M3AP_MCE_STATE_WAITING     = 0x1,

  /* The MCE is successfully connected to another MCE. */
  M3AP_MCE_STATE_CONNECTED   = 0x2,

  /* M3AP is ready, and the MCE is successfully connected to another MCE. */
  M3AP_MCE_STATE_READY             = 0x3,

  M3AP_MCE_STATE_OVERLOAD          = 0x4,

  M3AP_MCE_STATE_RESETTING         = 0x5,

  /* Max number of states available */
  M3AP_MCE_STATE_MAX,
} m3ap_MCE_state_t;

/* Served PLMN identity element */
/*struct plmn_identity_s {
  uint16_t mcc;
  uint16_t mnc;
  uint8_t  mnc_digit_length;
  STAILQ_ENTRY(plmn_identity_s) next;
};*/

/* Served group id element */
/*struct served_group_id_s {
  uint16_t mce_group_id;
  STAILQ_ENTRY(served_group_id_s) next;
};*/

/* Served enn code for a particular MCE */
/*struct mce_code_s {
  uint8_t mce_code;
  STAILQ_ENTRY(mce_code_s) next;
};*/

struct m3ap_MCE_instance_s;

/* This structure describes association of a MCE to another MCE */
typedef struct m3ap_MCE_data_s {
  /* MCE descriptors tree, ordered by sctp assoc id */
  RB_ENTRY(m3ap_MCE_data_s) entry;

  /* This is the optional name provided by the MME */
  char *MCE_name;

  /*  target MCE ID */
  uint32_t MCE_id;

  /* Current MCE load information (if any). */
  //m3ap_load_state_t overload_state;

  /* Current MCE->MCE M3AP association state */
  m3ap_MCE_state_t state;

  /* Next usable stream for UE signalling */
  int32_t nextstream;

  /* Number of input/ouput streams */
  uint16_t in_streams;
  uint16_t out_streams;

  /* Connexion id used between SCTP/M3AP */
  uint16_t cnx_id;

  /* SCTP association id */
  int32_t  assoc_id;

  /* Nid cells */
  uint32_t                Nid_cell[MAX_NUM_CCs];
  int                     num_cc;

  /* Only meaningfull in virtual mode */
  struct m3ap_MCE_instance_s *m3ap_MCE_instance;
} m3ap_MCE_data_t;

typedef struct m3ap_MCE_instance_s {
  /* used in simulation to store multiple MCE instances*/
  STAILQ_ENTRY(m3ap_MCE_instance_s) m3ap_MCE_entries;

  /* Number of target MCEs requested by MCE (tree size) */
  uint32_t m3_target_mme_nb;
  /* Number of target MCEs for which association is pending */
  uint32_t m3_target_mme_pending_nb;
  /* Number of target MCE successfully associated to MCE */
  uint32_t m3_target_mme_associated_nb;
  /* Tree of M3AP MCE associations ordered by association ID */
  RB_HEAD(m3ap_mce_map, m3ap_MCE_data_s) m3ap_mce_head;

  /* Tree of UE ordered by MCE_ue_m3ap_id's */
  //  RB_HEAD(m3ap_ue_map, m3ap_MCE_ue_context_s) m3ap_ue_head;

  /* For virtual mode, mod_id as defined in the rest of the L1/L2 stack */
  instance_t instance;

  /* Displayable name of MCE */
  char *MCE_name;

  /* Unique MCE_id to identify the MCE within EPC.
   * In our case the MCE is a macro MCE so the id will be 20 bits long.
   * For Home MCE id, this field should be 28 bits long.
   */
  uint32_t MCE_id;
  /* The type of the cell */
  cell_type_t cell_type;

  /* Tracking area code */
  uint16_t tac;

  /* Mobile Country Code
   * Mobile Network Code
   */
  uint16_t  mcc;
  uint16_t  mnc;
  uint8_t   mnc_digit_length;

  /* CC params */
  int16_t                 eutra_band[MAX_NUM_CCs];
  uint32_t                downlink_frequency[MAX_NUM_CCs];
  int32_t                 uplink_frequency_offset[MAX_NUM_CCs];
  uint32_t                Nid_cell[MAX_NUM_CCs];
  int16_t                 N_RB_DL[MAX_NUM_CCs];
  frame_type_t            frame_type[MAX_NUM_CCs];
  uint32_t                fdd_earfcn_DL[MAX_NUM_CCs];
  uint32_t                fdd_earfcn_UL[MAX_NUM_CCs];
  int                     num_cc;

  net_ip_address_t target_mme_m3_ip_address[M3AP_MAX_NB_MCE_IP_ADDRESS];
  uint8_t          nb_m3;
  net_ip_address_t mme_m3_ip_address;
  uint16_t         sctp_in_streams;
  uint16_t         sctp_out_streams;
  uint32_t         mce_port_for_M3C;
  int              multi_sd;

  m3ap_id_manager  id_manager;
  m3ap_timers_t    timers;
} m3ap_MCE_instance_t;

typedef struct {
  /* List of served MCEs
   * Only used for virtual mode
   */
  STAILQ_HEAD(m3ap_MCE_instances_head_s, m3ap_MCE_instance_s) m3ap_MCE_instances_head;
  /* Nb of registered MCEs */
  uint8_t nb_registered_MCEs;

  /* Generate a unique connexion id used between M3AP and SCTP */
  uint16_t global_cnx_id;
} m3ap_MCE_internal_data_t;

int m3ap_MCE_compare_assoc_id(struct m3ap_MCE_data_s *p1, struct m3ap_MCE_data_s *p2);

/* Generate the tree management functions */
struct m3ap_MCE_map;
struct m3ap_MCE_data_s;
RB_PROTOTYPE(m3ap_MCE_map, m3ap_MCE_data_s, entry, m3ap_MCE_compare_assoc_id);


#endif /* M3AP_MCE_DEFS_H_ */
