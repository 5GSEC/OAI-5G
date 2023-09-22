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



#include "fill_rnd_data_slice.h"
#include "../../src/util/time_now_us.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


static
void fill_static_slice(static_slice_t* sta)
{
  assert(sta != NULL);

  sta->pos_high = abs(rand()%25);
  sta->pos_low = abs(rand()%25);
}


static
void fill_nvs_slice(nvs_slice_t* nvs)
{
  assert(nvs != NULL);

  const uint32_t type = abs(rand() % SLICE_SM_NVS_V0_END);

  if(type == SLICE_SM_NVS_V0_RATE ){
    nvs->conf = SLICE_SM_NVS_V0_RATE; 
    nvs->u.rate.u2.mbps_reference = 0.8; 
//      10.0*((float)rand()/(float)RAND_MAX); 
    nvs->u.rate.u1.mbps_required = 10.0;
    //*((float)rand()/(float)RAND_MAX); 
  } else if(type ==SLICE_SM_NVS_V0_CAPACITY ){
    nvs->conf = SLICE_SM_NVS_V0_CAPACITY; 
    nvs->u.capacity.u.pct_reserved = 15.0;
    //*((float)rand()/(float)RAND_MAX);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

static
void fill_scn19_slice(scn19_slice_t* scn19)
{
  assert(scn19 != NULL);

  const uint32_t type = abs(rand()% SLICE_SCN19_SM_V0_END);

  if(type == SLICE_SCN19_SM_V0_DYNAMIC ){
    scn19->conf = SLICE_SCN19_SM_V0_DYNAMIC ;
    scn19->u.dynamic.u2.mbps_reference = 10.0 * fabs((float)rand()/(float)RAND_MAX); 
    scn19->u.dynamic.u1.mbps_required = 8.0 * fabs((float)rand()/(float)RAND_MAX); 
  } else if(type == SLICE_SCN19_SM_V0_FIXED ) {
    scn19->conf = SLICE_SCN19_SM_V0_FIXED; 
    scn19->u.fixed.pos_high = abs(rand()%14);
    scn19->u.fixed.pos_low = abs(rand()%10);
  } else if(type ==SLICE_SCN19_SM_V0_ON_DEMAND){
    scn19->conf = SLICE_SCN19_SM_V0_ON_DEMAND;
//    scn19->u.on_demand.log_delta_byte = abs(rand()%121);
    scn19->u.on_demand.log_delta = 1.0 * fabs((float)rand()/RAND_MAX);
    scn19->u.on_demand.tau = abs(rand()%256);
    scn19->u.on_demand.pct_reserved = fabs((float)rand()/(float)RAND_MAX);
  } else {
    assert(0 != 0 && "Unknown type!!");
  }

}

static 
void fill_edf_slice(edf_slice_t* edf)
{
  assert(edf != NULL);

  int mod = 32;
  edf->deadline = abs(rand()%mod);
  edf->guaranteed_prbs = abs(rand()%mod);
  edf->max_replenish = abs(rand()%mod);

  edf->len_over = abs(rand()%mod);

  if(edf->len_over > 0){
    edf->over = calloc(edf->len_over, sizeof(uint32_t));
    assert(edf->over != NULL && "Memory exhausted");
  }

  for(uint32_t i = 0; i < edf->len_over; ++i){
    edf->over[i] = abs(rand()%mod);
  }
}

static
void fill_ul_dl_slice(ul_dl_slice_conf_t* slice)
{
  assert(slice != NULL);

  char const* name = "MY SLICE";
  slice->len_sched_name = strlen(name);
  slice->sched_name = malloc(strlen(name));
  assert(slice->sched_name != NULL && "memory exhausted");
  memcpy(slice->sched_name, name, strlen(name));

  slice->len_slices = abs(rand()%4);

  if(slice->len_slices > 0){
    slice->slices = calloc(slice->len_slices, sizeof(fr_slice_t));
    assert(slice->slices != NULL && "memory exhausted");
  }

  for(uint32_t i = 0; i < slice->len_slices; ++i){
    slice->slices[i].id = abs(rand()%1024);
    fr_slice_t* s = &slice->slices[i];

    const char* label = "This is my label";
    s->len_label = strlen(label);
    s->label = malloc(s->len_label);
    assert(s->label != NULL && "Memory exhausted");
    memcpy(s->label, label, s->len_label );

    const char* sched_str = "Scheduler string";
    s->len_sched = strlen(sched_str); 
    s->sched = malloc(s->len_sched);
    assert(s->sched != NULL && "Memory exhausted");
    memcpy(s->sched, sched_str, s->len_sched);

    uint32_t type = abs(rand()% SLICE_ALG_SM_V0_END);

    if(type == SLICE_ALG_SM_V0_NONE || SLICE_ALG_SM_V0_SCN19)
      type = SLICE_ALG_SM_V0_STATIC; 


    if(type == SLICE_ALG_SM_V0_NONE ){
      s->params.type =SLICE_ALG_SM_V0_NONE; 
    } else if (type == SLICE_ALG_SM_V0_STATIC ){
      s->params.type = SLICE_ALG_SM_V0_STATIC; 
      fill_static_slice(&s->params.u.sta);
    } else if (type == SLICE_ALG_SM_V0_NVS){
      s->params.type =  SLICE_ALG_SM_V0_NVS; 
      fill_nvs_slice(&s->params.u.nvs);
    } else if (type == SLICE_ALG_SM_V0_SCN19) {
      s->params.type = SLICE_ALG_SM_V0_SCN19; 
      fill_scn19_slice(&s->params.u.scn19);
    } else if (type == SLICE_ALG_SM_V0_EDF){
      s->params.type =  SLICE_ALG_SM_V0_EDF; 
      fill_edf_slice(&s->params.u.edf);
    } else {
      assert(0 != 0 && "Unknown type encountered");
    }
  }
}

static
void fill_slice_conf(slice_conf_t* conf)
{
  assert(conf != NULL);

  fill_ul_dl_slice(&conf->ul);
  fill_ul_dl_slice(&conf->dl);
}

static
void fill_ue_slice_conf(ue_slice_conf_t* conf)
{
  assert(conf != NULL);
  conf->len_ue_slice = abs(rand()%10);
  if(conf->len_ue_slice > 0){
    conf->ues = calloc(conf->len_ue_slice, sizeof(ue_slice_assoc_t));
    assert(conf->ues != NULL && "memory exhausted");
  }

  for(uint32_t i = 0; i < conf->len_ue_slice; ++i){
    conf->ues[i].rnti = abs(rand()%1024);  
    conf->ues[i].dl_id = abs(rand()%16); 
    conf->ues[i].ul_id = abs(rand()%16); 
  }

}

static
void fill_slice_del(del_slice_conf_t* conf)
{
  assert(conf != NULL);

  uint32_t const len_dl = rand()%5;
  conf->len_dl = len_dl;
  if (conf->len_dl > 0) {
    conf->dl = calloc(len_dl, sizeof(uint32_t));
    assert(conf->dl != NULL && "memory exhausted");
  }
  for (uint32_t i = 0; i < conf->len_dl; ++i)
    conf->dl[i] = abs(rand()%16);

  uint32_t const len_ul = rand()%5;
  conf->len_ul = len_ul;
  if (conf->len_ul > 0) {
    conf->ul = calloc(len_ul, sizeof(uint32_t));
    assert(conf->ul != NULL && "memory exhausted");
  }
  for (uint32_t i = 0; i < conf->len_ul; ++i)
    conf->ul[i] = abs(rand()%16);

}

void fill_slice_ind_data(slice_ind_data_t* ind_msg)
{
  assert(ind_msg != NULL);

  srand(time(0));

  fill_slice_conf(&ind_msg->msg.slice_conf);
  fill_ue_slice_conf(&ind_msg->msg.ue_slice_conf);
  ind_msg->msg.tstamp = time_now_us();
}

void fill_slice_ctrl(slice_ctrl_req_data_t* ctrl)
{
   assert(ctrl != NULL);

   uint32_t type = rand()%SLICE_CTRL_SM_V0_END;
   ctrl->msg.type = type;

   if(type == SLICE_CTRL_SM_V0_ADD){
     fill_slice_conf(&ctrl->msg.u.add_mod_slice);
     //printf("SLICE_CTRL_SM_V0_ADD \n");
   } else if (type == SLICE_CTRL_SM_V0_DEL){
     fill_slice_del(&ctrl->msg.u.del_slice);
     //printf("SLICE_CTRL_SM_V0_DEL \n");
   } else if (type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
     fill_ue_slice_conf(&ctrl->msg.u.ue_slice); 
     //printf("SLICE_CTRL_SM_V0_MOD \n");
   } else {
      assert(0!=0 && "Unknown type");
   }
}

