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

/*! \file PHY/NR_TRANSPORT/nr_transport_proto.h.c
* \brief Function prototypes for PHY physical/transport channel processing and generation
* \author Ahmed Hussein
* \date 2019
* \version 0.1
* \company Fraunhofer IIS
* \email: ahmed.hussein@iis.fraunhofer.de
* \note
* \warning
*/

#ifndef __NR_TRANSPORT__H__
#define __NR_TRANSPORT__H__

#include "PHY/defs_nr_common.h"
#include "PHY/defs_gNB.h"

#define NR_PBCH_PDU_BITS 24

NR_gNB_PHY_STATS_t *get_phy_stats(PHY_VARS_gNB *gNB, uint16_t rnti);

int nr_generate_prs(uint32_t **nr_gold_prs,
                    c16_t *txdataF,
                    int16_t amp,
                    prs_config_t *prs_cfg,
                    nfapi_nr_config_request_scf_t *config,
                    NR_DL_FRAME_PARMS *frame_parms);

/*!
\fn int nr_generate_pss
\brief Generation of the NR PSS
@param
@returns 0 on success
 */
int nr_generate_pss(c16_t *txdataF,
                    int16_t amp,
                    uint8_t ssb_start_symbol,
                    nfapi_nr_config_request_scf_t *config,
                    NR_DL_FRAME_PARMS *frame_parms);

/*!
\fn int nr_generate_sss
\brief Generation of the NR SSS
@param
@returns 0 on success
 */
int nr_generate_sss(c16_t *txdataF,
                    int16_t amp,
                    uint8_t ssb_start_symbol,
                    nfapi_nr_config_request_scf_t *config,
                    NR_DL_FRAME_PARMS *frame_parms);

/*!
\fn int nr_generate_pbch_dmrs
\brief Generation of the DMRS for the PBCH
@param
@returns 0 on success
 */
int nr_generate_pbch_dmrs(uint32_t *gold_pbch_dmrs,
                          c16_t *txdataF,
                          int16_t amp,
                          uint8_t ssb_start_symbol,
                          nfapi_nr_config_request_scf_t *config,
                          NR_DL_FRAME_PARMS *frame_parms);

/*!
\fn int nr_generate_pbch
\brief Generation of the PBCH
@param
@returns 0 on success
 */
int nr_generate_pbch(nfapi_nr_dl_tti_ssb_pdu *ssb_pdu,
                     uint8_t *interleaver,
                     c16_t *txdataF,
                     int16_t amp,
                     uint8_t ssb_start_symbol,
                     uint8_t n_hf,
                     int sfn,
                     nfapi_nr_config_request_scf_t *config,
                     NR_DL_FRAME_PARMS *frame_parms);

/*!
\fn int nr_generate_pbch
\brief PBCH interleaving function
@param bit index i of the input payload
@returns the bit index of the output
 */
void nr_init_pbch_interleaver(uint8_t *interleaver);

NR_gNB_DLSCH_t new_gNB_dlsch(NR_DL_FRAME_PARMS *frame_parms, uint16_t N_RB);

void free_gNB_dlsch(NR_gNB_DLSCH_t *dlsch, uint16_t N_RB, const NR_DL_FRAME_PARMS *frame_parms);

/** \brief This function is the top-level entry point to PUSCH demodulation, after frequency-domain transformation and channel estimation.  It performs
    - RB extraction (signal and channel estimates)
    - channel compensation (matched filtering)
    - RE extraction (dmrs)
    - antenna combining (MRC, Alamouti, cycling)
    - LLR computation
    This function supports TM1, 2, 3, 5, and 6.
    @param ue Pointer to PHY variables
    @param UE_id id of current UE
    @param frame Frame number
    @param slot Slot number
    @param harq_pid HARQ process ID
*/
void nr_rx_pusch(PHY_VARS_gNB *gNB,
                 uint8_t UE_id,
                 uint32_t frame,
                 uint8_t slot,
                 unsigned char harq_pid);

