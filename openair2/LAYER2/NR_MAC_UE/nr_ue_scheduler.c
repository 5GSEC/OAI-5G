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

/* \file        nr_ue_scheduler.c
 * \brief       Routines for UE scheduling
 * \author      Guido Casati
 * \date        Jan 2021
 * \version     0.1
 * \company     Fraunhofer IIS
 * \email       guido.casati@iis.fraunhofer.de
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>

/* exe */
#include <common/utils/nr/nr_common.h>

/* PHY */
#include "openair1/PHY/impl_defs_top.h"

/* MAC */
#include "NR_MAC_COMMON/nr_mac.h"
#include "NR_MAC_COMMON/nr_mac_common.h"
#include "NR_MAC_UE/mac_proto.h"
#include "NR_MAC_UE/mac_extern.h"

/* utils */
#include "assertions.h"
#include "oai_asn1.h"
#include "SIMULATION/TOOLS/sim.h" // for taus
#include "utils.h"

#include <executables/softmodem-common.h>

#include "LAYER2/NR_MAC_COMMON/nr_mac_extern.h"
#include "LAYER2/RLC/rlc.h"

//#define SRS_DEBUG

static prach_association_pattern_t prach_assoc_pattern;
static ssb_list_info_t ssb_list;

void fill_ul_config(fapi_nr_ul_config_request_t *ul_config, frame_t frame_tx, int slot_tx, uint8_t pdu_type){

  AssertFatal(ul_config->number_pdus < sizeof(ul_config->ul_config_list) / sizeof(ul_config->ul_config_list[0]),
              "Number of PDUS in ul_config = %d > ul_config_list num elements", ul_config->number_pdus);
  // clear ul_config for new frame/slot
  if ((ul_config->slot != slot_tx || ul_config->sfn != frame_tx) &&
      ul_config->number_pdus != 0 &&
      !get_softmodem_params()->emulate_l1) {
    LOG_D(MAC, "%d.%d %d.%d f clear ul_config %p t %d pdu %d\n", frame_tx, slot_tx, ul_config->sfn, ul_config->slot, ul_config, pdu_type, ul_config->number_pdus);
    ul_config->number_pdus = 0;
    memset(ul_config->ul_config_list, 0, sizeof(ul_config->ul_config_list)); 
  }
  ul_config->ul_config_list[ul_config->number_pdus].pdu_type = pdu_type;
  //ul_config->slot = slot_tx;
  //ul_config->sfn = frame_tx;
  ul_config->slot = slot_tx;
  ul_config->sfn = frame_tx;
  ul_config->number_pdus++;

  LOG_D(NR_MAC, "In %s: Set config request for UL transmission in [%d.%d], number of UL PDUs: %d\n", __FUNCTION__, ul_config->sfn, ul_config->slot, ul_config->number_pdus);

}

void fill_scheduled_response(nr_scheduled_response_t *scheduled_response,
                             fapi_nr_dl_config_request_t *dl_config,
                             fapi_nr_ul_config_request_t *ul_config,
                             fapi_nr_tx_request_t *tx_request,
                             module_id_t mod_id,
                             int cc_id,
                             frame_t frame,
                             int slot,
                             void *phy_data){

  scheduled_response->dl_config  = dl_config;
  scheduled_response->ul_config  = ul_config;
  scheduled_response->tx_request = tx_request;
  scheduled_response->module_id  = mod_id;
  scheduled_response->CC_id      = cc_id;
  scheduled_response->frame      = frame;
  scheduled_response->slot       = slot;
  scheduled_response->phy_data   = phy_data;

}

/*
 * This function returns the UL config corresponding to a given UL slot
 * from MAC instance .
 */
fapi_nr_ul_config_request_t *get_ul_config_request(NR_UE_MAC_INST_t *mac, int slot, int fb_time)
{

  NR_TDD_UL_DL_ConfigCommon_t *tdd_config = mac->scc==NULL ? mac->scc_SIB->tdd_UL_DL_ConfigurationCommon : mac->scc->tdd_UL_DL_ConfigurationCommon;

  //Check if requested on the right slot
  AssertFatal(is_nr_UL_slot(tdd_config, slot, mac->frame_type) != 0, "UL config_request called at wrong slot %d\n", slot);

  int mu = mac->current_UL_BWP.scs;
  const int n = nr_slots_per_frame[mu];
  AssertFatal(fb_time < n, "Cannot schedule to a slot more than 1 frame away, ul_config_request is not big enough\n");
  AssertFatal(mac->ul_config_request != NULL, "mac->ul_config_request not initialized, logic bug\n");
  return &mac->ul_config_request[slot];
}

/*
 * This function returns the DL config corresponding to a given DL slot
 * from MAC instance .
 */
fapi_nr_dl_config_request_t *get_dl_config_request(NR_UE_MAC_INST_t *mac, int slot)
{
  AssertFatal(mac->dl_config_request != NULL, "mac->dl_config_request not initialized, logic bug\n");
  return &mac->dl_config_request[slot];
}

void ul_layers_config(NR_UE_MAC_INST_t *mac, nfapi_nr_ue_pusch_pdu_t *pusch_config_pdu, dci_pdu_rel15_t *dci, nr_dci_format_t dci_format)
{
  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;
  NR_SRS_Config_t *srs_config = current_UL_BWP->srs_Config;
  NR_PUSCH_Config_t *pusch_Config = current_UL_BWP->pusch_Config;

  long transformPrecoder = pusch_config_pdu->transform_precoding;

  /* PRECOD_NBR_LAYERS */
  // 0 bits if the higher layer parameter txConfig = nonCodeBook

  if (*pusch_Config->txConfig == NR_PUSCH_Config__txConfig_codebook){

    // The UE shall transmit PUSCH using the same antenna port(s) as the SRS port(s) in the SRS resource indicated by the DCI format 0_1
    // 38.214  Section 6.1.1

    uint8_t n_antenna_port = get_pusch_nb_antenna_ports(pusch_Config, srs_config, dci->srs_resource_indicator);

    // 1 antenna port and the higher layer parameter txConfig = codebook 0 bits

    if (n_antenna_port == 4) { // 4 antenna port and the higher layer parameter txConfig = codebook

      // Table 7.3.1.1.2-2: transformPrecoder=disabled and maxRank = 2 or 3 or 4
      if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled)
        && ((*pusch_Config->maxRank == 2) ||
        (*pusch_Config->maxRank == 3) ||
        (*pusch_Config->maxRank == 4))){

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_fullyAndPartialAndNonCoherent) {
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][0];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][1];
        }

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_partialAndNonCoherent){
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][2];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][3];
        }

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_nonCoherent){
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][4];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][5];
        }
      }

      // Table 7.3.1.1.2-3: transformPrecoder= enabled, or transformPrecoder=disabled and maxRank = 1
      if (((transformPrecoder == NR_PUSCH_Config__transformPrecoder_enabled)
        || (transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled))
        && (*pusch_Config->maxRank == 1)) {

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_fullyAndPartialAndNonCoherent) {
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][6];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][7];
        }

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_partialAndNonCoherent){
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][8];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][9];
        }

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_nonCoherent){
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][10];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][11];
        }
      }
    }

    if (n_antenna_port == 2) {
      // 2 antenna port and the higher layer parameter txConfig = codebook
      // Table 7.3.1.1.2-4: transformPrecoder=disabled and maxRank = 2
      if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled) && (*pusch_Config->maxRank == 2)) {
        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_fullyAndPartialAndNonCoherent) {
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][12];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][13];
        }

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_nonCoherent){
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][14];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][15];
        }

      }

      // Table 7.3.1.1.2-5: transformPrecoder= enabled, or transformPrecoder= disabled and maxRank = 1
      if (((transformPrecoder == NR_PUSCH_Config__transformPrecoder_enabled)
        || (transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled))
        && (*pusch_Config->maxRank == 1)) {

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_fullyAndPartialAndNonCoherent) {
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][16];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][17];
        }

        if (*pusch_Config->codebookSubset == NR_PUSCH_Config__codebookSubset_nonCoherent){
          pusch_config_pdu->nrOfLayers = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][18];
          pusch_config_pdu->Tpmi = table_7_3_1_1_2_2_3_4_5[dci->precoding_information.val][19];
        }

      }
    }
  }
}

