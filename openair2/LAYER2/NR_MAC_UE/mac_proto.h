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

/* \file proto.h
 * \brief MAC functions prototypes for gNB and UE
 * \author R. Knopp, K.H. HSU
 * \date 2018
 * \version 0.1
 * \company Eurecom / NTUST
 * \email: knopp@eurecom.fr, kai-hsiang.hsu@eurecom.fr
 * \note
 * \warning
 */

#ifndef __LAYER2_MAC_UE_PROTO_H__
#define __LAYER2_MAC_UE_PROTO_H__

#include "mac_defs.h"
#include "RRC/NR_UE/rrc_defs.h"

#define NR_DL_MAX_DAI                            (4)                      /* TS 38.213 table 9.1.3-1 Value of counter DAI for DCI format 1_0 and 1_1 */
#define NR_DL_MAX_NB_CW                          (2)                      /* number of downlink code word */

/**\brief initialize the field in nr_mac instance
   \param module_id      module id */
void nr_ue_init_mac(module_id_t module_idP);

/**\brief apply default configuration values in nr_mac instance
   \param mac           mac instance */
void nr_ue_mac_default_configs(NR_UE_MAC_INST_t *mac);

void nr_ue_decode_mib(module_id_t module_id, int cc_id);

/**\brief decode SIB1 and other SIs pdus in NR_UE, from if_module dl_ind
   \param module_id      module id
   \param cc_id          component carrier id
   \param gNB_index      gNB index
   \param sibs_mask      sibs mask
   \param pduP           pointer to pdu
   \param pdu_length     length of pdu */
int8_t nr_ue_decode_BCCH_DL_SCH(module_id_t module_id,
                                int cc_id,
                                unsigned int gNB_index,
                                uint8_t ack_nack,
                                uint8_t *pduP,
                                uint32_t pdu_len);

/**\brief primitive from RRC layer to MAC layer to set if bearer exists for a logical channel. todo handle mac_LogicalChannelConfig
   \param module_id                 module id
   \param cc_id                     component carrier id
   \param gNB_index                 gNB index
   \param long                      logicalChannelIdentity
   \param bool                      status*/
int nr_rrc_mac_config_req_ue_logicalChannelBearer(module_id_t module_id,
                                                  int         cc_idP,
                                                  uint8_t     gNB_index,
                                                  long        logicalChannelIdentity,
                                                  bool        status);

void nr_rrc_mac_config_req_scg(module_id_t module_id,
                               int cc_idP,
                               NR_CellGroupConfig_t *scell_group_config);

void nr_rrc_mac_config_req_mcg(module_id_t module_id,
                               int cc_idP,
                               NR_CellGroupConfig_t *scell_group_config);

void nr_rrc_mac_config_req_mib(module_id_t module_id,
                               int cc_idP,
                               NR_MIB_t *mibP,
                               int sched_sib1);

void nr_rrc_mac_config_req_sib1(module_id_t module_id,
                                int cc_idP,
                                struct NR_SI_SchedulingInfo *si_SchedulingInfo,
                                NR_ServingCellConfigCommonSIB_t *scc);

/**\brief initialization NR UE MAC instance(s), total number of MAC instance based on NB_NR_UE_MAC_INST*/
NR_UE_MAC_INST_t * nr_l2_init_ue(NR_UE_RRC_INST_t* rrc_inst);

/**\brief fetch MAC instance by module_id, within 0 - (NB_NR_UE_MAC_INST-1)
   \param module_id index of MAC instance(s)*/
NR_UE_MAC_INST_t *get_mac_inst(
    module_id_t module_id);

/**\brief called at each slot, slot length based on numerology. now use u=0, scs=15kHz, slot=1ms
          performs BSR/SR/PHR procedures, random access procedure handler and DLSCH/ULSCH procedures.
   \param dl_info     DL indication
   \param ul_info     UL indication*/
void nr_ue_ul_scheduler(nr_uplink_indication_t *ul_info);
void nr_ue_dl_scheduler(nr_downlink_indication_t *dl_info);