/** \brief This function performs RB extraction (signal and channel estimates) (currently signal only until channel estimation and compensation are implemented)
    @param rxdataF pointer to the received frequency domain signal
    @param rxdataF_ext pointer to the extracted frequency domain signal
    @param rb_alloc RB allocation map (used for Resource Allocation Type 0 in NR)
    @param symbol Symbol on which to act (within-in nr_TTI_rx)
    @param start_rb The starting RB in the RB allocation (used for Resource Allocation Type 1 in NR)
    @param nb_rb_pusch The number of RBs allocated (used for Resource Allocation Type 1 in NR)
    @param frame_parms, Pointer to frame descriptor structure
*/
void nr_ulsch_extract_rbs(c16_t **rxdataF,
                          NR_gNB_PUSCH *pusch_vars,
                          int slot,
                          unsigned char symbol,
                          uint8_t is_dmrs_symbol,
                          nfapi_nr_pusch_pdu_t *pusch_pdu,
                          NR_DL_FRAME_PARMS *frame_parms);

void nr_ulsch_scale_channel(int32_t **ul_ch_estimates_ext,
                            NR_DL_FRAME_PARMS *frame_parms,
                            NR_gNB_ULSCH_t *ulsch_gNB,
                            uint8_t symbol, 
                            uint8_t is_dmrs_symbol,                           
                            uint32_t len,
                            uint8_t nrOfLayers,
                            uint16_t nb_rb,
                            int shift_ch_ext);

/** \brief This function computes the average channel level over all allocated RBs and antennas (TX/RX) in order to compute output shift for compensated signal
    @param ul_ch_estimates_ext Channel estimates in allocated RBs
    @param frame_parms Pointer to frame descriptor
    @param avg Pointer to average signal strength
    @param pilots_flag Flag to indicate pilots in symbol
    @param nb_rb Number of allocated RBs
*/
void nr_ulsch_channel_level(int **ul_ch_estimates_ext,
                            NR_DL_FRAME_PARMS *frame_parms,
                            int32_t *avg,
                            uint8_t symbol,
                            uint32_t len,
                            uint8_t  nrOfLayers,
                            unsigned short nb_rb);

/** \brief This function performs channel compensation (matched filtering) on the received RBs for this allocation.  In addition, it computes the squared-magnitude of the channel with weightings for 16QAM/64QAM detection as well as dual-stream detection (cross-correlation)
    @param rxdataF_ext Frequency-domain received signal in RBs to be demodulated
    @param ul_ch_estimates_ext Frequency-domain channel estimates in RBs to be demodulated
    @param ul_ch_mag First Channel magnitudes (16QAM/64QAM/256QAM)
    @param ul_ch_magb Second weighted Channel magnitudes (64QAM/256QAM)
    @param ul_ch_magc Third weighted Channel magnitudes (256QAM)
    @param rxdataF_comp Compensated received waveform
    @param frame_parms Pointer to frame descriptor
    @param symbol Symbol on which to operate
    @param Qm Modulation order of allocation
    @param nb_rb Number of RBs in allocation
    @param output_shift Rescaling for compensated output (should be energy-normalizing)
*/
void nr_ulsch_channel_compensation(int **rxdataF_ext,
                                int **ul_ch_estimates_ext,
                                int **ul_ch_mag,
                                int **ul_ch_magb,
                                int **ul_ch_magc,
                                int **rxdataF_comp,
                                int ***rho,
                                NR_DL_FRAME_PARMS *frame_parms,
                                unsigned char symbol,
                                int length,
                                uint8_t is_dmrs_symbol,
                                unsigned char mod_order,
                                uint8_t  nrOfLayers,
                                unsigned short nb_rb,
                                unsigned char output_shift);

/*!
\brief This function implements the idft transform precoding in PUSCH
\param z Pointer to input in frequnecy domain, and it is also the output in time domain
\param Msc_PUSCH number of allocated data subcarriers
*/
void nr_idft(int32_t *z, uint32_t Msc_PUSCH);