// todo: this function shall be reviewed completely because of the many comments left by the author
void ul_ports_config(NR_UE_MAC_INST_t *mac, int *n_front_load_symb, nfapi_nr_ue_pusch_pdu_t *pusch_config_pdu, dci_pdu_rel15_t *dci, nr_dci_format_t dci_format)
{
  uint8_t rank = pusch_config_pdu->nrOfLayers;

  NR_PUSCH_Config_t *pusch_Config = mac->current_UL_BWP.pusch_Config;
  AssertFatal(pusch_Config!=NULL,"pusch_Config shouldn't be null\n");

  long transformPrecoder = pusch_config_pdu->transform_precoding;
  LOG_D(NR_MAC,"transformPrecoder %s\n", transformPrecoder==NR_PUSCH_Config__transformPrecoder_disabled ? "disabled" : "enabled");

  long *max_length = NULL;
  long *dmrs_type = NULL;
  if (pusch_Config->dmrs_UplinkForPUSCH_MappingTypeA) {
    max_length = pusch_Config->dmrs_UplinkForPUSCH_MappingTypeA->choice.setup->maxLength;
    dmrs_type = pusch_Config->dmrs_UplinkForPUSCH_MappingTypeA->choice.setup->dmrs_Type;
  }
  else {
    max_length = pusch_Config->dmrs_UplinkForPUSCH_MappingTypeB->choice.setup->maxLength;
    dmrs_type = pusch_Config->dmrs_UplinkForPUSCH_MappingTypeB->choice.setup->dmrs_Type;
  }

  LOG_D(NR_MAC,"MappingType%s max_length %s, dmrs_type %s, antenna_ports %d\n",
        pusch_Config->dmrs_UplinkForPUSCH_MappingTypeA?"A":"B",max_length?"len2":"len1",dmrs_type?"type2":"type1",dci->antenna_ports.val);

  if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_enabled) &&
      (dmrs_type == NULL) && (max_length == NULL)) { // tables 7.3.1.1.2-6
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2;
      pusch_config_pdu->dmrs_ports = 1<<dci->antenna_ports.val;
  }

  if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_enabled) &&
      (dmrs_type == NULL) && (max_length != NULL)) { // tables 7.3.1.1.2-7

    pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2; //TBC
    pusch_config_pdu->dmrs_ports = 1<<((dci->antenna_ports.val > 3)?(dci->antenna_ports.val-4):(dci->antenna_ports.val));
    *n_front_load_symb = (dci->antenna_ports.val > 3)?2:1;
  }

  if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled) &&
    (dmrs_type == NULL) && (max_length == NULL)) { // tables 7.3.1.1.2-8/9/10/11

    if (rank == 1) {
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = (dci->antenna_ports.val > 1)?2:1;
      pusch_config_pdu->dmrs_ports =1<<((dci->antenna_ports.val > 1)?(dci->antenna_ports.val-2):(dci->antenna_ports.val));
    }

    if (rank == 2){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = (dci->antenna_ports.val > 0)?2:1;
      pusch_config_pdu->dmrs_ports = (dci->antenna_ports.val > 1)?((dci->antenna_ports.val> 2)?0x5:0xc):0x3;
    }

    if (rank == 3){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2;
      pusch_config_pdu->dmrs_ports = 0x7;  // ports 0-2
    }

    if (rank == 4){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2;
      pusch_config_pdu->dmrs_ports = 0xf;  // ports 0-3
    }
  }

  if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled) &&
    (dmrs_type == NULL) && (max_length != NULL)) { // tables 7.3.1.1.2-12/13/14/15

    if (rank == 1){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = (dci->antenna_ports.val > 1)?2:1; //TBC
      pusch_config_pdu->dmrs_ports = 1<<((dci->antenna_ports.val > 1)?(dci->antenna_ports.val > 5 ?(dci->antenna_ports.val-6):(dci->antenna_ports.val-2)):dci->antenna_ports.val);
      *n_front_load_symb = (dci->antenna_ports.val > 6)?2:1;
    }

    if (rank == 2){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = (dci->antenna_ports.val > 0)?2:1; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_13[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_13[dci->antenna_ports.val][2];
      //n_front_load_symb = (dci->antenna_ports.val > 3)?2:1; // FIXME
    }

    if (rank == 3){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_14[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_14[dci->antenna_ports.val][2];
      //pusch_config_pdu->dmrs_ports[2] = table_7_3_1_1_2_14[dci->antenna_ports.val][3];
      //n_front_load_symb = (dci->antenna_ports.val > 1)?2:1; //FIXME
    }

    if (rank == 4){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_15[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_15[dci->antenna_ports.val][2];
      //pusch_config_pdu->dmrs_ports[2] = table_7_3_1_1_2_15[dci->antenna_ports.val][3];
      //pusch_config_pdu->dmrs_ports[3] = table_7_3_1_1_2_15[dci->antenna_ports.val][4];
      //n_front_load_symb = (dci->antenna_ports.val > 1)?2:1; //FIXME
    }
  }

  if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled) &&
    (dmrs_type != NULL) &&
    (max_length == NULL)) { // tables 7.3.1.1.2-16/17/18/19

    if (rank == 1){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = (dci->antenna_ports.val > 1)?((dci->antenna_ports.val > 5)?3:2):1; //TBC
      pusch_config_pdu->dmrs_ports = (dci->antenna_ports.val > 1)?(dci->antenna_ports.val > 5 ?(dci->antenna_ports.val-6):(dci->antenna_ports.val-2)):dci->antenna_ports.val; //TBC
    }

    if (rank == 2){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = (dci->antenna_ports.val > 0)?((dci->antenna_ports.val > 2)?3:2):1; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_17[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_17[dci->antenna_ports.val][2];
    }

    if (rank == 3){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = (dci->antenna_ports.val > 0)?3:2; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_18[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_18[dci->antenna_ports.val][2];
      //pusch_config_pdu->dmrs_ports[2] = table_7_3_1_1_2_18[dci->antenna_ports.val][3];
    }

    if (rank == 4){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = dci->antenna_ports.val + 2; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = 0;
      //pusch_config_pdu->dmrs_ports[1] = 1;
      //pusch_config_pdu->dmrs_ports[2] = 2;
      //pusch_config_pdu->dmrs_ports[3] = 3;
    }
  }

  if ((transformPrecoder == NR_PUSCH_Config__transformPrecoder_disabled) &&
    (dmrs_type != NULL) && (max_length != NULL)) { // tables 7.3.1.1.2-20/21/22/23

    if (rank == 1){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = table_7_3_1_1_2_20[dci->antenna_ports.val][0]; //TBC
      pusch_config_pdu->dmrs_ports = table_7_3_1_1_2_20[dci->antenna_ports.val][1]; //TBC
      //n_front_load_symb = table_7_3_1_1_2_20[dci->antenna_ports.val][2]; //FIXME
    }

    if (rank == 2){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = table_7_3_1_1_2_21[dci->antenna_ports.val][0]; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_21[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_21[dci->antenna_ports.val][2];
      //n_front_load_symb = table_7_3_1_1_2_21[dci->antenna_ports.val][3]; //FIXME
      }

    if (rank == 3){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = table_7_3_1_1_2_22[dci->antenna_ports.val][0]; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_22[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_22[dci->antenna_ports.val][2];
      //pusch_config_pdu->dmrs_ports[2] = table_7_3_1_1_2_22[dci->antenna_ports.val][3];
      //n_front_load_symb = table_7_3_1_1_2_22[dci->antenna_ports.val][4]; //FIXME
    }

    if (rank == 4){
      pusch_config_pdu->num_dmrs_cdm_grps_no_data = table_7_3_1_1_2_23[dci->antenna_ports.val][0]; //TBC
      pusch_config_pdu->dmrs_ports = 0; //FIXME
      //pusch_config_pdu->dmrs_ports[0] = table_7_3_1_1_2_23[dci->antenna_ports.val][1];
      //pusch_config_pdu->dmrs_ports[1] = table_7_3_1_1_2_23[dci->antenna_ports.val][2];
      //pusch_config_pdu->dmrs_ports[2] = table_7_3_1_1_2_23[dci->antenna_ports.val][3];
      //pusch_config_pdu->dmrs_ports[3] = table_7_3_1_1_2_23[dci->antenna_ports.val][4];
      //n_front_load_symb = table_7_3_1_1_2_23[dci->antenna_ports.val][5]; //FIXME
    }
  }
  LOG_D(NR_MAC,"num_dmrs_cdm_grps_no_data %d, dmrs_ports %d\n",pusch_config_pdu->num_dmrs_cdm_grps_no_data,pusch_config_pdu->dmrs_ports);
}

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
                        const nr_dci_format_t *dci_format)
{

  int f_alloc;
  int mask;
  uint8_t nb_dmrs_re_per_rb;

  uint16_t        l_prime_mask = 0;
  uint16_t number_dmrs_symbols = 0;
  int                N_PRB_oh  = 0;

  int rnti_type = get_rnti_type(mac, rnti);
  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;

  // Common configuration
  pusch_config_pdu->dmrs_config_type = pusch_dmrs_type1;
  pusch_config_pdu->pdu_bit_map      = PUSCH_PDU_BITMAP_PUSCH_DATA;
  pusch_config_pdu->nrOfLayers       = 1;
  pusch_config_pdu->Tpmi             = 0;
  pusch_config_pdu->rnti             = rnti;

  pusch_dmrs_AdditionalPosition_t add_pos = pusch_dmrs_pos2;
  int dmrslength = 1;
  NR_PUSCH_Config_t *pusch_Config = current_UL_BWP->pusch_Config;

  if (rar_grant) {

    // Note: for Msg3 or MsgA PUSCH transmission the N_PRB_oh is always set to 0
    int ibwp_start = current_UL_BWP->initial_BWPStart;
    int ibwp_size = current_UL_BWP->initial_BWPSize;
    int abwp_start = current_UL_BWP->BWPStart;
    int abwp_size = current_UL_BWP->BWPSize;
    int scs = current_UL_BWP->scs;

      // BWP start selection according to 8.3 of TS 38.213
    if ((ibwp_start < abwp_start) || (ibwp_size > abwp_size)) {
      pusch_config_pdu->bwp_start = abwp_start;
      pusch_config_pdu->bwp_size = abwp_size;
    } else {
      pusch_config_pdu->bwp_start = ibwp_start;
      pusch_config_pdu->bwp_size = ibwp_size;
    }

    //// Resource assignment from RAR
    // Frequency domain allocation according to 8.3 of TS 38.213
    if (ibwp_size < 180)
      mask = (1 << ((int) ceil(log2((ibwp_size*(ibwp_size+1))>>1)))) - 1;
    else
      mask = (1 << (28 - (int)(ceil(log2((ibwp_size*(ibwp_size+1))>>1))))) - 1;

    f_alloc = rar_grant->Msg3_f_alloc & mask;
    if (nr_ue_process_dci_freq_dom_resource_assignment(pusch_config_pdu, NULL, ibwp_size, 0, f_alloc) < 0)
      return -1;

    // virtual resource block to physical resource mapping for Msg3 PUSCH (6.3.1.7 in 38.211)
    //pusch_config_pdu->rb_start += ibwp_start - abwp_start;

    // Time domain allocation
    pusch_config_pdu->start_symbol_index = tda_info->startSymbolIndex;
    pusch_config_pdu->nr_of_symbols = tda_info->nrOfSymbols;

    l_prime_mask =
        get_l_prime(tda_info->nrOfSymbols, tda_info->mapping_type, add_pos, dmrslength, tda_info->startSymbolIndex, mac->scc ? mac->scc->dmrs_TypeA_Position : mac->mib->dmrs_TypeA_Position);
    LOG_D(NR_MAC, "MSG3 start_sym:%d NR Symb:%d mappingtype:%d, DMRS_MASK:%x\n", pusch_config_pdu->start_symbol_index, pusch_config_pdu->nr_of_symbols, tda_info->mapping_type, l_prime_mask);

#ifdef DEBUG_MSG3
    LOG_D(NR_MAC, "In %s BWP assignment (BWP (start %d, size %d) \n", __FUNCTION__, pusch_config_pdu->bwp_start, pusch_config_pdu->bwp_size);
#endif

    // MCS
    pusch_config_pdu->mcs_index = rar_grant->mcs;
    // Frequency hopping
    pusch_config_pdu->frequency_hopping = rar_grant->freq_hopping;

    // DM-RS configuration according to 6.2.2 UE DM-RS transmission procedure in 38.214
    pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2;
    pusch_config_pdu->dmrs_ports = 1;

    // DMRS sequence initialization [TS 38.211, sec 6.4.1.1.1].
    // Should match what is sent in DCI 0_1, otherwise set to 0.
    pusch_config_pdu->scid = 0;

    // Transform precoding according to 6.1.3 UE procedure for applying transform precoding on PUSCH in 38.214
    pusch_config_pdu->transform_precoding = get_transformPrecoding(current_UL_BWP, NR_UL_DCI_FORMAT_0_0, 0); // as if it was DCI 0_0

    // Resource allocation in frequency domain according to 6.1.2.2 in TS 38.214
    pusch_config_pdu->resource_alloc = 1;

    //// Completing PUSCH PDU
    pusch_config_pdu->mcs_table = 0;
    pusch_config_pdu->cyclic_prefix = 0;
    pusch_config_pdu->data_scrambling_id = mac->physCellId;
    pusch_config_pdu->ul_dmrs_scrambling_id = mac->physCellId;
    pusch_config_pdu->subcarrier_spacing = scs;
    pusch_config_pdu->vrb_to_prb_mapping = 0;
    pusch_config_pdu->uplink_frequency_shift_7p5khz = 0;
    //Optional Data only included if indicated in pduBitmap
    pusch_config_pdu->pusch_data.rv_index = 0;  // 8.3 in 38.213
    pusch_config_pdu->pusch_data.harq_process_id = 0;
    pusch_config_pdu->pusch_data.new_data_indicator = 1; // new data
    pusch_config_pdu->pusch_data.num_cb = 0;
    pusch_config_pdu->tbslbrm = 0;

  } else if (dci) {
    pusch_config_pdu->bwp_start = current_UL_BWP->BWPStart;
    pusch_config_pdu->bwp_size = current_UL_BWP->BWPSize;

    // Basic sanity check for MCS value to check for a false or erroneous DCI
    if (dci->mcs > 28) {
      LOG_W(NR_MAC, "MCS value %d out of bounds! Possibly due to false DCI. Ignoring DCI!\n", dci->mcs);
      return -1;
    }

    /* Transform precoding */
    pusch_config_pdu->transform_precoding = get_transformPrecoding(current_UL_BWP, *dci_format, 0);

    /*DCI format-related configuration*/
    int target_ss;
    if (*dci_format == NR_UL_DCI_FORMAT_0_0) {
      target_ss = NR_SearchSpace__searchSpaceType_PR_common;
      if ((pusch_config_pdu->transform_precoding == NR_PUSCH_Config__transformPrecoder_disabled) &&
          pusch_config_pdu->nr_of_symbols < 3)
        pusch_config_pdu->num_dmrs_cdm_grps_no_data = 1;
      else
        pusch_config_pdu->num_dmrs_cdm_grps_no_data = 2;
    } else if (*dci_format == NR_UL_DCI_FORMAT_0_1) {
      target_ss = NR_SearchSpace__searchSpaceType_PR_ue_Specific;
      ul_layers_config(mac, pusch_config_pdu, dci, *dci_format);
      ul_ports_config(mac, &dmrslength, pusch_config_pdu, dci, *dci_format);
    } else {
      LOG_E(NR_MAC, "In %s: UL grant from DCI format %d is not handled...\n", __FUNCTION__, *dci_format);
      return -1;
    }

    int mappingtype = tda_info->mapping_type;

    NR_DMRS_UplinkConfig_t *NR_DMRS_ulconfig = NULL;
    if(pusch_Config) {
      NR_DMRS_ulconfig = (mappingtype == NR_PUSCH_TimeDomainResourceAllocation__mappingType_typeA)
                         ? pusch_Config->dmrs_UplinkForPUSCH_MappingTypeA->choice.setup : pusch_Config->dmrs_UplinkForPUSCH_MappingTypeB->choice.setup;
    }

    pusch_config_pdu->scid = 0;
    pusch_config_pdu->ul_dmrs_scrambling_id = mac->physCellId;
    if(*dci_format == NR_UL_DCI_FORMAT_0_1)
      pusch_config_pdu->scid = dci->dmrs_sequence_initialization.val;

    /* TRANSFORM PRECODING ------------------------------------------------------------------------------------------*/
    if (pusch_config_pdu->transform_precoding == NR_PUSCH_Config__transformPrecoder_enabled) {

      uint32_t n_RS_Id = 0;
      if (NR_DMRS_ulconfig->transformPrecodingEnabled &&
          NR_DMRS_ulconfig->transformPrecodingEnabled->nPUSCH_Identity != NULL)
        n_RS_Id = *NR_DMRS_ulconfig->transformPrecodingEnabled->nPUSCH_Identity;
      else
        n_RS_Id = mac->physCellId;

      // U as specified in section 6.4.1.1.1.2 in 38.211, if sequence hopping and group hopping are disabled
      pusch_config_pdu->dfts_ofdm.low_papr_group_number = n_RS_Id % 30;

      // V as specified in section 6.4.1.1.1.2 in 38.211 V = 0 if sequence hopping and group hopping are disabled
      if (!NR_DMRS_ulconfig || !NR_DMRS_ulconfig->transformPrecodingEnabled ||
          (!NR_DMRS_ulconfig->transformPrecodingEnabled->sequenceGroupHopping && !NR_DMRS_ulconfig->transformPrecodingEnabled->sequenceHopping))
        pusch_config_pdu->dfts_ofdm.low_papr_sequence_number = 0;
      else
        AssertFatal(1==0,"SequenceGroupHopping or sequenceHopping are NOT Supported\n");

      LOG_D(NR_MAC,"TRANSFORM PRECODING IS ENABLED. CDM groups: %d, U: %d \n", pusch_config_pdu->num_dmrs_cdm_grps_no_data,
                pusch_config_pdu->dfts_ofdm.low_papr_group_number);
    }
    else {
      if (pusch_config_pdu->scid == 0 && NR_DMRS_ulconfig &&
          NR_DMRS_ulconfig->transformPrecodingDisabled->scramblingID0)
        pusch_config_pdu->ul_dmrs_scrambling_id = *NR_DMRS_ulconfig->transformPrecodingDisabled->scramblingID0;
      if (pusch_config_pdu->scid == 1 && NR_DMRS_ulconfig &&
          NR_DMRS_ulconfig->transformPrecodingDisabled->scramblingID1)
        pusch_config_pdu->ul_dmrs_scrambling_id = *NR_DMRS_ulconfig->transformPrecodingDisabled->scramblingID1;
    }

    /* TRANSFORM PRECODING --------------------------------------------------------------------------------------------------------*/

    /* IDENTIFIER_DCI_FORMATS */
    /* FREQ_DOM_RESOURCE_ASSIGNMENT_UL */
    if (nr_ue_process_dci_freq_dom_resource_assignment(pusch_config_pdu, NULL, current_UL_BWP->BWPSize, 0, dci->frequency_domain_assignment.val) < 0){
      return -1;
    }

    pusch_config_pdu->start_symbol_index = tda_info->startSymbolIndex;
    pusch_config_pdu->nr_of_symbols = tda_info->nrOfSymbols;

    /* FREQ_HOPPING_FLAG */
    if ((pusch_Config!=NULL) && (pusch_Config->frequencyHopping!=NULL) && (pusch_Config->resourceAllocation != NR_PUSCH_Config__resourceAllocation_resourceAllocationType0)){
      pusch_config_pdu->frequency_hopping = dci->frequency_hopping_flag.val;
    }

    /* MCS */
    pusch_config_pdu->mcs_index = dci->mcs;

    /* MCS TABLE */
    if (pusch_config_pdu->transform_precoding == NR_PUSCH_Config__transformPrecoder_disabled) {
      pusch_config_pdu->mcs_table = get_pusch_mcs_table(pusch_Config ? pusch_Config->mcs_Table : NULL, 0, *dci_format, rnti_type, target_ss, false);
    } else {
      pusch_config_pdu->mcs_table = get_pusch_mcs_table(pusch_Config ? pusch_Config->mcs_TableTransformPrecoder : NULL, 1, *dci_format, rnti_type, target_ss, false);
    }

    /* NDI */
    pusch_config_pdu->pusch_data.new_data_indicator = dci->ndi;
    /* RV */
    pusch_config_pdu->pusch_data.rv_index = dci->rv;
    /* HARQ_PROCESS_NUMBER */
    pusch_config_pdu->pusch_data.harq_process_id = dci->harq_pid;
    /* TPC_PUSCH */
    // according to TS 38.213 Table Table 7.1.1-1
    if (dci->tpc == 0) {
      pusch_config_pdu->absolute_delta_PUSCH = -4;
    }
    if (dci->tpc == 1) {
      pusch_config_pdu->absolute_delta_PUSCH = -1;
    }
    if (dci->tpc == 2) {
      pusch_config_pdu->absolute_delta_PUSCH = 1;
    }
    if (dci->tpc == 3) {
      pusch_config_pdu->absolute_delta_PUSCH = 4;
    }

    if (NR_DMRS_ulconfig != NULL)
      add_pos = (NR_DMRS_ulconfig->dmrs_AdditionalPosition == NULL) ? 2 : *NR_DMRS_ulconfig->dmrs_AdditionalPosition;

    /* DMRS */
    l_prime_mask = get_l_prime(pusch_config_pdu->nr_of_symbols,
                               mappingtype, add_pos, dmrslength,
                               pusch_config_pdu->start_symbol_index,
                               mac->scc ? mac->scc->dmrs_TypeA_Position : mac->mib->dmrs_TypeA_Position);

    // Num PRB Overhead from PUSCH-ServingCellConfig
    if (current_UL_BWP->pusch_servingcellconfig && current_UL_BWP->pusch_servingcellconfig->xOverhead)
      N_PRB_oh = *current_UL_BWP->pusch_servingcellconfig->xOverhead;
    else
      N_PRB_oh = 0;

    if (current_UL_BWP->pusch_servingcellconfig && current_UL_BWP->pusch_servingcellconfig->rateMatching) {
      long *maxMIMO_Layers = current_UL_BWP->pusch_servingcellconfig->ext1->maxMIMO_Layers;
      if (!maxMIMO_Layers)
        maxMIMO_Layers = pusch_Config ? pusch_Config->maxRank : NULL;
      AssertFatal (maxMIMO_Layers != NULL,"Option with max MIMO layers not configured is not supported\n");
      int bw_tbslbrm = get_ulbw_tbslbrm(current_UL_BWP->initial_BWPSize, mac->cg);
      pusch_config_pdu->tbslbrm = nr_compute_tbslbrm(pusch_config_pdu->mcs_table,
                                                     bw_tbslbrm,
                                                     *maxMIMO_Layers);
    } else
      pusch_config_pdu->tbslbrm = 0;

    /* PTRS */
    if (pusch_Config && pusch_Config->dmrs_UplinkForPUSCH_MappingTypeB && pusch_Config->dmrs_UplinkForPUSCH_MappingTypeB->choice.setup->phaseTrackingRS) {
      if (pusch_config_pdu->transform_precoding == NR_PUSCH_Config__transformPrecoder_disabled) {
        nfapi_nr_ue_ptrs_ports_t ptrs_ports_list;
        pusch_config_pdu->pusch_ptrs.ptrs_ports_list = &ptrs_ports_list;
        bool valid_ptrs_setup = set_ul_ptrs_values(pusch_Config->dmrs_UplinkForPUSCH_MappingTypeB->choice.setup->phaseTrackingRS->choice.setup,
                                                   pusch_config_pdu->rb_size,
                                                   pusch_config_pdu->mcs_index,
                                                   pusch_config_pdu->mcs_table,
                                                   &pusch_config_pdu->pusch_ptrs.ptrs_freq_density,
                                                   &pusch_config_pdu->pusch_ptrs.ptrs_time_density,
                                                   &pusch_config_pdu->pusch_ptrs.ptrs_ports_list->ptrs_re_offset,
                                                   &pusch_config_pdu->pusch_ptrs.num_ptrs_ports,
                                                   &pusch_config_pdu->pusch_ptrs.ul_ptrs_power,
                                                   pusch_config_pdu->nr_of_symbols);
        if(valid_ptrs_setup == true) {
          pusch_config_pdu->pdu_bit_map |= PUSCH_PDU_BITMAP_PUSCH_PTRS;
        }
        LOG_D(NR_MAC, "UL PTRS values: PTRS time den: %d, PTRS freq den: %d\n", pusch_config_pdu->pusch_ptrs.ptrs_time_density, pusch_config_pdu->pusch_ptrs.ptrs_freq_density);
      }
    }
  }

  LOG_D(NR_MAC, "In %s: received UL grant (rb_start %d, rb_size %d, start_symbol_index %d, nr_of_symbols %d) for RNTI type %s \n",
    __FUNCTION__,
    pusch_config_pdu->rb_start,
    pusch_config_pdu->rb_size,
    pusch_config_pdu->start_symbol_index,
    pusch_config_pdu->nr_of_symbols,
    rnti_types[rnti_type]);

  pusch_config_pdu->ul_dmrs_symb_pos = l_prime_mask;
  uint16_t R = nr_get_code_rate_ul(pusch_config_pdu->mcs_index, pusch_config_pdu->mcs_table);
  pusch_config_pdu->target_code_rate = R;
  pusch_config_pdu->qam_mod_order = nr_get_Qm_ul(pusch_config_pdu->mcs_index, pusch_config_pdu->mcs_table);

  if (pusch_config_pdu->target_code_rate == 0 || pusch_config_pdu->qam_mod_order == 0) {
    LOG_W(NR_MAC, "In %s: Invalid code rate or Mod order, likely due to unexpected UL DCI. Ignoring DCI! \n", __FUNCTION__);
    return -1;
  }

  int start_symbol = pusch_config_pdu->start_symbol_index;
  int number_of_symbols = pusch_config_pdu->nr_of_symbols;
  for (int i = start_symbol; i < start_symbol + number_of_symbols; i++) {
    if((pusch_config_pdu->ul_dmrs_symb_pos >> i) & 0x01)
      number_dmrs_symbols += 1;
  }

  nb_dmrs_re_per_rb = ((pusch_config_pdu->dmrs_config_type == pusch_dmrs_type1) ? 6:4)*pusch_config_pdu->num_dmrs_cdm_grps_no_data;

  // Compute TBS
  pusch_config_pdu->pusch_data.tb_size = nr_compute_tbs(pusch_config_pdu->qam_mod_order,
                                                        R,
                                                        pusch_config_pdu->rb_size,
                                                        pusch_config_pdu->nr_of_symbols,
                                                        nb_dmrs_re_per_rb*number_dmrs_symbols,
                                                        N_PRB_oh,
                                                        0, // TBR to verify tb scaling
                                                        pusch_config_pdu->nrOfLayers)>>3;
  return 0;

}

void configure_srs_pdu(NR_UE_MAC_INST_t *mac,
                       NR_SRS_Resource_t *srs_resource,
                       fapi_nr_ul_config_srs_pdu *srs_config_pdu,
                       int period, int offset)
{
  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;

  srs_config_pdu->rnti = mac->crnti;
  srs_config_pdu->handle = 0;
  srs_config_pdu->bwp_size = current_UL_BWP->BWPSize;
  srs_config_pdu->bwp_start = current_UL_BWP->BWPStart;
  srs_config_pdu->subcarrier_spacing = current_UL_BWP->scs;
  srs_config_pdu->cyclic_prefix = 0;
  srs_config_pdu->num_ant_ports = srs_resource->nrofSRS_Ports;
  srs_config_pdu->num_symbols = srs_resource->resourceMapping.nrofSymbols;
  srs_config_pdu->num_repetitions = srs_resource->resourceMapping.repetitionFactor;
  srs_config_pdu->time_start_position = srs_resource->resourceMapping.startPosition;
  srs_config_pdu->config_index = srs_resource->freqHopping.c_SRS;
  srs_config_pdu->sequence_id = srs_resource->sequenceId;
  srs_config_pdu->bandwidth_index = srs_resource->freqHopping.b_SRS;
  srs_config_pdu->comb_size = srs_resource->transmissionComb.present - 1;

  switch(srs_resource->transmissionComb.present) {
    case NR_SRS_Resource__transmissionComb_PR_n2:
      srs_config_pdu->comb_offset = srs_resource->transmissionComb.choice.n2->combOffset_n2;
      srs_config_pdu->cyclic_shift = srs_resource->transmissionComb.choice.n2->cyclicShift_n2;
      break;
    case NR_SRS_Resource__transmissionComb_PR_n4:
      srs_config_pdu->comb_offset = srs_resource->transmissionComb.choice.n4->combOffset_n4;
      srs_config_pdu->cyclic_shift = srs_resource->transmissionComb.choice.n4->cyclicShift_n4;
      break;
    default:
      LOG_W(NR_MAC, "Invalid or not implemented comb_size!\n");
  }

  srs_config_pdu->frequency_position = srs_resource->freqDomainPosition;
  srs_config_pdu->frequency_shift = srs_resource->freqDomainShift;
  srs_config_pdu->frequency_hopping = srs_resource->freqHopping.b_hop;
  srs_config_pdu->group_or_sequence_hopping = srs_resource->groupOrSequenceHopping;
  srs_config_pdu->resource_type = srs_resource->resourceType.present - 1;
  if(srs_config_pdu->resource_type > 0) { // not aperiodic
    srs_config_pdu->t_srs = period;
    srs_config_pdu->t_offset = offset;
  }

#ifdef SRS_DEBUG
  LOG_I(NR_MAC,"Frame = %i, slot = %i\n", frame, slot);
  LOG_I(NR_MAC,"srs_config_pdu->rnti = 0x%04x\n", srs_config_pdu->rnti);
  LOG_I(NR_MAC,"srs_config_pdu->handle = %u\n", srs_config_pdu->handle);
  LOG_I(NR_MAC,"srs_config_pdu->bwp_size = %u\n", srs_config_pdu->bwp_size);
  LOG_I(NR_MAC,"srs_config_pdu->bwp_start = %u\n", srs_config_pdu->bwp_start);
  LOG_I(NR_MAC,"srs_config_pdu->subcarrier_spacing = %u\n", srs_config_pdu->subcarrier_spacing);
  LOG_I(NR_MAC,"srs_config_pdu->cyclic_prefix = %u (0: Normal; 1: Extended)\n", srs_config_pdu->cyclic_prefix);
  LOG_I(NR_MAC,"srs_config_pdu->num_ant_ports = %u (0 = 1 port, 1 = 2 ports, 2 = 4 ports)\n", srs_config_pdu->num_ant_ports);
  LOG_I(NR_MAC,"srs_config_pdu->num_symbols = %u (0 = 1 symbol, 1 = 2 symbols, 2 = 4 symbols)\n", srs_config_pdu->num_symbols);
  LOG_I(NR_MAC,"srs_config_pdu->num_repetitions = %u (0 = 1, 1 = 2, 2 = 4)\n", srs_config_pdu->num_repetitions);
  LOG_I(NR_MAC,"srs_config_pdu->time_start_position = %u\n", srs_config_pdu->time_start_position);
  LOG_I(NR_MAC,"srs_config_pdu->config_index = %u\n", srs_config_pdu->config_index);
  LOG_I(NR_MAC,"srs_config_pdu->sequence_id = %u\n", srs_config_pdu->sequence_id);
  LOG_I(NR_MAC,"srs_config_pdu->bandwidth_index = %u\n", srs_config_pdu->bandwidth_index);
  LOG_I(NR_MAC,"srs_config_pdu->comb_size = %u (0 = comb size 2, 1 = comb size 4, 2 = comb size 8)\n", srs_config_pdu->comb_size);
  LOG_I(NR_MAC,"srs_config_pdu->comb_offset = %u\n", srs_config_pdu->comb_offset);
  LOG_I(NR_MAC,"srs_config_pdu->cyclic_shift = %u\n", srs_config_pdu->cyclic_shift);
  LOG_I(NR_MAC,"srs_config_pdu->frequency_position = %u\n", srs_config_pdu->frequency_position);
  LOG_I(NR_MAC,"srs_config_pdu->frequency_shift = %u\n", srs_config_pdu->frequency_shift);
  LOG_I(NR_MAC,"srs_config_pdu->frequency_hopping = %u\n", srs_config_pdu->frequency_hopping);
  LOG_I(NR_MAC,"srs_config_pdu->group_or_sequence_hopping = %u (0 = No hopping, 1 = Group hopping groupOrSequenceHopping, 2 = Sequence hopping)\n", srs_config_pdu->group_or_sequence_hopping);
  LOG_I(NR_MAC,"srs_config_pdu->resource_type = %u (0: aperiodic, 1: semi-persistent, 2: periodic)\n", srs_config_pdu->resource_type);
  LOG_I(NR_MAC,"srs_config_pdu->t_srs = %u\n", srs_config_pdu->t_srs);
  LOG_I(NR_MAC,"srs_config_pdu->t_offset = %u\n", srs_config_pdu->t_offset);
#endif
}

// Aperiodic SRS scheduling
void nr_ue_aperiodic_srs_scheduling(NR_UE_MAC_INST_t *mac, long resource_trigger, int frame, int slot)
{
  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;
  NR_SRS_Config_t *srs_config = current_UL_BWP->srs_Config;

  if (!srs_config) {
    LOG_E(NR_MAC, "DCI is triggering aperiodic SRS but there is no SRS configuration\n");
    return;
  }

  int slot_offset = 0;
  NR_SRS_Resource_t *srs_resource = NULL;
  for(int rs = 0; rs < srs_config->srs_ResourceSetToAddModList->list.count; rs++) {

    // Find aperiodic resource set
    NR_SRS_ResourceSet_t *srs_resource_set = srs_config->srs_ResourceSetToAddModList->list.array[rs];
    if(srs_resource_set->resourceType.present != NR_SRS_ResourceSet__resourceType_PR_aperiodic)
      continue;
    // the resource trigger need to match the DCI one
    if(srs_resource_set->resourceType.choice.aperiodic->aperiodicSRS_ResourceTrigger != resource_trigger)
      continue;
    // if slotOffset is null -> offset = 0
    if(srs_resource_set->resourceType.choice.aperiodic->slotOffset)
      slot_offset = *srs_resource_set->resourceType.choice.aperiodic->slotOffset;

    // Find the corresponding srs resource
    for(int r1 = 0; r1 < srs_resource_set->srs_ResourceIdList->list.count; r1++) {
      for (int r2 = 0; r2 < srs_config->srs_ResourceToAddModList->list.count; r2++) {
        if ((*srs_resource_set->srs_ResourceIdList->list.array[r1] == srs_config->srs_ResourceToAddModList->list.array[r2]->srs_ResourceId) &&
            (srs_config->srs_ResourceToAddModList->list.array[r2]->resourceType.present == NR_SRS_Resource__resourceType_PR_aperiodic)) {
          srs_resource = srs_config->srs_ResourceToAddModList->list.array[r2];
          break;
        }
      }
    }
  }

  if(srs_resource == NULL) {
    LOG_E(NR_MAC, "Couldn't find SRS aperiodic resource with trigger %ld\n", resource_trigger);
    return;
  }

  AssertFatal(slot_offset > DURATION_RX_TO_TX,
              "Slot offset between DCI and aperiodic SRS (%d) needs to be higher than DURATION_RX_TO_TX (%d)\n",
              slot_offset, DURATION_RX_TO_TX);
  int n_slots_frame = nr_slots_per_frame[current_UL_BWP->scs];
  int sched_slot = (slot + slot_offset) % n_slots_frame;
  NR_TDD_UL_DL_ConfigCommon_t *tdd_config = mac->scc==NULL ? mac->scc_SIB->tdd_UL_DL_ConfigurationCommon : mac->scc->tdd_UL_DL_ConfigurationCommon;
  if (!is_nr_UL_slot(tdd_config, sched_slot, mac->frame_type)) {
    LOG_E(NR_MAC, "Slot for scheduling aperiodic SRS %d is not an UL slot\n", sched_slot);
    return;
  }
  int sched_frame = frame + (slot + slot_offset >= n_slots_frame) % 1024;

  fapi_nr_ul_config_request_t *ul_config = get_ul_config_request(mac, sched_slot, slot_offset);
  fapi_nr_ul_config_srs_pdu *srs_config_pdu = &ul_config->ul_config_list[ul_config->number_pdus].srs_config_pdu;
  configure_srs_pdu(mac, srs_resource, srs_config_pdu, 0, 0);
  fill_ul_config(ul_config, sched_frame, sched_slot, FAPI_NR_UL_CONFIG_TYPE_SRS);
}


// Periodic SRS scheduling
bool nr_ue_periodic_srs_scheduling(module_id_t mod_id, frame_t frame, slot_t slot)
{
  bool srs_scheduled = false;

  NR_UE_MAC_INST_t *mac = get_mac_inst(mod_id);
  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;

  NR_SRS_Config_t *srs_config = current_UL_BWP->srs_Config;

  if (!srs_config) {
    return false;
  }

  for(int rs = 0; rs < srs_config->srs_ResourceSetToAddModList->list.count; rs++) {

    // Find periodic resource set
    NR_SRS_ResourceSet_t *srs_resource_set = srs_config->srs_ResourceSetToAddModList->list.array[rs];
    if(srs_resource_set->resourceType.present != NR_SRS_ResourceSet__resourceType_PR_periodic) {
      continue;
    }

    // Find the corresponding srs resource
    NR_SRS_Resource_t *srs_resource = NULL;
    for(int r1 = 0; r1 < srs_resource_set->srs_ResourceIdList->list.count; r1++) {
      for (int r2 = 0; r2 < srs_config->srs_ResourceToAddModList->list.count; r2++) {
        if ((*srs_resource_set->srs_ResourceIdList->list.array[r1] == srs_config->srs_ResourceToAddModList->list.array[r2]->srs_ResourceId) &&
            (srs_config->srs_ResourceToAddModList->list.array[r2]->resourceType.present == NR_SRS_Resource__resourceType_PR_periodic)) {
          srs_resource = srs_config->srs_ResourceToAddModList->list.array[r2];
          break;
        }
      }
    }

    if(srs_resource == NULL) {
      continue;
    }

    uint16_t period = srs_period[srs_resource->resourceType.choice.periodic->periodicityAndOffset_p.present];
    uint16_t offset = get_nr_srs_offset(srs_resource->resourceType.choice.periodic->periodicityAndOffset_p);

    int n_slots_frame = nr_slots_per_frame[current_UL_BWP->scs];

    // Check if UE should transmit the SRS
    if((frame*n_slots_frame+slot-offset)%period == 0) {

      fapi_nr_ul_config_request_t *ul_config = get_ul_config_request(mac, slot, 0);
      fapi_nr_ul_config_srs_pdu *srs_config_pdu = &ul_config->ul_config_list[ul_config->number_pdus].srs_config_pdu;

      configure_srs_pdu(mac, srs_resource, srs_config_pdu, period, offset);

      fill_ul_config(ul_config, frame, slot, FAPI_NR_UL_CONFIG_TYPE_SRS);
      srs_scheduled = true;
    }
  }
  return srs_scheduled;
}

// Performs :
// 1. TODO: Call RRC for link status return to PHY
// 2. TODO: Perform SR/BSR procedures for scheduling feedback
// 3. TODO: Perform PHR procedures
void nr_ue_dl_scheduler(nr_downlink_indication_t *dl_info)
{
  module_id_t mod_id    = dl_info->module_id;
  uint32_t gNB_index    = dl_info->gNB_index;
  int cc_id             = dl_info->cc_id;
  frame_t rx_frame      = dl_info->frame;
  slot_t rx_slot        = dl_info->slot;
  NR_UE_MAC_INST_t *mac = get_mac_inst(mod_id);

  fapi_nr_dl_config_request_t *dl_config = get_dl_config_request(mac, rx_slot);
  dl_config->sfn  = rx_frame;
  dl_config->slot = rx_slot;

  nr_scheduled_response_t scheduled_response;
  nr_dcireq_t dcireq;

  if(mac->state == UE_CONNECTED) {

    dcireq.module_id = mod_id;
    dcireq.gNB_index = gNB_index;
    dcireq.cc_id     = cc_id;
    dcireq.frame     = rx_frame;
    dcireq.slot      = rx_slot;
    dcireq.dl_config_req.number_pdus = 0;
    nr_ue_dcireq(&dcireq); //to be replaced with function pointer later
    *dl_config = dcireq.dl_config_req;

    if(mac->ul_time_alignment.ta_apply)
      schedule_ta_command(dl_config, &mac->ul_time_alignment);
    nr_schedule_csirs_reception(mac, rx_frame, rx_slot);
    nr_schedule_csi_for_im(mac, rx_frame, rx_slot);
    dcireq.dl_config_req = *dl_config;

    fill_scheduled_response(&scheduled_response, &dcireq.dl_config_req, NULL, NULL, mod_id, cc_id, rx_frame, rx_slot, dl_info->phy_data);
    if(mac->if_module != NULL && mac->if_module->scheduled_response != NULL) {
      LOG_D(NR_MAC,"1# scheduled_response transmitted, %d, %d\n", rx_frame, rx_slot);
      mac->if_module->scheduled_response(&scheduled_response);
    }
  }
  else if (mac->state == UE_PERFORMING_RA) {
    // this is for Msg2/Msg4
    if (mac->ra.ra_state >= WAIT_RAR) {
      if(mac->ul_time_alignment.ta_apply)
        schedule_ta_command(dl_config, &mac->ul_time_alignment);
      config_dci_pdu(mac, dl_config, mac->ra.ra_state == WAIT_RAR ? NR_RNTI_RA : NR_RNTI_TC , rx_slot, mac->ra_SS);
      dl_config->number_pdus = 1;
      LOG_D(MAC,"mac->cg %p: Calling fill_scheduled_response for type0_pdcch, num_pdus %d\n", mac->cg, dl_config->number_pdus);
      fill_scheduled_response(&scheduled_response, dl_config, NULL, NULL, mod_id, cc_id, rx_frame, rx_slot, dl_info->phy_data);
      if(mac->if_module != NULL && mac->if_module->scheduled_response != NULL)
        mac->if_module->scheduled_response(&scheduled_response);
    }
  }
  else
    dl_config->number_pdus = 0;
}

void nr_ue_ul_scheduler(nr_uplink_indication_t *ul_info)
{
  int cc_id             = ul_info->cc_id;
  frame_t frame_tx      = ul_info->frame_tx;
  slot_t slot_tx        = ul_info->slot_tx;
  module_id_t mod_id    = ul_info->module_id;
  uint32_t gNB_index    = ul_info->gNB_index;

  NR_UE_MAC_INST_t *mac = get_mac_inst(mod_id);
  RA_config_t *ra       = &mac->ra;

  fapi_nr_ul_config_request_t *ul_config = get_ul_config_request(mac, slot_tx, 0);
  if (!ul_config)
    LOG_E(NR_MAC, "mac->ul_config is null!\n");

  if(mac->state < UE_CONNECTED) {
    nr_ue_get_rach(mod_id, cc_id, frame_tx, gNB_index, slot_tx);
    nr_ue_prach_scheduler(mod_id, frame_tx, slot_tx);
  }

  // Periodic SRS scheduling
  if(mac->state == UE_CONNECTED)
    nr_ue_periodic_srs_scheduling(mod_id, frame_tx, slot_tx);

  // Schedule ULSCH only if the current frame and slot match those in ul_config_req
  // AND if a UL grant (UL DCI or Msg3) has been received (as indicated by num_pdus)
  if (ul_config) {
    pthread_mutex_lock(&ul_config->mutex_ul_config);
    if ((ul_info->slot_tx == ul_config->slot && ul_info->frame_tx == ul_config->sfn) && ul_config->number_pdus > 0){

      LOG_D(NR_MAC, "[%d.%d]: number of UL PDUs: %d with UL transmission in [%d.%d]\n", frame_tx, slot_tx, ul_config->number_pdus, ul_config->sfn, ul_config->slot);

      uint8_t ulsch_input_buffer_array[NFAPI_MAX_NUM_UL_PDU][MAX_ULSCH_PAYLOAD_BYTES];
      nr_scheduled_response_t scheduled_response;
      fapi_nr_tx_request_t tx_req;
      tx_req.slot = slot_tx;
      tx_req.sfn = frame_tx;
      tx_req.number_of_pdus = 0;

      for (int j = 0; j < ul_config->number_pdus; j++) {
        uint8_t *ulsch_input_buffer = ulsch_input_buffer_array[tx_req.number_of_pdus];

        fapi_nr_ul_config_request_pdu_t *ulcfg_pdu = &ul_config->ul_config_list[j];

        if (ulcfg_pdu->pdu_type == FAPI_NR_UL_CONFIG_TYPE_PUSCH) {
          int mac_pdu_exist = 0;
          uint16_t TBS_bytes = ulcfg_pdu->pusch_config_pdu.pusch_data.tb_size;
          LOG_D(NR_MAC,"harq_id %d, NDI %d NDI_DCI %d, TBS_bytes %d (ra_state %d)\n",
                ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id,
                mac->UL_ndi[ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id],
                ulcfg_pdu->pusch_config_pdu.pusch_data.new_data_indicator,
                TBS_bytes,ra->ra_state);
          if (ra->ra_state == WAIT_RAR && !ra->cfra){
            memcpy(ulsch_input_buffer, mac->ulsch_pdu.payload, TBS_bytes);
            LOG_D(NR_MAC,"[RAPROC] Msg3 to be transmitted:\n");
            for (int k = 0; k < TBS_bytes; k++) {
              LOG_D(NR_MAC,"(%i): 0x%x\n",k,mac->ulsch_pdu.payload[k]);
            }
            LOG_D(NR_MAC,"Flipping NDI for harq_id %d (Msg3)\n",ulcfg_pdu->pusch_config_pdu.pusch_data.new_data_indicator);
            mac->UL_ndi[ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id] = ulcfg_pdu->pusch_config_pdu.pusch_data.new_data_indicator;
            mac->first_ul_tx[ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id] = 0;
            mac_pdu_exist = 1;
          } else {

            if ((mac->UL_ndi[ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id] != ulcfg_pdu->pusch_config_pdu.pusch_data.new_data_indicator ||
                mac->first_ul_tx[ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id] == 1) &&
                (mac->state == UE_CONNECTED ||
                (ra->ra_state == WAIT_RAR && ra->cfra))){

              // Getting IP traffic to be transmitted
              nr_ue_get_sdu(mod_id, cc_id,frame_tx, slot_tx, gNB_index, ulsch_input_buffer, TBS_bytes);
              mac_pdu_exist = 1;
            }

            LOG_D(NR_MAC,"Flipping NDI for harq_id %d\n",ulcfg_pdu->pusch_config_pdu.pusch_data.new_data_indicator);
            mac->UL_ndi[ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id] = ulcfg_pdu->pusch_config_pdu.pusch_data.new_data_indicator;
            mac->first_ul_tx[ulcfg_pdu->pusch_config_pdu.pusch_data.harq_process_id] = 0;

          }

          // Config UL TX PDU
          if (mac_pdu_exist) {
            tx_req.tx_request_body[tx_req.number_of_pdus].pdu_length = TBS_bytes;
            tx_req.tx_request_body[tx_req.number_of_pdus].pdu_index = j;
            tx_req.tx_request_body[tx_req.number_of_pdus].pdu = ulsch_input_buffer;
            tx_req.number_of_pdus++;
          }
          if (ra->ra_state == WAIT_CONTENTION_RESOLUTION && !ra->cfra){
            LOG_I(NR_MAC,"[RAPROC][%d.%d] RA-Msg3 retransmitted\n", frame_tx, slot_tx);
            // 38.321 restart the ra-ContentionResolutionTimer at each HARQ retransmission in the first symbol after the end of the Msg3 transmission
            nr_Msg3_transmitted(ul_info->module_id, ul_info->cc_id, ul_info->frame_tx, ul_info->slot_tx, ul_info->gNB_index);
          }
          if (ra->ra_state == WAIT_RAR && !ra->cfra){
            LOG_A(NR_MAC, "[RAPROC][%d.%d] RA-Msg3 transmitted\n", frame_tx, slot_tx);
            nr_Msg3_transmitted(ul_info->module_id, ul_info->cc_id, ul_info->frame_tx, ul_info->slot_tx, ul_info->gNB_index);
          }
        }
      }
      pthread_mutex_unlock(&ul_config->mutex_ul_config); // avoid double lock
      fill_scheduled_response(&scheduled_response, NULL, ul_config, &tx_req, mod_id, cc_id, frame_tx, slot_tx, ul_info->phy_data);
      if(mac->if_module != NULL && mac->if_module->scheduled_response != NULL){
        LOG_D(NR_MAC,"3# scheduled_response transmitted,%d, %d\n", frame_tx, slot_tx);
        mac->if_module->scheduled_response(&scheduled_response);
      }
      pthread_mutex_lock(&ul_config->mutex_ul_config);
    }
    pthread_mutex_unlock(&ul_config->mutex_ul_config);
  }

  // Call BSR procedure as described in Section 5.4.5 in 38.321

  // First check ReTxBSR Timer because it is always configured
  // Decrement ReTxBSR Timer if it is running and not null
  if ((mac->scheduling_info.retxBSR_SF != MAC_UE_BSR_TIMER_NOT_RUNNING) && (mac->scheduling_info.retxBSR_SF != 0)) {
    mac->scheduling_info.retxBSR_SF--;
  }

  // Decrement Periodic Timer if it is running and not null
  if ((mac->scheduling_info.periodicBSR_SF != MAC_UE_BSR_TIMER_NOT_RUNNING) && (mac->scheduling_info.periodicBSR_SF != 0)) {
    mac->scheduling_info.periodicBSR_SF--;
  }

  //Check whether Regular BSR is triggered
  if (nr_update_bsr(mod_id, frame_tx, slot_tx, gNB_index) == true) {
    // call SR procedure to generate pending SR and BSR for next PUCCH/PUSCH TxOp.  This should implement the procedures
    // outlined in Sections 5.4.4 an 5.4.5 of 38.321
    mac->scheduling_info.SR_pending = 1;
    // Regular BSR trigger
    mac->BSR_reporting_active |= NR_BSR_TRIGGER_REGULAR;
    LOG_D(NR_MAC, "[UE %d][BSR] Regular BSR Triggered Frame %d slot %d SR for PUSCH is pending\n",
          mod_id, frame_tx, slot_tx);
  }

  if(mac->state >= UE_PERFORMING_RA)
    nr_ue_pucch_scheduler(mod_id,frame_tx, slot_tx, ul_info->phy_data);
}

bool nr_update_bsr(module_id_t module_idP, frame_t frameP, slot_t slotP, uint8_t gNB_index)
{
  bool bsr_regular_triggered = false;
  uint8_t lcid;
  uint8_t lcgid;
  uint8_t num_lcid_with_data = 0; // for LCID with data only if LCGID is defined
  uint32_t lcgid_buffer_remain[NR_MAX_NUM_LCGID] = {0,0,0,0,0,0,0,0};
  int32_t lcid_bytes_in_buffer[NR_MAX_NUM_LCID];
  /* Array for ordering LCID with data per decreasing priority order */
  uint8_t lcid_reordered_array[NR_MAX_NUM_LCID]=
  {NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,
   NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,
   NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,
   NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,NR_MAX_NUM_LCID,
  };
  uint8_t pos_next = 0;
  //uint8_t highest_priority = 16;
  uint8_t array_index = 0;
  // Reset All BSR Infos
  lcid_bytes_in_buffer[0] = 0;
  NR_UE_MAC_INST_t *mac = get_mac_inst(module_idP);
  for (lcid=DCCH; lcid < NR_MAX_NUM_LCID; lcid++) {
    // Reset transmission status
    lcid_bytes_in_buffer[lcid] = 0;
    mac->scheduling_info.LCID_status[lcid]=LCID_EMPTY;
  }

  for (lcgid=0; lcgid < NR_MAX_NUM_LCGID; lcgid++) {
    // Reset Buffer Info
    mac->scheduling_info.BSR[lcgid]=0;
    mac->scheduling_info.BSR_bytes[lcgid]=0;
  }

  //Get Buffer Occupancy and fill lcid_reordered_array
  for (lcid=DCCH; lcid < NR_MAX_NUM_LCID; lcid++) {
    //if (mac->logicalChannelConfig[lcid]) {
    if (mac->logicalChannelBearer_exist[lcid] ) { // todo
      lcgid = mac->scheduling_info.LCGID[lcid];

      // Store already available data to transmit per Group
      if (lcgid < NR_MAX_NUM_LCGID) {
        lcgid_buffer_remain[lcgid] += mac->scheduling_info.LCID_buffer_remain[lcid];
      }

      mac_rlc_status_resp_t rlc_status = mac_rlc_status_ind(module_idP, mac->crnti,gNB_index,frameP,slotP,ENB_FLAG_NO,MBMS_FLAG_NO, lcid, 0, 0);

      lcid_bytes_in_buffer[lcid] = rlc_status.bytes_in_buffer;

      if (rlc_status.bytes_in_buffer > 0) {
        LOG_D(NR_MAC,"[UE %d] PDCCH Tick : LCID%d LCGID%d has data to transmit =%d bytes at frame %d slot %d\n",
              module_idP, lcid,lcgid,rlc_status.bytes_in_buffer,frameP,slotP);
        mac->scheduling_info.LCID_status[lcid] = LCID_NOT_EMPTY;

        //Update BSR_bytes and position in lcid_reordered_array only if Group is defined
        if (lcgid < NR_MAX_NUM_LCGID) {
          num_lcid_with_data ++;
          // sum lcid buffer which has same lcgid
          mac->scheduling_info.BSR_bytes[lcgid] += rlc_status.bytes_in_buffer;
          //Fill in the array
          array_index = 0;

          do {
            //if (mac->logicalChannelConfig[lcid]->ul_SpecificParameters->priority <= highest_priority) {
            if (1) { // todo
              //Insert if priority is higher or equal (lower or equal in value)
              for (pos_next=num_lcid_with_data-1; pos_next > array_index; pos_next--) {
                lcid_reordered_array[pos_next] = lcid_reordered_array[pos_next - 1];
              }

              lcid_reordered_array[array_index] = lcid;
              break;
            }

            array_index ++;
          } while ((array_index < num_lcid_with_data) && (array_index < NR_MAX_NUM_LCID));
        }
      }
    }
  }

  // Check whether a regular BSR can be triggered according to the first cases in 38.321
  if (num_lcid_with_data) {
    LOG_D(NR_MAC, "[UE %d] PDCCH Tick at frame %d slot %d: NumLCID with data=%d Reordered LCID0=%d LCID1=%d LCID2=%d\n",
          module_idP, frameP, slotP, num_lcid_with_data,
          lcid_reordered_array[0], lcid_reordered_array[1],
          lcid_reordered_array[2]);

    for (array_index = 0; array_index < num_lcid_with_data; array_index++) {
      lcid = lcid_reordered_array[array_index];

      /* UL data, for a logical channel which belongs to a LCG, becomes available for transmission in the RLC entity
         either the data belongs to a logical channel with higher priority than the priorities of the logical channels
         which belong to any LCG and for which data is already available for transmission
       */
      {
        bsr_regular_triggered = true;
        LOG_D(NR_MAC, "[UE %d] PDCCH Tick : MAC BSR Triggered LCID%d LCGID%d data become available at frame %d slot %d\n",
              module_idP, lcid,
              mac->scheduling_info.LCGID[lcid],
              frameP, slotP);
        break;
      }
    }

    // Trigger Regular BSR if ReTxBSR Timer has expired and UE has data for transmission
    if (mac->scheduling_info.retxBSR_SF == 0) {
      bsr_regular_triggered = true;

      if ((mac->BSR_reporting_active & NR_BSR_TRIGGER_REGULAR) == 0) {
        LOG_I(NR_MAC, "[UE %d] PDCCH Tick : MAC BSR Triggered ReTxBSR Timer expiry at frame %d slot %d\n",
              module_idP, frameP, slotP);
      }
    }
  }

  //Store Buffer Occupancy in remain buffers for next TTI
  for (lcid = DCCH; lcid < NR_MAX_NUM_LCID; lcid++) {
    mac->scheduling_info.LCID_buffer_remain[lcid] = lcid_bytes_in_buffer[lcid];
  }

  return bsr_regular_triggered;
}

uint8_t
nr_locate_BsrIndexByBufferSize(const uint32_t *table, int size, int value) {
  uint8_t ju, jm, jl;
  int ascend;
  //DevAssert(size > 0);
  //DevAssert(size <= 256);

  if (value == 0) {
    return 0;   //elseif (value > 150000) return 63;
  }

  jl = 0;     // lower bound
  ju = size - 1;    // upper bound
  ascend = (table[ju] >= table[jl]) ? 1 : 0;  // determine the order of the the table:  1 if ascending order of table, 0 otherwise

  while (ju - jl > 1) { //If we are not yet done,
    jm = (ju + jl) >> 1;  //compute a midpoint,

    if ((value >= table[jm]) == ascend) {
      jl = jm;    // replace the lower limit
    } else {
      ju = jm;    //replace the upper limit
    }

    LOG_T(NR_MAC, "[UE] searching BSR index %d for (BSR TABLE %d < value %d)\n",
          jm, table[jm], value);
  }

  if (value == table[jl]) {
    return jl;
  } else {
    return jl + 1;    //equally  ju
  }
}

int nr_get_sf_periodicBSRTimer(uint8_t sf_offset) {
  switch (sf_offset) {
    case NR_BSR_Config__periodicBSR_Timer_sf1:
      return 1;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf5:
      return 5;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf10:
      return 10;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf16:
      return 16;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf20:
      return 20;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf32:
      return 32;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf40:
      return 40;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf64:
      return 64;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf80:
      return 80;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf128:
      return 128;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf160:
      return 160;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf320:
      return 320;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf640:
      return 640;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf1280:
      return 1280;
      break;

    case NR_BSR_Config__periodicBSR_Timer_sf2560:
      return 2560;
      break;

    case NR_BSR_Config__periodicBSR_Timer_infinity:
    default:
      return 0xFFFF;
      break;
  }
}

int nr_get_sf_retxBSRTimer(uint8_t sf_offset) {
  switch (sf_offset) {
    case NR_BSR_Config__retxBSR_Timer_sf10:
      return 10;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf20:
      return 20;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf40:
      return 40;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf80:
      return 80;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf160:
      return 160;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf320:
      return 320;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf640:
      return 640;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf1280:
      return 1280;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf2560:
      return 2560;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf5120:
      return 5120;
      break;

    case NR_BSR_Config__retxBSR_Timer_sf10240:
      return 10240;
      break;

    default:
      return -1;
      break;
  }
}

// PUSCH scheduler:
// - Calculate the slot in which ULSCH should be scheduled. This is current slot + K2,
// - where K2 is the offset between the slot in which UL DCI is received and the slot
// - in which ULSCH should be scheduled. K2 is configured in RRC configuration.  
// PUSCH Msg3 scheduler:
// - scheduled by RAR UL grant according to 8.3 of TS 38.213
// Note: Msg3 tx in the uplink symbols of mixed slot
int nr_ue_pusch_scheduler(NR_UE_MAC_INST_t *mac, uint8_t is_Msg3, frame_t current_frame, int current_slot, frame_t *frame_tx, int *slot_tx, long k2)
{
  AssertFatal(k2 > DURATION_RX_TO_TX,
              "Slot offset K2 (%ld) needs to be higher than DURATION_RX_TO_TX (%d). Please set min_rxtxtime at least to %d in gNB config file or gNBs.[0].min_rxtxtime=%d via command line.\n",
              k2,
              DURATION_RX_TO_TX,
              DURATION_RX_TO_TX,
              DURATION_RX_TO_TX);

  int delta = 0;
  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;

  // Get the numerology to calculate the Tx frame and slot
  int mu = current_UL_BWP->scs;

  // k2 as per 3GPP TS 38.214 version 15.9.0 Release 15 ch 6.1.2.1.1
  // PUSCH time domain resource allocation is higher layer configured from uschTimeDomainAllocationList in either pusch-ConfigCommon

  if (is_Msg3) {

    switch (mu) {
      case 0:
        delta = 2;
        break;
      case 1:
        delta = 3;
        break;
      case 2:
        delta = 4;
        break;
      case 3:
        delta = 6;
        break;
      default:
        AssertFatal(1 == 0, "Invalid numerology %i\n", mu);
    }

    AssertFatal((k2 + delta) > DURATION_RX_TO_TX,
                "Slot offset (%ld) for Msg3 needs to be higher than DURATION_RX_TO_TX (%d). Please set min_rxtxtime at least to %d in gNB config file or gNBs.[0].min_rxtxtime=%d via command line.\n",
                k2,
                DURATION_RX_TO_TX,
                DURATION_RX_TO_TX,
                DURATION_RX_TO_TX);

    *slot_tx = (current_slot + k2 + delta) % nr_slots_per_frame[mu];
    if (current_slot + k2 + delta > nr_slots_per_frame[mu]){
      *frame_tx = (current_frame + 1) % 1024;
    } else {
      *frame_tx = current_frame;
    }

  } else {

    if (k2 < 0) { // This can happen when a false DCI is received
      LOG_W(PHY, "%d.%d. Received k2 %ld\n", current_frame, current_slot, k2);
      return -1;
    }

    // Calculate TX slot and frame
    *slot_tx = (current_slot + k2) % nr_slots_per_frame[mu];
    *frame_tx = ((current_slot + k2) > (nr_slots_per_frame[mu]-1)) ? (current_frame + 1) % 1024 : current_frame;

  }

  LOG_D(NR_MAC, "[%04d.%02d] UL transmission in [%04d.%02d] (k2 %ld delta %d)\n", current_frame, current_slot, *frame_tx, *slot_tx, k2, delta);

  return 0;
}

// Build the list of all the valid RACH occasions in the maximum association pattern period according to the PRACH config
static void build_ro_list(NR_UE_MAC_INST_t *mac) {

  int x,y; // PRACH Configuration Index table variables used to compute the valid frame numbers
  int y2;  // PRACH Configuration Index table additional variable used to compute the valid frame numbers
  uint8_t slot_shift_for_map;
  uint8_t map_shift;
  bool even_slot_invalid;
  int64_t s_map;
  uint8_t prach_conf_start_symbol; // Starting symbol of the PRACH occasions in the PRACH slot
  uint8_t N_t_slot; // Number of PRACH occasions in a 14-symbols PRACH slot
  uint8_t N_dur; // Duration of a PRACH occasion (nb of symbols)
  uint8_t frame; // Maximum is NB_FRAMES_IN_MAX_ASSOCIATION_PATTERN_PERIOD
  uint8_t slot; // Maximum is the number of slots in a frame @ SCS 240kHz
  uint16_t format = 0xffff;
  uint8_t format2 = 0xff;
  int nb_fdm;

  uint8_t config_index;
  int msg1_FDM;

  uint8_t prach_conf_period_idx;
  uint8_t nb_of_frames_per_prach_conf_period;
  uint8_t prach_conf_period_frame_idx;

  NR_RACH_ConfigCommon_t *setup = mac->current_UL_BWP.rach_ConfigCommon;
  NR_RACH_ConfigGeneric_t *rach_ConfigGeneric = &setup->rach_ConfigGeneric;

  config_index = rach_ConfigGeneric->prach_ConfigurationIndex;

  int mu;
  if (setup->msg1_SubcarrierSpacing)
    mu = *setup->msg1_SubcarrierSpacing;
  else
    mu = mac->current_UL_BWP.scs;

  msg1_FDM = rach_ConfigGeneric->msg1_FDM;

  switch (msg1_FDM){
    case 0:
    case 1:
    case 2:
    case 3:
      nb_fdm = 1 << msg1_FDM;
      break;
    default:
      AssertFatal(1 == 0, "Unknown msg1_FDM from rach_ConfigGeneric %d\n", msg1_FDM);
  }

  // Create the PRACH occasions map
  // ==============================
  // WIP: For now assume no rejected PRACH occasions because of conflict with SSB or TDD_UL_DL_ConfigurationCommon schedule

  int unpaired = mac->phy_config.config_req.cell_config.frame_duplex_type;

  const int64_t *prach_config_info_p = get_prach_config_info(mac->frequency_range, config_index, unpaired);

  // Identify the proper PRACH Configuration Index table according to the operating frequency
  LOG_D(NR_MAC,"mu = %u, PRACH config index  = %u, unpaired = %u\n", mu, config_index, unpaired);

  if (mac->frequency_range == FR2) { //FR2

    x = prach_config_info_p[2];
    y = prach_config_info_p[3];
    y2 = prach_config_info_p[4];

    s_map = prach_config_info_p[5];

    prach_conf_start_symbol = prach_config_info_p[6];
    N_t_slot = prach_config_info_p[8];
    N_dur = prach_config_info_p[9];
    if (prach_config_info_p[1] != -1)
      format2 = (uint8_t) prach_config_info_p[1];
    format = ((uint8_t) prach_config_info_p[0]) | (format2<<8);

    slot_shift_for_map = mu-2;
    if ( (mu == 3) && (prach_config_info_p[7] == 1) )
      even_slot_invalid = true;
    else
      even_slot_invalid = false;
  }
  else { // FR1
    x = prach_config_info_p[2];
    y = prach_config_info_p[3];
    y2 = y;

    s_map = prach_config_info_p[4];

    prach_conf_start_symbol = prach_config_info_p[5];
    N_t_slot = prach_config_info_p[7];
    N_dur = prach_config_info_p[8];
    LOG_D(NR_MAC,"N_t_slot %d, N_dur %d\n",N_t_slot,N_dur);
    if (prach_config_info_p[1] != -1)
      format2 = (uint8_t) prach_config_info_p[1];
    format = ((uint8_t) prach_config_info_p[0]) | (format2<<8);

    slot_shift_for_map = mu;
    if ( (mu == 1) && (prach_config_info_p[6] <= 1) )
      // no prach in even slots @ 30kHz for 1 prach per subframe
      even_slot_invalid = true;
    else
      even_slot_invalid = false;
  } // FR2 / FR1

  prach_assoc_pattern.nb_of_prach_conf_period_in_max_period = MAX_NB_PRACH_CONF_PERIOD_IN_ASSOCIATION_PATTERN_PERIOD / x;
  nb_of_frames_per_prach_conf_period = x;

  LOG_D(NR_MAC,"nb_of_prach_conf_period_in_max_period %d\n", prach_assoc_pattern.nb_of_prach_conf_period_in_max_period);

  // Fill in the PRACH occasions table for every slot in every frame in every PRACH configuration periods in the maximum association pattern period
  // ----------------------------------------------------------------------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------------------------------------------------------------------
  // For every PRACH configuration periods
  // -------------------------------------
  for (prach_conf_period_idx=0; prach_conf_period_idx<prach_assoc_pattern.nb_of_prach_conf_period_in_max_period; prach_conf_period_idx++) {
    prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].nb_of_prach_occasion = 0;
    prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].nb_of_frame = nb_of_frames_per_prach_conf_period;
    prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].nb_of_slot = nr_slots_per_frame[mu];

    LOG_D(NR_MAC,"PRACH Conf Period Idx %d\n", prach_conf_period_idx);

    // For every frames in a PRACH configuration period
    // ------------------------------------------------
    for (prach_conf_period_frame_idx=0; prach_conf_period_frame_idx<nb_of_frames_per_prach_conf_period; prach_conf_period_frame_idx++) {
      frame = (prach_conf_period_idx * nb_of_frames_per_prach_conf_period) + prach_conf_period_frame_idx;

      LOG_D(NR_MAC,"PRACH Conf Period Frame Idx %d - Frame %d\n", prach_conf_period_frame_idx, frame);
      // Is it a valid frame for this PRACH configuration index? (n_sfn mod x = y)
      if ( (frame%x)==y || (frame%x)==y2 ) {

        // For every slot in a frame
        // -------------------------
        for (slot=0; slot<nr_slots_per_frame[mu]; slot++) {
          // Is it a valid slot?
          map_shift = slot >> slot_shift_for_map; // in PRACH configuration index table slots are numbered wrt 60kHz
          if ( (s_map>>map_shift)&0x01 ) {
            // Valid slot

            // Additionally, for 30kHz/120kHz, we must check for the n_RA_Slot param also
            if ( even_slot_invalid && (slot%2 == 0) )
                continue; // no prach in even slots @ 30kHz/120kHz for 1 prach per 60khz slot/subframe

            // We're good: valid frame and valid slot
            // Compute all the PRACH occasions in the slot

            uint8_t n_prach_occ_in_time;
            uint8_t n_prach_occ_in_freq;

            prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].prach_occasion_slot_map[prach_conf_period_frame_idx][slot].nb_of_prach_occasion_in_time = N_t_slot;
            prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].prach_occasion_slot_map[prach_conf_period_frame_idx][slot].nb_of_prach_occasion_in_freq = nb_fdm;

            for (n_prach_occ_in_time=0; n_prach_occ_in_time<N_t_slot; n_prach_occ_in_time++) {
              uint8_t start_symbol = prach_conf_start_symbol + n_prach_occ_in_time * N_dur;
              LOG_D(NR_MAC,"PRACH Occ in time %d\n", n_prach_occ_in_time);

              for (n_prach_occ_in_freq=0; n_prach_occ_in_freq<nb_fdm; n_prach_occ_in_freq++) {
                prach_occasion_info_t *prach_occasion_p = &prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].prach_occasion_slot_map[prach_conf_period_frame_idx][slot].prach_occasion[n_prach_occ_in_time][n_prach_occ_in_freq];

                prach_occasion_p->start_symbol = start_symbol;
                prach_occasion_p->fdm = n_prach_occ_in_freq;
                prach_occasion_p->frame = frame;
                prach_occasion_p->slot = slot;
                prach_occasion_p->format = format;
                prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].nb_of_prach_occasion++;

                LOG_D(NR_MAC,"Adding a PRACH occasion: frame %u, slot-symbol %d-%d, occ_in_time-occ_in-freq %d-%d, nb ROs in conf period %d, for this slot: RO# in time %d, RO# in freq %d\n",
                    frame, slot, start_symbol, n_prach_occ_in_time, n_prach_occ_in_freq, prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].nb_of_prach_occasion,
                    prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].prach_occasion_slot_map[prach_conf_period_frame_idx][slot].nb_of_prach_occasion_in_time,
                    prach_assoc_pattern.prach_conf_period_list[prach_conf_period_idx].prach_occasion_slot_map[prach_conf_period_frame_idx][slot].nb_of_prach_occasion_in_freq);
              } // For every freq in the slot
            } // For every time occasions in the slot
          } // Valid slot?
        } // For every slots in a frame
      } // Valid frame?
    } // For every frames in a prach configuration period
  } // For every prach configuration periods in the maximum association pattern period (160ms)
}

