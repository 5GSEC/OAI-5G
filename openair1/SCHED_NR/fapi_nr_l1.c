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

/*! \file fapi_nr_l1.c
 * \brief functions for FAPI L1 interface
 * \author R. Knopp, WEI-TAI CHEN
 * \date 2017, 2018
 * \version 0.1
 * \company Eurecom, NTUST
 * \email: knopp@eurecom.fr, kroempa@gmail.com
 * \note
 * \warning
 */
#include "fapi_nr_l1.h"
#include "common/ran_context.h"
#include "PHY/NR_TRANSPORT/nr_transport_proto.h"
#include "PHY/NR_TRANSPORT/nr_dlsch.h"
#include "PHY/NR_TRANSPORT/nr_dci.h"
#include "nfapi/oai_integration/vendor_ext.h"
#include "openair2/NR_PHY_INTERFACE/nr_sched_response.h"


extern int oai_nfapi_dl_tti_req(nfapi_nr_dl_tti_request_t *dl_config_req);
extern int oai_nfapi_tx_data_req(nfapi_nr_tx_data_request_t *tx_data_req);
extern int oai_nfapi_ul_dci_req(nfapi_nr_ul_dci_request_t *ul_dci_req);
extern int oai_nfapi_ul_tti_req(nfapi_nr_ul_tti_request_t *ul_tti_req);


extern uint8_t nfapi_mode;

void handle_nr_nfapi_ssb_pdu(processingData_L1tx_t *msgTx,int frame,int slot,
                             nfapi_nr_dl_tti_request_pdu_t *dl_tti_pdu)
{

  AssertFatal(dl_tti_pdu->ssb_pdu.ssb_pdu_rel15.bchPayloadFlag== 1, "bchPayloadFlat %d != 1\n",
              dl_tti_pdu->ssb_pdu.ssb_pdu_rel15.bchPayloadFlag);

  uint8_t i_ssb = dl_tti_pdu->ssb_pdu.ssb_pdu_rel15.SsbBlockIndex;

  LOG_D(PHY,"%d.%d : ssb index %d pbch_pdu: %x\n",frame,slot,i_ssb,dl_tti_pdu->ssb_pdu.ssb_pdu_rel15.bchPayload);
  if (msgTx->ssb[i_ssb].active)
    AssertFatal(1==0,"SSB PDU with index %d already active\n",i_ssb);
  else {
    msgTx->ssb[i_ssb].active = true;
    memcpy((void*)&msgTx->ssb[i_ssb].ssb_pdu,&dl_tti_pdu->ssb_pdu,sizeof(dl_tti_pdu->ssb_pdu));
  }
}

/*void handle_nr_nfapi_pdsch_pdu(PHY_VARS_gNB *gNB,int frame,int subframe,gNB_L1_rxtx_proc_t *proc,
                            uint8_t codeword_index,
                            uint8_t *sdu)
{

	int UE_id = 0; //Hardcode UE_id for now
	int harq_pid;

	NR_gNB_DLSCH_t *dlsch0=NULL, *dlsch1=NULL;
	NR_DL_gNB_HARQ_t *dlsch0_harq=NULL,*dlsch1_harq=NULL;

    // Based on nr_fill_dci_and_dlsch only gNB->dlsch[0][0] gets filled now. So maybe we do not need dlsch1.
	dlsch0 = gNB->dlsch[UE_id][0];
	dlsch1 = gNB->dlsch[UE_id][1];

	harq_pid        = dlsch0->harq_ids[subframe];
	dlsch0_harq     = dlsch0->harq_processes[harq_pid];
	dlsch1_harq     = dlsch1->harq_processes[harq_pid];


	//if (dlsch0_harq->round==0) {  //get pointer to SDU if this a new SDU
    if(sdu == NULL) {
      LOG_E(PHY,"NFAPI: SFN/SF:%04d%d proc:TX:[frame %d subframe %d]: programming dlsch for round 0 \n",
            frame,subframe,
            proc->frame_tx,proc->slot_tx);
      return;
    }
    //AssertFatal(sdu!=NULL,"NFAPI: SFN/SF:%04d%d proc:TX:[frame %d subframe %d]: programming dlsch for round 0, rnti %x, UE_id %d, harq_pid %d : sdu is null for pdu_index %d dlsch0_harq[round:%d SFN/SF:%d%d pdu:%p mcs:%d ndi:%d pdschstart:%d]\n",
    //            frame,subframe,
    //            proc->frame_tx,proc->subframe_tx,rel8->rnti,UE_id,harq_pid,
    //            dl_tti_pdu->pdsch_pdu.pdsch_pdu_rel8.pdu_index,dlsch0_harq->round,dlsch0_harq->frame,dlsch0_harq->subframe,dlsch0_harq->pdu,dlsch0_harq->mcs,dlsch0_harq->ndi,dlsch0_harq->pdsch_start);
    if (codeword_index == 0) dlsch0_harq->pdu                    = sdu;
    else                     dlsch1_harq->pdu                    = sdu;
    LOG_I(PHY, "SFN/SF: %d/%d DLSCH PDU filled \n",frame, subframe);
//  }

}*/

