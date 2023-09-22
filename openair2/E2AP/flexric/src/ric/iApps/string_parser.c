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



#include "string_parser.h"
#include "string.h"
#include <assert.h>                                      // for assert
#include <stdio.h>                                       // for snprintf
#include "ric/iApps/../../sm/mac_sm/ie/mac_data_ie.h"    // for mac_ue_stats...
#include "ric/iApps/../../sm/pdcp_sm/ie/pdcp_data_ie.h"  // for pdcp_radio_b...
#include "ric/iApps/../../sm/rlc_sm/ie/rlc_data_ie.h"    // for rlc_radio_be...
#include "ric/iApps/../../sm/slice_sm/ie/slice_data_ie.h"
#include "ric/iApps/../../sm/kpm_sm_v03.00/ie/kpm_data_ie.h"


void to_string_mac_ue_stats(mac_ue_stats_impl_t* stats, int64_t tstamp, char* out, size_t out_len)
{
  assert(stats != NULL);
  assert(out != NULL);
  const size_t max = 1024;
  assert(out_len >= max);
  int rc = snprintf(out, max, "mac_stats: "
        "tstamp=%ld,"
        "frame=%d,"
        "slot=%d,"
        "dl_aggr_tbs=%lu,"
        "ul_aggr_tbs=%lu,"
        "dl_aggr_bytes_sdus=%lu,"
        "ul_aggr_bytes_sdus=%lu,"
        "dl_curr_tbs=%lu,"
        "ul_curr_tbs=%lu,"
        "dl_sched_rb=%lu,"
        "ul_sched_rb=%lu,"
        "pusch_snr=%g,"
        "pucch_snr=%g,"
        "rnti=%x,"
        "dl_aggr_prb=%u,"
        "ul_aggr_prb=%u,"
        "dl_aggr_sdus=%u,"
        "ul_aggr_sdus=%u,"
        "dl_aggr_retx_prb=%u,"
        "ul_aggr_retx_prb=%u,"
        "wb_cqi=%u,"
        "dl_mcs1=%u,"
        "ul_mcs1=%u,"
        "dl_mcs2=%u,"
        "ul_mcs2=%u,"
        "phr=%d,"
        "bsr=%u,"
        "dl_bler=%f,"
        "ul_bler=%f,"
        "dl_num_harq=%d,"
        "dl_harq[0]=%u,"
        "dl_harq[1]=%u,"
        "dl_harq[2]=%u,"
        "dl_harq[3]=%u,"
        "dlsch_errors=%u,"
        "ul_num_harq=%d,"
        "ul_harq[0]=%u,"
        "ul_harq[1]=%u,"
        "ul_harq[2]=%u,"
        "ul_harq[3]=%u,"
        "ulsch_errors=%u"
        "\n"
        ,tstamp
        ,stats->frame
        ,stats->slot
        ,stats->dl_aggr_tbs
        ,stats->ul_aggr_tbs 
        ,stats->dl_aggr_bytes_sdus 
        ,stats->ul_aggr_bytes_sdus  
        ,stats->dl_curr_tbs
        ,stats->ul_curr_tbs
        ,stats->dl_sched_rb
        ,stats->ul_sched_rb
        ,stats->pusch_snr 
        ,stats->pucch_snr 
        ,stats->rnti 
        ,stats->dl_aggr_prb  
        ,stats->ul_aggr_prb 
        ,stats->dl_aggr_sdus 
        ,stats->ul_aggr_sdus 
        ,stats->dl_aggr_retx_prb  
        ,stats->ul_aggr_retx_prb
        ,stats->wb_cqi
        ,stats->dl_mcs1
        ,stats->ul_mcs1
        ,stats->dl_mcs2
        ,stats->ul_mcs2
        ,stats->phr 
        ,stats->bsr
        ,stats->dl_bler
        ,stats->ul_bler
        ,stats->dl_num_harq
        ,stats->dl_harq[0]
        ,stats->dl_harq[1]
        ,stats->dl_harq[2]
        ,stats->dl_harq[3]
        ,stats->dl_harq[4]
        ,stats->ul_num_harq
        ,stats->ul_harq[0]
        ,stats->ul_harq[1]
        ,stats->ul_harq[2]
        ,stats->ul_harq[3]
        ,stats->ul_harq[4]
        );
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
}


