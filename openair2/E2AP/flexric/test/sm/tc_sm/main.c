#include "../../rnd/fill_rnd_data_tc.h"
#include "../../../src/sm/tc_sm/tc_sm_agent.h"
#include "../../../src/sm/tc_sm/tc_sm_ric.h"
#include "../../../src/util/alg_ds/alg/defer.h"
#include "../../../src/sm/tc_sm/ie/tc_data_ie.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


// For testing purposes
static
tc_ind_data_t cp;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static
tc_ctrl_req_data_t cp_ctrl;

//
// Functions 


/////
// AGENT
////

static
void read_ind_tc(void* read)
{
  assert(read != NULL);

  tc_ind_data_t* tc = ( tc_ind_data_t*)read;

  fill_tc_ind_data(tc);
  cp.msg = cp_tc_ind_msg(&tc->msg);
}

static 
sm_ag_if_ans_t write_ctrl_tc(void const* data)
{
  assert(data != NULL);

  tc_ctrl_req_data_t const* ctrl = ( tc_ctrl_req_data_t const*)data;

  tc_ctrl_msg_e const t = ctrl->msg.type;

  assert(t == TC_CTRL_SM_V0_CLS || t == TC_CTRL_SM_V0_PLC 
      || t == TC_CTRL_SM_V0_QUEUE || t ==TC_CTRL_SM_V0_SCH 
      || t == TC_CTRL_SM_V0_SHP || t == TC_CTRL_SM_V0_PCR);

  sm_ag_if_ans_t ans = {0};
  ans.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0;
  ans.ctrl_out.type = TC_AGENT_IF_CTRL_ANS_V0; 

  return ans; 
}


/////////////////////////////
// Check Functions
// //////////////////////////

static
void check_eq_ran_function(sm_agent_t const* ag, sm_ric_t const* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
  assert(ag->ran_func_id == ric->ran_func_id);
}

// RIC -> E2
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
 
  char sub[] = "2_ms";
  sm_subs_data_t data = ric->proc.on_subscription(ric, &sub);
  subscribe_timer_t t = ag->proc.on_subscription(ag, &data); 
  assert(t.ms == 2);

  free_sm_subs_data(&data);
}

// E2 -> RIC
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  sm_ind_data_t sm_data = ag->proc.on_indication(ag, NULL);
  defer({ free_sm_ind_data(&sm_data); }); 


  sm_ag_if_rd_ind_t msg = ric->proc.on_indication(ric, &sm_data);

  tc_ind_data_t* data = &msg.tc;
  assert(msg.type == TC_STATS_V0);

  assert(eq_tc_ind_msg(&cp.msg, &data->msg) == true);

  free_tc_ind_hdr(&data->hdr); 
  free_tc_ind_msg(&data->msg); 

  free_tc_ind_hdr(&cp.hdr);
  free_tc_ind_msg(&cp.msg);
}

static
void check_ctrl(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

//  sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
//  wr.ctrl.type = TC_CTRL_REQ_V0; 

  tc_ctrl_req_data_t tc_req_ctrl;
  fill_tc_ctrl(&tc_req_ctrl);

  cp_ctrl.hdr = cp_tc_ctrl_hdr(&tc_req_ctrl.hdr);
  cp_ctrl.msg = cp_tc_ctrl_msg(&tc_req_ctrl.msg);

  sm_ctrl_req_data_t ctrl_req = ric->proc.on_control_req(ric, &tc_req_ctrl);

  sm_ctrl_out_data_t out_data = ag->proc.on_control(ag, &ctrl_req);

  sm_ag_if_ans_ctrl_t ans = ric->proc.on_control_out(ric, &out_data);
  assert(ans.type == TC_AGENT_IF_CTRL_ANS_V0 );

  if(ctrl_req.len_hdr > 0)
    free(ctrl_req.ctrl_hdr);

  if(ctrl_req.len_msg > 0)
    free(ctrl_req.ctrl_msg);

  if(out_data.len_out > 0)
    free(out_data.ctrl_out);

  free_tc_ctrl_out(&ans.tc);

  free_tc_ctrl_hdr(&tc_req_ctrl.hdr); 
  free_tc_ctrl_msg(&tc_req_ctrl.msg); 

  free_tc_ctrl_hdr(&cp_ctrl.hdr);
  free_tc_ctrl_msg(&cp_ctrl.msg);
}


int main()
{
  srand(time(0)); 

  sm_io_ag_ran_t io_ag = {0}; //.read = read_RAN, .write = write_RAN};  
  io_ag.read_ind_tbl[TC_STATS_V0] = read_ind_tc;
  io_ag.write_ctrl_tbl[TC_CTRL_REQ_V0] = write_ctrl_tc;

  sm_agent_t* sm_ag = make_tc_sm_agent(io_ag);

  sm_ric_t* sm_ric = make_tc_sm_ric();

  for(int i =0 ; i < 256*4096; ++i){
    check_eq_ran_function(sm_ag, sm_ric);
    check_subscription(sm_ag, sm_ric);
    check_indication(sm_ag, sm_ric);
    check_ctrl(sm_ag, sm_ric);
  }

  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);

  printf("Traffic Control SM run with success\n");
  return EXIT_SUCCESS;
}

