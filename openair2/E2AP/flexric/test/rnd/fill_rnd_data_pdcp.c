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

#include "fill_rnd_data_pdcp.h"
#include "../../src/util/time_now_us.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

void fill_pdcp_ind_data(pdcp_ind_data_t* ind)
{
  assert(ind != NULL);

  srand(time(0));

  pdcp_ind_msg_t* ind_msg = &ind->msg; 

  ind_msg->len = abs(rand()%8);
  ind_msg->tstamp = time_now_us();

  if(ind_msg->len > 0){
    ind_msg->rb = calloc(ind_msg->len, sizeof(pdcp_radio_bearer_stats_t));
    assert(ind_msg->rb != NULL && "Memory exhausted!");
  }

  for(uint32_t i = 0; i < ind_msg->len; ++i){
    pdcp_radio_bearer_stats_t* rb = &ind_msg->rb[i];

    int const mod = 1024;
    rb->txpdu_bytes = abs(rand()%mod);    /* aggregated bytes of tx packets */
    rb->txpdu_pkts = rb->txpdu_bytes != 0 ? rb->txpdu_bytes - rand()%rb->txpdu_bytes : 0;     /* aggregated number of tx packets */
    rb->txpdu_sn= abs(rand()%mod);       /* current sequence number of last tx packet (or TX_NEXT) */

    rb->rxpdu_bytes = abs(rand()%mod);    /* aggregated bytes of rx packets */
    rb->rxpdu_pkts = rb->rxpdu_bytes != 0 ? rb->rxpdu_bytes - rand()%rb->rxpdu_bytes : 0;     /* aggregated number of rx packets */
    rb->rxpdu_sn= abs(rand()%mod);       /* current sequence number of last rx packet (or  RX_NEXT) */
    rb->rxpdu_oo_pkts= abs(rand()%mod);       /* aggregated number of out-of-order rx pkts  (or RX_REORD) */
    rb->rxpdu_oo_bytes= abs(rand()%mod); /* aggregated amount of out-of-order rx bytes */
    rb->rxpdu_dd_pkts= abs(rand()%mod);  /* aggregated number of duplicated discarded packets (or dropped packets because of other reasons such as integrity failure) (or RX_DELIV) */
    rb->rxpdu_dd_bytes= abs(rand()%mod); /* aggregated amount of discarded packets' bytes */
    rb->rxpdu_ro_count= abs(rand()%mod); /* this state variable indicates the COUNT value following the COUNT value associated with the PDCP Data PDU which triggered t-Reordering. (RX_REORD) */
    rb->txsdu_bytes = abs(rand()%mod);    /* number of bytes of SDUs delivered */
    rb->txsdu_pkts = rb->txsdu_bytes != 0 ? rb->txsdu_bytes - rand()% rb->txsdu_bytes : 0;     /* number of SDUs delivered */

    rb->rxsdu_bytes = abs(rand()%mod);    /* number of bytes of SDUs received */
    rb->rxsdu_pkts =  rb->rxsdu_bytes != 0 ? rb->rxsdu_bytes - rand()%rb->rxsdu_bytes : 0;     /* number of SDUs received */

    rb->rnti= abs(rand()%mod);
    rb->mode= abs(rand()%3);               /* 0: PDCP AM, 1: PDCP UM, 2: PDCP TM */
    rb->rbid= abs(rand()%11);
  }
}