/** \brief This function generates log-likelihood ratios (decoder input) for single-stream QPSK received waveforms.
    @param rxdataF_comp Compensated channel output
    @param ulsch_llr llr output
    @param nb_re number of REs for this allocation
    @param symbol OFDM symbol index in sub-frame
*/
void nr_ulsch_qpsk_llr(int32_t *rxdataF_comp,
                       int16_t *ulsch_llr,                          
                       uint32_t nb_re,
                       uint8_t  symbol);


/** \brief This function generates log-likelihood ratios (decoder input) for single-stream 16 QAM received waveforms.
    @param rxdataF_comp Compensated channel output
    @param ul_ch_mag uplink channel magnitude multiplied by the 1st amplitude threshold in QAM 16
    @param ulsch_llr llr output
    @param nb_re number of RBs for this allocation
    @param symbol OFDM symbol index in sub-frame
*/
void nr_ulsch_16qam_llr(int32_t *rxdataF_comp,
                        int32_t **ul_ch_mag,
                        int16_t  *ulsch_llr,
                        uint32_t nb_rb,
                        uint32_t nb_re,
                        uint8_t  symbol);


/** \brief This function generates log-likelihood ratios (decoder input) for single-stream 64 QAM received waveforms.
    @param rxdataF_comp Compensated channel output
    @param ul_ch_mag  uplink channel magnitude multiplied by the 1st amplitude threshold in QAM 64
    @param ul_ch_magb uplink channel magnitude multiplied by the 2bd amplitude threshold in QAM 64
    @param ulsch_llr llr output
    @param nb_re number of REs for this allocation
    @param symbol OFDM symbol index in sub-frame
*/
void nr_ulsch_64qam_llr(int32_t *rxdataF_comp,
                        int32_t **ul_ch_mag,
                        int32_t **ul_ch_magb,
                        int16_t  *ulsch_llr,
                        uint32_t nb_rb,
                        uint32_t nb_re,
                        uint8_t  symbol);

/** \brief This function generates log-likelihood ratios (decoder input) for single-stream 256 QAM received waveforms.
    @param rxdataF_comp Compensated channel output
    @param ul_ch_mag  uplink channel magnitude multiplied by the 1st amplitude threshold in QAM 256
    @param ul_ch_magb uplink channel magnitude multiplied by the 2bd amplitude threshold in QAM 256
    @param ul_ch_magc uplink channel magnitude multiplied by the 3rd amplitude threshold in QAM 256 
    @param ulsch_llr llr output
    @param nb_re number of REs for this allocation
    @param symbol OFDM symbol index in sub-frame
*/
void nr_ulsch_256qam_llr(int32_t *rxdataF_comp,
                        int32_t **ul_ch_mag,
                        int32_t **ul_ch_magb,
                        int32_t **ul_ch_magc,
                        int16_t  *ulsch_llr,
                        uint32_t nb_rb,
                        uint32_t nb_re,
                        uint8_t  symbol);

/** \brief This function computes the log-likelihood ratios for 4, 16, and 64 QAM
    @param rxdataF_comp Compensated channel output
    @param ul_ch_mag  uplink channel magnitude multiplied by the 1st amplitude threshold in QAM 64
    @param ul_ch_magb uplink channel magnitude multiplied by the 2bd amplitude threshold in QAM 64
    @param ulsch_llr llr output
    @param nb_re number of REs for this allocation
    @param symbol OFDM symbol index in sub-frame
    @param mod_order modulation order
*/
void nr_ulsch_compute_llr(int32_t *rxdataF_comp,
                          int32_t *ul_ch_mag,
                          int32_t *ul_ch_magb,
                          int32_t *ul_ch_magc,
                          int16_t  *ulsch_llr,
                          uint32_t nb_rb,
                          uint32_t nb_re,
                          uint8_t  symbol,
                          uint8_t  mod_order);

void reset_active_stats(PHY_VARS_gNB *gNB, int frame);
void reset_active_ulsch(PHY_VARS_gNB *gNB, int frame);

void nr_fill_ulsch(PHY_VARS_gNB *gNB,
                   int frame,
                   int slot,
                   nfapi_nr_pusch_pdu_t *ulsch_pdu);