void handle_nfapi_nr_csirs_pdu(processingData_L1tx_t *msgTx, int frame, int slot, nfapi_nr_dl_tti_csi_rs_pdu *csirs_pdu)
{
  int found = 0;

  for (int id = 0; id < NR_SYMBOLS_PER_SLOT; id++) {
    NR_gNB_CSIRS_t *csirs = &msgTx->csirs_pdu[id];
    if (csirs->active == 0) {
      LOG_D(PHY,"Frame %d Slot %d CSI_RS with ID %d is now active\n",frame,slot,id);
      csirs->active = 1;
      memcpy((void*)&csirs->csirs_pdu, (void*)csirs_pdu, sizeof(nfapi_nr_dl_tti_csi_rs_pdu));
      found = 1;
      break;
    }
  }
  if (found == 0)
    LOG_E(MAC,"CSI-RS list is full\n");
}

void handle_nr_nfapi_pdsch_pdu(processingData_L1tx_t *msgTx,
                            nfapi_nr_dl_tti_pdsch_pdu *pdsch_pdu,
                            uint8_t *sdu)
{

  nr_fill_dlsch(msgTx,pdsch_pdu,sdu);

}

void nr_schedule_response(NR_Sched_Rsp_t *Sched_INFO)
{
  // copy data from L2 interface into L1 structures
  module_id_t                   Mod_id       = Sched_INFO->module_id;
  nfapi_nr_dl_tti_request_t     *DL_req      = &Sched_INFO->DL_req;
  nfapi_nr_tx_data_request_t    *TX_req      = &Sched_INFO->TX_req;
  nfapi_nr_ul_tti_request_t     *UL_tti_req  = &Sched_INFO->UL_tti_req;
  nfapi_nr_ul_dci_request_t     *UL_dci_req  = &Sched_INFO->UL_dci_req;
  frame_t                       frame        = Sched_INFO->frame;
  sub_frame_t                   slot         = Sched_INFO->slot;

  AssertFatal(RC.gNB!=NULL,"RC.gNB is null\n");
  AssertFatal(RC.gNB[Mod_id]!=NULL,"RC.gNB[%d] is null\n",Mod_id);

  PHY_VARS_gNB *gNB = RC.gNB[Mod_id];
  start_meas(&gNB->schedule_response_stats);

  nfapi_nr_config_request_scf_t *cfg = &gNB->gNB_config;

  int slot_type = nr_slot_select(cfg,frame,slot);

  uint8_t number_dl_pdu             = (DL_req==NULL) ? 0 : DL_req->dl_tti_request_body.nPDUs;
  uint8_t number_ul_dci_pdu         = (UL_dci_req==NULL) ? 0 : UL_dci_req->numPdus;
  uint8_t number_ul_tti_pdu         = (UL_tti_req==NULL) ? 0 : UL_tti_req->n_pdus;
  uint8_t number_tx_data_pdu        = (TX_req == NULL) ? 0 : TX_req->Number_of_PDUs;

  if (NFAPI_MODE == NFAPI_MONOLITHIC){

    if (slot_type == NR_DOWNLINK_SLOT || slot_type == NR_MIXED_SLOT) {
      notifiedFIFO_elt_t *res=NULL;
      processingData_L1tx_t *msgTx=NULL;
      if (!gNB->reorder_thread_disable) {
	res = pullTpool(&gNB->L1_tx_free, &gNB->threadPool);
        if (res == NULL)
          return; // Tpool has been stopped, nothing to process
        msgTx = (processingData_L1tx_t *)NotifiedFifoData(res);
      } else {
        msgTx = gNB->msgDataTx; //newNotifiedFIFO_elt(sizeof(processingData_L1tx_t),0, &gNB->L1_tx_out,NULL);
      }
      /*const time_stats_t ts = exec_time_stats_NotifiedFIFO(res);
      merge_meas(&gNB->phy_proc_tx, &ts);
*/
      msgTx->num_pdsch_slot = 0;
      msgTx->num_dl_pdcch = 0;
      msgTx->num_ul_pdcch = number_ul_dci_pdu;
      msgTx->slot = slot;
      msgTx->frame = frame;
      /* store the sched_response_id for the TX thread to release it when done */
      msgTx->sched_response_id = Sched_INFO->sched_response_id;

      for (int i=0;i<number_dl_pdu;i++) {
        nfapi_nr_dl_tti_request_pdu_t *dl_tti_pdu = &DL_req->dl_tti_request_body.dl_tti_pdu_list[i];
        LOG_D(PHY,"NFAPI: dl_pdu %d : type %d\n",i,dl_tti_pdu->PDUType);
        switch (dl_tti_pdu->PDUType) {
          case NFAPI_NR_DL_TTI_SSB_PDU_TYPE:
            handle_nr_nfapi_ssb_pdu(msgTx,frame,slot,
                                    dl_tti_pdu);
            break;

          case NFAPI_NR_DL_TTI_PDCCH_PDU_TYPE:
            LOG_D(PHY,"frame %d, slot %d, Got NFAPI_NR_DL_TTI_PDCCH_PDU_TYPE for %d.%d\n",frame,slot,DL_req->SFN,DL_req->Slot);
            msgTx->pdcch_pdu[msgTx->num_dl_pdcch] = dl_tti_pdu->pdcch_pdu;
            msgTx->num_dl_pdcch++;
            break;

          case NFAPI_NR_DL_TTI_CSI_RS_PDU_TYPE:
            LOG_D(PHY,"frame %d, slot %d, Got NFAPI_NR_DL_TTI_CSI_RS_PDU_TYPE for %d.%d\n",frame,slot,DL_req->SFN,DL_req->Slot);
            handle_nfapi_nr_csirs_pdu(msgTx,frame,slot,&dl_tti_pdu->csi_rs_pdu);
            break;

          case NFAPI_NR_DL_TTI_PDSCH_PDU_TYPE:
            LOG_D(PHY,"frame %d, slot %d, Got NFAPI_NR_DL_TTI_PDSCH_PDU_TYPE for %d.%d\n",frame,slot,DL_req->SFN,DL_req->Slot);
            nfapi_nr_dl_tti_pdsch_pdu_rel15_t *pdsch_pdu_rel15 = &dl_tti_pdu->pdsch_pdu.pdsch_pdu_rel15;
            uint16_t pduIndex = pdsch_pdu_rel15->pduIndex;
            AssertFatal(TX_req->pdu_list[pduIndex].num_TLV == 1, "TX_req->pdu_list[%d].num_TLV %d != 1\n",
            pduIndex,TX_req->pdu_list[pduIndex].num_TLV);
            uint8_t *sdu = (uint8_t *)TX_req->pdu_list[pduIndex].TLVs[0].value.direct;
            AssertFatal(msgTx->num_pdsch_slot < gNB->max_nb_pdsch,
                        "Number of PDSCH PDUs %d exceeded the limit %d\n",
                        msgTx->num_pdsch_slot,
                        gNB->max_nb_pdsch);
            handle_nr_nfapi_pdsch_pdu(msgTx,&dl_tti_pdu->pdsch_pdu, sdu);
        }
      }

      for (int i=0; i<number_ul_dci_pdu; i++)
        msgTx->ul_pdcch_pdu[i] = UL_dci_req->ul_dci_pdu_list[i];

      /* Both the current thread and the TX thread will access the sched_info
       * at the same time, so increase its reference counter, so that it is
       * released only when both threads are done with it.
       */
      inc_ref_sched_response(Sched_INFO->sched_response_id);
      if (!gNB->reorder_thread_disable)
	pushNotifiedFIFO(&gNB->L1_tx_filled,res);
    }

    for (int i = 0; i < number_ul_tti_pdu; i++) {
      switch (UL_tti_req->pdus_list[i].pdu_type) {
        case NFAPI_NR_UL_CONFIG_PUSCH_PDU_TYPE:
          LOG_D(PHY,"frame %d, slot %d, Got NFAPI_NR_UL_TTI_PUSCH_PDU_TYPE for %d.%d\n", frame, slot, UL_tti_req->SFN, UL_tti_req->Slot);
          nr_fill_ulsch(gNB,UL_tti_req->SFN, UL_tti_req->Slot, &UL_tti_req->pdus_list[i].pusch_pdu);
          break;
        case NFAPI_NR_UL_CONFIG_PUCCH_PDU_TYPE:
          LOG_D(PHY,"frame %d, slot %d, Got NFAPI_NR_UL_TTI_PUCCH_PDU_TYPE for %d.%d\n", frame, slot, UL_tti_req->SFN, UL_tti_req->Slot);
          nr_fill_pucch(gNB,UL_tti_req->SFN, UL_tti_req->Slot, &UL_tti_req->pdus_list[i].pucch_pdu);
          break;
        case NFAPI_NR_UL_CONFIG_PRACH_PDU_TYPE:
          LOG_D(PHY,"frame %d, slot %d, Got NFAPI_NR_UL_TTI_PRACH_PDU_TYPE for %d.%d\n", frame, slot, UL_tti_req->SFN, UL_tti_req->Slot);
          nfapi_nr_prach_pdu_t *prach_pdu = &UL_tti_req->pdus_list[i].prach_pdu;
          nr_fill_prach(gNB, UL_tti_req->SFN, UL_tti_req->Slot, prach_pdu);
          if (gNB->RU_list[0]->if_south == LOCAL_RF || 
              gNB->RU_list[0]->if_south == REMOTE_IF5) nr_fill_prach_ru(gNB->RU_list[0], UL_tti_req->SFN, UL_tti_req->Slot, prach_pdu);
          break;
        case NFAPI_NR_UL_CONFIG_SRS_PDU_TYPE:
          LOG_D(PHY,"frame %d, slot %d, Got NFAPI_NR_UL_CONFIG_SRS_PDU_TYPE for %d.%d\n", frame, slot, UL_tti_req->SFN, UL_tti_req->Slot);
          nr_fill_srs(gNB,UL_tti_req->SFN, UL_tti_req->Slot, &UL_tti_req->pdus_list[i].srs_pdu);
          break;
      }
    }
  }

  if (NFAPI_MODE == NFAPI_MODE_VNF) { //If VNF, oai_nfapi functions send respective p7 msgs to PNF for which nPDUs is greater than 0

    if(number_ul_tti_pdu>0)
      oai_nfapi_ul_tti_req(UL_tti_req);

    if (number_ul_dci_pdu>0)
      oai_nfapi_ul_dci_req(UL_dci_req);

    if (number_tx_data_pdu>0)
      oai_nfapi_tx_data_req(TX_req);

    if (number_dl_pdu>0)
      oai_nfapi_dl_tti_req(DL_req);
  }

  /* this thread is done with the sched_info, decrease the reference counter */
  deref_sched_response(Sched_INFO->sched_response_id);

  stop_meas(&gNB->schedule_response_stats);
}
