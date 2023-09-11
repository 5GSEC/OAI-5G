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

#ifndef X2AP_MESSAGES_TYPES_H_
#define X2AP_MESSAGES_TYPES_H_

#include "s1ap_messages_types.h"
#include "LTE_PhysCellId.h"

typedef enum {
  X2AP_CAUSE_T_DC_PREP_TIMEOUT,
  X2AP_CAUSE_T_DC_OVERALL_TIMEOUT,
  X2AP_CAUSE_RADIO_CONNECTION_WITH_UE_LOST,
} x2ap_cause_t;

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.


#define X2AP_REGISTER_ENB_REQ(mSGpTR)                           (mSGpTR)->ittiMsg.x2ap_register_enb_req
#define X2AP_SETUP_REQ(mSGpTR)                                  (mSGpTR)->ittiMsg.x2ap_setup_req
#define X2AP_SETUP_RESP(mSGpTR)                                 (mSGpTR)->ittiMsg.x2ap_setup_resp
#define X2AP_RESET_REQ(mSGpTR)                                  (mSGpTR)->ittiMsg.x2ap_reset_req
#define X2AP_RESET_RESP(mSGpTR)                                 (mSGpTR)->ittiMsg.x2ap_reset_resp
#define X2AP_HANDOVER_REQ(mSGpTR)                               (mSGpTR)->ittiMsg.x2ap_handover_req
#define X2AP_HANDOVER_REQ_ACK(mSGpTR)                           (mSGpTR)->ittiMsg.x2ap_handover_req_ack
#define X2AP_REGISTER_ENB_CNF(mSGpTR)                           (mSGpTR)->ittiMsg.x2ap_register_enb_cnf
#define X2AP_DEREGISTERED_ENB_IND(mSGpTR)                       (mSGpTR)->ittiMsg.x2ap_deregistered_enb_ind
#define X2AP_UE_CONTEXT_RELEASE(mSGpTR)                         (mSGpTR)->ittiMsg.x2ap_ue_context_release
#define X2AP_HANDOVER_CANCEL(mSGpTR)                            (mSGpTR)->ittiMsg.x2ap_handover_cancel
#define X2AP_SENB_ADDITION_REQ(mSGpTR)                          (mSGpTR)->ittiMsg.x2ap_senb_addition_req
#define X2AP_ENDC_SGNB_ADDITION_REQ(mSGpTR)                     (mSGpTR)->ittiMsg.x2ap_ENDC_sgnb_addition_req
#define X2AP_ENDC_SGNB_ADDITION_REQ_ACK(mSGpTR)                 (mSGpTR)->ittiMsg.x2ap_ENDC_sgnb_addition_req_ACK
#define X2AP_ENDC_SGNB_RECONF_COMPLETE(mSGpTR)                  (mSGpTR)->ittiMsg.x2ap_ENDC_sgnb_reconf_complete
#define X2AP_ENDC_SGNB_RELEASE_REQUEST(mSGpTR)                  (mSGpTR)->ittiMsg.x2ap_ENDC_sgnb_release_request
#define X2AP_ENDC_SGNB_RELEASE_REQUIRED(mSGpTR)                 (mSGpTR)->ittiMsg.x2ap_ENDC_sgnb_release_required
#define X2AP_ENDC_DC_PREP_TIMEOUT(mSGpTR)                       (mSGpTR)->ittiMsg.x2ap_ENDC_dc_prep_timeout
#define X2AP_ENDC_DC_OVERALL_TIMEOUT(mSGpTR)                    (mSGpTR)->ittiMsg.x2ap_ENDC_dc_overall_timeout
#define X2AP_ENDC_SETUP_REQ(mSGpTR)                             (mSGpTR)->ittiMsg.x2ap_ENDC_setup_req

#define X2AP_MAX_NB_ENB_IP_ADDRESS 2

// eNB application layer -> X2AP messages

typedef struct x2ap_setup_req_s {
  uint32_t Nid_cell[MAX_NUM_CCs];
  int num_cc;
} x2ap_setup_req_t;

typedef struct x2ap_setup_resp_s {
  uint32_t Nid_cell[MAX_NUM_CCs];
  int num_cc;
} x2ap_setup_resp_t;

typedef struct x2ap_reset_req_s {
  uint32_t cause;
} x2ap_reset_req_t;

typedef struct x2ap_reset_resp_s {
  int dummy;
} x2ap_reset_resp_t;