// Build the list of all the valid/transmitted SSBs according to the config
static void build_ssb_list(NR_UE_MAC_INST_t *mac) {

  // Create the list of transmitted SSBs
  // ===================================
  BIT_STRING_t *ssb_bitmap;
  uint64_t ssb_positionsInBurst;
  uint8_t ssb_idx = 0;

  if (mac->scc) {
    NR_ServingCellConfigCommon_t *scc = mac->scc;
    switch (scc->ssb_PositionsInBurst->present) {
      case NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_shortBitmap:
        ssb_bitmap = &scc->ssb_PositionsInBurst->choice.shortBitmap;

        ssb_positionsInBurst = BIT_STRING_to_uint8(ssb_bitmap);
        LOG_D(NR_MAC,"SSB config: SSB_positions_in_burst 0x%lx\n", ssb_positionsInBurst);

        for (uint8_t bit_nb=3; bit_nb<=3; bit_nb--) {
          // If SSB is transmitted
          if ((ssb_positionsInBurst>>bit_nb) & 0x01) {
            ssb_list.nb_tx_ssb++;
            ssb_list.tx_ssb[ssb_idx].transmitted = true;
            LOG_D(NR_MAC,"SSB idx %d transmitted\n", ssb_idx);
          }
          ssb_idx++;
        }
        break;
      case NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_mediumBitmap:
        ssb_bitmap = &scc->ssb_PositionsInBurst->choice.mediumBitmap;

        ssb_positionsInBurst = BIT_STRING_to_uint8(ssb_bitmap);
        LOG_D(NR_MAC,"SSB config: SSB_positions_in_burst 0x%lx\n", ssb_positionsInBurst);

        for (uint8_t bit_nb=7; bit_nb<=7; bit_nb--) {
          // If SSB is transmitted
          if ((ssb_positionsInBurst>>bit_nb) & 0x01) {
            ssb_list.nb_tx_ssb++;
            ssb_list.tx_ssb[ssb_idx].transmitted = true;
            LOG_D(NR_MAC,"SSB idx %d transmitted\n", ssb_idx);
          }
          ssb_idx++;
        }
        break;
      case NR_ServingCellConfigCommon__ssb_PositionsInBurst_PR_longBitmap:
        ssb_bitmap = &scc->ssb_PositionsInBurst->choice.longBitmap;

        ssb_positionsInBurst = BIT_STRING_to_uint64(ssb_bitmap);
        LOG_D(NR_MAC,"SSB config: SSB_positions_in_burst 0x%lx\n", ssb_positionsInBurst);

        for (uint8_t bit_nb=63; bit_nb<=63; bit_nb--) {
          // If SSB is transmitted
          if ((ssb_positionsInBurst>>bit_nb) & 0x01) {
            ssb_list.nb_tx_ssb++;
            ssb_list.tx_ssb[ssb_idx].transmitted = true;
            LOG_D(NR_MAC,"SSB idx %d transmitted\n", ssb_idx);
          }
          ssb_idx++;
        }
        break;
      default:
        AssertFatal(false,"ssb_PositionsInBurst not present\n");
        break;
    }
  } else { // This is configuration from SIB1

    AssertFatal(mac->scc_SIB->ssb_PositionsInBurst.groupPresence == NULL, "Handle case for >8 SSBs\n");
    ssb_bitmap = &mac->scc_SIB->ssb_PositionsInBurst.inOneGroup;

    ssb_positionsInBurst = BIT_STRING_to_uint8(ssb_bitmap);

    LOG_D(NR_MAC,"SSB config: SSB_positions_in_burst 0x%lx\n", ssb_positionsInBurst);

    for (uint8_t bit_nb=7; bit_nb<=7; bit_nb--) {
      // If SSB is transmitted
      if ((ssb_positionsInBurst>>bit_nb) & 0x01) {
        ssb_list.nb_tx_ssb++;
        ssb_list.tx_ssb[ssb_idx].transmitted = true;
        LOG_D(NR_MAC,"SSB idx %d transmitted\n", ssb_idx);
      }
      ssb_idx++;
    }
  }
}

