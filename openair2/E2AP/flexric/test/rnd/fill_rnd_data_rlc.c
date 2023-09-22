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

#include "fill_rnd_data_rlc.h"
#include "../../src/util/time_now_us.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

void fill_rlc_ind_data(rlc_ind_data_t* ind)
{
  assert(ind != NULL);

  srand(time(0));

  int const mod = 1024;

  rlc_ind_msg_t* ind_msg = &ind->msg;

  ind_msg->tstamp = time_now_us();

  ind_msg->len = rand()%4;
  if(ind_msg->len > 0 ){
    ind_msg->rb = calloc(ind_msg->len, sizeof(rlc_radio_bearer_stats_t) );
    assert(ind_msg->rb != NULL);
  }

  for(uint32_t i = 0; i < ind_msg->len; ++i){
    rlc_radio_bearer_stats_t* rb = &ind_msg->rb[i];

    rb->txpdu_pkts=abs(rand()%mod) ;         /* aggregated number of transmitted RLC PDUs */
    rb->txpdu_bytes=abs(rand()%mod);        /* aggregated amount of transmitted bytes in RLC PDUs */
    /* TODO? */
    rb->txpdu_wt_ms=abs(rand()%mod);      /* aggregated head-of-line tx packet waiting time to be transmitted (i.e. send to the MAC layer) */
    rb->txpdu_dd_pkts=abs(rand()%mod);      /* aggregated number of dropped or discarded tx packets by RLC */
    rb->txpdu_dd_bytes=abs(rand()%mod);     /* aggregated amount of bytes dropped or discarded tx packets by RLC */
    rb->txpdu_retx_pkts=abs(rand()%mod);    /* aggregated number of tx pdus/pkts to be re-transmitted (only applicable to RLC AM) */
    rb->txpdu_retx_bytes=abs(rand()%mod);   /* aggregated amount of bytes to be re-transmitted (only applicable to RLC AM) */
    rb->txpdu_segmented=abs(rand()%mod);    /* aggregated number of segmentations */
    rb->txpdu_status_pkts=abs(rand()%mod);  /* aggregated number of tx status pdus/pkts (only applicable to RLC AM) */
    rb->txpdu_status_bytes=abs(rand()%mod); /* aggregated amount of tx status bytes  (only applicable to RLC AM) */
    /* TODO? */
    rb->txbuf_occ_bytes=abs(rand()%mod);    /* current tx buffer occupancy in terms of amount of bytes (average: NOT IMPLEMENTED) */
    /* TODO? */
    rb->txbuf_occ_pkts=abs(rand()%mod);     /* current tx buffer occupancy in terms of number of packets (average: NOT IMPLEMENTED) */
    /* txbuf_wd_ms: the time window for which the txbuf  occupancy value is obtained - NOT IMPLEMENTED */

    /* RX */
    rb->rxpdu_pkts=abs(rand()%mod);         /* aggregated number of received RLC PDUs */
    rb->rxpdu_bytes=abs(rand()%mod);        /* amount of bytes received by the RLC */
    rb->rxpdu_dup_pkts=abs(rand()%mod);     /* aggregated number of duplicate packets */
    rb->rxpdu_dup_bytes=abs(rand()%mod);    /* aggregated amount of duplicated bytes */
    rb->rxpdu_dd_pkts=abs(rand()%mod);      /* aggregated number of rx packets dropped or discarded by RLC */
    rb->rxpdu_dd_bytes=abs(rand()%mod);     /* aggregated amount of rx bytes dropped or discarded by RLC */
    rb->rxpdu_ow_pkts=abs(rand()%mod);      /* aggregated number of out of window received RLC pdu */
    rb->rxpdu_ow_bytes=abs(rand()%mod);     /* aggregated number of out of window bytes received RLC pdu */
    rb->rxpdu_status_pkts=abs(rand()%mod);  /* aggregated number of rx status pdus/pkts (only applicable to RLC AM) */
    rb->rxpdu_status_bytes=abs(rand()%mod); /* aggregated amount of rx status bytes  (only applicable to RLC AM) */
    /* rxpdu_rotout_ms: flag indicating rx reordering  timeout in ms - NOT IMPLEMENTED */
    /* rxpdu_potout_ms: flag indicating the poll retransmit time out in ms - NOT IMPLEMENTED */
    /* rxpdu_sptout_ms: flag indicating status prohibit timeout in ms - NOT IMPLEMENTED */
    /* TODO? */
    rb->rxbuf_occ_bytes=abs(rand()%mod);    /* current rx buffer occupancy in terms of amount of bytes (average: NOT IMPLEMENTED) */
    /* TODO? */
    rb->rxbuf_occ_pkts=abs(rand()%mod);     /* current rx buffer occupancy in terms of number of packets (average: NOT IMPLEMENTED) */

    /* SDU stats */
    /* TX */
    rb->txsdu_pkts=abs(rand()%mod);         /* number of SDUs delivered */
    rb->txsdu_bytes=abs(rand()%mod);        /* number of bytes of SDUs delivered */

    /* RX */
    rb->rxsdu_pkts=abs(rand()%mod);         /* number of SDUs received */
    rb->rxsdu_bytes=abs(rand()%mod);        /* number of bytes of SDUs received */
    rb->rxsdu_dd_pkts=abs(rand()%mod);      /* number of dropped or discarded SDUs */
    rb->rxsdu_dd_bytes=abs(rand()%mod);     /* number of bytes of SDUs dropped or discarded */

    rb->rnti=abs(rand()%mod);
    rb->mode=abs(rand()%3);               /* 0: RLC AM, 1: RLC UM, 2: RLC TM */
    rb->rbid=abs(rand()%16);

  }
}