void to_string_rlc_rb(rlc_radio_bearer_stats_t* rlc, int64_t tstamp , char* out, size_t out_len)
{
  assert(rlc != NULL);
  assert(out != NULL);
  const size_t max = 1024;
  assert(out_len >= max);
 
  int const rc = snprintf(out, max,
        "rlc_stats: tstamp=%ld,"
        "txpdu_pkts=%u,"
        "txpdu_bytes=%u,"
        "txpdu_wt_ms=%u,"
        "txpdu_dd_pkts=%u,"
        "txpdu_dd_bytes=%u,"
        "txpdu_retx_pkts=%u,"
        "txpdu_retx_bytes=%u,"
        "txpdu_segmented=%u,"
        "txpdu_status_pkts=%u,"
        "txpdu_status_bytes=%u,"
        "txbuf_occ_bytes=%u,"
        "txbuf_occ_pkts=%u,"
        "rxpdu_pkts=%u,"
        "rxpdu_bytes=%u,"
        "rxpdu_dup_pkts=%u,"
        "rxpdu_dup_bytes=%u,"
        "rxpdu_dd_pkts=%u,"
        "rxpdu_dd_bytes=%u,"
        "rxpdu_ow_pkts=%u,"
        "rxpdu_ow_bytes=%u,"
        "rxpdu_status_pkts=%u,"
        "rxpdu_status_bytes=%u,"
        "rxbuf_occ_bytes=%u,"
        "rxbuf_occ_pkts=%u,"
        "txsdu_pkts=%u,"
        "txsdu_bytes=%u,"
        "rxsdu_pkts=%u,"
        "rxsdu_bytes=%u,"
        "rxsdu_dd_pkts=%u,"
        "rxsdu_dd_bytes=%u,"
        "rnti=%u,"
        "mode=%u,"
        "rbid=%u"
        "\n"
        , tstamp 
        , rlc->txpdu_pkts
        , rlc->txpdu_bytes
        , rlc->txpdu_wt_ms
        , rlc->txpdu_dd_pkts
        , rlc->txpdu_dd_bytes
        , rlc->txpdu_retx_pkts
        , rlc->txpdu_retx_bytes
        , rlc->txpdu_segmented
        , rlc->txpdu_status_pkts
        , rlc->txpdu_status_bytes
        , rlc->txbuf_occ_bytes
        , rlc->txbuf_occ_pkts
        , rlc->rxpdu_pkts
        , rlc->rxpdu_bytes
        , rlc->rxpdu_dup_pkts
        , rlc->rxpdu_dup_bytes
        , rlc->rxpdu_dd_pkts
        , rlc->rxpdu_dd_bytes
        , rlc->rxpdu_ow_pkts
        , rlc->rxpdu_ow_bytes
        , rlc->rxpdu_status_pkts
        , rlc->rxpdu_status_bytes
        , rlc->rxbuf_occ_bytes
        , rlc->rxbuf_occ_pkts
        , rlc->txsdu_pkts
        , rlc->txsdu_bytes
        , rlc->rxsdu_pkts
        , rlc->rxsdu_bytes
        , rlc->rxsdu_dd_pkts
        , rlc->rxsdu_dd_bytes
        , rlc->rnti
        , rlc->mode
        , rlc->rbid
        );
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
}


void to_string_pdcp_rb(pdcp_radio_bearer_stats_t* pdcp, int64_t tstamp, char* out, size_t out_len)
{
  assert(pdcp != NULL);
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);
 
  int rc = snprintf(out, out_len,  "pdcp_stats: " 
        "tstamp=%ld,"
        "txpdu_pkts=%u,"
        "txpdu_bytes=%u,"
        "txpdu_sn=%u,"
        "rxpdu_pkts=%u,"
        "rxpdu_bytes=%u,"
        "rxpdu_sn=%u,"
        "rxpdu_oo_pkts=%u,"
        "rxpdu_oo_bytes=%u,"
        "rxpdu_dd_pkts=%u,"
        "rxpdu_dd_bytes=%u,"
        "rxpdu_ro_count=%u,"
        "txsdu_pkts=%u,"
        "txsdu_bytes=%u,"
        "rxsdu_pkts=%u,"
        "rxsdu_bytes=%u,"
        "rnti=%u,"
        "mode=%u,"
        "rbid=%u"
        "\n"
        , tstamp
        , pdcp->txpdu_pkts
        , pdcp->txpdu_bytes 
        , pdcp->txpdu_sn
        , pdcp->rxpdu_pkts
        , pdcp->rxpdu_bytes
        , pdcp->rxpdu_sn
        , pdcp->rxpdu_oo_pkts
        , pdcp->rxpdu_oo_bytes
        , pdcp->rxpdu_dd_pkts
        , pdcp->rxpdu_dd_bytes
        , pdcp->rxpdu_ro_count
        , pdcp->txsdu_pkts
        , pdcp->txsdu_bytes
        , pdcp->rxsdu_pkts
        , pdcp->rxsdu_bytes
        , pdcp->rnti
        , pdcp->mode
        , pdcp->rbid
        );
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
}