/**\brief fill nr_scheduled_response struct instance
   @param nr_scheduled_response_t *    pointer to scheduled_response instance to fill
   @param fapi_nr_dl_config_request_t* pointer to dl_config,
   @param fapi_nr_ul_config_request_t* pointer to ul_config,
   @param fapi_nr_tx_request_t*        pointer to tx_request;
   @param module_id_t mod_id           module ID
   @param int cc_id                    CC ID
   @param frame_t frame                frame number
   @param int slot                     reference number
   @param void *phy_pata               pointer to a PHY specific structure to be filled in the scheduler response (can be null) */
void fill_scheduled_response(nr_scheduled_response_t *scheduled_response,
                             fapi_nr_dl_config_request_t *dl_config,
                             fapi_nr_ul_config_request_t *ul_config,
                             fapi_nr_tx_request_t *tx_request,
                             module_id_t mod_id,
                             int cc_id,
                             frame_t frame,
                             int slot,
                             void *phy_data);

/*! \fn int8_t nr_ue_get_SR(module_id_t module_idP, frame_t frameP, slot_t slotP);
   \brief Called by PHY to get sdu for PUSCH transmission.  It performs the following operations: Checks BSR for DCCH, DCCH1 and DTCH corresponding to previous values computed either in SR or BSR procedures.  It gets rlc status indications on DCCH,DCCH1 and DTCH and forms BSR elements and PHR in MAC header.  CRNTI element is not supported yet.  It computes transport block for up to 3 SDUs and generates header and forms the complete MAC SDU.
\param[in] Mod_id Instance id of UE in machine
\param[in] frameP subframe number
\param[in] slotP slot number
*/
int8_t nr_ue_get_SR(module_id_t module_idP, frame_t frameP, slot_t slotP);

/*! \fn  bool update_bsr(module_id_t module_idP, frame_t frameP, slot_t slotP, uint8_t gNB_index)
   \brief get the rlc stats and update the bsr level for each lcid
\param[in] Mod_id instance of the UE
\param[in] frameP Frame index
\param[in] slot slotP number
\param[in] uint8_t gNB_index
*/
bool nr_update_bsr(module_id_t module_idP, frame_t frameP, slot_t slotP, uint8_t gNB_index);

/*! \fn  nr_locate_BsrIndexByBufferSize (int *table, int size, int value)
   \brief locate the BSR level in the table as defined in 38.321. This function requires that he values in table to be monotonic, either increasing or decreasing. The returned value is not less than 0, nor greater than n-1, where n is the size of table.
\param[in] *table Pointer to BSR table
\param[in] size Size of the table
\param[in] value Value of the buffer
\return the index in the BSR_LEVEL table
*/
uint8_t nr_locate_BsrIndexByBufferSize(const uint32_t *table, int size,
                                    int value);

/*! \fn  int nr_get_sf_periodicBSRTimer(uint8_t periodicBSR_Timer)
   \brief get the number of subframe from the periodic BSR timer configured by the higher layers
\param[in] periodicBSR_Timer timer for periodic BSR
\return the number of subframe
*/
int nr_get_sf_periodicBSRTimer(uint8_t bucketSize);

/*! \fn  int nr_get_sf_retxBSRTimer(uint8_t retxBSR_Timer)
   \brief get the number of subframe form the bucket size duration configured by the higher layer
\param[in]  retxBSR_Timer timer for regular BSR
\return the time in sf
*/
int nr_get_sf_retxBSRTimer(uint8_t retxBSR_Timer);

int8_t nr_ue_process_dci(module_id_t module_id, int cc_id, uint8_t gNB_index, frame_t frame, int slot, dci_pdu_rel15_t *dci, fapi_nr_dci_indication_pdu_t *dci_ind);
int nr_ue_process_dci_indication_pdu(module_id_t module_id, int cc_id, int gNB_index, frame_t frame, int slot, fapi_nr_dci_indication_pdu_t *dci);
int8_t nr_ue_process_csirs_measurements(module_id_t module_id, frame_t frame, int slot, fapi_nr_csirs_measurements_t *csirs_measurements);

uint32_t get_ssb_frame(uint32_t test);

void nr_ue_aperiodic_srs_scheduling(NR_UE_MAC_INST_t *mac, long resource_trigger, int frame, int slot);