/* X2AP UE CONTEXT RELEASE */
typedef struct x2ap_ue_context_release_s {
  /* used for X2AP->RRC in source and RRC->X2AP in target */
  int rnti;

  int source_assoc_id;
} x2ap_ue_context_release_t;

typedef enum {
  X2AP_T_RELOC_PREP_TIMEOUT,
  X2AP_TX2_RELOC_OVERALL_TIMEOUT
} x2ap_handover_cancel_cause_t;

typedef enum {
	X2AP_RECONF_RESPONSE_SUCCESS,
	X2AP_RECONF_RESPONSE_REJECT
	/* Extensions may appear below */

} x2ap_sgNB_reconf_response_information_t;

typedef struct x2ap_handover_cancel_s {
  int rnti;
  x2ap_handover_cancel_cause_t cause;
} x2ap_handover_cancel_t;

typedef struct x2ap_register_enb_req_s {
  /* Unique eNB_id to identify the eNB within EPC.
   * For macro eNB ids this field should be 20 bits long.
   * For home eNB ids this field should be 28 bits long.
   */
  uint32_t eNB_id;
  /* The type of the cell */
  enum cell_type_e cell_type;

  /* Optional name for the cell
   * NOTE: the name can be NULL (i.e no name) and will be cropped to 150
   * characters.
   */
  char *eNB_name;

  /* Tracking area code */
  uint16_t tac;

  /* Mobile Country Code
   * Mobile Network Code
   */
  uint16_t mcc;
  uint16_t mnc;
  uint8_t  mnc_digit_length;

  /*
   * CC Params
   */
  int16_t                 eutra_band[MAX_NUM_CCs];
  int32_t                 nr_band[MAX_NUM_CCs];
  int32_t                 nrARFCN[MAX_NUM_CCs];
  uint32_t                downlink_frequency[MAX_NUM_CCs];
  int32_t                 uplink_frequency_offset[MAX_NUM_CCs];
  uint32_t                Nid_cell[MAX_NUM_CCs];
  int16_t                 N_RB_DL[MAX_NUM_CCs];
  frame_type_t            frame_type[MAX_NUM_CCs];
  uint32_t                fdd_earfcn_DL[MAX_NUM_CCs];
  uint32_t                fdd_earfcn_UL[MAX_NUM_CCs];
  uint32_t                subframeAssignment[MAX_NUM_CCs];
  uint32_t                specialSubframe[MAX_NUM_CCs];
  int                     num_cc;

  /* To be considered for TDD */
  //uint16_t tdd_EARFCN;
  //uint16_t tdd_Transmission_Bandwidth;

  /* The local eNB IP address to bind */
  net_ip_address_t enb_x2_ip_address;

  /* Nb of MME to connect to */
  uint8_t          nb_x2;

  /* List of target eNB to connect to for X2*/
  net_ip_address_t target_enb_x2_ip_address[X2AP_MAX_NB_ENB_IP_ADDRESS];

  /* Number of SCTP streams used for associations */
  uint16_t sctp_in_streams;
  uint16_t sctp_out_streams;

  /* eNB port for X2C*/
  uint32_t enb_port_for_X2C;

  /* timers (unit: millisecond) */
  int t_reloc_prep;
  int tx2_reloc_overall;
  int t_dc_prep;
  int t_dc_overall;
} x2ap_register_enb_req_t;

typedef struct x2ap_subframe_process_s {
  /* nothing, we simply use the module ID in the header */
  // This dummy element is to avoid CLANG warning: empty struct has size 0 in C, size 1 in C++
  // To be removed if the structure is filled
  uint32_t dummy;
} x2ap_subframe_process_t;

//-------------------------------------------------------------------------------------------//
// X2AP -> eNB application layer messages
typedef struct x2ap_register_enb_cnf_s {
  /* Nb of connected eNBs*/
  uint8_t          nb_x2;
} x2ap_register_enb_cnf_t;

typedef struct x2ap_deregistered_enb_ind_s {
  /* Nb of connected eNBs */
  uint8_t          nb_x2;
} x2ap_deregistered_enb_ind_t;

//-------------------------------------------------------------------------------------------//
// X2AP <-> RRC
typedef struct x2ap_gummei_s {
  uint16_t mcc;
  uint16_t mnc;
  uint8_t  mnc_len;
  uint8_t  mme_code;
  uint16_t mme_group_id;
} x2ap_gummei_t;

