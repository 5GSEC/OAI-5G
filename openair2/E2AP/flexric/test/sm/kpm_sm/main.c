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


#include "../../rnd/fill_rnd_data_kpm.h"
#include "../../../src/util/alg_ds/alg/defer.h"
#include "../../../src/sm/kpm_sm_v03.00/kpm_sm_agent.h"
#include "../../../src/sm/kpm_sm_v03.00/kpm_sm_ric.h"
#include "../../../src/sm/kpm_sm_v03.00/ie/kpm_data_ie.h"
#include "../../../src/sm/kpm_sm_v03.00/kpm_sm_id.h"
#include "../../rnd/fill_rnd_data_kpm.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

//
// Functions 

/////
// AGENT
////

// For testing purposes
static
kpm_ind_data_t cp_ind;

static
kpm_e2_setup_t cp_e2_setup; 

static
void read_ind_kpm(void* read)
{
  assert(read != NULL);

  kpm_rd_ind_data_t* kpm = (kpm_rd_ind_data_t*)read;

  kpm->ind.hdr = fill_kpm_ind_hdr();
  kpm->ind.msg = fill_kpm_ind_msg(); 
  assert(kpm->act_def!= NULL);
  
  //
  cp_ind.hdr = cp_kpm_ind_hdr(&kpm->ind.hdr);
  cp_ind.msg = cp_kpm_ind_msg(&kpm->ind.msg);
  

  assert(eq_kpm_ind_hdr(&kpm->ind.hdr, &cp_ind.hdr) == true);
  assert(eq_kpm_ind_msg(&kpm->ind.msg, &cp_ind.msg) == true);
}

static
void read_e2_setup_kpm(void* data)
{
  assert(data != NULL);

  kpm_e2_setup_t* kpm = (kpm_e2_setup_t*)data;

  kpm->ran_func_def = fill_kpm_ran_func_def(); 
  cp_e2_setup.ran_func_def = cp_kpm_ran_function_def(&kpm->ran_func_def);

  assert(eq_kpm_ran_function_def(&cp_e2_setup.ran_func_def, &kpm->ran_func_def) == true);
}

/////////////////////////////
// Check Functions
// //////////////////////////

// RIC -> E2
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  sm_ag_if_wr_subs_t sub = {.type = KPM_SUBS_V3_0};
  sub.kpm.ev_trg_def = fill_kpm_event_trigger_def();
  defer({ free_kpm_event_trigger_def(&sub.kpm.ev_trg_def); });

  sub.kpm.sz_ad = 1;
  sub.kpm.ad = calloc(sub.kpm.sz_ad, sizeof(kpm_act_def_t));
  assert(sub.kpm.ad != NULL && "Memory exhausted");
  defer({free(sub.kpm.ad );});
 
  sub.kpm.ad[0] = fill_kpm_action_def();
  defer({ free_kpm_action_def(&sub.kpm.ad[0]) ;});

  sm_subs_data_t data = ric->proc.on_subscription(ric, &sub.kpm);
  defer({ free_sm_subs_data(&data); });

  subscribe_timer_t t = ag->proc.on_subscription(ag, &data); 
  defer({ free_kpm_action_def(t.act_def); free(t.act_def); });
  assert(t.ms != -1 && t.ms == sub.kpm.ev_trg_def.kpm_ric_event_trigger_format_1.report_period_ms);
  assert(eq_kpm_action_def(&sub.kpm.ad[0], t.act_def) == true);
}

// E2 -> RIC
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
/*  
  sm_ag_if_wr_subs_t sub = {.type = KPM_SUBS_V3_0 }; 
  defer({ free_kpm_sub_data(&sub.kpm); });

  sub.kpm.ev_trg_def = fill_kpm_event_trigger_def();
  sub.kpm.sz_ad = 1;
  sub.kpm.ad = calloc(sub.kpm.sz_ad, sizeof(kpm_act_def_t));
  assert(sub.kpm.ad != NULL && "Memory exhausted");
  sub.kpm.ad[0] = 
*/
  
  kpm_act_def_t act_def = fill_kpm_action_def();
  defer({  free_kpm_action_def(&act_def); } );
  
  sm_ind_data_t sm_data = ag->proc.on_indication(ag, &act_def);
  defer({ free_sm_ind_data(&sm_data); }); 
  defer({ free_kpm_ind_data(&cp_ind); });

  sm_ag_if_rd_ind_t msg = ric->proc.on_indication(ric, &sm_data);
  assert(msg.type == KPM_STATS_V3_0);
  defer({ free_kpm_ind_data(&msg.kpm.ind); });

  kpm_ind_data_t const* data = &msg.kpm.ind;

  assert(eq_kpm_ind_data(&cp_ind, data) == true);
}

void check_e2_setup(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  sm_e2_setup_data_t data = ag->proc.on_e2_setup(ag);
  defer({ free_sm_e2_setup(&data); });

  sm_ag_if_rd_e2setup_t out = ric->proc.on_e2_setup(ric, &data);
  assert(out.type == KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0);

  defer({ free_kpm_ran_function_def(&out.kpm.ran_func_def); });

  assert(eq_kpm_ran_function_def(&out.kpm.ran_func_def, &cp_e2_setup.ran_func_def) == true);
}

int main()
{
  srand(time(0)); 

  sm_io_ag_ran_t io_ag = {0};
  io_ag.read_ind_tbl[KPM_STATS_V3_0] = read_ind_kpm;
  io_ag.read_setup_tbl[KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0] = read_e2_setup_kpm;

  sm_agent_t* sm_ag = make_kpm_sm_agent(io_ag);
  sm_ric_t* sm_ric = make_kpm_sm_ric();

  for(int i =0 ; i < 1024; ++i){
 //   check_eq_ran_function(sm_ag, sm_ric);
 //
    check_indication(sm_ag, sm_ric);
    check_subscription(sm_ag, sm_ric);
//    check_ctrl(sm_ag, sm_ric);
    check_e2_setup(sm_ag, sm_ric);

// check_ric_service_update(sm_ag, sm_ric);

  }

  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);

  printf("Key Performance Metric (KPM-SM) version 3.0 Release 3 run with success\n");
  return EXIT_SUCCESS;
}

