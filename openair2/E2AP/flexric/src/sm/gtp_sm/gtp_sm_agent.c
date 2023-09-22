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


#include "gtp_sm_agent.h"
#include "gtp_sm_id.h"
#include "enc/gtp_enc_generic.h"
#include "dec/gtp_dec_generic.h"
#include "../../util/alg_ds/alg/defer.h"


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  gtp_enc_asn_t enc;
#elif FLATBUFFERS 
  gtp_enc_fb_t enc;
#elif PLAIN
  gtp_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif

} sm_gtp_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
subscribe_timer_t on_subscription_gtp_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_gtp_agent_t* sm = (sm_gtp_agent_t*)sm_agent;
 
  gtp_event_trigger_t ev = gtp_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  subscribe_timer_t timer = {.ms = ev.ms };
  return timer;
}

static
sm_ind_data_t on_indication_gtp_sm_ag(sm_agent_t const* sm_agent, void* act_def)
{
  assert(sm_agent != NULL);
  assert(act_def == NULL && "Action Definition data not needed for this SM");

  sm_gtp_agent_t* sm = (sm_gtp_agent_t*)sm_agent;

  sm_ind_data_t ret = {0};

  // Fill Indication Header
  gtp_ind_hdr_t hdr = {.dummy = 0 };
  byte_array_t ba_hdr = gtp_enc_ind_hdr(&sm->enc, &hdr );
  ret.ind_hdr = ba_hdr.buf;
  ret.len_hdr = ba_hdr.len;

  // Fill Indication Message 
  gtp_ind_data_t gtp = {0};
  sm->base.io.read_ind(&gtp);
  // Liberate the memory if previously allocated by the RAN. It sucks
  defer({ free_gtp_ind_hdr(&gtp.hdr) ;});
  defer({ free_gtp_ind_msg(&gtp.msg) ;});
  defer({ free_gtp_call_proc_id(gtp.proc_id);});

  byte_array_t ba = gtp_enc_ind_msg(&sm->enc, &gtp.msg);
  ret.ind_msg = ba.buf;
  ret.len_msg = ba.len;

  // Fill Call Process ID
  ret.call_process_id = NULL;
  ret.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_gtp_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_gtp_agent_t* sm = (sm_gtp_agent_t*) sm_agent;

  gtp_ctrl_hdr_t hdr = gtp_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  assert(hdr.dummy == 0 && "Only dummy == 0 supported ");

  gtp_ctrl_msg_t msg = gtp_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  assert(msg.action == 42 && "Only action number 42 supported");

  //sm_ag_if_wr_ctrl_t ctrl = {.type = GTP_CTRL_REQ_V0}; 
  gtp_ctrl_req_data_t gtp_ctrl = {0};
  gtp_ctrl.hdr.dummy = 0; 
  gtp_ctrl.msg.action = msg.action;

  sm->base.io.write_ctrl(&gtp_ctrl);

  // Answer from the E2 Node
  sm_ctrl_out_data_t ret = {0};
  ret.ctrl_out = NULL;
  ret.len_out = 0;

  printf("on_control called \n");
  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_gtp_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  //printf("on_e2_setup called \n");
  sm_gtp_agent_t* sm = (sm_gtp_agent_t*)sm_agent;

  // ToDO: Fill RAN Function from the RAN
  sm_e2_setup_data_t setup = {.len_rfd =0, .ran_fun_def = NULL  }; 

  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));

  // RAN Function
  setup.rf.def = cp_str_to_ba(SM_GTP_SHORT_NAME);
  setup.rf.id = SM_GTP_ID;
  setup.rf.rev = SM_GTP_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_GTP_OID);

  return setup;
}

static
 sm_ric_service_update_data_t on_ric_service_update_gtp_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
sm_ric_service_update_data_t dst = {0};
return dst;
}

static
void free_gtp_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_gtp_agent_t* sm = (sm_gtp_agent_t*)sm_agent;
  free(sm);
}

sm_agent_t* make_gtp_sm_agent(sm_io_ag_ran_t io)
{
  sm_gtp_agent_t* sm = calloc(1, sizeof(sm_gtp_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  *(uint16_t*)(&sm->base.ran_func_id) = SM_GTP_ID; 

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[GTP_STATS_V0];
  sm->base.io.read_setup = io.read_setup_tbl[GTP_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[GTP_CTRL_REQ_V0];
  sm->base.io.write_subs = io.write_subs_tbl[GTP_SUBS_V0];

  sm->base.free_sm = free_gtp_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_gtp_sm_ag;

  sm->base.proc.on_subscription = on_subscription_gtp_sm_ag;
  sm->base.proc.on_indication = on_indication_gtp_sm_ag;
  sm->base.proc.on_control = on_control_gtp_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_gtp_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_gtp_sm_ag;
  sm->base.handle = NULL;

  assert(strlen(SM_GTP_STR) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name, SM_GTP_STR, strlen(SM_GTP_STR)); 

  return &sm->base;
}


uint16_t id_gtp_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_gtp_agent_t* sm = (sm_gtp_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}