// Map the transmitted SSBs to the ROs and create the association pattern according to the config
static void map_ssb_to_ro(NR_UE_MAC_INST_t *mac) {

  // Map SSBs to PRACH occasions
  // ===========================
  // WIP: Assumption: No PRACH occasion is rejected because of a conflict with SSBs or TDD_UL_DL_ConfigurationCommon schedule
  NR_RACH_ConfigCommon_t *setup = mac->current_UL_BWP.rach_ConfigCommon;
  NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR ssb_perRACH_config = setup->ssb_perRACH_OccasionAndCB_PreamblesPerSSB->present;

  bool multiple_ssb_per_ro; // true if more than one or exactly one SSB per RACH occasion, false if more than one RO per SSB
  uint8_t ssb_rach_ratio; // Nb of SSBs per RACH or RACHs per SSB
  uint16_t required_nb_of_prach_occasion; // Nb of RACH occasions required to map all the SSBs
  uint8_t required_nb_of_prach_conf_period; // Nb of PRACH configuration periods required to map all the SSBs

  // Determine the SSB to RACH mapping ratio
  // =======================================
  switch (ssb_perRACH_config){
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_oneEighth:
      multiple_ssb_per_ro = false;
      ssb_rach_ratio = 8;
      break;
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_oneFourth:
      multiple_ssb_per_ro = false;
      ssb_rach_ratio = 4;
      break;
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_oneHalf:
      multiple_ssb_per_ro = false;
      ssb_rach_ratio = 2;
      break;
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_one:
      multiple_ssb_per_ro = true;
      ssb_rach_ratio = 1;
      break;
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_two:
      multiple_ssb_per_ro = true;
      ssb_rach_ratio = 2;
      break;
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_four:
      multiple_ssb_per_ro = true;
      ssb_rach_ratio = 4;
      break;
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_eight:
      multiple_ssb_per_ro = true;
      ssb_rach_ratio = 8;
      break;
    case NR_RACH_ConfigCommon__ssb_perRACH_OccasionAndCB_PreamblesPerSSB_PR_sixteen:
      multiple_ssb_per_ro = true;
      ssb_rach_ratio = 16;
      break;
    default:
      AssertFatal(1 == 0, "Unsupported ssb_perRACH_config %d\n", ssb_perRACH_config);
      break;
  }
  LOG_D(NR_MAC,"SSB rach ratio %d, Multiple SSB per RO %d\n", ssb_rach_ratio, multiple_ssb_per_ro);

  // Evaluate the number of PRACH configuration periods required to map all the SSBs and set the association period
  // ==============================================================================================================
  // WIP: Assumption for now is that all the PRACH configuration periods within a maximum association pattern period have the same number of PRACH occasions
  //      (No PRACH occasions are conflicting with SSBs nor TDD_UL_DL_ConfigurationCommon schedule)
  //      There is only one possible association period which can contain up to 16 PRACH configuration periods
  LOG_D(NR_MAC,"Evaluate the number of PRACH configuration periods required to map all the SSBs and set the association period\n");
  if (true == multiple_ssb_per_ro) {
    required_nb_of_prach_occasion = ((ssb_list.nb_tx_ssb-1) + ssb_rach_ratio) / ssb_rach_ratio;
  }
  else {
    required_nb_of_prach_occasion = ssb_list.nb_tx_ssb * ssb_rach_ratio;
  }

  AssertFatal(prach_assoc_pattern.prach_conf_period_list[0].nb_of_prach_occasion>0,
              "prach_assoc_pattern.prach_conf_period_list[0].nb_of_prach_occasion shouldn't be 0 (ssb_list.nb_tx_ssb %d, ssb_rach_ratio %d\n",
              ssb_list.nb_tx_ssb,ssb_rach_ratio);
  required_nb_of_prach_conf_period = ((required_nb_of_prach_occasion-1) + prach_assoc_pattern.prach_conf_period_list[0].nb_of_prach_occasion) /
                                     prach_assoc_pattern.prach_conf_period_list[0].nb_of_prach_occasion;

  if (required_nb_of_prach_conf_period == 1) {
    prach_assoc_pattern.prach_association_period_list[0].nb_of_prach_conf_period = 1;
  }
  else if (required_nb_of_prach_conf_period == 2) {
    prach_assoc_pattern.prach_association_period_list[0].nb_of_prach_conf_period = 2;
  }
  else if (required_nb_of_prach_conf_period <= 4) {
    prach_assoc_pattern.prach_association_period_list[0].nb_of_prach_conf_period = 4;
  }
  else if (required_nb_of_prach_conf_period <= 8) {
    prach_assoc_pattern.prach_association_period_list[0].nb_of_prach_conf_period = 8;
  }
  else if (required_nb_of_prach_conf_period <= 16) {
    prach_assoc_pattern.prach_association_period_list[0].nb_of_prach_conf_period = 16;
  }
  else {
    AssertFatal(1 == 0, "Invalid number of PRACH config periods within an association period %d\n", required_nb_of_prach_conf_period);
  }

  prach_assoc_pattern.nb_of_assoc_period = 1; // WIP: only one possible association period
  prach_assoc_pattern.prach_association_period_list[0].nb_of_frame = prach_assoc_pattern.prach_association_period_list[0].nb_of_prach_conf_period * prach_assoc_pattern.prach_conf_period_list[0].nb_of_frame;
  prach_assoc_pattern.nb_of_frame = prach_assoc_pattern.prach_association_period_list[0].nb_of_frame;

  LOG_D(NR_MAC,"Assoc period %d, Nb of frames in assoc period %d\n",
        prach_assoc_pattern.prach_association_period_list[0].nb_of_prach_conf_period,
        prach_assoc_pattern.prach_association_period_list[0].nb_of_frame);

  // Proceed to the SSB to RO mapping
  // ================================
  uint8_t association_period_idx; // Association period index within the association pattern
  uint8_t ssb_idx = 0;
  uint8_t prach_configuration_period_idx; // PRACH Configuration period index within the association pattern
  prach_conf_period_t *prach_conf_period_p;

  // Map all the association periods within the association pattern period
  LOG_D(NR_MAC,"Proceed to the SSB to RO mapping\n");
  for (association_period_idx=0; association_period_idx<prach_assoc_pattern.nb_of_assoc_period; association_period_idx++) {
    uint8_t n_prach_conf=0; // PRACH Configuration period index within the association period
    uint8_t frame=0;
    uint8_t slot=0;
    uint8_t ro_in_time=0;
    uint8_t ro_in_freq=0;

    // Set the starting PRACH Configuration period index in the association_pattern map for this particular association period
    prach_configuration_period_idx = 0;  // WIP: only one possible association period so the starting PRACH configuration period is automatically 0

    // Check if we need to map multiple SSBs per RO or multiple ROs per SSB
    if (true == multiple_ssb_per_ro) {
      // --------------------
      // --------------------
      // Multiple SSBs per RO
      // --------------------
      // --------------------

      // WIP: For the moment, only map each SSB idx once per association period if configuration is multiple SSBs per RO
      //      this is true if no PRACH occasions are conflicting with SSBs nor TDD_UL_DL_ConfigurationCommon schedule
      ssb_idx = 0;

      // Go through the list of PRACH config periods within this association period
      for (n_prach_conf=0; n_prach_conf<prach_assoc_pattern.prach_association_period_list[association_period_idx].nb_of_prach_conf_period; n_prach_conf++, prach_configuration_period_idx++) {
        // Build the association period with its association PRACH Configuration indexes
        prach_conf_period_p = &prach_assoc_pattern.prach_conf_period_list[prach_configuration_period_idx];
        prach_assoc_pattern.prach_association_period_list[association_period_idx].prach_conf_period_list[n_prach_conf] = prach_conf_period_p;

        // Go through all the ROs within the PRACH config period
        for (frame=0; frame<prach_conf_period_p->nb_of_frame; frame++) {
          for (slot=0; slot<prach_conf_period_p->nb_of_slot; slot++) {
            for (ro_in_time=0; ro_in_time<prach_conf_period_p->prach_occasion_slot_map[frame][slot].nb_of_prach_occasion_in_time; ro_in_time++) {
              for (ro_in_freq=0; ro_in_freq<prach_conf_period_p->prach_occasion_slot_map[frame][slot].nb_of_prach_occasion_in_freq; ro_in_freq++) {
                prach_occasion_info_t *ro_p = &prach_conf_period_p->prach_occasion_slot_map[frame][slot].prach_occasion[ro_in_time][ro_in_freq];

                // Go through the list of transmitted SSBs and map the required amount of SSBs to this RO
                // WIP: For the moment, only map each SSB idx once per association period if configuration is multiple SSBs per RO
                //      this is true if no PRACH occasions are conflicting with SSBs nor TDD_UL_DL_ConfigurationCommon schedule
                for (; ssb_idx<MAX_NB_SSB; ssb_idx++) {
                  // Map only the transmitted ssb_idx
                  if (true == ssb_list.tx_ssb[ssb_idx].transmitted) {
                    ro_p->mapped_ssb_idx[ro_p->nb_mapped_ssb] = ssb_idx;
                    ro_p->nb_mapped_ssb++;
                    ssb_list.tx_ssb[ssb_idx].mapped_ro[ssb_list.tx_ssb[ssb_idx].nb_mapped_ro] = ro_p;
                    ssb_list.tx_ssb[ssb_idx].nb_mapped_ro++;
                    AssertFatal(MAX_NB_RO_PER_SSB_IN_ASSOCIATION_PATTERN > ssb_list.tx_ssb[ssb_idx].nb_mapped_ro,"Too many mapped ROs (%d) to a single SSB\n", ssb_list.tx_ssb[ssb_idx].nb_mapped_ro);

                    LOG_D(NR_MAC,"Mapped ssb_idx %u to RO slot-symbol %u-%u, %u-%u-%u/%u\n",
                          ssb_idx, ro_p->slot, ro_p->start_symbol, slot, ro_in_time, ro_in_freq,
                          prach_conf_period_p->prach_occasion_slot_map[frame][slot].nb_of_prach_occasion_in_freq);
                    LOG_D(NR_MAC,"Nb mapped ROs for this ssb idx: in the association period only %u\n", ssb_list.tx_ssb[ssb_idx].nb_mapped_ro);

                    // If all the required SSBs are mapped to this RO, exit the loop of SSBs
                    if (ro_p->nb_mapped_ssb == ssb_rach_ratio) {
                      ssb_idx++;
                      break;
                    }
                  } // if ssb_idx is transmitted
                } // for ssb_idx

                // Exit the loop of ROs if there is no more SSB to map
                if (MAX_NB_SSB == ssb_idx) break;
              } // for ro_in_freq

              // Exit the loop of ROs if there is no more SSB to map
              if (MAX_NB_SSB == ssb_idx) break;
            } // for ro_in_time

            // Exit the loop of slots if there is no more SSB to map
            if (MAX_NB_SSB == ssb_idx) break;
          } // for slot

          // Exit the loop frames if there is no more SSB to map
          if (MAX_NB_SSB == ssb_idx) break;
        } // for frame

        // Exit the loop of PRACH configurations if there is no more SSB to map
        if (MAX_NB_SSB == ssb_idx) break;
      } // for n_prach_conf

      // WIP: note that there is no re-mapping of the SSBs within the association period since there is no invalid ROs in the PRACH config periods that would create this situation

    } // if multiple_ssbs_per_ro

    else {
      // --------------------
      // --------------------
      // Multiple ROs per SSB
      // --------------------
      // --------------------

      n_prach_conf = 0;

      // Go through the list of transmitted SSBs
      for (ssb_idx=0; ssb_idx<MAX_NB_SSB; ssb_idx++) {
        uint8_t nb_mapped_ro_in_association_period=0; // Reset the nb of mapped ROs for the new SSB index

	      LOG_D(NR_MAC,"Checking ssb_idx %d => %d\n",
	      ssb_idx,ssb_list.tx_ssb[ssb_idx].transmitted);

        // Map only the transmitted ssb_idx
        if (true == ssb_list.tx_ssb[ssb_idx].transmitted) {

          // Map all the required ROs to this SSB
          // Go through the list of PRACH config periods within this association period
          for (; n_prach_conf<prach_assoc_pattern.prach_association_period_list[association_period_idx].nb_of_prach_conf_period; n_prach_conf++, prach_configuration_period_idx++) {

            // Build the association period with its association PRACH Configuration indexes
            prach_conf_period_p = &prach_assoc_pattern.prach_conf_period_list[prach_configuration_period_idx];
            prach_assoc_pattern.prach_association_period_list[association_period_idx].prach_conf_period_list[n_prach_conf] = prach_conf_period_p;

            for (; frame<prach_conf_period_p->nb_of_frame; frame++) {
              for (; slot<prach_conf_period_p->nb_of_slot; slot++) {
                for (; ro_in_time<prach_conf_period_p->prach_occasion_slot_map[frame][slot].nb_of_prach_occasion_in_time; ro_in_time++) {
                  for (; ro_in_freq<prach_conf_period_p->prach_occasion_slot_map[frame][slot].nb_of_prach_occasion_in_freq; ro_in_freq++) {
                    prach_occasion_info_t *ro_p = &prach_conf_period_p->prach_occasion_slot_map[frame][slot].prach_occasion[ro_in_time][ro_in_freq];

                    ro_p->mapped_ssb_idx[0] = ssb_idx;
                    ro_p->nb_mapped_ssb = 1;
                    ssb_list.tx_ssb[ssb_idx].mapped_ro[ssb_list.tx_ssb[ssb_idx].nb_mapped_ro] = ro_p;
                    ssb_list.tx_ssb[ssb_idx].nb_mapped_ro++;
                    AssertFatal(MAX_NB_RO_PER_SSB_IN_ASSOCIATION_PATTERN > ssb_list.tx_ssb[ssb_idx].nb_mapped_ro,"Too many mapped ROs (%d) to a single SSB\n",
                                ssb_list.tx_ssb[ssb_idx].nb_mapped_ro);
                    nb_mapped_ro_in_association_period++;

                    LOG_D(NR_MAC,"Mapped ssb_idx %u to RO slot-symbol %u-%u, %u-%u-%u/%u\n",
                          ssb_idx, ro_p->slot, ro_p->start_symbol, slot, ro_in_time, ro_in_freq,
                          prach_conf_period_p->prach_occasion_slot_map[frame][slot].nb_of_prach_occasion_in_freq);
                    LOG_D(NR_MAC,"Nb mapped ROs for this ssb idx: in the association period only %u / total %u\n",
                          ssb_list.tx_ssb[ssb_idx].nb_mapped_ro, nb_mapped_ro_in_association_period);

                    // Exit the loop if this SSB has been mapped to all the required ROs
                    // WIP: Assuming that ssb_rach_ratio equals the maximum nb of times a given ssb_idx is mapped within an association period:
                    //      this is true if no PRACH occasions are conflicting with SSBs nor TDD_UL_DL_ConfigurationCommon schedule
                    if (nb_mapped_ro_in_association_period == ssb_rach_ratio) {
                      ro_in_freq++;
                      break;
                    }
                  } // for ro_in_freq

                  // Exit the loop if this SSB has been mapped to all the required ROs
                  if (nb_mapped_ro_in_association_period == ssb_rach_ratio) {
                    break;
                  }
                  else ro_in_freq = 0; // else go to the next time symbol in that slot and reset the freq index
                } // for ro_in_time

                // Exit the loop if this SSB has been mapped to all the required ROs
                if (nb_mapped_ro_in_association_period == ssb_rach_ratio) {
                  break;
                }
                else ro_in_time = 0; // else go to the next slot in that PRACH config period and reset the symbol index
              } // for slot

              // Exit the loop if this SSB has been mapped to all the required ROs
              if (nb_mapped_ro_in_association_period == ssb_rach_ratio) {
                break;
              }
              else slot = 0; // else go to the next frame in that PRACH config period and reset the slot index
            } // for frame

            // Exit the loop if this SSB has been mapped to all the required ROs
            if (nb_mapped_ro_in_association_period == ssb_rach_ratio) {
              break;
            }
            else frame = 0; // else go to the next PRACH config period in that association period and reset the frame index
          } // for n_prach_conf

        } // if ssb_idx is transmitted
      } // for ssb_idx
    } // else if multiple_ssbs_per_ro

  } // for association_period_index
}