typedef struct x2ap_lastvisitedcell_info_s {
  uint16_t mcc;
  uint16_t mnc;
  uint8_t  mnc_len;
  LTE_PhysCellId_t target_physCellId;
  cell_type_t cell_type;
  uint64_t time_UE_StayedInCell;
}x2ap_lastvisitedcell_info_t;

typedef struct x2ap_handover_req_s {
  /* used for RRC->X2AP in source eNB */
  int rnti;

  /* used for X2AP->RRC in target eNB */
  int x2_id;

  LTE_PhysCellId_t target_physCellId;

  x2ap_gummei_t ue_gummei;

  /*UE-ContextInformation */

  /* MME UE id  */
  uint32_t mme_ue_s1ap_id;

  security_capabilities_t security_capabilities;

  uint8_t      kenb[32]; // keNB or keNB*

  /*next_hop_chaining_coun */
  long int     kenb_ncc;

  /* UE aggregate maximum bitrate */
  ambr_t ue_ambr;

  uint8_t nb_e_rabs_tobesetup;

 /* list of e_rab setup-ed by RRC layers */
  e_rab_setup_t e_rabs_tobesetup[S1AP_MAX_E_RAB];

  /* list of e_rab to be setup by RRC layers */
  e_rab_t  e_rab_param[S1AP_MAX_E_RAB];

  x2ap_lastvisitedcell_info_t lastvisitedcell_info;

  uint8_t rrc_buffer[8192 /* arbitrary, big enough */];
  int rrc_buffer_size;

  int target_assoc_id;
} x2ap_handover_req_t;

typedef struct x2ap_handover_req_ack_s {
  /* used for RRC->X2AP in target and X2AP->RRC in source */
  int rnti;

  /* used for RRC->X2AP in target */
  int x2_id_target;

  int source_assoc_id;

  uint8_t nb_e_rabs_tobesetup;

 /* list of e_rab setup-ed by RRC layers */
  e_rab_setup_t e_rabs_tobesetup[S1AP_MAX_E_RAB];

  /* list of e_rab to be setup by RRC layers */
  e_rab_t  e_rab_param[S1AP_MAX_E_RAB];

  uint8_t rrc_buffer[1024 /* arbitrary, big enough */];
  int rrc_buffer_size;

  uint32_t mme_ue_s1ap_id;
} x2ap_handover_req_ack_t;

typedef struct x2ap_senb_addition_req_s {

	/* MeNB UE X2AP ID*/
	int x2_MeNB_UE_id;

	/*SCG Bearer option*/
	security_capabilities_t UE_security_capabilities;

	/*SCG Bearer option*/
	uint8_t SeNB_security_key[256];

	/*SeNB UE aggregate maximum bitrate */
	ambr_t SeNB_ue_ambr;

	uint8_t total_nb_e_rabs_tobeadded;

	uint8_t nb_sCG_e_rabs_tobeadded;

	uint8_t nb_split_e_rabs_tobeadded;

	/*list of total e_rabs (SCG or split) to be added*/
    //e_rab_setup_t total_e_rabs_tobeadded[S1AP_MAX_E_RAB];

	/* list of SCG e_rab to be added by RRC layers */
	e_rab_setup_t e_sCG_rabs_tobeadded[S1AP_MAX_E_RAB];

	/* list of split e_rab to be added by RRC layers */
	e_rab_setup_t e_split_rabs_tobeadded[S1AP_MAX_E_RAB];

	/* list of SCG e_rab to be added by RRC layers */
	e_rab_t  e_sCG_rab_param[S1AP_MAX_E_RAB];

	/* list of split e_rab to be added by RRC layers */
	e_rab_t  e_split_rab_param[S1AP_MAX_E_RAB];

	/*Used for the MeNB to SeNB Container to include the SCG-ConfigInfo as per 36.331*/
	uint8_t rrc_buffer[1024 /* arbitrary, big enough */];

	int rrc_buffer_size;

}x2ap_senb_addition_req_t;

typedef struct x2ap_senb_addition_req_ack_s {

  int MeNB_UE_X2_id;

  int SgNB_UE_X2_id;

  uint8_t nb_sCG_e_rabs_tobeadded;

  uint8_t nb_split_e_rabs_tobeadded;

 /* list of SCG e_rab to be added by RRC layers */
  e_rab_setup_t e_sCG_rabs_tobeadded[S1AP_MAX_E_RAB];

  /* list of split e_rab to be added by RRC layers */
  e_rab_setup_t e_split_rabs_tobeadded[S1AP_MAX_E_RAB];

  uint8_t rrc_buffer[1024 /* arbitrary, big enough */];
  int rrc_buffer_size;

} x2ap_senb_addition_req_ack_t;


