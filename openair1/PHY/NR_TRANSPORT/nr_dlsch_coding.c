/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.0  (the "License"); you may not use this file
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

/*! \file PHY/LTE_TRANSPORT/dlsch_coding.c
* \brief Top-level routines for implementing LDPC-coded (DLSCH) transport channels from 38-212, 15.2
* \author H.Wang
* \date 2018
* \version 0.1
* \company Eurecom
* \email:
* \note
* \warning
*/

#include "PHY/defs_gNB.h"
#include "PHY/CODING/coding_extern.h"
#include "PHY/CODING/coding_defs.h"
#include "PHY/CODING/lte_interleaver_inline.h"
#include "PHY/CODING/nrLDPC_extern.h"
#include "PHY/NR_TRANSPORT/nr_transport_proto.h"
#include "PHY/NR_TRANSPORT/nr_transport_common_proto.h"
#include "PHY/NR_TRANSPORT/nr_dlsch.h"
#include "SCHED_NR/sched_nr.h"
#include "common/utils/LOG/vcd_signal_dumper.h"
#include "common/utils/LOG/log.h"
#include "common/utils/nr/nr_common.h"
#include <syscall.h>
#include <openair2/UTIL/OPT/opt.h>

//#define DEBUG_DLSCH_CODING
//#define DEBUG_DLSCH_FREE 1

void free_gNB_dlsch(NR_gNB_DLSCH_t *dlsch, uint16_t N_RB, const NR_DL_FRAME_PARMS *frame_parms)
{
  int max_layers = (frame_parms->nb_antennas_tx<NR_MAX_NB_LAYERS) ? frame_parms->nb_antennas_tx : NR_MAX_NB_LAYERS;
  uint16_t a_segments = MAX_NUM_NR_DLSCH_SEGMENTS_PER_LAYER*max_layers;

  if (N_RB != 273) {
    a_segments = a_segments*N_RB;
    a_segments = a_segments/273 +1;
  }

  NR_DL_gNB_HARQ_t *harq = &dlsch->harq_process;
  if (harq->b) {
    free16(harq->b, a_segments * 1056);
    harq->b = NULL;
  }
  if (harq->f) {
    free16(harq->f, N_RB * NR_SYMBOLS_PER_SLOT * NR_NB_SC_PER_RB * 8 * NR_MAX_NB_LAYERS);
    harq->f = NULL;
  }
  for (int r = 0; r < a_segments; r++) {
    free(harq->c[r]);
    harq->c[r] = NULL;
  }
  free(harq->c);
  free(harq->pdu);

  int nb_codewords = NR_MAX_NB_LAYERS > 4 ? 2 : 1;
  for (int q=0; q<nb_codewords; q++)
    free(dlsch->mod_symbs[q]);
  free(dlsch->mod_symbs);

  for (int layer = 0; layer < max_layers; layer++) {
    free(dlsch->txdataF[layer]);
    for (int aa = 0; aa < 64; aa++)
      free(dlsch->ue_spec_bf_weights[layer][aa]);
    free(dlsch->ue_spec_bf_weights[layer]);
  }
  free(dlsch->txdataF);
  free(dlsch->ue_spec_bf_weights);
}

