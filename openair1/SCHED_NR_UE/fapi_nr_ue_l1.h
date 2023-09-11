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

/* \file fapi_nr_ue_l1.c
 * \brief functions for FAPI L1 interface
 * \author R. Knopp, K.H. HSU
 * \date 2018
 * \version 0.1
 * \company Eurecom / NTUST
 * \email: knopp@eurecom.fr, kai-hsiang.hsu@eurecom.fr
 * \note
 * \warning
 */

#ifndef __FAPI_NR_UE_L1_H__
#define __FAPI_NR_UE_L1_H__

#include "NR_IF_Module.h"
#include "openair2/NR_UE_PHY_INTERFACE/NR_IF_Module.h"
#include "openair2/LAYER2/NR_MAC_UE/mac_proto.h"

/**\brief NR UE FAPI-like P7 messages, scheduled response from L2 indicating L1
   \param scheduled_response including transmission config(dl_config, ul_config) and data transmission (tx_req)*/
int8_t nr_ue_scheduled_response(nr_scheduled_response_t *scheduled_response);

int8_t nr_ue_scheduled_response_stub(nr_scheduled_response_t *scheduled_response);

/**\brief NR UE FAPI-like P5 message, physical configuration from L2 to configure L1
   \param scheduled_response including transmission config(dl_config, ul_config) and data transmission (tx_req)*/
int8_t nr_ue_phy_config_request(nr_phy_config_t *phy_config);

/**\brief NR UE FAPI message to schedule a synchronization with target gNB
   \param synch_request including target_Nid_cell*/
void nr_ue_synch_request(nr_synch_request_t *synch_request);

void update_harq_status(module_id_t module_id, uint8_t harq_pid, uint8_t ack_nack);

#endif