// Returns a RACH occasion if any matches the SSB idx, the frame and the slot
static int get_nr_prach_info_from_ssb_index(uint8_t ssb_idx,
                                            int frame,
                                            int slot,
                                            prach_occasion_info_t **prach_occasion_info_pp) {

  ssb_info_t *ssb_info_p;
  prach_occasion_slot_t *prach_occasion_slot_p = NULL;

  *prach_occasion_info_pp = NULL;

  // Search for a matching RO slot in the SSB_to_RO map
  // A valid RO slot will match:
  //      - ssb_idx mapped to one of the ROs in that RO slot
  //      - exact slot number
  //      - frame offset
  ssb_info_p = &ssb_list.tx_ssb[ssb_idx];
  LOG_D(NR_MAC,"checking for prach : ssb_info_p->nb_mapped_ro %d\n",ssb_info_p->nb_mapped_ro);
  for (uint8_t n_mapped_ro=0; n_mapped_ro<ssb_info_p->nb_mapped_ro; n_mapped_ro++) {
    LOG_D(NR_MAC,"%d.%d: mapped_ro[%d]->frame.slot %d.%d, prach_assoc_pattern.nb_of_frame %d\n",
          frame,slot,n_mapped_ro,ssb_info_p->mapped_ro[n_mapped_ro]->frame,ssb_info_p->mapped_ro[n_mapped_ro]->slot,prach_assoc_pattern.nb_of_frame);
    if ((slot == ssb_info_p->mapped_ro[n_mapped_ro]->slot) &&
        (ssb_info_p->mapped_ro[n_mapped_ro]->frame == (frame % prach_assoc_pattern.nb_of_frame))) {

      uint8_t prach_config_period_nb = ssb_info_p->mapped_ro[n_mapped_ro]->frame / prach_assoc_pattern.prach_conf_period_list[0].nb_of_frame;
      uint8_t frame_nb_in_prach_config_period = ssb_info_p->mapped_ro[n_mapped_ro]->frame % prach_assoc_pattern.prach_conf_period_list[0].nb_of_frame;
      prach_occasion_slot_p = &prach_assoc_pattern.prach_conf_period_list[prach_config_period_nb].prach_occasion_slot_map[frame_nb_in_prach_config_period][slot];
    }
  }

  // If there is a matching RO slot in the SSB_to_RO map
  if (NULL != prach_occasion_slot_p)
  {
    // A random RO mapped to the SSB index should be selected in the slot

    // First count the number of times the SSB index is found in that RO
    uint8_t nb_mapped_ssb = 0;

    for (int ro_in_time=0; ro_in_time < prach_occasion_slot_p->nb_of_prach_occasion_in_time; ro_in_time++) {
      for (int ro_in_freq=0; ro_in_freq < prach_occasion_slot_p->nb_of_prach_occasion_in_freq; ro_in_freq++) {
        prach_occasion_info_t *prach_occasion_info_p = &prach_occasion_slot_p->prach_occasion[ro_in_time][ro_in_freq];

        for (uint8_t ssb_nb=0; ssb_nb<prach_occasion_info_p->nb_mapped_ssb; ssb_nb++) {
          if (prach_occasion_info_p->mapped_ssb_idx[ssb_nb] == ssb_idx) {
            nb_mapped_ssb++;
          }
        }
      }
    }

    // Choose a random SSB nb
    uint8_t random_ssb_nb = 0;

    random_ssb_nb = ((taus()) % nb_mapped_ssb);

    // Select the RO according to the chosen random SSB nb
    nb_mapped_ssb=0;
    for (int ro_in_time=0; ro_in_time < prach_occasion_slot_p->nb_of_prach_occasion_in_time; ro_in_time++) {
      for (int ro_in_freq=0; ro_in_freq < prach_occasion_slot_p->nb_of_prach_occasion_in_freq; ro_in_freq++) {
        prach_occasion_info_t *prach_occasion_info_p = &prach_occasion_slot_p->prach_occasion[ro_in_time][ro_in_freq];

        for (uint8_t ssb_nb=0; ssb_nb<prach_occasion_info_p->nb_mapped_ssb; ssb_nb++) {
          if (prach_occasion_info_p->mapped_ssb_idx[ssb_nb] == ssb_idx) {
            if (nb_mapped_ssb == random_ssb_nb) {
              *prach_occasion_info_pp = prach_occasion_info_p;
              return 1;
            }
            else {
              nb_mapped_ssb++;
            }
          }
        }
      }
    }
  }

  return 0;
}