NR_gNB_DLSCH_t new_gNB_dlsch(NR_DL_FRAME_PARMS *frame_parms, uint16_t N_RB)
{
  int max_layers = (frame_parms->nb_antennas_tx<NR_MAX_NB_LAYERS) ? frame_parms->nb_antennas_tx : NR_MAX_NB_LAYERS;
  uint16_t a_segments = MAX_NUM_NR_DLSCH_SEGMENTS_PER_LAYER*max_layers;  //number of segments to be allocated

  if (N_RB != 273) {
    a_segments = a_segments*N_RB;
    a_segments = a_segments/273 +1;
  }

  LOG_D(PHY,"Allocating %d segments (MAX %d, N_PRB %d)\n",a_segments,MAX_NUM_NR_DLSCH_SEGMENTS_PER_LAYER,N_RB);
  uint32_t dlsch_bytes = a_segments*1056;  // allocated bytes per segment
  NR_gNB_DLSCH_t dlsch;

  int txdataf_size = frame_parms->N_RB_DL*NR_SYMBOLS_PER_SLOT*NR_NB_SC_PER_RB*8; // max pdsch encoded length for each layer

  dlsch.txdataF = (int32_t **)malloc16(max_layers * sizeof(int32_t *));

  dlsch.ue_spec_bf_weights = (int32_t ***)malloc16(max_layers * sizeof(int32_t **));
  for (int layer=0; layer<max_layers; layer++) {
    dlsch.ue_spec_bf_weights[layer] = (int32_t **)malloc16(64 * sizeof(int32_t *));

    for (int aa=0; aa<64; aa++) {
      dlsch.ue_spec_bf_weights[layer][aa] = (int32_t *)malloc16(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES * sizeof(int32_t));

      for (int re=0; re<OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; re++) {
        dlsch.ue_spec_bf_weights[layer][aa][re] = 0x00007fff;
      }
    }
    dlsch.txdataF[layer] = (int32_t *)malloc16((txdataf_size) * sizeof(int32_t));
  }

  int nb_codewords = NR_MAX_NB_LAYERS > 4 ? 2 : 1;
  dlsch.mod_symbs = (int32_t **)malloc16(nb_codewords * sizeof(int32_t *));
  for (int q=0; q<nb_codewords; q++)
    dlsch.mod_symbs[q] = (int32_t *)malloc16(txdataf_size * max_layers * sizeof(int32_t));

  NR_DL_gNB_HARQ_t *harq = &dlsch.harq_process;
  bzero(harq, sizeof(NR_DL_gNB_HARQ_t));
  harq->b = malloc16(dlsch_bytes);
  AssertFatal(harq->b, "cannot allocate memory for harq->b\n");
  harq->pdu = malloc16(dlsch_bytes);
  AssertFatal(harq->pdu, "cannot allocate memory for harq->pdu\n");
  bzero(harq->pdu, dlsch_bytes);
  nr_emulate_dlsch_payload(harq->pdu, (dlsch_bytes) >> 3);
  bzero(harq->b, dlsch_bytes);

  harq->c = (uint8_t **)malloc16(a_segments*sizeof(uint8_t *));
  for (int r = 0; r < a_segments; r++) {
    // account for filler in first segment and CRCs for multiple segment case
    // [hna] 8448 is the maximum CB size in NR
    //       68*348 = 68*(maximum size of Zc)
    //       In section 5.3.2 in 38.212, the for loop is up to N + 2*Zc (maximum size of N is 66*Zc, therefore 68*Zc)
    harq->c[r] = malloc16(8448);
    AssertFatal(harq->c[r], "cannot allocate harq->c[%d]\n", r);
    bzero(harq->c[r], 8448);
  }

  harq->f = malloc16(N_RB * NR_SYMBOLS_PER_SLOT * NR_NB_SC_PER_RB * 8 * NR_MAX_NB_LAYERS);
  AssertFatal(harq->f, "cannot allocate harq->f\n");
  bzero(harq->f, N_RB * NR_SYMBOLS_PER_SLOT * NR_NB_SC_PER_RB * 8 * NR_MAX_NB_LAYERS);

  return(dlsch);
}

void clean_gNB_dlsch(NR_gNB_DLSCH_t *dlsch) {
  AssertFatal(dlsch!=NULL,"dlsch is null\n");
  dlsch->active = 0;
}