bool trigger_periodic_scheduling_request(NR_UE_MAC_INST_t *mac,
                                         PUCCH_sched_t *pucch,
                                         frame_t frame,
                                         int slot);

int nr_get_csi_measurements(NR_UE_MAC_INST_t *mac, frame_t frame, int slot, PUCCH_sched_t *pucch);

uint8_t get_ssb_rsrp_payload(NR_UE_MAC_INST_t *mac,
                             PUCCH_sched_t *pucch,
                             struct NR_CSI_ReportConfig *csi_reportconfig,
                             NR_CSI_ResourceConfigId_t csi_ResourceConfigId,
                             NR_CSI_MeasConfig_t *csi_MeasConfig);

uint8_t get_csirs_RI_PMI_CQI_payload(NR_UE_MAC_INST_t *mac,
                                     PUCCH_sched_t *pucch,
                                     struct NR_CSI_ReportConfig *csi_reportconfig,
                                     NR_CSI_ResourceConfigId_t csi_ResourceConfigId,
                                     NR_CSI_MeasConfig_t *csi_MeasConfig);

uint8_t get_csirs_RSRP_payload(NR_UE_MAC_INST_t *mac,
                               PUCCH_sched_t *pucch,
                               struct NR_CSI_ReportConfig *csi_reportconfig,
                               NR_CSI_ResourceConfigId_t csi_ResourceConfigId,
                               NR_CSI_MeasConfig_t *csi_MeasConfig);

uint8_t nr_get_csi_payload(NR_UE_MAC_INST_t *mac,
                           PUCCH_sched_t *pucch,
                           int csi_report_id,
                           NR_CSI_MeasConfig_t *csi_MeasConfig);

uint8_t get_rsrp_index(int rsrp);
uint8_t get_rsrp_diff_index(int best_rsrp,int current_rsrp);

/* \brief Get payload (MAC PDU) from UE PHY
@param dl_info            pointer to dl indication
@param ul_time_alignment  pointer to timing advance parameters
@param pdu_id             index of DL PDU
@returns void
*/
void nr_ue_send_sdu(nr_downlink_indication_t *dl_info,
                    int pdu_id);

void nr_ue_process_mac_pdu(nr_downlink_indication_t *dl_info,
                           int pdu_id);

int nr_write_ce_ulsch_pdu(uint8_t *mac_ce,
                          NR_UE_MAC_INST_t *mac,
                          uint8_t power_headroom, // todo: NR_POWER_HEADROOM_CMD *power_headroom,
                          uint16_t *crnti,
                          NR_BSR_SHORT *truncated_bsr,
                          NR_BSR_SHORT *short_bsr,
                          NR_BSR_LONG  *long_bsr);

void config_dci_pdu(NR_UE_MAC_INST_t *mac,
                    fapi_nr_dl_config_request_t *dl_config,
                    const int rnti_type,
                    const int slot,
                    const NR_SearchSpace_t *ss);

void ue_dci_configuration(NR_UE_MAC_INST_t *mac, fapi_nr_dl_config_request_t *dl_config, const frame_t frame, const int slot);

uint8_t nr_ue_get_sdu(module_id_t module_idP,
                      int cc_id,
                      frame_t frameP,
                      sub_frame_t subframe,
                      uint8_t gNB_index,
                      uint8_t *ulsch_buffer,
                      uint16_t buflen);

void set_harq_status(NR_UE_MAC_INST_t *mac,
                     uint8_t pucch_id,
                     uint8_t harq_id,
                     int8_t delta_pucch,
                     uint8_t data_toul_fb,
                     uint8_t dai,
                     int n_CCE,
                     int N_CCE,
                     frame_t frame,
                     int slot);

bool get_downlink_ack(NR_UE_MAC_INST_t *mac, frame_t frame, int slot, PUCCH_sched_t *pucch);

int find_pucch_resource_set(NR_UE_MAC_INST_t *mac, int uci_size);

void multiplex_pucch_resource(NR_UE_MAC_INST_t *mac, PUCCH_sched_t *pucch, int num_res);