typedef struct x2ap_ENDC_setup_req_s {
  uint32_t Nid_cell[MAX_NUM_CCs];
  int num_cc;
  uint32_t servedNrCell_band[MAX_NUM_CCs];
} x2ap_ENDC_setup_req_t;

typedef struct x2ap_ENDC_sgnb_addition_req_s {
  int ue_x2_id;
  LTE_PhysCellId_t target_physCellId; 
  /* used for RRC->X2AP in source eNB */
  int rnti;

  nr_security_capabilities_t security_capabilities;

  /* SgNB Security Key */
  uint8_t      kgnb[32];

  /*next_hop_chaining_coun */
  long int     kgnb_ncc;

  /* UE aggregate maximum bitrate */
  ambr_t ue_ambr;

  uint8_t nb_e_rabs_tobeadded;

 /* list of e_rab to be added by RRC layers */
  e_rab_tobe_added_t e_rabs_tobeadded[S1AP_MAX_E_RAB];

  /* list of e_rab to be setup by RRC layers */
  e_rab_t  e_rab_param[S1AP_MAX_E_RAB];

  x2ap_lastvisitedcell_info_t lastvisitedcell_info;

  uint8_t rrc_buffer[4096 /* arbitrary, big enough */];
  int rrc_buffer_size;

  int target_assoc_id;

  	/*long int pDCPatSgNB = X2AP_EN_DC_ResourceConfiguration__pDCPatSgNB_present;
  	long int mCGresources = X2AP_EN_DC_ResourceConfiguration__mCGresources_not_present;
  	long int sCGresources = X2AP_EN_DC_ResourceConfiguration__sCGresources_not_present;*/


} x2ap_ENDC_sgnb_addition_req_t;

typedef struct x2ap_ENDC_sgnb_addition_req_ACK_s {
  int MeNB_ue_x2_id;

  int SgNB_ue_x2_id;

  int gnb_x2_assoc_id;  // to be stored in the rrc's ue context, used when sending 'sgnb reconfiguration complete'

  /* used for X2AP->RRC in source eNB */
  int rnti;

  uint8_t nb_e_rabs_admitted_tobeadded;

 /* list of e_rab to be added by RRC layers */
  e_rab_admitted_tobe_added_t e_rabs_admitted_tobeadded[S1AP_MAX_E_RAB];

  /* list of e_rab to be setup by RRC layers */
  e_rab_t  e_rab_param[S1AP_MAX_E_RAB];

  x2ap_lastvisitedcell_info_t lastvisitedcell_info;

  uint8_t rrc_buffer[4096 /* arbitrary, big enough */];
  int rrc_buffer_size;

  int target_assoc_id;

  	/*long int pDCPatSgNB = X2AP_EN_DC_ResourceConfiguration__pDCPatSgNB_present;
  	long int mCGresources = X2AP_EN_DC_ResourceConfiguration__mCGresources_not_present;
  	long int sCGresources = X2AP_EN_DC_ResourceConfiguration__sCGresources_not_present;*/


} x2ap_ENDC_sgnb_addition_req_ACK_t;

typedef struct x2ap_ENDC_reconf_complete_s {
  int MeNB_ue_x2_id;
  int SgNB_ue_x2_id;
  int gnb_x2_assoc_id;
} x2ap_ENDC_reconf_complete_t;

typedef struct x2ap_ENDC_sgnb_release_request_s {
  int          rnti;
  x2ap_cause_t cause;
  int          assoc_id;
} x2ap_ENDC_sgnb_release_request_t;

typedef struct x2ap_ENDC_sgnb_release_required_s {
  int gnb_rnti;
} x2ap_ENDC_sgnb_release_required_t;

typedef struct x2ap_ENDC_dc_prep_timeout_s {
  int rnti;
} x2ap_ENDC_dc_prep_timeout_t;

typedef struct x2ap_ENDC_dc_overall_timeout_s {
  int rnti;
} x2ap_ENDC_dc_overall_timeout_t;

#endif /* X2AP_MESSAGES_TYPES_H_ */