void ldpc8blocks( void *p) {
  encoder_implemparams_t *impp=(encoder_implemparams_t *) p;
  NR_DL_gNB_HARQ_t *harq = (NR_DL_gNB_HARQ_t *)impp->harq;
  uint16_t Kr= impp->K;
  nfapi_nr_dl_tti_pdsch_pdu_rel15_t *rel15 = &harq->pdsch_pdu.pdsch_pdu_rel15;
  uint8_t mod_order = rel15->qamModOrder[0];
  uint16_t nb_rb = rel15->rbSize;
  uint8_t nb_symb_sch = rel15->NrOfSymbols;
  uint16_t length_dmrs = get_num_dmrs(rel15->dlDmrsSymbPos);
  uint32_t A = rel15->TBSize[0]<<3;
  uint8_t nb_re_dmrs;

  if (rel15->dmrsConfigType==NFAPI_NR_DMRS_TYPE1)
    nb_re_dmrs = 6*rel15->numDmrsCdmGrpsNoData;
  else
    nb_re_dmrs = 4*rel15->numDmrsCdmGrpsNoData;

  unsigned int G = nr_get_G(nb_rb, nb_symb_sch, nb_re_dmrs, length_dmrs,mod_order,rel15->nrOfLayers);
  LOG_D(PHY,"dlsch coding A %d  Kr %d G %d (nb_rb %d, nb_symb_sch %d, nb_re_dmrs %d, length_dmrs %d, mod_order %d)\n",
        A,impp->K,G, nb_rb,nb_symb_sch,nb_re_dmrs,length_dmrs,(int)mod_order);
  // nrLDPC_encoder output is in "d"
  // let's make this interface happy!
  uint8_t tmp[8][68 * 384]__attribute__((aligned(32)));
  for (int rr=impp->macro_num*8, i=0; rr < impp->n_segments && rr < (impp->macro_num+1)*8; rr++,i++ )
    impp->d[rr]=tmp[i];
  nrLDPC_encoder(harq->c,impp->d,*impp->Zc, impp->Kb,Kr,impp->BG,impp);
  // Compute where to place in output buffer that is concatenation of all segments
  uint32_t r_offset=0;
  for (int i=0; i < impp->macro_num*8; i++ )
     r_offset+=nr_get_E(G, impp->n_segments, mod_order, rel15->nrOfLayers, i);
  for (int rr=impp->macro_num*8; rr < impp->n_segments && rr < (impp->macro_num+1)*8; rr++ ) {
    if (impp->F>0) {
      // writing into positions d[r][k-2Zc] as in clause 5.3.2 step 2) in 38.212
      memset(&impp->d[rr][Kr-impp->F-2*(*impp->Zc)], NR_NULL, impp->F);
    }

#ifdef DEBUG_DLSCH_CODING
    LOG_D(PHY,"rvidx in encoding = %d\n", rel15->rvIndex[0]);
#endif
    uint32_t E = nr_get_E(G, impp->n_segments, mod_order, rel15->nrOfLayers, rr);
    //#ifdef DEBUG_DLSCH_CODING
    LOG_D(NR_PHY,"Rate Matching, Code segment %d/%d (coded bits (G) %u, E %d, Filler bits %d, Filler offset %d mod_order %d, nb_rb %d,nrOfLayer %d)...\n",
          rr,
          impp->n_segments,
          G,
          E,
          impp->F,
          Kr-impp->F-2*(*impp->Zc),
          mod_order,nb_rb,rel15->nrOfLayers);

    uint32_t Tbslbrm = rel15->maintenance_parms_v3.tbSizeLbrmBytes;

    uint8_t e[E];
    bzero (e, E);
    nr_rate_matching_ldpc(Tbslbrm,
                          impp->BG,
                          *impp->Zc,
                          impp->d[rr],
                          e,
                          impp->n_segments,
                          impp->F,
                          Kr-impp->F-2*(*impp->Zc),
                          rel15->rvIndex[0],
                          E);
   if (Kr-impp->F-2*(*impp->Zc)> E)  {
    LOG_E(PHY,"dlsch coding A %d  Kr %d G %d (nb_rb %d, nb_symb_sch %d, nb_re_dmrs %d, length_dmrs %d, mod_order %d)\n",
          A,impp->K,G, nb_rb,nb_symb_sch,nb_re_dmrs,length_dmrs,(int)mod_order);
 
    LOG_E(NR_PHY,"Rate Matching, Code segment %d/%d (coded bits (G) %u, E %d, Kr %d, Filler bits %d, Filler offset %d mod_order %d, nb_rb %d)...\n",
          rr,
          impp->n_segments,
          G,
          E,
          Kr,
          impp->F,
          Kr-impp->F-2*(*impp->Zc),
          mod_order,nb_rb);
    }
#ifdef DEBUG_DLSCH_CODING

    for (int i =0; i<16; i++)
      printf("output ratematching e[%d]= %d r_offset %u\n", i,e[i], r_offset);

#endif
    nr_interleaving_ldpc(E,
                         mod_order,
                         e,
                         impp->output+r_offset);
#ifdef DEBUG_DLSCH_CODING

    for (int i =0; i<16; i++)
      printf("output interleaving f[%d]= %d r_offset %u\n", i,impp->output[i+r_offset], r_offset);

    if (r==impp->n_segments-1)
      write_output("enc_output.m","enc",impp->output,G,1,4);

#endif
    r_offset += E;
  }
}