int16_t get_pucch_tx_power_ue(NR_UE_MAC_INST_t *mac,
                              int scs,
                              NR_PUCCH_Config_t *pucch_Config,
                              int delta_pucch,
                              uint8_t format_type,
                              uint16_t nb_of_prbs,
                              uint8_t freq_hop_flag,
                              uint8_t add_dmrs_flag,
                              uint8_t N_symb_PUCCH,
                              int subframe_number,
                              int O_uci);

int get_deltatf(uint16_t nb_of_prbs,
                uint8_t N_symb_PUCCH,
                uint8_t freq_hop_flag,
                uint8_t add_dmrs_flag,
                int N_sc_ctrl_RB,
                int O_UCI);

void nr_ue_configure_pucch(NR_UE_MAC_INST_t *mac,
                           int slot,
                           uint16_t rnti,
                           PUCCH_sched_t *pucch,
                           fapi_nr_ul_config_pucch_pdu *pucch_pdu);

int nr_get_Pcmax(NR_UE_MAC_INST_t *mac, int Qm, bool powerBoostPi2BPSK, int scs, int N_RB_UL, bool is_transform_precoding, int n_prbs, int start_prb);

/* Random Access */

/* \brief This function schedules the PRACH according to prach_ConfigurationIndex and TS 38.211 tables 6.3.3.2.x
and fills the PRACH PDU per each FD occasion.
@param module_idP Index of UE instance
@param frameP Frame index
@param slotP Slot index
@returns void
*/
void nr_ue_pucch_scheduler(module_id_t module_idP, frame_t frameP, int slotP, void *phy_data);
void nr_schedule_csirs_reception(NR_UE_MAC_INST_t *mac, int frame, int slot);
void nr_schedule_csi_for_im(NR_UE_MAC_INST_t *mac, int frame, int slot);
void schedule_ta_command(fapi_nr_dl_config_request_t *dl_config, NR_UL_TIME_ALIGNMENT_t *ul_time_alignment);

/* \brief This function schedules the Msg3 transmission
@param
@param
@param
@returns void
*/
void nr_ue_msg3_scheduler(NR_UE_MAC_INST_t *mac,
                          frame_t current_frame,
                          sub_frame_t current_slot,
                          uint8_t Msg3_tda_id);

/* \brief Function called by PHY to process the received RAR and check that the preamble matches what was sent by the gNB. It provides the timing advance and t-CRNTI.
@param Mod_id Index of UE instance
@param CC_id Index to a component carrier
@param frame Frame index
@param ra_rnti RA_RNTI value
@param dlsch_buffer  Pointer to dlsch_buffer containing RAR PDU
@param t_crnti Pointer to PHY variable containing the T_CRNTI
@param preamble_index Preamble Index used by PHY to transmit the PRACH.  This should match the received RAR to trigger the rest of
random-access procedure
@param selected_rar_buffer the output buffer for storing the selected RAR header and RAR payload
@returns timing advance or 0xffff if preamble doesn't match
*/
int nr_ue_process_rar(nr_downlink_indication_t *dl_info, int pdu_id);

void nr_ue_contention_resolution(module_id_t module_id, int cc_id, frame_t frame, int slot, NR_PRACH_RESOURCES_t *prach_resources);

void nr_ra_failed(uint8_t mod_id, uint8_t CC_id, NR_PRACH_RESOURCES_t *prach_resources, frame_t frame, int slot);

void nr_ra_succeeded(const module_id_t mod_id, const uint8_t gNB_index, const frame_t frame, const int slot);

void nr_get_RA_window(NR_UE_MAC_INST_t *mac);

/* \brief Function called by PHY to retrieve information to be transmitted using the RA procedure.
If the UE is not in PUSCH mode for a particular eNB index, this is assumed to be an Msg3 and MAC
attempts to retrieves the CCCH message from RRC. If the UE is in PUSCH mode for a particular eNB
index and PUCCH format 0 (Scheduling Request) is not activated, the MAC may use this resource for
andom-access to transmit a BSR along with the C-RNTI control element (see 5.1.4 from 38.321)
@param mod_id Index of UE instance
@param CC_id Component Carrier Index
@param frame
@param gNB_id gNB index
@param nr_slot_tx slot for PRACH transmission
@returns indication to generate PRACH to phy */
uint8_t nr_ue_get_rach(module_id_t mod_id,
                       int CC_id,
                       frame_t frame,
                       uint8_t gNB_id,
                       int nr_slot_tx);