// Build the SSB to RO mapping upon RRC configuration update
void build_ssb_to_ro_map(NR_UE_MAC_INST_t *mac) {

  // Clear all the lists and maps
  memset(&prach_assoc_pattern, 0, sizeof(prach_association_pattern_t));
  memset(&ssb_list, 0, sizeof(ssb_list_info_t));

  // Build the list of all the valid RACH occasions in the maximum association pattern period according to the PRACH config
  LOG_D(NR_MAC,"Build RO list\n");
  build_ro_list(mac);

  // Build the list of all the valid/transmitted SSBs according to the config
  LOG_D(NR_MAC,"Build SSB list\n");
  build_ssb_list(mac);

  // Map the transmitted SSBs to the ROs and create the association pattern according to the config
  LOG_D(NR_MAC,"Map SSB to RO\n");
  map_ssb_to_ro(mac);
  LOG_D(NR_MAC,"Map SSB to RO done\n");
}

void nr_ue_pucch_scheduler(module_id_t module_idP, frame_t frameP, int slotP, void *phy_data)
{
  NR_UE_MAC_INST_t *mac = get_mac_inst(module_idP);
  PUCCH_sched_t pucch[3] = {0}; // TODO the size might change in the future in case of multiple SR or multiple CSI in a slot

  mac->nr_ue_emul_l1.num_srs = 0;
  mac->nr_ue_emul_l1.num_harqs = 0;
  mac->nr_ue_emul_l1.num_csi_reports = 0;
  int num_res = 0;

  // SR
  if (mac->state == UE_CONNECTED && trigger_periodic_scheduling_request(mac, &pucch[0], frameP, slotP)) {
    num_res++;
    /* sr_payload = 1 means that this is a positive SR, sr_payload = 0 means that it is a negative SR */
    pucch[0].sr_payload = nr_ue_get_SR(module_idP, frameP, slotP);
  }

  // CSI
  int csi_res = 0;
  if (mac->state == UE_CONNECTED)
    csi_res = nr_get_csi_measurements(mac, frameP, slotP, &pucch[num_res]);
  if (csi_res > 0) {
    num_res += csi_res;
  }

  // ACKNACK
  bool any_harq = get_downlink_ack(mac, frameP, slotP, &pucch[num_res]);
  if (any_harq)
    num_res++;

  if (num_res == 0)
    return;
  // do no transmit pucch if only SR scheduled and it is negative
  if (num_res == 1 && pucch[0].n_sr > 0 && pucch[0].sr_payload == 0)
    return;

  if (num_res > 1)
    multiplex_pucch_resource(mac, pucch, num_res);
  fapi_nr_ul_config_request_t *ul_config = get_ul_config_request(mac, slotP, 0);
  pthread_mutex_lock(&ul_config->mutex_ul_config);
  for (int j = 0; j < num_res; j++) {
    if (pucch[j].n_harq + pucch[j].n_sr + pucch[j].n_csi != 0) {
      LOG_D(NR_MAC,
            "%d.%d configure pucch, O_ACK %d, O_SR %d, O_CSI %d\n",
            frameP,
            slotP,
            pucch[j].n_harq,
            pucch[j].n_sr,
            pucch[j].n_csi);
      mac->nr_ue_emul_l1.num_srs = pucch[j].n_sr;
      mac->nr_ue_emul_l1.num_harqs = pucch[j].n_harq;
      mac->nr_ue_emul_l1.num_csi_reports = pucch[j].n_csi;
      AssertFatal(ul_config->number_pdus < FAPI_NR_UL_CONFIG_LIST_NUM,
                  "ul_config->number_pdus %d out of bounds\n",
                  ul_config->number_pdus);
      fapi_nr_ul_config_pucch_pdu *pucch_pdu = &ul_config->ul_config_list[ul_config->number_pdus].pucch_config_pdu;
      fill_ul_config(ul_config, frameP, slotP, FAPI_NR_UL_CONFIG_TYPE_PUCCH);
      mac->nr_ue_emul_l1.active_uci_sfn_slot = NFAPI_SFNSLOT2HEX(frameP, slotP);
      pthread_mutex_unlock(&ul_config->mutex_ul_config);
      nr_ue_configure_pucch(mac,
                            slotP,
                            mac->crnti, // FIXME not sure this is valid for all pucch instances
                            &pucch[j],
                            pucch_pdu);
      nr_scheduled_response_t scheduled_response;
      fill_scheduled_response(&scheduled_response, NULL, ul_config, NULL, module_idP, 0 /*TBR fix*/, frameP, slotP, phy_data);
      if (mac->if_module != NULL && mac->if_module->scheduled_response != NULL)
        mac->if_module->scheduled_response(&scheduled_response);
      if (mac->state == UE_WAIT_TX_ACK_MSG4)
        mac->state = UE_CONNECTED;
    }
  }
}

void nr_schedule_csi_for_im(NR_UE_MAC_INST_t *mac, int frame, int slot) {

  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;

  if (!current_UL_BWP->csi_MeasConfig)
    return;

  NR_CSI_MeasConfig_t *csi_measconfig = current_UL_BWP->csi_MeasConfig;

  if (csi_measconfig->csi_IM_ResourceToAddModList == NULL)
    return;

  fapi_nr_dl_config_request_t *dl_config = get_dl_config_request(mac, slot);
  NR_CSI_IM_Resource_t *imcsi;
  int period, offset;

  NR_UE_DL_BWP_t *current_DL_BWP = &mac->current_DL_BWP;
  int mu = current_DL_BWP->scs;
  uint16_t bwp_size = current_DL_BWP->BWPSize;
  uint16_t bwp_start = current_DL_BWP->BWPStart;

  for (int id = 0; id < csi_measconfig->csi_IM_ResourceToAddModList->list.count; id++){
    imcsi = csi_measconfig->csi_IM_ResourceToAddModList->list.array[id];
    csi_period_offset(NULL,imcsi->periodicityAndOffset,&period,&offset);
    if((frame*nr_slots_per_frame[mu]+slot-offset)%period != 0)
      continue;
    fapi_nr_dl_config_csiim_pdu_rel15_t *csiim_config_pdu = &dl_config->dl_config_list[dl_config->number_pdus].csiim_config_pdu.csiim_config_rel15;
    csiim_config_pdu->bwp_size = bwp_size;
    csiim_config_pdu->bwp_start = bwp_start;
    csiim_config_pdu->subcarrier_spacing = mu;
    csiim_config_pdu->start_rb = imcsi->freqBand->startingRB;
    csiim_config_pdu->nr_of_rbs = imcsi->freqBand->nrofRBs;
    // As specified in 5.2.2.4 of 38.214
    switch (imcsi->csi_IM_ResourceElementPattern->present) {
      case NR_CSI_IM_Resource__csi_IM_ResourceElementPattern_PR_pattern0:
        for (int i = 0; i<4; i++) {
          csiim_config_pdu->k_csiim[i] = (imcsi->csi_IM_ResourceElementPattern->choice.pattern0->subcarrierLocation_p0<<1) + (i>>1);
          csiim_config_pdu->l_csiim[i] = imcsi->csi_IM_ResourceElementPattern->choice.pattern0->symbolLocation_p0 + (i%2);
        }
        break;
      case NR_CSI_IM_Resource__csi_IM_ResourceElementPattern_PR_pattern1:
        for (int i = 0; i<4; i++) {
          csiim_config_pdu->k_csiim[i] = (imcsi->csi_IM_ResourceElementPattern->choice.pattern1->subcarrierLocation_p1<<2) + i;
          csiim_config_pdu->l_csiim[i] = imcsi->csi_IM_ResourceElementPattern->choice.pattern1->symbolLocation_p1;
        }
        break;
      default:
        AssertFatal(1==0, "Invalid CSI-IM pattern\n");
    }
    dl_config->dl_config_list[dl_config->number_pdus].pdu_type = FAPI_NR_DL_CONFIG_TYPE_CSI_IM;
    dl_config->number_pdus += 1;
  }
}

NR_CSI_ResourceConfigId_t find_CSI_resourceconfig(NR_CSI_MeasConfig_t *csi_measconfig,
                                                  NR_BWP_Id_t dl_bwp_id,
                                                  NR_NZP_CSI_RS_ResourceId_t csi_id)
{
  bool found = false;
  for (int csi_list = 0; csi_list < csi_measconfig->csi_ResourceConfigToAddModList->list.count; csi_list++) {
    NR_CSI_ResourceConfig_t *csires = csi_measconfig->csi_ResourceConfigToAddModList->list.array[csi_list];
    if(csires->bwp_Id != dl_bwp_id)
      continue;
    struct NR_CSI_ResourceConfig__csi_RS_ResourceSetList *resset = &csires->csi_RS_ResourceSetList;
    if(resset->present != NR_CSI_ResourceConfig__csi_RS_ResourceSetList_PR_nzp_CSI_RS_SSB)
      continue;
    if(!resset->choice.nzp_CSI_RS_SSB->nzp_CSI_RS_ResourceSetList)
      continue;
    for(int i = 0; i < resset->choice.nzp_CSI_RS_SSB->nzp_CSI_RS_ResourceSetList->list.count; i++) {
      NR_NZP_CSI_RS_ResourceSetId_t *res_id = resset->choice.nzp_CSI_RS_SSB->nzp_CSI_RS_ResourceSetList->list.array[i];
      AssertFatal(res_id, "NR_NZP_CSI_RS_ResourceSetId shouldn't be NULL\n");
      struct NR_CSI_MeasConfig__nzp_CSI_RS_ResourceSetToAddModList *res_list = csi_measconfig->nzp_CSI_RS_ResourceSetToAddModList;
      AssertFatal(res_list, "nzp_CSI_RS_ResourceSetToAddModList shouldn't be NULL\n");
      for (int j = 0; j < res_list->list.count; j++) {
        NR_NZP_CSI_RS_ResourceSet_t *csi_res = res_list->list.array[j];
        if(*res_id != csi_res->nzp_CSI_ResourceSetId)
          continue;
        for (int k = 0; k < csi_res->nzp_CSI_RS_Resources.list.count; k++) {
          AssertFatal(csi_res->nzp_CSI_RS_Resources.list.array[k],
                      "NZP_CSI_RS_ResourceId shoulan't be NULL\n");
          if (csi_id == *csi_res->nzp_CSI_RS_Resources.list.array[k]) {
            found = true;
            break;
          }
        }
        if (found && csi_res->trs_Info)
          // CRI-RS for Tracking (not implemented yet)
          // in this case we there is no associated CSI report
          // therefore to signal this we return a value higher than
          // maxNrofCSI-ResourceConfigurations
          return NR_maxNrofCSI_ResourceConfigurations + 1;
        else if (found)
          return csires->csi_ResourceConfigId;
      }
    }
  }
  return -1; // not found any CSI-resource in current DL BWP associated with this CSI-RS ID
}

uint8_t set_csirs_measurement_bitmap(NR_CSI_MeasConfig_t *csi_measconfig,
                                     NR_CSI_ResourceConfigId_t csi_res_id)
{
  uint8_t meas_bitmap = 0;
  if (csi_res_id > NR_maxNrofCSI_ResourceConfigurations)
    return meas_bitmap; // CSI-RS for tracking
  for(int i = 0; i < csi_measconfig->csi_ReportConfigToAddModList->list.count; i++) {
    struct NR_CSI_ReportConfig *report_config = csi_measconfig->csi_ReportConfigToAddModList->list.array[i];
    if(report_config->resourcesForChannelMeasurement != csi_res_id)
      continue;
    // bit 0 RSRP bit 1 RI bit 2 LI bit 3 PMI bit 4 CQI bit 5 i1
    switch (report_config->reportQuantity.present) {
      case NR_CSI_ReportConfig__reportQuantity_PR_cri_RI_PMI_CQI :
        meas_bitmap += (1 << 1) + (1 << 3) + (1 << 4);
        break;
      case NR_CSI_ReportConfig__reportQuantity_PR_cri_RI_i1 :
        meas_bitmap += (1 << 1) + (1 << 5);
        break;
      case NR_CSI_ReportConfig__reportQuantity_PR_cri_RI_i1_CQI :
        meas_bitmap += (1 << 1) + (1 << 4) + (1 << 5);
        break;
      case NR_CSI_ReportConfig__reportQuantity_PR_cri_RI_CQI :
        meas_bitmap += (1 << 1) + (1 << 4);
        break;
      case NR_CSI_ReportConfig__reportQuantity_PR_cri_RSRP :
        meas_bitmap += 1;
        break;
      case NR_CSI_ReportConfig__reportQuantity_PR_cri_RI_LI_PMI_CQI :
        meas_bitmap += (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4);
        break;
      default :
        AssertFatal(false, "Unexpected measurement report type %d\n",
                    report_config->reportQuantity.present);
    }
  }
  AssertFatal(meas_bitmap > 0, "Expected to have at least 1 measurement configured for CSI-RS\n");
  return meas_bitmap;
}

void nr_schedule_csirs_reception(NR_UE_MAC_INST_t *mac, int frame, int slot) {

  NR_UE_UL_BWP_t *current_UL_BWP = &mac->current_UL_BWP;

  if (!current_UL_BWP->csi_MeasConfig)
    return;

  NR_CSI_MeasConfig_t *csi_measconfig = current_UL_BWP->csi_MeasConfig;

  if (csi_measconfig->nzp_CSI_RS_ResourceToAddModList == NULL)
    return;

  fapi_nr_dl_config_request_t *dl_config = get_dl_config_request(mac, slot);
  NR_UE_DL_BWP_t *current_DL_BWP = &mac->current_DL_BWP;
  NR_BWP_Id_t dl_bwp_id = current_DL_BWP->bwp_id;

  int mu = current_DL_BWP->scs;
  uint16_t bwp_size = current_DL_BWP->BWPSize;
  uint16_t bwp_start = current_DL_BWP->BWPStart;

  for (int id = 0; id < csi_measconfig->nzp_CSI_RS_ResourceToAddModList->list.count; id++){
    NR_NZP_CSI_RS_Resource_t *nzpcsi = csi_measconfig->nzp_CSI_RS_ResourceToAddModList->list.array[id];
    int period, offset;
    csi_period_offset(NULL, nzpcsi->periodicityAndOffset, &period, &offset);
    if((frame * nr_slots_per_frame[mu] + slot-offset) % period != 0)
      continue;
    NR_CSI_ResourceConfigId_t csi_res_id = find_CSI_resourceconfig(csi_measconfig,
                                                                   dl_bwp_id,
                                                                   nzpcsi->nzp_CSI_RS_ResourceId);
    // do not schedule reseption of this CSI-RS if not associated with current BWP
    if(csi_res_id < 0)
      continue;
    LOG_D(MAC,"Scheduling reception of CSI-RS in frame %d slot %d\n", frame, slot);
    fapi_nr_dl_config_csirs_pdu_rel15_t *csirs_config_pdu = &dl_config->dl_config_list[dl_config->number_pdus].csirs_config_pdu.csirs_config_rel15;
    csirs_config_pdu->measurement_bitmap = set_csirs_measurement_bitmap(csi_measconfig, csi_res_id);
    NR_CSI_RS_ResourceMapping_t  resourceMapping = nzpcsi->resourceMapping;
    csirs_config_pdu->subcarrier_spacing = mu;
    csirs_config_pdu->cyclic_prefix = current_DL_BWP->cyclicprefix ? *current_DL_BWP->cyclicprefix : 0;

    // According to last paragraph of TS 38.214 5.2.2.3.1
    if (resourceMapping.freqBand.startingRB < bwp_start) {
      csirs_config_pdu->start_rb = bwp_start;
    } else {
      csirs_config_pdu->start_rb = resourceMapping.freqBand.startingRB;
    }
    if (resourceMapping.freqBand.nrofRBs > (bwp_start + bwp_size - csirs_config_pdu->start_rb)) {
      csirs_config_pdu->nr_of_rbs = bwp_start + bwp_size - csirs_config_pdu->start_rb;
    } else {
      csirs_config_pdu->nr_of_rbs = resourceMapping.freqBand.nrofRBs;
    }
    AssertFatal(csirs_config_pdu->nr_of_rbs >= 24, "CSI-RS has %d RBs, but the minimum is 24\n", csirs_config_pdu->nr_of_rbs);

    csirs_config_pdu->csi_type = 1; // NZP-CSI-RS
    csirs_config_pdu->symb_l0 = resourceMapping.firstOFDMSymbolInTimeDomain;
    if (resourceMapping.firstOFDMSymbolInTimeDomain2)
      csirs_config_pdu->symb_l1 = *resourceMapping.firstOFDMSymbolInTimeDomain2;
    csirs_config_pdu->cdm_type = resourceMapping.cdm_Type;
    csirs_config_pdu->freq_density = resourceMapping.density.present;
    if ((resourceMapping.density.present == NR_CSI_RS_ResourceMapping__density_PR_dot5)
        && (resourceMapping.density.choice.dot5 == NR_CSI_RS_ResourceMapping__density__dot5_evenPRBs))
      csirs_config_pdu->freq_density--;
    csirs_config_pdu->scramb_id = nzpcsi->scramblingID;
    csirs_config_pdu->power_control_offset = nzpcsi->powerControlOffset + 8;
    if (nzpcsi->powerControlOffsetSS)
      csirs_config_pdu->power_control_offset_ss = *nzpcsi->powerControlOffsetSS;
    else
      csirs_config_pdu->power_control_offset_ss = 1; // 0 dB
    switch(resourceMapping.frequencyDomainAllocation.present){
      case NR_CSI_RS_ResourceMapping__frequencyDomainAllocation_PR_row1:
        csirs_config_pdu->row = 1;
        csirs_config_pdu->freq_domain = ((resourceMapping.frequencyDomainAllocation.choice.row1.buf[0])>>4)&0x0f;
        break;
      case NR_CSI_RS_ResourceMapping__frequencyDomainAllocation_PR_row2:
        csirs_config_pdu->row = 2;
        csirs_config_pdu->freq_domain = (((resourceMapping.frequencyDomainAllocation.choice.row2.buf[1]>>4)&0x0f) |
                                        ((resourceMapping.frequencyDomainAllocation.choice.row2.buf[0]<<4)&0xff0));
        break;
      case NR_CSI_RS_ResourceMapping__frequencyDomainAllocation_PR_row4:
        csirs_config_pdu->row = 4;
        csirs_config_pdu->freq_domain = ((resourceMapping.frequencyDomainAllocation.choice.row4.buf[0])>>5)&0x07;
        break;
      case NR_CSI_RS_ResourceMapping__frequencyDomainAllocation_PR_other:
        csirs_config_pdu->freq_domain = ((resourceMapping.frequencyDomainAllocation.choice.other.buf[0])>>2)&0x3f;
        // determining the row of table 7.4.1.5.3-1 in 38.211
        switch(resourceMapping.nrofPorts){
          case NR_CSI_RS_ResourceMapping__nrofPorts_p1:
            AssertFatal(1==0,"Resource with 1 CSI port shouldn't be within other rows\n");
            break;
          case NR_CSI_RS_ResourceMapping__nrofPorts_p2:
            csirs_config_pdu->row = 3;
            break;
          case NR_CSI_RS_ResourceMapping__nrofPorts_p4:
            csirs_config_pdu->row = 5;
            break;
          case NR_CSI_RS_ResourceMapping__nrofPorts_p8:
            if (resourceMapping.cdm_Type == NR_CSI_RS_ResourceMapping__cdm_Type_cdm4_FD2_TD2)
              csirs_config_pdu->row = 8;
            else{
              int num_k = 0;
              for (int k=0; k<6; k++)
                num_k+=(((csirs_config_pdu->freq_domain)>>k)&0x01);
              if(num_k==4)
                csirs_config_pdu->row = 6;
              else
                csirs_config_pdu->row = 7;
            }
            break;
          case NR_CSI_RS_ResourceMapping__nrofPorts_p12:
            if (resourceMapping.cdm_Type == NR_CSI_RS_ResourceMapping__cdm_Type_cdm4_FD2_TD2)
              csirs_config_pdu->row = 10;
            else
              csirs_config_pdu->row = 9;
            break;
          case NR_CSI_RS_ResourceMapping__nrofPorts_p16:
            if (resourceMapping.cdm_Type == NR_CSI_RS_ResourceMapping__cdm_Type_cdm4_FD2_TD2)
              csirs_config_pdu->row = 12;
            else
              csirs_config_pdu->row = 11;
            break;
          case NR_CSI_RS_ResourceMapping__nrofPorts_p24:
            if (resourceMapping.cdm_Type == NR_CSI_RS_ResourceMapping__cdm_Type_cdm4_FD2_TD2)
              csirs_config_pdu->row = 14;
            else{
              if (resourceMapping.cdm_Type == NR_CSI_RS_ResourceMapping__cdm_Type_cdm8_FD2_TD4)
                csirs_config_pdu->row = 15;
              else
                csirs_config_pdu->row = 13;
            }
            break;
          case NR_CSI_RS_ResourceMapping__nrofPorts_p32:
            if (resourceMapping.cdm_Type == NR_CSI_RS_ResourceMapping__cdm_Type_cdm4_FD2_TD2)
              csirs_config_pdu->row = 17;
            else{
              if (resourceMapping.cdm_Type == NR_CSI_RS_ResourceMapping__cdm_Type_cdm8_FD2_TD4)
                csirs_config_pdu->row = 18;
              else
                csirs_config_pdu->row = 16;
            }
            break;
        default:
          AssertFatal(1==0,"Invalid number of ports in CSI-RS resource\n");
        }
        break;
      default:
        AssertFatal(1==0,"Invalid freqency domain allocation in CSI-RS resource\n");
    }
    dl_config->dl_config_list[dl_config->number_pdus].pdu_type = FAPI_NR_DL_CONFIG_TYPE_CSI_RS;
    dl_config->number_pdus += 1;
  }
}