void to_string_slice(slice_ind_msg_t const* slice, int64_t tstamp, char* out, size_t out_len)
{
  assert(slice != NULL);
  assert(out != NULL);
  const size_t max = 2048;
  assert(out_len >= max);

  char temp[2048] = {0};
  size_t sz = 0;

  if (slice->slice_conf.dl.len_slices == 0) {
    int rc = snprintf(temp, out_len,  "slice_stats: "
                      "tstamp=%ld"
                      ",dl->sched_name=%s"
                      , tstamp
                      , slice->slice_conf.dl.sched_name
                      );
    assert(rc < (int)max && "Not enough space in the char array to write all the data");

    memcpy(out, temp, strlen(temp));
    sz += strlen(temp);
  }

  for(uint32_t i = 0; i < slice->slice_conf.dl.len_slices; ++i) {
    fr_slice_t* s = &slice->slice_conf.dl.slices[i];

    if (i == 0) {
      int rc = snprintf(temp, out_len, "slice_stats: tstamp=%ld,slice_conf,dl,len_slices=%u", tstamp, slice->slice_conf.dl.len_slices);
      assert(rc < (int)max && "Not enough space in the char array to write all the data");

      memcpy(out, temp, strlen(temp));
      sz += strlen(temp);
    }

    memset(temp, 0, sizeof(temp));
    // static
    if (s->params.type == SLICE_ALG_SM_V0_STATIC) {
      int rc = snprintf(temp, out_len,
                        ",slice[%u]"
                        ",id=%u"
                        ",label=%s"
                        ",type=%d,static"
                        ",sched=%s"
                        ",pos_high=%u"
                        ",pos_low=%u",
                        i,
                        s->id,
                        s->label,
                        s->params.type,
                        s->sched,
                        s->params.u.sta.pos_high,
                        s->params.u.sta.pos_low);
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
    } else if (s->params.type == SLICE_ALG_SM_V0_NVS) {
      // nvs
      if (s->params.u.nvs.conf == SLICE_SM_NVS_V0_RATE) {
         int rc = snprintf(temp, out_len,
                        ",slice[%u]"
                        ",id=%u"
                        ",label=%s"
                        ",type=%d,nvs"
                        ",sched=%s"
                        ",conf=%d,rate"
                        ",mbps_required=%.2f"
                        ",mbps_reference=%.2f",
                        i,
                        s->id,
                        s->label,
                        s->params.type,
                        s->sched,
                        s->params.u.nvs.conf,
                        s->params.u.nvs.u.rate.u1.mbps_required,
                        s->params.u.nvs.u.rate.u2.mbps_reference);
        assert(rc < (int)max && "Not enough space in the char array to write all the data");
      } else if (s->params.u.nvs.conf == SLICE_SM_NVS_V0_CAPACITY) {
        int rc = snprintf(temp, out_len,
                          ",slice[%u]"
                          ",id=%u"
                          ",label=%s"
                          ",type=%d,nvs"
                          ",sched=%s"
                          ",conf=%d,capacity"
                          ",pct_reserved=%.2f",
                          i,
                          s->id,
                          s->label,
                          s->params.type,
                          s->sched,
                          s->params.u.nvs.conf,
                          s->params.u.nvs.u.capacity.u.pct_reserved);
        assert(rc < (int)max && "Not enough space in the char array to write all the data");
      }
    } else if (s->params.type == SLICE_ALG_SM_V0_EDF) {
      // edf
      int rc = snprintf(temp, out_len,
                        ",slice[%u]"
                        ",id=%u"
                        ",label=%s"
                        ",type=%d,edf"
                        ",sched=%s"
                        ",deadline=%u"
                        ",guaranteed_prbs=%u"
                        ",max_replenish=%u",
                        i,
                        s->id,
                        s->label,
                        s->params.type,
                        s->sched,
                        s->params.u.edf.deadline,
                        s->params.u.edf.guaranteed_prbs,
                        s->params.u.edf.max_replenish);
      // TODO: edf.len_over & edf.over[]
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
    }
    memcpy(out + sz, temp, strlen(temp));
    sz += strlen(temp);
  }

  for(uint32_t i = 0; i < slice->ue_slice_conf.len_ue_slice; ++i) {
    ue_slice_assoc_t * u = &slice->ue_slice_conf.ues[i];

    if (i == 0) {
      memset(temp, 0, sizeof(temp));
      int rc = snprintf(temp, out_len, ",ue_slice_conf,len_ue_slice=%u", slice->ue_slice_conf.len_ue_slice);
      assert(rc < (int)max && "Not enough space in the char array to write all the data");

      memcpy(out + sz, temp, strlen(temp));
      sz += strlen(temp);
    }

    memset(temp, 0, sizeof(temp));
    int rc = snprintf(temp, out_len,
                      ",ues[%u]"
                      ",rnti=%x"
                      ",dl_id=%d",
                      i,
                      u->rnti,
                      u->dl_id);
    assert(rc < (int)max && "Not enough space in the char array to write all the data");

    memcpy(out + sz, temp, strlen(temp));
    sz += strlen(temp);
  }

  char end[] = "\n";
  memcpy(out + sz, end, strlen(end));
  sz += strlen(end);
  out[sz] = '\0';
  assert(strlen(out) < max && "Not enough space in the char array to write all the data");
}