int nr_dlsch_encoding(PHY_VARS_gNB *gNB,
                      int frame,
                      uint8_t slot,
                      NR_DL_gNB_HARQ_t *harq,
                      NR_DL_FRAME_PARMS *frame_parms,
                      unsigned char *output,
                      time_stats_t *tinput,
                      time_stats_t *tprep,
                      time_stats_t *tparity,
                      time_stats_t *toutput,
                      time_stats_t *dlsch_rate_matching_stats,
                      time_stats_t *dlsch_interleaving_stats,
                      time_stats_t *dlsch_segmentation_stats)
{
  encoder_implemparams_t impp;
  impp.output=output;
  unsigned int crc=1;
  nfapi_nr_dl_tti_pdsch_pdu_rel15_t *rel15 = &harq->pdsch_pdu.pdsch_pdu_rel15;
  impp.Zc = &harq->Z;
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_gNB_DLSCH_ENCODING, VCD_FUNCTION_IN);
  uint32_t A = rel15->TBSize[0]<<3;
  unsigned char *a=harq->pdu;
  if (rel15->rnti != SI_RNTI)
    trace_NRpdu(DIRECTION_DOWNLINK, a, rel15->TBSize[0], WS_C_RNTI, rel15->rnti, frame, slot,0, 0);

  NR_gNB_PHY_STATS_t *phy_stats = NULL;
  if (rel15->rnti != 0xFFFF)
    phy_stats = get_phy_stats(gNB, rel15->rnti);

  if (phy_stats) {
    phy_stats->frame = frame;
    phy_stats->dlsch_stats.total_bytes_tx += rel15->TBSize[0];
    phy_stats->dlsch_stats.current_RI = rel15->nrOfLayers;
    phy_stats->dlsch_stats.current_Qm = rel15->qamModOrder[0];
  }

  int max_bytes = MAX_NUM_NR_DLSCH_SEGMENTS_PER_LAYER*rel15->nrOfLayers*1056;
  if (A > 3824) {
    // Add 24-bit crc (polynomial A) to payload
    crc = crc24a(a,A)>>8;
    a[A>>3] = ((uint8_t *)&crc)[2];
    a[1+(A>>3)] = ((uint8_t *)&crc)[1];
    a[2+(A>>3)] = ((uint8_t *)&crc)[0];
    //printf("CRC %x (A %d)\n",crc,A);
    //printf("a0 %d a1 %d a2 %d\n", a[A>>3], a[1+(A>>3)], a[2+(A>>3)]);
    harq->B = A+24;
    //    harq->b = a;
    AssertFatal((A / 8) + 4 <= max_bytes,
                "A %d is too big (A/8+4 = %d > %d)\n",
                A,
                (A / 8) + 4,
                max_bytes);
    memcpy(harq->b, a, (A / 8) + 4); // why is this +4 if the CRC is only 3 bytes?
  } else {
    // Add 16-bit crc (polynomial A) to payload
    crc = crc16(a,A)>>16;
    a[A>>3] = ((uint8_t *)&crc)[1];
    a[1+(A>>3)] = ((uint8_t *)&crc)[0];
    //printf("CRC %x (A %d)\n",crc,A);
    //printf("a0 %d a1 %d \n", a[A>>3], a[1+(A>>3)]);
    harq->B = A+16;
    //    harq->b = a;
    AssertFatal((A / 8) + 3 <= max_bytes,
                "A %d is too big (A/8+3 = %d > %d)\n",
                A,
                (A / 8) + 3,
                max_bytes);
    memcpy(harq->b, a, (A / 8) + 3); // using 3 bytes to mimic the case of 24 bit crc
  }

  impp.BG = rel15->maintenance_parms_v3.ldpcBaseGraph;

  start_meas(dlsch_segmentation_stats);
  impp.Kb = nr_segmentation(harq->b, harq->c, harq->B, &impp.n_segments, &impp.K, impp.Zc, &impp.F, impp.BG);
  stop_meas(dlsch_segmentation_stats);

  if (impp.n_segments>MAX_NUM_NR_DLSCH_SEGMENTS_PER_LAYER*rel15->nrOfLayers) {
    LOG_E(PHY,"nr_segmentation.c: too many segments %d, B %d\n",impp.n_segments,harq->B);
    return(-1);
  }

  for (int r=0; r<impp.n_segments; r++) {
    //d_tmp[r] = &harq->d[r][0];
    //channel_input[r] = &harq->d[r][0];
#ifdef DEBUG_DLSCH_CODING
    LOG_D(PHY,"Encoder: B %d F %d \n",harq->B, impp.F);
    LOG_D(PHY,"start ldpc encoder segment %d/%d\n",r,impp.n_segments);
    LOG_D(PHY,"input %d %d %d %d %d \n", harq->c[r][0], harq->c[r][1], harq->c[r][2],harq->c[r][3], harq->c[r][4]);

    for (int cnt =0 ; cnt < 22*(*impp.Zc)/8; cnt ++) {
      LOG_D(PHY,"%d ", harq->c[r][cnt]);
    }

    LOG_D(PHY,"\n");
#endif
    //ldpc_encoder_orig((unsigned char*)harq->c[r],harq->d[r],*Zc,Kb,Kr,BG,0);
    //ldpc_encoder_optim((unsigned char*)harq->c[r],(unsigned char*)&harq->d[r][0],*Zc,Kb,Kr,BG,NULL,NULL,NULL,NULL);
  }

  impp.tprep = tprep;
  impp.tinput = tinput;
  impp.tparity = tparity;
  impp.toutput = toutput;

  impp.harq=harq;
  notifiedFIFO_t nf;
  initNotifiedFIFO(&nf);
  int nbJobs=0;
  for(int j=0; j<(impp.n_segments/8+((impp.n_segments&7)==0 ? 0 : 1)); j++) {
    notifiedFIFO_elt_t *req=newNotifiedFIFO_elt(sizeof(impp), j, &nf, ldpc8blocks);
    encoder_implemparams_t* perJobImpp=(encoder_implemparams_t*)NotifiedFifoData(req);
    *perJobImpp=impp;
    perJobImpp->macro_num=j;
    pushTpool(&gNB->threadPool, req);
    nbJobs++;
  }
  while(nbJobs) {
    notifiedFIFO_elt_t *req=pullTpool(&nf, &gNB->threadPool);
    if (req == NULL)
      break; // Tpool has been stopped
    delNotifiedFIFO_elt(req);
    nbJobs--;

  }
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_gNB_DLSCH_ENCODING, VCD_FUNCTION_OUT);
  return 0;
}
