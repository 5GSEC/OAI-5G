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

#include "mac_sm_agent.h"

#include "../../util/alg_ds/alg/defer.h"
#include "dec/mac_dec_generic.h"
#include "mac_sm_id.h"
#include "enc/mac_enc_generic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  mac_enc_asn_t enc;
#elif FLATBUFFERS 
  mac_enc_fb_t enc;
#elif PLAIN
  mac_enc_plain_t enc;
#else
  static_assert(false, "No encryptioin type selected");
#endif

} sm_mac_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
subscribe_timer_t on_subscription_mac_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_mac_agent_t* sm = (sm_mac_agent_t*)sm_agent;
 
  mac_event_trigger_t ev = mac_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  subscribe_timer_t timer = {.ms = ev.ms };
  return timer;
//  const sm_wr_if_t wr = {.type = SUBSCRIBE_TIMER, .sub_timer = timer };
//  sm->base.io.write(&wr);
//  printf("on_subscription called with event trigger = %u \n", ev.ms);
}

static
sm_ind_data_t on_indication_mac_sm_ag(sm_agent_t const* sm_agent, void* act_def)
{
  //printf("on_indication called \n");
  assert(sm_agent != NULL);
  assert(act_def == NULL && "Action definition data not needed for this SM");
  sm_mac_agent_t* sm = (sm_mac_agent_t*)sm_agent;

  sm_ind_data_t ret = {0};

  // Fill Indication Header
  mac_ind_hdr_t hdr = {.dummy = 0 };
  byte_array_t ba_hdr = mac_enc_ind_hdr(&sm->enc, &hdr);
  ret.ind_hdr = ba_hdr.buf;
  ret.len_hdr = ba_hdr.len;

  // Fill Indication Message 
  //sm_ag_if_rd_t rd_if = {.type = INDICATION_MSG_AGENT_IF_ANS_V0};
  //rd_if.ind.type = MAC_STATS_V0;

  mac_ind_data_t mac = {0};
  // This may allocate memory by the RAN
  sm->base.io.read_ind(&mac);
  // Liberate the memory if previously allocated by the RAN. It sucks. Profoundly
  //  defer({ free_sm_rd_if(&rd_if); }; );
  defer({ free_mac_ind_hdr(&mac.hdr) ;});
  defer({ free_mac_ind_msg(&mac.msg) ;});
  defer({ free_mac_call_proc_id(mac.proc_id);});

  byte_array_t ba = mac_enc_ind_msg(&sm->enc, &mac.msg);
  ret.ind_msg = ba.buf;
  ret.len_msg = ba.len;

  // Fill the optional Call Process ID
  ret.call_process_id = NULL;
  ret.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_mac_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_mac_agent_t* sm = (sm_mac_agent_t*) sm_agent;

  mac_ctrl_hdr_t hdr = mac_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  assert(hdr.dummy == 0 && "Only dummy == 0 supported ");

  mac_ctrl_msg_t msg = mac_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  assert(msg.action == 42 && "Only action number 42 supported");

//  sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
//  wr.ctrl.type = MAC_CTRL_REQ_V0; 

  mac_ctrl_req_data_t mac_ctrl = {0};
  mac_ctrl.hdr.dummy = 0;
  mac_ctrl.msg.action = msg.action;

  sm->base.io.write_ctrl(&mac_ctrl);
  sm_ctrl_out_data_t ret = {0};
  ret.len_out = 0;
  ret.ctrl_out = NULL;

  //printf("on_control called \n");
  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_mac_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
//  printf("[E2SM MAC] on_e2_setup called \n");

  // Fill E2 Setup Request 
  // sm_ag_if_rd_t rd_if = {.type = E2_SETUP__AGENT_IF_ANS_V0};
  // rd_if.e2ap.type = MAC_AGENT_IF_E2_SETUP_ANS_V0;

  // This may allocate memory by the RAN
  // sm->base.io.read(&rd_if);
  // Liberate the memory if previously allocated by the RAN. It sucks. Profoundly
  //mac_e2_setup_data_t* setup = &rd_if.e2ap.mac;

  sm_mac_agent_t* sm = (sm_mac_agent_t*)sm_agent;

  sm_e2_setup_data_t setup = {.len_rfd = 0, .ran_fun_def = NULL }; 

  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));

  // RAN Function
  setup.rf.def = cp_str_to_ba(SM_MAC_SHORT_NAME);
  setup.rf.id = SM_MAC_ID;
  setup.rf.rev = SM_MAC_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_MAC_OID);

  return setup;
}

static
 sm_ric_service_update_data_t on_ric_service_update_mac_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0}; 
  return dst;
}

static
void free_mac_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_mac_agent_t* sm = (sm_mac_agent_t*)sm_agent;
  free(sm);
}

sm_agent_t* make_mac_sm_agent(sm_io_ag_ran_t io)
{
  sm_mac_agent_t* sm = calloc(1, sizeof(sm_mac_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[MAC_STATS_V0];
  sm->base.io.read_setup = io.read_setup_tbl[MAC_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[MAC_CTRL_REQ_V0];
  sm->base.io.write_subs = io.write_subs_tbl[MAC_SUBS_V0];

  sm->base.free_sm = free_mac_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_mac_sm_ag;

  sm->base.proc.on_subscription = on_subscription_mac_sm_ag;
  sm->base.proc.on_indication = on_indication_mac_sm_ag;
  sm->base.proc.on_control = on_control_mac_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_mac_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_mac_sm_ag;
  sm->base.handle = NULL;

  *(uint16_t*)(&sm->base.ran_func_id) = SM_MAC_ID; 
  assert(strlen( SM_MAC_STR ) < sizeof(sm->base.ran_func_name));
  memcpy(sm->base.ran_func_name, SM_MAC_STR, strlen(SM_MAC_STR));

  return &sm->base;
}

uint16_t id_mac_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_mac_agent_t* sm = (sm_mac_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}