void to_string_gtp_ngu(gtp_ngu_t_stats_t const* gtp, int64_t tstamp , char* out, size_t out_len)
{
  assert(gtp != NULL);
  assert(out != NULL);
  const size_t max = 512;
  assert(out_len >= max);
 
  int const rc = snprintf(out, max,
        "gtp_stats: tstamp=%ld,"
        "rnti=%u,"
        "qfi=%u,"
        "teidgnb=%u,"
        "teidupf=%u"
        "\n"
        , tstamp 
        , gtp->rnti
        , gtp->qfi
        , gtp->teidgnb
        , gtp->teidupf
        );
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
}

void to_string_kpm_labelInfo(label_info_lst_t const* labelInfo, size_t idx,  char *out, size_t out_len)
{
  assert(labelInfo != NULL);
  
  size_t avail = out_len;
  char *begp = out;
  
  int nprinted = snprintf(begp, avail, ", labelInfo[%lu]=", idx);
  assert((nprinted > 0) && ((size_t) nprinted < avail) && "Not enough space in the char array to write all the data");
  avail -= nprinted;
  begp += nprinted;

  if (labelInfo->noLabel) {
    nprinted = snprintf(begp, avail, "(noLabel = %d)", *(labelInfo->noLabel));
    assert((nprinted > 0) && ((size_t) nprinted < avail) && "Not enough space in the char array to write all the data");
    avail -= nprinted;
    begp += nprinted;
  }
/*
  nprinted = snprintf(out, out_len,
                        ",labelInfo"
                        ",noLabel=%ld"
                        ",plmnID->len=%zu"
                        ",plmnID->buf=%u"
                        ",sliceID->sD->len=%zu"
                        ",sliceID->sD->buf=%u"
                        ",sliceID->sST->len=%zu"
                        ",sliceID->sST->buf=%u"
                        ",fiveQI=%ld"
                        ",qFI=%ld"
                        ",qCI=%ld"
                        ",qFI=%ld"
                        ",qCImax=%ld"
                        ",qCImin=%ld"
                        ",aRPmax=%ld"
                        ",aRPmin=%ld"
                        ",bitrateRange=%ld"
                        ",layerMU_MIMO=%ld"
                        ",sUM=%ld"
                        ",distBinX=%ld"
                        ",distBinY=%ld"
                        ",distBinZ=%ld"
                        ",preLabelOverride=%ld"
                        ",startEndInd=%ld"
                        ",min=%ld"
                        ",max=%ld"
                        ",avg=%ld"
                        , *labelInfo->noLabel
                        , labelInfo->plmnID->len
                        , *labelInfo->plmnID->buf
                        , labelInfo->sliceID->sD->len
                        , *labelInfo->sliceID->sD->buf
                        , labelInfo->sliceID->sST.len
                        , *labelInfo->sliceID->sST.buf
                        , *labelInfo->fiveQI
                        , *labelInfo->qFI
                        , *labelInfo->qCI
                        , *labelInfo->qFI
                        , *labelInfo->qCImax
                        , *labelInfo->qCImin
                        , *labelInfo->aRPmax
                        , *labelInfo->aRPmin
                        , *labelInfo->bitrateRange
                        , *labelInfo->layerMU_MIMO
                        , *labelInfo->sUM
                        , *labelInfo->distBinX
                        , *labelInfo->distBinY
                        , *labelInfo->distBinZ
                        , *labelInfo->preLabelOverride
                        , *labelInfo->startEndInd
                        , *labelInfo->min
                        , *labelInfo->max
                        , *labelInfo->avg
                        );

    assert(rc < (int)out_len && "Not enough space in the char array to write all the data");
    */
}

void to_string_kpm_measRecord(meas_record_lst_t const* measRecord, size_t idx, char *out, size_t out_len)
{
  assert(measRecord != NULL);
  int rc = 0;
  if (measRecord->value == INTEGER_MEAS_VALUE){
    rc = snprintf(out, out_len, ",Record[%lu]=%d", idx, measRecord->int_val);
  } 
  else if(measRecord->value == REAL_MEAS_VALUE){
    rc = snprintf(out, out_len, ",Record[%lu]=%f", idx, measRecord->real_val);
  } 
  else {
    rc = snprintf(out, out_len, ",Record[%lu]=nulltype", idx); 
  }

  assert(rc < (int)out_len && "Not enough space in the char array to write all the data");
}
