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

#include "fill_rnd_data_mac.h"
#include "../../src/util/time_now_us.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

void fill_mac_ind_data(mac_ind_data_t* ind)
{
  assert(ind != NULL);
  srand(time(0));

  int const mod = 1024;

  mac_ind_msg_t* ind_msg = &ind->msg; 
  
  int const NUM_UES = abs(rand()%5);

  ind_msg->len_ue_stats = NUM_UES;

  ind_msg->tstamp = time_now_us();

  if(NUM_UES > 0){
    ind_msg->ue_stats = calloc(NUM_UES, sizeof(mac_ue_stats_impl_t));
    assert(ind_msg->ue_stats != NULL && "memory exhausted");
  }

  const size_t numDLHarq = 4;
  const size_t numUlHarq = 4;

  for(uint32_t i = 0; i < ind_msg->len_ue_stats; ++i){
    ind_msg->ue_stats[i].dl_aggr_tbs = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_aggr_tbs = abs(rand()%mod);
    ind_msg->ue_stats[i].dl_aggr_bytes_sdus = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_aggr_bytes_sdus = abs(rand()%mod);
    ind_msg->ue_stats[i].pusch_snr = 64.0; //: float = -64;
    ind_msg->ue_stats[i].pucch_snr = 64.0; //: float = -64;
    ind_msg->ue_stats[i].rnti = abs(rand()%mod);
    ind_msg->ue_stats[i].dl_aggr_prb = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_aggr_prb = abs(rand()%mod);
    ind_msg->ue_stats[i].dl_aggr_sdus = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_aggr_sdus= abs(rand()%mod);
    ind_msg->ue_stats[i].dl_aggr_retx_prb= abs(rand()%mod);
    ind_msg->ue_stats[i].ul_aggr_retx_prb= abs(rand()%mod);
    ind_msg->ue_stats[i].wb_cqi= abs(rand()%mod);
    ind_msg->ue_stats[i].dl_mcs1= abs(rand()%mod);
    ind_msg->ue_stats[i].ul_mcs1= abs(rand()%mod);
    ind_msg->ue_stats[i].dl_mcs2= abs(rand()%mod);
    ind_msg->ue_stats[i].ul_mcs2= abs(rand()%mod);
    ind_msg->ue_stats[i].phr= (rand()%64) - 23; // 41 abs(rand()%mod);
    ind_msg->ue_stats[i].bsr= abs(rand()%mod);
    ind_msg->ue_stats[i].dl_num_harq = numUlHarq;
    for (uint8_t j = 0; j < numDLHarq; j++)
      ind_msg->ue_stats[i].dl_harq[j] = abs(rand()%mod);
    ind_msg->ue_stats[i].ul_num_harq = numUlHarq;
    for (uint8_t j = 0; j < numUlHarq; j++)
      ind_msg->ue_stats[i].ul_harq[j] = abs(rand()%mod);
  }
}


