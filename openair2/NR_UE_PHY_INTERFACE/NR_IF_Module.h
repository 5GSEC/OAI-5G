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

/* \file NR_IF_Module.h
 * \brief data structures for L1/L2 interface modules
 * \author R. Knopp, K.H. HSU
 * \date 2018
 * \version 0.1
 * \company Eurecom / NTUST
 * \email: knopp@eurecom.fr, kai-hsiang.hsu@eurecom.fr
 * \note
 * \warning
 */

#ifndef __NR_IF_MODULE_H__
#define __NR_IF_MODULE_H__

#include "platform_types.h"
#include <semaphore.h>
#include "fapi_nr_ue_interface.h"
#include "openair2/PHY_INTERFACE/queue_t.h"
#include "nfapi_nr_interface_scf.h"
#include "openair2/NR_PHY_INTERFACE/NR_IF_Module.h"
#include "NR_Packet_Drop.h"
#include "nfapi/open-nFAPI/nfapi/public_inc/sidelink_nr_ue_interface.h"

extern slot_rnti_mcs_s slot_rnti_mcs[NUM_NFAPI_SLOT];

typedef struct NR_UL_TIME_ALIGNMENT NR_UL_TIME_ALIGNMENT_t;

typedef struct {
    /// module id
  module_id_t module_id;
  /// gNB index
  uint32_t gNB_index;
  /// component carrier id
  int cc_id;
  /// frame 
  frame_t frame;
  /// slot
  int slot;

  fapi_nr_dl_config_request_t dl_config_req;
  fapi_nr_ul_config_request_t ul_config_req;
} nr_dcireq_t;

typedef struct {
    /// module id
    module_id_t module_id;
    /// gNB index
    uint32_t gNB_index;
    /// component carrier id
    int cc_id;
    /// frame 
    frame_t frame;
    /// slot
    int slot;

    /// NR UE FAPI-like P7 message, direction: L1 to L2
    /// data reception indication structure
    fapi_nr_rx_indication_t *rx_ind;
    /// ssb_index, if ssb is not present in current TTI, value set to -1
    int ssb_index;
    /// dci reception indication structure
    fapi_nr_dci_indication_t *dci_ind;

    /// PHY specific data structure that can be passed on to L2 via nr_downlink_indication_t and
    /// back to L1 via the nr_scheduled_response_t 
    void *phy_data;
} nr_downlink_indication_t;


typedef struct {
    /// module id
    module_id_t module_id;
    /// gNB index
    uint32_t gNB_index;
    /// component carrier id
    int cc_id;
    /// frame 
    frame_t frame_rx;
    /// slot rx
    uint32_t slot_rx;
    /// frame tx
    frame_t frame_tx;
    /// slot tx
    uint32_t slot_tx;

    /// dci reception indication structure
    fapi_nr_dci_indication_t *dci_ind;

    void *phy_data;

} nr_uplink_indication_t;

typedef struct {
    /// module id
    module_id_t module_id;
    /// gNB index
    uint32_t gNB_index;
    /// component carrier id
    int cc_id;
    /// frame
    frame_t frame_rx;
    /// slot rx
    uint32_t slot_rx;
    /// frame tx
    frame_t frame_tx;
    /// slot tx
    uint32_t slot_tx;

    /// NR UE FAPI-like P7 message, direction: L1 to L2
    /// data reception indication structure
    sl_nr_rx_indication_t *rx_ind;

    //SCI received by PHY sent to MAC here
    sl_nr_sci_indication_t *sci_ind;

    /// PHY specific data structure that can be passed on to L2 via nr_sidelink_indication_t and
    /// back to L1 via the nr_scheduled_response_t
    void *phy_data;
} nr_sidelink_indication_t;


// Downlink subframe P7


typedef struct {
    /// module id
    module_id_t module_id; 
    /// component carrier id
    int CC_id;
    /// frame
    frame_t frame;
    /// slot
    int slot;

    /// NR UE FAPI-like P7 message, direction: L2 to L1
    /// downlink transmission configuration request structure
    fapi_nr_dl_config_request_t *dl_config;

    /// uplink transmission configuration request structure
    fapi_nr_ul_config_request_t *ul_config;

    // Sidelink RX configuration request
    sl_nr_rx_config_request_t *sl_rx_config;

    // Sidelink TX configuration request
    sl_nr_tx_config_request_t *sl_tx_config;

    /// data transmission request structure
    fapi_nr_tx_request_t *tx_request;

    /// PHY data structure initially passed on to L2 via the nr_downlink_indication_t and
    /// returned to L1 via nr_scheduled_response_t
    void *phy_data;
} nr_scheduled_response_t;

typedef struct {
    /// module id
    uint8_t Mod_id;
    /// component carrier id
    uint8_t CC_id;
    
    /// NR UE FAPI-like P5 message
    /// physical layer configuration request structure
    fapi_nr_config_request_t config_req;

} nr_phy_config_t;

typedef struct {
    /// module id
    uint8_t Mod_id;
    /// component carrier id
    uint8_t CC_id;

    /// NR UE FAPI-like P5 message
    /// physical layer configuration request structure
    sl_nr_phy_config_request_t sl_config_req;

} nr_sl_phy_config_t;