// This function schedules the PRACH according to prach_ConfigurationIndex and TS 38.211, tables 6.3.3.2.x
// PRACH formats 9, 10, 11 are corresponding to dual PRACH format configurations A1/B1, A2/B2, A3/B3.
// - todo:
// - Partial configuration is actually already stored in (fapi_nr_prach_config_t) &mac->phy_config.config_req->prach_config
void nr_ue_prach_scheduler(module_id_t module_idP, frame_t frameP, sub_frame_t slotP) {

  NR_UE_MAC_INST_t *mac = get_mac_inst(module_idP);
  RA_config_t *ra = &mac->ra;
  ra->RA_offset = 2; // to compensate the rx frame offset at the gNB
  if(ra->ra_state != GENERATE_PREAMBLE)
    return;

  fapi_nr_ul_config_request_t *ul_config = get_ul_config_request(mac, slotP, 0);
  if (!ul_config) {
    LOG_E(NR_MAC, "mac->ul_config is null! \n");
    return;
  }

  fapi_nr_ul_config_prach_pdu *prach_config_pdu;
  fapi_nr_config_request_t *cfg = &mac->phy_config.config_req;
  fapi_nr_prach_config_t *prach_config = &cfg->prach_config;
  nr_scheduled_response_t scheduled_response;

  NR_ServingCellConfigCommon_t *scc = mac->scc;
  NR_ServingCellConfigCommonSIB_t *scc_SIB = mac->scc_SIB;
  NR_RACH_ConfigCommon_t *setup = mac->current_UL_BWP.rach_ConfigCommon;
  NR_RACH_ConfigGeneric_t *rach_ConfigGeneric = &setup->rach_ConfigGeneric;

  NR_TDD_UL_DL_ConfigCommon_t *tdd_config = scc==NULL ? scc_SIB->tdd_UL_DL_ConfigurationCommon : scc->tdd_UL_DL_ConfigurationCommon;

  if (is_nr_UL_slot(tdd_config, slotP, mac->frame_type)) {

    // WIP Need to get the proper selected ssb_idx
    //     Initial beam selection functionality is not available yet
    uint8_t selected_gnb_ssb_idx = mac->mib_ssb;

    // Get any valid PRACH occasion in the current slot for the selected SSB index
    prach_occasion_info_t *prach_occasion_info_p;
    int is_nr_prach_slot = get_nr_prach_info_from_ssb_index(selected_gnb_ssb_idx,
                                                       (int)frameP,
                                                       (int)slotP,
                                                        &prach_occasion_info_p);

    if (is_nr_prach_slot) {
      AssertFatal(NULL != prach_occasion_info_p,"PRACH Occasion Info not returned in a valid NR Prach Slot\n");

      init_RA(module_idP, &ra->prach_resources, setup, rach_ConfigGeneric, ra->rach_ConfigDedicated);
      nr_get_RA_window(mac);

      uint16_t format = prach_occasion_info_p->format;
      uint16_t format0 = format & 0xff;        // single PRACH format
      uint16_t format1 = (format >> 8) & 0xff; // dual PRACH format
      AssertFatal(ul_config->number_pdus < sizeof(ul_config->ul_config_list) / sizeof(ul_config->ul_config_list[0]),
                  "Number of PDUS in ul_config = %d > ul_config_list num elements", ul_config->number_pdus);

      pthread_mutex_lock(&ul_config->mutex_ul_config);
      AssertFatal(ul_config->number_pdus<FAPI_NR_UL_CONFIG_LIST_NUM, "ul_config->number_pdus %d out of bounds\n",ul_config->number_pdus);
      prach_config_pdu = &ul_config->ul_config_list[ul_config->number_pdus].prach_config_pdu;
      memset(prach_config_pdu, 0, sizeof(fapi_nr_ul_config_prach_pdu));
      fill_ul_config(ul_config, frameP, slotP, FAPI_NR_UL_CONFIG_TYPE_PRACH);
      pthread_mutex_unlock(&ul_config->mutex_ul_config);
      LOG_D(PHY, "In %s: (%p) %d UL PDUs:\n", __FUNCTION__, ul_config, ul_config->number_pdus);

      memset(prach_config_pdu, 0, sizeof(fapi_nr_ul_config_prach_pdu));

      uint16_t ncs = get_NCS(rach_ConfigGeneric->zeroCorrelationZoneConfig, format0, setup->restrictedSetConfig);

      prach_config_pdu->phys_cell_id = mac->physCellId;
      prach_config_pdu->num_prach_ocas = 1;
      prach_config_pdu->prach_slot = prach_occasion_info_p->slot;
      prach_config_pdu->prach_start_symbol = prach_occasion_info_p->start_symbol;
      prach_config_pdu->num_ra = prach_occasion_info_p->fdm;

      prach_config_pdu->num_cs = ncs;
      prach_config_pdu->root_seq_id = prach_config->num_prach_fd_occasions_list[prach_occasion_info_p->fdm].prach_root_sequence_index;
      prach_config_pdu->restricted_set = prach_config->restricted_set_config;
      prach_config_pdu->freq_msg1 = prach_config->num_prach_fd_occasions_list[prach_occasion_info_p->fdm].k1;

      LOG_I(NR_MAC,"PRACH scheduler: Selected RO Frame %u, Slot %u, Symbol %u, Fdm %u\n",
            frameP, prach_config_pdu->prach_slot, prach_config_pdu->prach_start_symbol, prach_config_pdu->num_ra);

      // Search which SSB is mapped in the RO (among all the SSBs mapped to this RO)
      for (int ssb_nb_in_ro=0; ssb_nb_in_ro<prach_occasion_info_p->nb_mapped_ssb; ssb_nb_in_ro++) {
        if (prach_occasion_info_p->mapped_ssb_idx[ssb_nb_in_ro] == selected_gnb_ssb_idx) {
          ra->ssb_nb_in_ro = ssb_nb_in_ro;
          break;
        }
      }
      AssertFatal(ra->ssb_nb_in_ro<prach_occasion_info_p->nb_mapped_ssb, "%u not found in the mapped SSBs to the PRACH occasion", selected_gnb_ssb_idx);

      if (format1 != 0xff) {
        switch(format0) { // dual PRACH format
          case 0xa1:
            prach_config_pdu->prach_format = 11;
            break;
          case 0xa2:
            prach_config_pdu->prach_format = 12;
            break;
          case 0xa3:
            prach_config_pdu->prach_format = 13;
            break;
        default:
          AssertFatal(1 == 0, "Only formats A1/B1 A2/B2 A3/B3 are valid for dual format");
        }
      } else {
        switch(format0) { // single PRACH format
          case 0:
            prach_config_pdu->prach_format = 0;
            break;
          case 1:
            prach_config_pdu->prach_format = 1;
            break;
          case 2:
            prach_config_pdu->prach_format = 2;
            break;
          case 3:
            prach_config_pdu->prach_format = 3;
            break;
          case 0xa1:
            prach_config_pdu->prach_format = 4;
            break;
          case 0xa2:
            prach_config_pdu->prach_format = 5;
            break;
          case 0xa3:
            prach_config_pdu->prach_format = 6;
            break;
          case 0xb1:
            prach_config_pdu->prach_format = 7;
            break;
          case 0xb4:
            prach_config_pdu->prach_format = 8;
            break;
          case 0xc0:
            prach_config_pdu->prach_format = 9;
            break;
          case 0xc2:
            prach_config_pdu->prach_format = 10;
            break;
          default:
            AssertFatal(1 == 0, "Invalid PRACH format");
        }
      } // if format1

      nr_get_prach_resources(module_idP, 0, 0, &ra->prach_resources, ra->rach_ConfigDedicated);
      prach_config_pdu->ra_PreambleIndex = ra->ra_PreambleIndex;
      prach_config_pdu->prach_tx_power = get_prach_tx_power(module_idP);
      set_ra_rnti(mac, prach_config_pdu);

      fill_scheduled_response(&scheduled_response, NULL, ul_config, NULL, module_idP, 0 /*TBR fix*/, frameP, slotP, NULL);
      if(mac->if_module != NULL && mac->if_module->scheduled_response != NULL)
        mac->if_module->scheduled_response(&scheduled_response);

      nr_Msg1_transmitted(module_idP);
    } // is_nr_prach_slot
  } // if is_nr_UL_slot
}

// This function schedules the reception of SIB1 after initial sync and before going to real time state
void nr_ue_sib1_scheduler(module_id_t module_idP,
                          int cc_id,
                          uint16_t ssb_start_symbol,
                          uint16_t frame,
                          uint8_t ssb_subcarrier_offset,
                          uint32_t ssb_index,
                          uint16_t ssb_start_subcarrier,
                          frequency_range_t frequency_range,
                          void *phy_data) {

  NR_UE_MAC_INST_t *mac = get_mac_inst(module_idP);
  nr_scheduled_response_t scheduled_response;

  uint8_t scs_ssb = get_softmodem_params()->numerology;
  uint16_t ssb_offset_point_a = (ssb_start_subcarrier - ssb_subcarrier_offset)/12;

  get_type0_PDCCH_CSS_config_parameters(&mac->type0_PDCCH_CSS_config,
                                        frame,
                                        mac->mib,
                                        nr_slots_per_frame[scs_ssb],
                                        ssb_subcarrier_offset,
                                        ssb_start_symbol,
                                        scs_ssb,
                                        frequency_range,
                                        mac->nr_band,
                                        ssb_index,
                                        1, // If the UE is not configured with a periodicity, the UE assumes a periodicity of a half frame
                                        ssb_offset_point_a);

  int frame_s,slot_s;
  if(mac->type0_PDCCH_CSS_config.type0_pdcch_ss_mux_pattern == 1){
    // same frame as ssb
    if ((mac->type0_PDCCH_CSS_config.frame & 0x1) == mac->type0_PDCCH_CSS_config.sfn_c)
      frame_s = 0;
    else
      frame_s = 1;
    slot_s = mac->type0_PDCCH_CSS_config.n_0;
  }
  else{
    frame_s = 0; // same frame as ssb
    slot_s = mac->type0_PDCCH_CSS_config.n_c;
  }

  if(mac->search_space_zero == NULL)
    mac->search_space_zero=calloc(1,sizeof(*mac->search_space_zero));
  if(mac->coreset0 == NULL)
    mac->coreset0 = calloc(1,sizeof(*mac->coreset0));
  fill_coresetZero(mac->coreset0, &mac->type0_PDCCH_CSS_config);
  fill_searchSpaceZero(mac->search_space_zero, &mac->type0_PDCCH_CSS_config);

  fapi_nr_dl_config_request_t *dl_config = &mac->dl_config_request[0]; // Take the first dl_config_request for SIB1
  config_dci_pdu(mac, dl_config, NR_RNTI_SI, slot_s, mac->search_space_zero);

  LOG_D(MAC,"Calling fill_scheduled_response, type0_pdcch, num_pdus %d\n", dl_config->number_pdus);
  fill_scheduled_response(&scheduled_response, dl_config, NULL, NULL, module_idP, cc_id, frame_s, slot_s, phy_data);

  if (dl_config->number_pdus) {
    if(mac->if_module != NULL && mac->if_module->scheduled_response != NULL)
      mac->if_module->scheduled_response(&scheduled_response);
  }
  else
    AssertFatal(1==0,"Unable to find aggregation level for type0 CSS\n");
}


#define MAX_LCID 8 // NR_MAX_NUM_LCID shall be used but the mac_rlc_data_req function can fetch data for max 8 LCID
typedef struct {
  uint8_t bsr_len;
  uint8_t bsr_ce_len;
  uint8_t bsr_header_len;
  uint8_t phr_len;
  uint8_t phr_ce_len;
  uint8_t phr_header_len;
  uint16_t sdu_length_total;
  NR_BSR_SHORT *bsr_s;
  NR_BSR_LONG *bsr_l;
  NR_BSR_SHORT *bsr_t;
  //NR_POWER_HEADROOM_CMD *phr_pr;
  int tot_mac_ce_len;
  uint8_t total_mac_pdu_header_len;
} NR_UE_MAC_CE_INFO;

/*
nr_ue_get_sdu_mac_ce_pre finds length in various mac_ce field
Need nothing from mac_ce_p:
Update the following in mac_ce_p:
	bsr_len;
	bsr_ce_len;
	bsr_header_len;
	phr_len; TBD
	phr_ce_len; TBD
	phr_header_len; TBD
*/
int nr_ue_get_sdu_mac_ce_pre(module_id_t module_idP,
                      int CC_id,
                      frame_t frameP,
                      sub_frame_t subframe,
                      uint8_t gNB_index,
                      uint8_t *ulsch_buffer,
                      uint16_t buflen,
                      NR_UE_MAC_CE_INFO *mac_ce_p) {
  NR_UE_MAC_INST_t *mac = get_mac_inst(module_idP);

  int num_lcg_id_with_data = 0;
  // Preparing the MAC CEs sub-PDUs and get the total size
  mac_ce_p->bsr_header_len = 0;
  mac_ce_p->phr_header_len = 0;   //sizeof(SCH_SUBHEADER_FIXED);
  int lcg_id = 0;
  while (lcg_id < NR_MAX_NUM_LCGID) {
    if (mac->scheduling_info.BSR_bytes[lcg_id]) {
      num_lcg_id_with_data++;
    }

    lcg_id++;
  }

  //Restart ReTxBSR Timer at new grant indication (38.321)
  if (mac->scheduling_info.retxBSR_SF != MAC_UE_BSR_TIMER_NOT_RUNNING) {
    mac->scheduling_info.retxBSR_SF = nr_get_sf_retxBSRTimer(mac->scheduling_info.retxBSR_Timer);
  }

  // periodicBSR-Timer expires, trigger BSR
  if ((mac->scheduling_info.periodicBSR_Timer != NR_BSR_Config__periodicBSR_Timer_infinity)
      && (mac->scheduling_info.periodicBSR_SF == 0)) {
    // Trigger BSR Periodic
    mac->BSR_reporting_active |= NR_BSR_TRIGGER_PERIODIC;
    LOG_D(NR_MAC, "[UE %d] MAC BSR Triggered PeriodicBSR Timer expiry at frame%d subframe %d TBS=%d\n",
          module_idP, frameP, subframe, buflen);
  }

  //Compute BSR Length if Regular or Periodic BSR is triggered
  //WARNING: if BSR long is computed, it may be changed to BSR short during or after multiplexing if there remains less than 1 LCGROUP with data after Tx
  if (mac->BSR_reporting_active) {
    AssertFatal((mac->BSR_reporting_active & NR_BSR_TRIGGER_PADDING) == 0,
                "Inconsistent BSR Trigger=%d !\n",
                mac->BSR_reporting_active);

    //A Regular or Periodic BSR can only be sent if TBS is sufficient as transmitting only a BSR is not allowed if UE has data to transmit
    if (num_lcg_id_with_data <= 1) {
      if (buflen >= (sizeof(NR_BSR_SHORT)+sizeof(NR_MAC_SUBHEADER_FIXED)+1)) {
        mac_ce_p->bsr_ce_len = sizeof(NR_BSR_SHORT); //1 byte
        mac_ce_p->bsr_header_len = sizeof(NR_MAC_SUBHEADER_FIXED); //1 byte
      }
    } else {
      if (buflen >= (num_lcg_id_with_data+1+sizeof(NR_MAC_SUBHEADER_SHORT)+1)) {
        mac_ce_p->bsr_ce_len = num_lcg_id_with_data + 1; //variable size
        mac_ce_p->bsr_header_len = sizeof(NR_MAC_SUBHEADER_SHORT); //2 bytes
      }
    }
  }

  mac_ce_p->bsr_len = mac_ce_p->bsr_ce_len + mac_ce_p->bsr_header_len;
  return (mac_ce_p->bsr_len + mac_ce_p->phr_len);
}