void nr_fill_prach(PHY_VARS_gNB *gNB,
                   int SFN,
                   int Slot,
                   nfapi_nr_prach_pdu_t *prach_pdu);

void rx_nr_prach(PHY_VARS_gNB *gNB,
                 nfapi_nr_prach_pdu_t *prach_pdu,
		 int prachOccasion,
                 int frame,
                 int subframe,
                 uint16_t *max_preamble,
                 uint16_t *max_preamble_energy,
                 uint16_t *max_preamble_delay);

void rx_nr_prach_ru(RU_t *ru,
                    int prach_fmt,
                    int numRA,
                    int prachStartSymbol,
		    int prachOccasion,
                    int frame,
                    int subframe);

void nr_fill_prach_ru(RU_t *ru,
                      int SFN,
                      int Slot,
                      nfapi_nr_prach_pdu_t *prach_pdu);

int16_t find_nr_prach(PHY_VARS_gNB *gNB,int frame,int slot, find_type_t type);
int16_t find_nr_prach_ru(RU_t *ru,int frame,int slot, find_type_t type);

void nr_fill_pucch(PHY_VARS_gNB *gNB,
                   int frame,
                   int slot,
                   nfapi_nr_pucch_pdu_t *pucch_pdu);

void nr_fill_srs(PHY_VARS_gNB *gNB,
                 frame_t frame,
                 slot_t slot,
                 nfapi_nr_srs_pdu_t *srs_pdu);

int nr_get_srs_signal(PHY_VARS_gNB *gNB,
                      frame_t frame,
                      slot_t slot,
                      nfapi_nr_srs_pdu_t *srs_pdu,
                      nr_srs_info_t *nr_srs_info,
                      int32_t srs_received_signal[][gNB->frame_parms.ofdm_symbol_size*(1<<srs_pdu->num_symbols)]);

void init_prach_list(PHY_VARS_gNB *gNB);
void init_prach_ru_list(RU_t *ru);
void free_nr_ru_prach_entry(RU_t *ru, int prach_id);
uint8_t get_nr_prach_duration(uint8_t prach_format);

void nr_generate_csi_rs(const NR_DL_FRAME_PARMS *frame_parms,
                        int32_t **dataF,
                        const int16_t amp,
                        nr_csi_info_t *nr_csi_info,
                        const nfapi_nr_dl_tti_csi_rs_pdu_rel15_t *csi_params,
                        const int slot,
                        uint8_t *N_cdm_groups,
                        uint8_t *CDM_group_size,
                        uint8_t *k_prime,
                        uint8_t *l_prime,
                        uint8_t *N_ports,
                        uint8_t *j_cdm,
                        uint8_t *k_overline,
                        uint8_t *l_overline);

void free_nr_prach_entry(PHY_VARS_gNB *gNB, int prach_id);

void nr_decode_pucch1(c16_t **rxdataF,
                      pucch_GroupHopping_t pucch_GroupHopping,
                      uint32_t n_id,       // hoppingID higher layer parameter
                      uint64_t *payload,
                      NR_DL_FRAME_PARMS *frame_parms,
                      int16_t amp,
                      int nr_tti_tx,
                      uint8_t m0,
                      uint8_t nrofSymbols,
                      uint8_t startingSymbolIndex,
                      uint16_t startingPRB,
                      uint16_t startingPRB_intraSlotHopping,
                      uint8_t timeDomainOCC,
                      uint8_t nr_bit);

void nr_decode_pucch2(PHY_VARS_gNB *gNB,
                      int frame,
                      int slot,
                      nfapi_nr_uci_pucch_pdu_format_2_3_4_t* uci_pdu,
                      nfapi_nr_pucch_pdu_t* pucch_pdu);

void nr_decode_pucch0(PHY_VARS_gNB *gNB,
                      int frame,
                      int slot,
                      nfapi_nr_uci_pucch_pdu_format_0_1_t* uci_pdu,
                      nfapi_nr_pucch_pdu_t* pucch_pdu);


#endif /*__NR_TRANSPORT__H__*/
