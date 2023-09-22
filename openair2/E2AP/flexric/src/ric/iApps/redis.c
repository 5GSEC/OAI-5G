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


#include "redis.h"

#include <assert.h>
#include <stdio.h>

void notify_redis_listener(sm_ag_if_rd_ind_t const* data)
{
  assert(data != NULL);
  assert(data->type == MAC_STATS_V0 || data->type == RLC_STATS_V0 || data->type == PDCP_STATS_V0 
      || data->type == SLICE_STATS_V0 || data->type == KPM_STATS_V3_0 || data->type == GTP_STATS_V0
      || data->type == RAN_CTRL_STATS_V1_03 || data->type == TC_STATS_V0  ); 
 
  /*
  if(data->type == MAC_STATS_V0)
    printf("REDIS data called from MAC stats!!\n");
  else if (data->type == RLC_STATS_V0 )
    printf("REDIS data called from RLC stats!!\n");
  else
    assert(0!=0 && "Invalid data path");
    */
}
