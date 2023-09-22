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


#include "../../rnd/fill_rnd_data_rc.h"
#include "../../../src/util/alg_ds/alg/defer.h"
#include "../../../src/sm/rc_sm/rc_sm_agent.h"
#include "../../../src/sm/rc_sm/rc_sm_ric.h"
#include "../../../src/sm/rc_sm/ie/rc_data_ie.h"
#include "../../../src/sm/rc_sm/rc_sm_id.h"


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
rc_ind_data_t cp_ind;

static
rc_e2_setup_t cp_e2_setup; 

static
void read_ind_rc(void* read)
{
  assert(read != NULL);

  rc_rd_ind_data_t* rc = (rc_rd_ind_data_t*)read;

  rc->ind = fill_rnd_rc_ind_data();
  cp_ind = cp_rc_ind_data(&rc->ind);

  assert(eq_rc_ind_data(&rc->ind, &cp_ind) == true);
}

static
void read_setup_rc(void* read)
{
  assert(read != NULL);

  rc_e2_setup_t* rc = (rc_e2_setup_t*) read;
  rc->ran_func_def = fill_rc_ran_func_def();
  cp_e2_setup.ran_func_def = cp_e2sm_rc_func_def(&rc->ran_func_def);
  assert(eq_e2sm_rc_func_def(&cp_e2_setup.ran_func_def, &rc->ran_func_def) == true);
}

// For testing purposes
static
rc_sub_data_t cp_rc_sub; 

static
rc_ctrl_req_data_t cp_rc_ctrl; 

static
e2sm_rc_ctrl_out_t cp_rc_ctrl_out; 


static 
sm_ag_if_ans_t write_ctrl_rc(void const* data)
{
  assert(data != NULL); 

  rc_ctrl_req_data_t const* ctrl = (rc_ctrl_req_data_t const*)data;

  // RAN Input
  cp_rc_ctrl = cp_rc_ctrl_req_data(ctrl);

  // RAN Output
  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0;
  ans.ctrl_out.rc = fill_rc_ctrl_out();
  cp_rc_ctrl_out = cp_e2sm_rc_ctrl_out( &ans.ctrl_out.rc );

  return ans;
}

static 
sm_ag_if_ans_t write_subs_rc(void const* data)
{
  assert(data != NULL); 

  wr_rc_sub_data_t const* wr_rc = (wr_rc_sub_data_t const*)data;

  cp_rc_sub = cp_rc_sub_data(&wr_rc->rc);

  sm_ag_if_ans_t ans = {.type = NONE_SM_AG_IF_ANS_V0};
  return ans;
}

/*
static 
sm_ag_if_ans_t write_RAN(sm_ag_if_wr_t const* data)
{
  assert(data != NULL); 

  sm_ag_if_ans_t ans = {.type = NONE_SM_AG_IF_ANS_V0};

  if(data->type == SUBSCRIPTION_SM_AG_IF_WR){
    assert(data->subs.type == RAN_CTRL_SUBS_V1_03); 
    cp_rc_sub = cp_rc_sub_data(&data->subs.wr_rc.rc);

  } else if(data->type == CONTROL_SM_AG_IF_WR){
    assert(data->ctrl.type == RAN_CONTROL_CTRL_V1_03);
    
    // RAN Input
    cp_rc_ctrl = cp_rc_ctrl_req_data(&data->ctrl.rc_ctrl);

    // RAN Output
    ans.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0; 
    ans.ctrl_out.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0;
    ans.ctrl_out.rc = fill_rc_ctrl_out();
    cp_rc_ctrl_out = cp_e2sm_rc_ctrl_out( &ans.ctrl_out.rc );

  } else {
    assert(0!=0 && "Unknown type");
  }

  return ans;
}
*/


/////////////////////////////
// Check Functions
// //////////////////////////

/*
static
void check_eq_ran_function(sm_agent_t const* ag, sm_ric_t const* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);


  assert(ag->ran_func_id == ric->ran_func_id);
}

*/

// RIC -> E2
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

