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


#include "pdcp_sm_agent.h"
#include "pdcp_sm_id.h"
#include "enc/pdcp_enc_generic.h"
#include "dec/pdcp_dec_generic.h"
#include "../../util/alg_ds/alg/defer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  pdcp_enc_asn_t enc;
#elif FLATBUFFERS 
  pdcp_enc_fb_t enc;
#elif PLAIN
  pdcp_enc_plain_t enc;
#else
  static_assert(false, "No encryptioin type selected");
#endif

} sm_pdcp_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
subscribe_timer_t on_subscription_pdcp_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_pdcp_agent_t* sm = (sm_pdcp_agent_t*)sm_agent;
 
  pdcp_event_trigger_t ev = pdcp_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  subscribe_timer_t timer = {.ms = ev.ms };
  return timer;
}


static
sm_ind_data_t on_indication_pdcp_sm_ag(sm_agent_t const* sm_agent, void* act_def)
{
  //printf("on_indication called \n");
  assert(sm_agent != NULL);
  assert(act_def == NULL && "Subscription data not needed for this SM");

  sm_pdcp_agent_t* sm = (sm_pdcp_agent_t*)sm_agent;

  sm_ind_data_t ret = {0};

  // Fill Indication Header
  pdcp_ind_hdr_t hdr = {.dummy = 0 };
  byte_array_t ba_hdr = pdcp_enc_ind_hdr(&sm->enc, &hdr );
  ret.ind_hdr = ba_hdr.buf;
  ret.len_hdr = ba_hdr.len;

  // Fill Indication Message 
  //sm_ag_if_rd_t rd_if = {.type = INDICATION_MSG_AGENT_IF_ANS_V0};
  //rd_if.ind.type = PDCP_STATS_V0;

  pdcp_ind_data_t pdcp = {0};
  sm->base.io.read_ind(&pdcp);

  // Liberate the memory if previously allocated by the RAN. It sucks
  //pdcp_ind_data_t* ind = &rd_if.ind.pdcp;
  defer({ free_pdcp_ind_hdr(&pdcp.hdr) ;});
  defer({ free_pdcp_ind_msg(&pdcp.msg) ;});
  defer({ free_pdcp_call_proc_id(pdcp.proc_id);});

  byte_array_t ba = pdcp_enc_ind_msg(&sm->enc, &pdcp.msg);
  ret.ind_msg = ba.buf;
  ret.len_msg = ba.len;

  // Fill Call Process ID
  ret.call_process_id = NULL;
  ret.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_pdcp_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_pdcp_agent_t* sm = (sm_pdcp_agent_t*) sm_agent;

  pdcp_ctrl_req_data_t pdcp_req_ctrl = {0};

  pdcp_req_ctrl.hdr = pdcp_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  defer({ free_pdcp_ctrl_hdr(&pdcp_req_ctrl.hdr); });
  assert(pdcp_req_ctrl.hdr.dummy == 0 && "Only dummy == 0 supported ");

  pdcp_req_ctrl.msg = pdcp_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  defer({ free_pdcp_ctrl_msg(&pdcp_req_ctrl.msg); });
  assert(pdcp_req_ctrl.msg.action == 42 && "Only action number 42 supported");

//  sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
//  wr.ctrl.type = PDCP_CTRL_REQ_V0; 
//  wr.ctrl.pdcp_req_ctrl.msg = cp_pdcp_ctrl_msg(&msg);

//  pdcp_ctrl_req_data_t pdcp_req_ctrl = {0};
//  pdcp_req_ctrl.msg = cp_pdcp_ctrl_msg(&msg); 

  // Call the RAN
  sm_ag_if_ans_t ans = sm->base.io.write_ctrl(&pdcp_req_ctrl);
  assert(ans.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
  assert(ans.ctrl_out.type == PDCP_AGENT_IF_CTRL_ANS_V0);

  defer({ free_pdcp_ctrl_out(&ans.ctrl_out.pdcp); });

  // Encode the answer from the RAN
  byte_array_t ba = pdcp_enc_ctrl_out(&sm->enc, &ans.ctrl_out.pdcp);

  sm_ctrl_out_data_t ret = {0}; 
  ret.len_out = ba.len;
  ret.ctrl_out = ba.buf;

//  printf("on_control called \n");
  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_pdcp_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  //printf("on_e2_setup called \n");
  sm_pdcp_agent_t* sm = (sm_pdcp_agent_t*)sm_agent;

  sm_e2_setup_data_t setup = {.len_rfd =0, .ran_fun_def = NULL  }; 

  // ToDo: Missing a call to the RAN to fill this data

  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));

  // RAN Function
  setup.rf.def = cp_str_to_ba(SM_PDCP_SHORT_NAME);
  setup.rf.id = SM_PDCP_ID;
  setup.rf.rev = SM_PDCP_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_PDCP_OID);

  return setup;
}

static
sm_ric_service_update_data_t on_ric_service_update_pdcp_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");

  sm_ric_service_update_data_t dst = {0};
  return dst;
}

static
void free_pdcp_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_pdcp_agent_t* sm = (sm_pdcp_agent_t*)sm_agent;
  free(sm);
}


sm_agent_t* make_pdcp_sm_agent(sm_io_ag_ran_t io)
{
  sm_pdcp_agent_t* sm = calloc(1, sizeof(*sm));
  assert(sm != NULL && "Memory exhausted!!!");

  *(uint16_t*)(&sm->base.ran_func_id) = SM_PDCP_ID; 

  //sm->base.io = io;

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[PDCP_STATS_V0];
  sm->base.io.read_setup = io.read_setup_tbl[PDCP_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[PDCP_CTRL_REQ_V0];
  sm->base.io.write_subs = io.write_subs_tbl[PDCP_SUBS_V0];

  sm->base.free_sm = free_pdcp_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_pdcp_sm_ag;

  // O-RAN E2SM 5 Procedures
  sm->base.proc.on_subscription = on_subscription_pdcp_sm_ag;
  sm->base.proc.on_indication = on_indication_pdcp_sm_ag;
  sm->base.proc.on_control = on_control_pdcp_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_pdcp_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_pdcp_sm_ag;

  assert(strlen(SM_PDCP_STR) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name, SM_PDCP_STR, strlen(SM_PDCP_STR)); 

  return &sm->base;
}