/*
nr_ue_get_sdu_mac_ce_post recalculates length and prepares the mac_ce field
Need the following from mac_ce_p:
	bsr_ce_len
	bsr_len
	sdu_length_total
	total_mac_pdu_header_len
Update the following in mac_ce_p:
	bsr_ce_len
	bsr_header_len
	bsr_len
	tot_mac_ce_len
	total_mac_pdu_header_len
	bsr_s
	bsr_l
	bsr_t
*/
void nr_ue_get_sdu_mac_ce_post(module_id_t module_idP,
                      int CC_id,
                      frame_t frameP,
                      sub_frame_t subframe,
                      uint8_t gNB_index,
                      uint8_t *ulsch_buffer,
                      uint16_t buflen,
                      NR_UE_MAC_CE_INFO *mac_ce_p) {
  NR_UE_MAC_INST_t *mac = get_mac_inst(module_idP);

  // Compute BSR Values and update Nb LCGID with data after multiplexing
  unsigned short padding_len = 0;
  uint8_t lcid = 0;
  int lcg_id = 0;
  int num_lcg_id_with_data = 0;
  int lcg_id_bsr_trunc = 0;
  for (lcg_id = 0; lcg_id < NR_MAX_NUM_LCGID; lcg_id++) {
	if (mac_ce_p->bsr_ce_len == sizeof(NR_BSR_SHORT)) {
      mac->scheduling_info.BSR[lcg_id] = nr_locate_BsrIndexByBufferSize(NR_SHORT_BSR_TABLE, NR_SHORT_BSR_TABLE_SIZE, mac->scheduling_info.BSR_bytes[lcg_id]);
	} else {
      mac->scheduling_info.BSR[lcg_id] = nr_locate_BsrIndexByBufferSize(NR_LONG_BSR_TABLE, NR_LONG_BSR_TABLE_SIZE, mac->scheduling_info.BSR_bytes[lcg_id]);
	}
    if (mac->scheduling_info.BSR_bytes[lcg_id]) {
      num_lcg_id_with_data++;
      lcg_id_bsr_trunc = lcg_id;
    }
  }

  // TS 38.321 Section 5.4.5
  // Check BSR padding: it is done after PHR according to Logical Channel Prioritization order
  // Check for max padding size, ie MAC Hdr for last RLC PDU = 1
  /* For Padding BSR:
     -  if the number of padding bits is equal to or larger than the size of the Short BSR plus its subheader but smaller than the size of the Long BSR plus its subheader:
     -  if more than one LCG has data available for transmission in the TTI where the BSR is transmitted: report Truncated BSR of the LCG with the highest priority logical channel with data available for transmission;
     -  else report Short BSR.
     -  else if the number of padding bits is equal to or larger than the size of the Long BSR plus its subheader, report Long BSR.
   */
  if (mac_ce_p->sdu_length_total) {
    padding_len = buflen - (mac_ce_p->total_mac_pdu_header_len + mac_ce_p->sdu_length_total);
  }

  if ((padding_len) && (mac_ce_p->bsr_len == 0)) {
    /* if the number of padding bits is equal to or larger than the size of the Long BSR plus its subheader, report Long BSR */
    if (padding_len >= (num_lcg_id_with_data+1+sizeof(NR_MAC_SUBHEADER_SHORT))) {
      mac_ce_p->bsr_ce_len = num_lcg_id_with_data + 1; //variable size
      mac_ce_p->bsr_header_len = sizeof(NR_MAC_SUBHEADER_SHORT); //2 bytes
      // Trigger BSR Padding
      mac->BSR_reporting_active |= NR_BSR_TRIGGER_PADDING;
    } else if (padding_len >= (sizeof(NR_BSR_SHORT)+sizeof(NR_MAC_SUBHEADER_FIXED))) {
      mac_ce_p->bsr_ce_len = sizeof(NR_BSR_SHORT); //1 byte
      mac_ce_p->bsr_header_len = sizeof(NR_MAC_SUBHEADER_FIXED); //1 byte

      if (num_lcg_id_with_data > 1) {
        // REPORT SHORT TRUNCATED BSR
        //Get LCGID of highest priority LCID with data (todo)
        for (lcid = DCCH; lcid < NR_MAX_NUM_LCID; lcid++) {
          lcg_id = mac->scheduling_info.LCGID[lcid];
          if ((lcg_id < NR_MAX_NUM_LCGID) && (mac->scheduling_info.BSR_bytes[lcg_id])) {
            lcg_id_bsr_trunc = lcg_id;
          }
        }
      } else {
        //Report SHORT BSR, clear bsr_t
        mac_ce_p->bsr_t = NULL;
      }

      // Trigger BSR Padding
      mac->BSR_reporting_active |= NR_BSR_TRIGGER_PADDING;
    }

    mac_ce_p->bsr_len = mac_ce_p->bsr_header_len + mac_ce_p->bsr_ce_len;
    mac_ce_p->tot_mac_ce_len += mac_ce_p->bsr_len;
    mac_ce_p->total_mac_pdu_header_len += mac_ce_p->bsr_len;
  }

  //Fill BSR Infos
  if (mac_ce_p->bsr_ce_len == 0) {
    mac_ce_p->bsr_s = NULL;
    mac_ce_p->bsr_l = NULL;
    mac_ce_p->bsr_t = NULL;
  } else if (mac_ce_p->bsr_header_len == sizeof(NR_MAC_SUBHEADER_SHORT)) {
    mac_ce_p->bsr_s = NULL;
    mac_ce_p->bsr_t = NULL;
    mac_ce_p->bsr_l->Buffer_size0 = mac->scheduling_info.BSR[0];
    mac_ce_p->bsr_l->Buffer_size1 = mac->scheduling_info.BSR[1];
    mac_ce_p->bsr_l->Buffer_size2 = mac->scheduling_info.BSR[2];
    mac_ce_p->bsr_l->Buffer_size3 = mac->scheduling_info.BSR[3];
    mac_ce_p->bsr_l->Buffer_size4 = mac->scheduling_info.BSR[4];
    mac_ce_p->bsr_l->Buffer_size5 = mac->scheduling_info.BSR[5];
    mac_ce_p->bsr_l->Buffer_size6 = mac->scheduling_info.BSR[6];
    mac_ce_p->bsr_l->Buffer_size7 = mac->scheduling_info.BSR[7];
    LOG_D(NR_MAC, "[UE %d] Frame %d subframe %d BSR Trig=%d report LONG BSR (level LCGID0 %d,level LCGID1 %d,level LCGID2 %d,level LCGID3 %d level LCGID4 %d,level LCGID5 %d,level LCGID6 %d,level LCGID7 %d)\n",
          module_idP, frameP, subframe,
          mac->BSR_reporting_active,
          mac->scheduling_info.BSR[0],
          mac->scheduling_info.BSR[1],
          mac->scheduling_info.BSR[2],
          mac->scheduling_info.BSR[3],
          mac->scheduling_info.BSR[4],
          mac->scheduling_info.BSR[5],
          mac->scheduling_info.BSR[6],
          mac->scheduling_info.BSR[7]);
  } else if (mac_ce_p->bsr_header_len == sizeof(NR_MAC_SUBHEADER_FIXED)) {
    mac_ce_p->bsr_l = NULL;

    if ((mac_ce_p->bsr_t != NULL) && (mac->BSR_reporting_active & NR_BSR_TRIGGER_PADDING)) {
      //Truncated BSR
      mac_ce_p->bsr_s = NULL;
      mac_ce_p->bsr_t->LcgID = lcg_id_bsr_trunc;
      mac_ce_p->bsr_t->Buffer_size = mac->scheduling_info.BSR[lcg_id_bsr_trunc];
      LOG_D(NR_MAC, "[UE %d] Frame %d subframe %d BSR Trig=%d report TRUNCATED BSR with level %d for LCGID %d\n",
            module_idP, frameP, subframe,
            mac->BSR_reporting_active,
            mac->scheduling_info.BSR[lcg_id_bsr_trunc], lcg_id_bsr_trunc);
    } else {
      mac_ce_p->bsr_t = NULL;
      mac_ce_p->bsr_s->LcgID = lcg_id_bsr_trunc;
      mac_ce_p->bsr_s->Buffer_size = mac->scheduling_info.BSR[lcg_id_bsr_trunc];
      LOG_D(NR_MAC, "[UE %d] Frame %d subframe %d BSR Trig=%d report SHORT BSR with level %d for LCGID %d\n",
            module_idP, frameP, subframe,
            mac->BSR_reporting_active,
            mac->scheduling_info.BSR[lcg_id_bsr_trunc], lcg_id_bsr_trunc);
    }
  }

  LOG_D(NR_MAC, "[UE %d][SR] Gave SDU to PHY, clearing any scheduling request\n", module_idP);
  mac->scheduling_info.SR_pending = 0;
  mac->scheduling_info.SR_COUNTER = 0;

  /* Actions when a BSR is sent */
  if (mac_ce_p->bsr_ce_len) {
    LOG_D(NR_MAC, "[UE %d] MAC BSR Sent !! bsr (ce%d,hdr%d) buff_len %d\n",
          module_idP, mac_ce_p->bsr_ce_len, mac_ce_p->bsr_header_len, buflen);
    // Reset ReTx BSR Timer
    mac->scheduling_info.retxBSR_SF = nr_get_sf_retxBSRTimer(mac->scheduling_info.retxBSR_Timer);
    LOG_D(NR_MAC, "[UE %d] MAC ReTx BSR Timer Reset =%d\n", module_idP, mac->scheduling_info.retxBSR_SF);

    // Reset Periodic Timer except when BSR is truncated
    if ((mac_ce_p->bsr_t == NULL) && (mac->scheduling_info.periodicBSR_Timer != NR_BSR_Config__periodicBSR_Timer_infinity)) {
      mac->scheduling_info.periodicBSR_SF = nr_get_sf_periodicBSRTimer(mac->scheduling_info.periodicBSR_Timer);
      LOG_D(NR_MAC, "[UE %d] MAC Periodic BSR Timer Reset =%d\n",
            module_idP,
            mac->scheduling_info.periodicBSR_SF);
    }

    // Reset BSR Trigger flags
    mac->BSR_reporting_active = BSR_TRIGGER_NONE;
  }
}

/**
 * Function:      to fetch data to be transmitted from RLC, place it in the ULSCH PDU buffer
                  to generate the complete MAC PDU with sub-headers and MAC CEs according to ULSCH MAC PDU generation (6.1.2 TS 38.321)
                  the selected sub-header for the payload sub-PDUs is NR_MAC_SUBHEADER_LONG
 * @module_idP    Module ID
 * @CC_id         Component Carrier index
 * @frameP        current UL frame
 * @subframe      current UL slot
 * @gNB_index     gNB index
 * @ulsch_buffer  Pointer to ULSCH PDU
 * @buflen        TBS
 */
uint8_t nr_ue_get_sdu(module_id_t module_idP,
                      int CC_id,
                      frame_t frameP,
                      sub_frame_t subframe,
                      uint8_t gNB_index,
                      uint8_t *ulsch_buffer,
                      uint16_t buflen) {
  NR_UE_MAC_CE_INFO mac_ce_info;
  NR_UE_MAC_CE_INFO *mac_ce_p=&mac_ce_info;
  int16_t buflen_remain = 0;
  mac_ce_p->bsr_len = 0;
  mac_ce_p->bsr_ce_len = 0;
  mac_ce_p->bsr_header_len = 0;
  mac_ce_p->phr_len = 0;
  //mac_ce_p->phr_ce_len = 0;
  //mac_ce_p->phr_header_len = 0;

  uint8_t lcid = 0;
  uint16_t sdu_length = 0;
  uint16_t num_sdus = 0;
  mac_ce_p->sdu_length_total = 0;
  NR_BSR_SHORT bsr_short, bsr_truncated;
  NR_BSR_LONG bsr_long;
  mac_ce_p->bsr_s = &bsr_short;
  mac_ce_p->bsr_l = &bsr_long;
  mac_ce_p->bsr_t = &bsr_truncated;
  //NR_POWER_HEADROOM_CMD phr;
  //mac_ce_p->phr_p = &phr;
  NR_UE_MAC_INST_t *mac = get_mac_inst(module_idP);
  //int highest_priority = 16;
  const uint8_t sh_size = sizeof(NR_MAC_SUBHEADER_LONG);

  // Pointer used to build the MAC PDU by placing the RLC SDUs in the ULSCH buffer
  uint8_t *pdu = ulsch_buffer;

  //nr_ue_get_sdu_mac_ce_pre updates all mac_ce related header field related to length
  mac_ce_p->tot_mac_ce_len = nr_ue_get_sdu_mac_ce_pre(module_idP, CC_id, frameP, subframe, gNB_index, ulsch_buffer, buflen, mac_ce_p);
  mac_ce_p->total_mac_pdu_header_len = mac_ce_p->tot_mac_ce_len;

  LOG_D(NR_MAC, "In %s: [UE %d] [%d.%d] process UL transport block at with size TBS = %d bytes \n", __FUNCTION__, module_idP, frameP, subframe, buflen);

  // Check for DCCH first
  // TO DO: Multiplex in the order defined by the logical channel prioritization
  for (lcid = UL_SCH_LCID_SRB1; lcid < MAX_LCID; lcid++) {

    buflen_remain = buflen - (mac_ce_p->total_mac_pdu_header_len + mac_ce_p->sdu_length_total + sh_size);

    LOG_D(NR_MAC, "In %s: [UE %d] [%d.%d] UL-DXCH -> ULSCH, RLC with LCID 0x%02x (TBS %d bytes, sdu_length_total %d bytes, MAC header len %d bytes, buflen_remain %d bytes)\n",
          __FUNCTION__,
          module_idP,
          frameP,
          subframe,
          lcid,
          buflen,
          mac_ce_p->sdu_length_total,
          mac_ce_p->tot_mac_ce_len,
          buflen_remain);

    while (buflen_remain > 0){

      // Pointer used to build the MAC sub-PDU headers in the ULSCH buffer for each SDU
      NR_MAC_SUBHEADER_LONG *header = (NR_MAC_SUBHEADER_LONG *) pdu;

      pdu += sh_size;

      sdu_length = mac_rlc_data_req(module_idP,
                                    mac->crnti,
                                    gNB_index,
                                    frameP,
                                    ENB_FLAG_NO,
                                    MBMS_FLAG_NO,
                                    lcid,
                                    buflen_remain,
                                    (char *)pdu,
                                    0,
                                    0);

      AssertFatal(buflen_remain >= sdu_length, "In %s: LCID = 0x%02x RLC has segmented %d bytes but MAC has max %d remaining bytes\n",
                  __FUNCTION__,
                  lcid,
                  sdu_length,
                  buflen_remain);

      if (sdu_length > 0) {

        LOG_D(NR_MAC, "In %s: [UE %d] [%d.%d] UL-DXCH -> ULSCH, Generating UL MAC sub-PDU for SDU %d, length %d bytes, RB with LCID 0x%02x (buflen (TBS) %d bytes)\n",
          __FUNCTION__,
          module_idP,
          frameP,
          subframe,
          num_sdus + 1,
          sdu_length,
          lcid,
          buflen);

        header->R = 0;
        header->F = 1;
        header->LCID = lcid;
        header->L = htons(sdu_length);

        #ifdef ENABLE_MAC_PAYLOAD_DEBUG
        LOG_I(NR_MAC, "In %s: dumping MAC sub-header with length %d: \n", __FUNCTION__, sh_size);
        log_dump(NR_MAC, header, sh_size, LOG_DUMP_CHAR, "\n");
        LOG_I(NR_MAC, "In %s: dumping MAC SDU with length %d \n", __FUNCTION__, sdu_length);
        log_dump(NR_MAC, pdu, sdu_length, LOG_DUMP_CHAR, "\n");
        #endif

        pdu += sdu_length;
        mac_ce_p->sdu_length_total += sdu_length;
        mac_ce_p->total_mac_pdu_header_len += sh_size;

        num_sdus++;

      } else {
        pdu -= sh_size;
        LOG_D(NR_MAC, "In %s: no data to transmit for RB with LCID 0x%02x\n", __FUNCTION__, lcid);
        break;
      }

      buflen_remain = buflen - (mac_ce_p->total_mac_pdu_header_len + mac_ce_p->sdu_length_total + sh_size);

      //Update Buffer remain and BSR bytes after transmission
      mac->scheduling_info.LCID_buffer_remain[lcid] -= sdu_length;
      mac->scheduling_info.BSR_bytes[mac->scheduling_info.LCGID[lcid]] -= sdu_length;
      LOG_D(NR_MAC, "[UE %d] Update BSR [%d.%d] BSR_bytes for LCG%d=%d\n",
            module_idP, frameP, subframe, mac->scheduling_info.LCGID[lcid],
            mac->scheduling_info.BSR_bytes[mac->scheduling_info.LCGID[lcid]]);
      if (mac->scheduling_info.BSR_bytes[mac->scheduling_info.LCGID[lcid]] < 0)
        mac->scheduling_info.BSR_bytes[mac->scheduling_info.LCGID[lcid]] = 0;
    }
  }

  //nr_ue_get_sdu_mac_ce_post recalculates all mac_ce related header fields since buffer has been changed after mac_rlc_data_req.
  //Also, BSR padding is handled here after knowing mac_ce_p->sdu_length_total.
  nr_ue_get_sdu_mac_ce_post(module_idP, CC_id, frameP, subframe, gNB_index, ulsch_buffer, buflen, mac_ce_p);

  if (mac_ce_p->tot_mac_ce_len > 0) {

    LOG_D(NR_MAC, "In %s copying %d bytes of MAC CEs to the UL PDU \n", __FUNCTION__, mac_ce_p->tot_mac_ce_len);
    nr_write_ce_ulsch_pdu(pdu, mac, 0, NULL, mac_ce_p->bsr_t, mac_ce_p->bsr_s, mac_ce_p->bsr_l);
    pdu += (unsigned char) mac_ce_p->tot_mac_ce_len;

    #ifdef ENABLE_MAC_PAYLOAD_DEBUG
    LOG_I(NR_MAC, "In %s: dumping MAC CE with length tot_mac_ce_len %d: \n", __FUNCTION__, mac_ce_p->tot_mac_ce_len);
    log_dump(NR_MAC, mac_header_control_elements, mac_ce_p->tot_mac_ce_len, LOG_DUMP_CHAR, "\n");
    #endif

  }

  buflen_remain = buflen - (mac_ce_p->total_mac_pdu_header_len + mac_ce_p->sdu_length_total);

  // Compute final offset for padding and fill remainder of ULSCH with 0
  if (buflen_remain > 0) {

    LOG_D(NR_MAC, "In %s filling remainder %d bytes to the UL PDU \n", __FUNCTION__, buflen_remain);
    ((NR_MAC_SUBHEADER_FIXED *) pdu)->R = 0;
    ((NR_MAC_SUBHEADER_FIXED *) pdu)->LCID = UL_SCH_LCID_PADDING;

    #ifdef ENABLE_MAC_PAYLOAD_DEBUG
    LOG_I(NR_MAC, "In %s: padding MAC sub-header with length %ld bytes \n", __FUNCTION__, sizeof(NR_MAC_SUBHEADER_FIXED));
    log_dump(NR_MAC, pdu, sizeof(NR_MAC_SUBHEADER_FIXED), LOG_DUMP_CHAR, "\n");
    #endif

    pdu++;
    buflen_remain--;

    if (IS_SOFTMODEM_RFSIM) {
      for (int j = 0; j < buflen_remain; j++) {
        pdu[j] = (unsigned char) rand();
      }
    } else {
      memset(pdu, 0, buflen_remain);
    }

    #ifdef ENABLE_MAC_PAYLOAD_DEBUG
    LOG_I(NR_MAC, "In %s: MAC padding sub-PDU with length %d bytes \n", __FUNCTION__, buflen_remain);
    log_dump(NR_MAC, pdu, buflen_remain, LOG_DUMP_CHAR, "\n");
    #endif

  }

  #ifdef ENABLE_MAC_PAYLOAD_DEBUG
  LOG_I(NR_MAC, "In %s: dumping MAC PDU with length %d: \n", __FUNCTION__, buflen);
  log_dump(NR_MAC, ulsch_buffer, buflen, LOG_DUMP_CHAR, "\n");
  #endif

  return num_sdus > 0 ? 1 : 0;
}

void schedule_ta_command(fapi_nr_dl_config_request_t *dl_config, NR_UL_TIME_ALIGNMENT_t *ul_time_alignment)
{
  fapi_nr_ta_command_pdu *ta = &dl_config->dl_config_list[dl_config->number_pdus].ta_command_pdu;
  ta->ta_frame = ul_time_alignment->frame;
  ta->ta_slot = ul_time_alignment->slot;
  ta->ta_command = ul_time_alignment->ta_command;
  dl_config->dl_config_list[dl_config->number_pdus].pdu_type = FAPI_NR_CONFIG_TA_COMMAND;
  dl_config->number_pdus += 1;
  ul_time_alignment->ta_apply = false;
}