//  sm_ag_if_wr_subs_t sub = {.type = RAN_CTRL_SUBS_V1_03};
  rc_sub_data_t rc = fill_rnd_rc_subscription();
  defer({ free_rc_sub_data(&rc); });

  sm_subs_data_t data = ric->proc.on_subscription(ric, &rc);
  defer({ free_sm_subs_data(&data); });

  subscribe_timer_t t = ag->proc.on_subscription(ag, &data); 
  defer({  free_rc_sub_data(&cp_rc_sub); });
  assert(t.ms == 0);

  assert(eq_rc_sub_data(&rc, &cp_rc_sub) == true);
}

// E2 -> RIC
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  rc_ind_data_t* d = calloc(1, sizeof(rc_ind_data_t)); 
  assert(d != NULL && "Memory exhausted");
  *d = fill_rnd_rc_ind_data();
  cp_ind = cp_rc_ind_data(d);

  sm_ind_data_t sm_data = ag->proc.on_indication(ag, d);
  defer({ free_sm_ind_data(&sm_data); }); 
  defer({ free_rc_ind_data(&cp_ind); });

  sm_ag_if_rd_ind_t msg = ric->proc.on_indication(ric, &sm_data);
  assert(msg.type == RAN_CTRL_STATS_V1_03);
  defer({ free_rc_ind_data(&msg.rc.ind); });

  rc_ind_data_t const* data = &msg.rc.ind;

  assert(eq_rc_ind_data(&cp_ind, data) == true);
}


static
void check_ctrl(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  rc_ctrl_req_data_t rc_ctrl = fill_rc_ctrl();
  defer({ free_rc_ctrl_req_data(&rc_ctrl); });

  sm_ctrl_req_data_t ctrl_req = ric->proc.on_control_req(ric, &rc_ctrl);
  defer({ free_sm_ctrl_req_data(&ctrl_req); });

  sm_ctrl_out_data_t out_data = ag->proc.on_control(ag, &ctrl_req);
  defer({ free_sm_ctrl_out_data(&out_data); });
  defer({ free_rc_ctrl_req_data(&cp_rc_ctrl); });

  assert(eq_rc_ctrl_req_data(&rc_ctrl, &cp_rc_ctrl));

  sm_ag_if_ans_ctrl_t ans = ric->proc.on_control_out(ric, &out_data);
  assert(ans.type == RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0);
  defer({ free_e2sm_rc_ctrl_out(&ans.rc); });
  defer({ free_e2sm_rc_ctrl_out(&cp_rc_ctrl_out); });

  assert(eq_e2sm_rc_ctrl_out(&ans.rc, &cp_rc_ctrl_out));
}

void check_e2_setup(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  sm_e2_setup_data_t data = ag->proc.on_e2_setup(ag);
  defer({ free_sm_e2_setup(&data); });

  sm_ag_if_rd_e2setup_t out = ric->proc.on_e2_setup(ric, &data);
  assert(out.type == RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0);
  defer({ free_e2sm_rc_func_def(&out.rc.ran_func_def); });

  assert(eq_e2sm_rc_func_def(&out.rc.ran_func_def, &cp_e2_setup.ran_func_def) == true);
  free_e2sm_rc_func_def(&cp_e2_setup.ran_func_def);
}

int main()
{
  srand(time(0)); 

  sm_io_ag_ran_t io_ag = {0}; //.read = read_RAN, .write = write_RAN};  
  // Read 
  io_ag.read_ind_tbl[RAN_CTRL_STATS_V1_03] = read_ind_rc; 
  io_ag.read_setup_tbl[RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0] = read_setup_rc; 

  // Write
  io_ag.write_ctrl_tbl[RAN_CONTROL_CTRL_V1_03] =  write_ctrl_rc;
  io_ag.write_subs_tbl[RAN_CTRL_SUBS_V1_03] =  write_subs_rc;

  sm_agent_t* sm_ag = make_rc_sm_agent(io_ag);
  sm_ric_t* sm_ric = make_rc_sm_ric();

  printf("Running RAN Control SM test. Patience. \n");
  for(int i =0 ; i < 10*1024; ++i){
    // check_eq_ran_function(sm_ag, sm_ric);
    check_indication(sm_ag, sm_ric);
    check_subscription(sm_ag, sm_ric);
    check_ctrl(sm_ag, sm_ric);
    check_e2_setup(sm_ag, sm_ric);
    // check_ric_service_update(sm_ag, sm_ric);
  }

  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);

  printf("RAN Control (RC-SM) version 1.3 Release 3 run with success\n");
  return EXIT_SUCCESS;
}

