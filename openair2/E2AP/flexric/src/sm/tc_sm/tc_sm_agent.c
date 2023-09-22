#include "tc_sm_agent.h"
#include "tc_sm_id.h"
#include "enc/tc_enc_generic.h"
#include "dec/tc_dec_generic.h"
#include "../../util/alg_ds/alg/defer.h"


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  tc_enc_asn_t enc;
#elif FLATBUFFERS 
  tc_enc_fb_t enc;
#elif PLAIN
  tc_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif

} sm_tc_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
subscribe_timer_t on_subscription_tc_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_tc_agent_t* sm = (sm_tc_agent_t*)sm_agent;
 
  tc_event_trigger_t ev = tc_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  subscribe_timer_t timer = {.ms = ev.ms };
  return timer;
}

static
sm_ind_data_t on_indication_tc_sm_ag(sm_agent_t const* sm_agent, void* act_def)
{
  assert(sm_agent != NULL);
  assert(act_def == NULL && "Action definition data not needed for this SM");
  sm_tc_agent_t* sm = (sm_tc_agent_t*)sm_agent;

  sm_ind_data_t ret = {0};

  // Fill Indication Header
  tc_ind_hdr_t hdr = {.dummy = 0 };
  byte_array_t ba_hdr = tc_enc_ind_hdr(&sm->enc, &hdr );
  ret.ind_hdr = ba_hdr.buf;
  ret.len_hdr = ba_hdr.len;

  // Fill Indication Message 
//  sm_ag_if_rd_t rd_if = {.type = INDICATION_MSG_AGENT_IF_ANS_V0};
//  rd_if.ind.type = TC_STATS_V0;

  tc_ind_data_t tc = {0};
  sm->base.io.read_ind(&tc);

// Liberate the memory if previously allocated by the RAN. It sucks
//  tc_ind_data_t* ind = &rd_if.ind.tc;
  defer({ free_tc_ind_hdr(&tc.hdr) ;});
  defer({ free_tc_ind_msg(&tc.msg) ;});
  defer({ free_tc_call_proc_id(tc.proc_id);});

  byte_array_t ba = tc_enc_ind_msg(&sm->enc, &tc.msg);
  ret.ind_msg = ba.buf;
  ret.len_msg = ba.len;

  // Fill Call Process ID
  ret.call_process_id = NULL;
  ret.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_tc_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_tc_agent_t* sm = (sm_tc_agent_t*) sm_agent;

  //sm_ag_if_wr_t wr = {.type =CONTROL_SM_AG_IF_WR };
  //wr.ctrl.type = TC_CTRL_REQ_V0;

  tc_ctrl_req_data_t tc_req_ctrl = {0};

  tc_req_ctrl.hdr = tc_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  defer({ free_tc_ctrl_hdr(&tc_req_ctrl.hdr ); });

  tc_req_ctrl.msg = tc_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  defer({ free_tc_ctrl_msg(&tc_req_ctrl.msg); });
   
  sm_ag_if_ans_t ans = sm->base.io.write_ctrl(&tc_req_ctrl);
  defer({free_tc_ctrl_out(&ans.ctrl_out.tc); });

  assert(ans.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
  assert(ans.ctrl_out.type == TC_AGENT_IF_CTRL_ANS_V0);

  byte_array_t ba = tc_enc_ctrl_out(&sm->enc, &ans.ctrl_out.tc);

  sm_ctrl_out_data_t ret = {0};
  ret.len_out = ba.len;
  ret.ctrl_out = ba.buf;

  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_tc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
//  printf("on_e2_setup called \n");
  sm_tc_agent_t* sm = (sm_tc_agent_t*)sm_agent;

  sm_e2_setup_data_t setup = {.len_rfd =0, .ran_fun_def = NULL  }; 

  // ToDo: RAN Function should be filled from the RAN

  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));

 // RAN Function
  setup.rf.def = cp_str_to_ba(SM_TC_SHORT_NAME);
  setup.rf.id = SM_TC_ID;
  setup.rf.rev = SM_TC_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_TC_OID);

  return setup;
}

static
sm_ric_service_update_data_t on_ric_service_update_tc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0};
  return dst;
}

static
void free_tc_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_tc_agent_t* sm = (sm_tc_agent_t*)sm_agent;
  free(sm);
}

sm_agent_t* make_tc_sm_agent(sm_io_ag_ran_t io)
{
  sm_tc_agent_t* sm = calloc(1, sizeof(sm_tc_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  *(uint16_t*)(&sm->base.ran_func_id) = SM_TC_ID; 

//  sm->base.io = io;

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[TC_STATS_V0];
  sm->base.io.read_setup = io.read_setup_tbl[TC_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[TC_CTRL_REQ_V0];
  sm->base.io.write_subs = io.write_subs_tbl[TC_SUBS_V0];

  sm->base.free_sm = free_tc_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_tc_sm_ag;

  sm->base.proc.on_subscription = on_subscription_tc_sm_ag;
  sm->base.proc.on_indication = on_indication_tc_sm_ag;
  sm->base.proc.on_control = on_control_tc_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_tc_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_tc_sm_ag;
  sm->base.handle = NULL;

  *(uint16_t*)(&sm->base.ran_func_id) = SM_TC_ID; 
  assert(strlen(SM_TC_STR) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name, SM_TC_STR, strlen(SM_TC_STR)); 

  return &sm->base;
}

uint16_t id_tc_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_tc_agent_t* sm = (sm_tc_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}