typedef struct {
    /// module id
    uint8_t Mod_id;
    /// component carrier id
    uint8_t CC_id;
    /// Flag signaling that synch_request was received
    uint8_t received_synch_request;
    /// NR UE FAPI message
    fapi_nr_synch_request_t synch_req;
} nr_synch_request_t;

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int8_t (nr_ue_scheduled_response_f)(nr_scheduled_response_t *scheduled_response);

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int8_t (nr_sl_ue_scheduled_response_f)(nr_scheduled_response_t *sl_scheduled_response);


/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int8_t (nr_ue_phy_config_request_f)(nr_phy_config_t *phy_config);

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int8_t (nr_sl_ue_phy_config_request_f)(nr_sl_phy_config_t *sl_phy_config);

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 */
typedef void (nr_ue_synch_request_f)(nr_synch_request_t *synch_request);


/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int (nr_ue_dl_indication_f)(nr_downlink_indication_t *dl_info);

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int (nr_ue_ul_indication_f)(nr_uplink_indication_t *ul_info);


/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int (nr_ue_sl_indication_f)(nr_sidelink_indication_t *sl_info);

//  TODO check this stuff can be reuse of need modification
typedef struct nr_ue_if_module_s {
  nr_ue_scheduled_response_f *scheduled_response;
  nr_ue_phy_config_request_f *phy_config_request;
  nr_ue_synch_request_f      *synch_request;
  nr_ue_dl_indication_f      *dl_indication;
  nr_ue_ul_indication_f      *ul_indication;
  nr_ue_sl_indication_f      *sl_indication;
  uint32_t cc_mask;
  uint32_t current_frame;
  uint32_t current_slot;
  //pthread_mutex_t nr_if_mutex;
} nr_ue_if_module_t;


/**\brief reserved one of the interface(if) module instantce from pointer pool and done memory allocation by module_id.
   \param module_id module id*/
nr_ue_if_module_t *nr_ue_if_module_init(uint32_t module_id);

void nrue_init_standalone_socket(int tx_port, int rx_port);

void *nrue_standalone_pnf_task(void *context);
extern sem_t sfn_slot_semaphore;

typedef struct nfapi_dl_tti_config_req_tx_data_req_t
{
    nfapi_nr_dl_tti_request_pdu_t *dl_itti_config_req;
    nfapi_nr_tx_data_request_t *tx_data_req_pdu_list;
} nfapi_dl_tti_config_req_tx_data_req_t;

void send_nsa_standalone_msg(NR_UL_IND_t *UL_INFO, uint16_t msg_id);

void save_nr_measurement_info(nfapi_nr_dl_tti_request_t *dl_tti_request);

void check_and_process_dci(nfapi_nr_dl_tti_request_t *dl_tti_request,
                           nfapi_nr_tx_data_request_t *tx_data_request,
                           nfapi_nr_ul_dci_request_t *ul_dci_request,
                           nfapi_nr_ul_tti_request_t *ul_tti_request);

bool sfn_slot_matcher(void *wanted, void *candidate);

/**\brief done free of memory allocation by module_id and release to pointer pool.
   \param module_id module id*/
int nr_ue_if_module_kill(uint32_t module_id);


/**\brief interface between L1/L2, indicating the downlink related information, like dci_ind and rx_req
   \param dl_info including dci_ind and rx_request messages*/
int nr_ue_dl_indication(nr_downlink_indication_t *dl_info);

int nr_ue_ul_indication(nr_uplink_indication_t *ul_info);

int nr_ue_dcireq(nr_dcireq_t *dcireq);

//  TODO check
/**\brief handle BCCH-BCH message from dl_indication
   \param phy_data        PHY structure to be filled in by the callee in the FAPI call (L1 caller -> indication to L2 -> FAPI call to L1 callee)
   \param pduP            pointer to bch pdu
   \param additional_bits corresponding to 38.212 ch.7
   \param ssb_index       SSB index within 0 - (L_ssb-1) corresponding to 38.331 ch.13 parameter i
   \param ssb_length      corresponding to L1 parameter L_ssb 
   \param cell_id         cell id */
int handle_bcch_bch(module_id_t module_id,
                    int cc_id,
                    unsigned int gNB_index,
                    void *phy_data,
                    uint8_t *pduP,
                    unsigned int additional_bits,
                    uint32_t ssb_index,
                    uint32_t ssb_length,
                    uint16_t ssb_start_subcarrier,
                    uint16_t cell_id);

//  TODO check
/**\brief handle BCCH-DL-SCH message from dl_indication
   \param pdu_len   length(bytes) of pdu
   \param pduP      pointer to pdu*/
int handle_bcch_dlsch(module_id_t module_id, int cc_id, unsigned int gNB_index, uint8_t ack_nack, uint8_t *pduP, uint32_t pdu_len);

int handle_dci(module_id_t module_id, int cc_id, unsigned int gNB_index, frame_t frame, int slot, fapi_nr_dci_indication_pdu_t *dci);


#endif