/* \brief Function implementing the routine for the selection of Random Access resources (5.1.2 TS 38.321).
@param module_idP Index of UE instance
@param CC_id Component Carrier Index
@param gNB_index gNB index
@param rach_ConfigDedicated
@returns void */
void nr_get_prach_resources(module_id_t mod_id,
                            int CC_id,
                            uint8_t gNB_id,
                            NR_PRACH_RESOURCES_t *prach_resources,
                            NR_RACH_ConfigDedicated_t * rach_ConfigDedicated);

void init_RA(module_id_t mod_id,
             NR_PRACH_RESOURCES_t *prach_resources,
             NR_RACH_ConfigCommon_t *nr_rach_ConfigCommon,
             NR_RACH_ConfigGeneric_t *rach_ConfigGeneric,
             NR_RACH_ConfigDedicated_t *rach_ConfigDedicated);

int16_t get_prach_tx_power(module_id_t mod_id);

void set_ra_rnti(NR_UE_MAC_INST_t *mac, fapi_nr_ul_config_prach_pdu *prach_pdu);

void nr_Msg1_transmitted(module_id_t mod_id);

void nr_Msg3_transmitted(module_id_t mod_id, uint8_t CC_id, frame_t frameP, slot_t slotP, uint8_t gNB_id);

void nr_ue_msg2_scheduler(module_id_t mod_id, uint16_t rach_frame, uint16_t rach_slot, uint16_t *msg2_frame, uint16_t *msg2_slot);

int8_t nr_ue_process_dci_freq_dom_resource_assignment(nfapi_nr_ue_pusch_pdu_t *pusch_config_pdu,
                                                      fapi_nr_dl_config_dlsch_pdu_rel15_t *dlsch_config_pdu,
                                                      uint16_t n_RB_ULBWP,
                                                      uint16_t n_RB_DLBWP,
                                                      uint16_t riv);

void build_ssb_to_ro_map(NR_UE_MAC_INST_t *mac);

void ue_init_config_request(NR_UE_MAC_INST_t *mac, int scs);

fapi_nr_ul_config_request_t *get_ul_config_request(NR_UE_MAC_INST_t *mac, int slot, int fb_time);
fapi_nr_dl_config_request_t *get_dl_config_request(NR_UE_MAC_INST_t *mac, int slot);

void fill_ul_config(fapi_nr_ul_config_request_t *ul_config, frame_t frame_tx, int slot_tx, uint8_t pdu_type);

int16_t compute_nr_SSB_PL(NR_UE_MAC_INST_t *mac, short ssb_rsrp_dBm);

// PUSCH scheduler:
// - Calculate the slot in which ULSCH should be scheduled. This is current slot + K2,
// - where K2 is the offset between the slot in which UL DCI is received and the slot
// - in which ULSCH should be scheduled. K2 is configured in RRC configuration.  
// PUSCH Msg3 scheduler:
// - scheduled by RAR UL grant according to 8.3 of TS 38.213
int nr_ue_pusch_scheduler(NR_UE_MAC_INST_t *mac, uint8_t is_Msg3, frame_t current_frame, int current_slot, frame_t *frame_tx, int *slot_tx, long k2);

int get_rnti_type(NR_UE_MAC_INST_t *mac, uint16_t rnti);

// Configuration of Msg3 PDU according to clauses:
// - 8.3 of 3GPP TS 38.213 version 16.3.0 Release 16
// - 6.1.2.2 of TS 38.214
// - 6.1.3 of TS 38.214
// - 6.2.2 of TS 38.214
// - 6.1.4.2 of TS 38.214
// - 6.4.1.1.1 of TS 38.211
// - 6.3.1.7 of 38.211
int nr_config_pusch_pdu(NR_UE_MAC_INST_t *mac,
                        NR_tda_info_t *tda_info,
                        nfapi_nr_ue_pusch_pdu_t *pusch_config_pdu,
                        dci_pdu_rel15_t *dci,
                        RAR_grant_t *rar_grant,
                        uint16_t rnti,
                        const nr_dci_format_t *dci_format);
#endif
/** @}*/
