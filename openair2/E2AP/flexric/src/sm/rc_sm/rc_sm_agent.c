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

#include "rc_sm_agent.h"

#include "../../util/alg_ds/alg/defer.h"
#include "rc_sm_id.h"
#include "enc/rc_enc_generic.h"
#include "dec/rc_dec_generic.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  rc_enc_asn_t enc;
#elif FLATBUFFERS 
  rc_enc_fb_t enc;
#elif PLAIN
  rc_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif

} sm_rc_agent_t;

/*
static
byte_array_t cp_str_to_ba(const char* str)
{
  assert(str != NULL);
  
  const size_t sz = strlen(str);

  byte_array_t dst = {.len = sz};

  dst.buf = calloc(sz,sizeof(uint8_t));
  assert(dst.buf != NULL && "Memory exhausted");

  memcpy(dst.buf, str, sz);

  return dst;
}
static
ran_function_name_t fill_rc_ran_func_name(void)
{
  ran_function_name_t dst = {0}; 

    // RAN Function Short Name
    // Mandatory
    // PrintableString [1-150]
    dst.name = cp_str_to_ba(SM_RAN_CTRL_SHORT_NAME);

    // RAN Function Service Model OID
    // Mandatory
    // PrintableString [1-1000]
    
    //iso(1) identified-organization(3)
    //dod(6) internet(1) private(4)
    //enterprise(1) 53148 e2(1)
    // version1 (1) e2sm(2) e2sm-RC-
    // IEs (3)
    dst.oid = cp_str_to_ba(SM_RAN_CTRL_OID);

    // RAN Function Description
    // Mandatory
    // PrintableString [1- 150]
    //RAN function RC “RAN Control” performs the following
    //functionalities:
    //- Exposure of RAN control and UE context related
    //information.
    //- Modification and initiation of RAN control related call
    //processes and messages
    //- Execution of policies that may result in change of
    //RAN control behavior 

    dst.description = cp_str_to_ba( SM_RAN_CTRL_DESCRIPTION);

    // RAN Function Instance
    // Optional
    // INTEGER
//    long* instance;	// OPTIONAL: it is suggested to be used when E2 Node declares
//                                multiple RAN Function ID supporting the same  E2SM specification

  return dst;
}

static
e2sm_rc_func_def_t fill_rc_ran_func_def(sm_rc_agent_t const* sm)
{
  assert(sm != NULL);

  // Call the RAN and fill the data  
  sm_ag_if_rd_t rd = {.type = E2_SETUP_AGENT_IF_ANS_V0};
  rd.e2ap.type = RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0;
  sm->base.io.read(&rd);

  return rd.e2ap.rc.func_def;
}
*/

// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
subscribe_timer_t on_subscription_rc_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;
  assert(data != NULL);

  //sm_ag_if_wr_t wr = {.type = SUBSCRIPTION_SM_AG_IF_WR};
  //wr.subs.type = RAN_CTRL_SUBS_V1_03;
  //wr.subs.wr_rc.ric_req_id = data->ric_req_id; 

  wr_rc_sub_data_t wr_rc = {0};
  wr_rc.ric_req_id = data->ric_req_id; 

  wr_rc.rc.et = rc_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);
  defer({ free_e2sm_rc_event_trigger(&wr_rc.rc.et); });

  // Only 1 supported
  wr_rc.rc.sz_ad = 1;
  wr_rc.rc.ad = calloc(wr_rc.rc.sz_ad, sizeof(e2sm_rc_action_def_t));
  assert(wr_rc.rc.ad != NULL && "Memory exhausted");
  defer({ free_e2sm_rc_action_def(wr_rc.rc.ad); free(wr_rc.rc.ad); });

  wr_rc.rc.ad[0] = rc_dec_action_def(&sm->enc, data->len_ad, data->action_def);

  sm->base.io.write_subs(&wr_rc);

  subscribe_timer_t timer = {.ms = 0};

  return timer;
}

static
sm_ind_data_t on_indication_rc_sm_ag(sm_agent_t const* sm_agent, void* ind_data)
{
//  printf("on_indication RC called \n");
  assert(sm_agent != NULL);
  assert(ind_data != NULL && "Indication data needed for this SM");
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;

  sm_ind_data_t ret = {0};

  // Liberate the memory if previously allocated by the RAN. It sucks
  rc_ind_data_t* ind = (rc_ind_data_t*)ind_data; 
  defer({ free_rc_ind_data(ind); free(ind); });

  // Fill Indication Header
  byte_array_t ba_hdr = rc_enc_ind_hdr(&sm->enc, &ind->hdr);
  assert(ba_hdr.len < 1024 && "Are you really encoding so much info?" );
  ret.ind_hdr = ba_hdr.buf;
  ret.len_hdr = ba_hdr.len;

  // Fill Indication Message
  byte_array_t ba_msg = rc_enc_ind_msg(&sm->enc, &ind->msg);
  assert(ba_msg.len < 10*1024 && "Are you really encoding so much info?" );
  ret.ind_msg = ba_msg.buf;
  ret.len_msg = ba_msg.len;

  // Fill Call Process ID
  assert(ind->proc_id == NULL && "Not implemented" );

  return ret;
}

static
sm_ctrl_out_data_t on_control_rc_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;

  //sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
  //wr.ctrl.type = RAN_CONTROL_CTRL_V1_03;

//  rc_ctrl_req_data_t* rc_ctrl = &wr.ctrl.rc_ctrl;

  rc_ctrl_req_data_t rc_ctrl = {0};
  rc_ctrl.hdr = rc_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  rc_ctrl.msg = rc_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  defer({ free_rc_ctrl_req_data(&rc_ctrl); });

  sm_ag_if_ans_t ret = sm->base.io.write_ctrl(&rc_ctrl);
  assert(ret.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
  assert(ret.ctrl_out.type == RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0);
  defer({ free_e2sm_rc_ctrl_out(&ret.ctrl_out.rc); });

  // Answer from the E2 Node
  byte_array_t ba = rc_enc_ctrl_out(&sm->enc, &ret.ctrl_out.rc);
  sm_ctrl_out_data_t ans = {.ctrl_out = ba.buf, .len_out = ba.len};

  //printf("on_control called \n");
  return ans;
}

static
sm_e2_setup_data_t on_e2_setup_rc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  //printf("on_e2_setup called \n");
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;

  // Call the RAN and fill the data  
//  sm_ag_if_rd_t rd = {.type = E2_SETUP_AGENT_IF_ANS_V0};
//  rd.e2ap.type = RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0;

  rc_e2_setup_t rc = {0};
  sm->base.io.read_setup(&rc);

  e2sm_rc_func_def_t* ran_func = &rc.ran_func_def; 
  defer({ free_e2sm_rc_func_def(ran_func); });

  byte_array_t ba = rc_enc_func_def(&sm->enc, ran_func);

  sm_e2_setup_data_t setup = {0}; 
  setup.len_rfd = ba.len;
  setup.ran_fun_def = ba.buf;

  // RAN Function
  setup.rf.def = cp_str_to_ba(SM_RAN_CTRL_SHORT_NAME);
  setup.rf.id = SM_RC_ID;
  setup.rf.rev = SM_RC_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_RAN_CTRL_OID);

  return setup;
}

static
sm_ric_service_update_data_t on_ric_service_update_rc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0};
  return dst;
}

static
void free_rc_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;
  free(sm);
}

sm_agent_t* make_rc_sm_agent(sm_io_ag_ran_t io)
{
  sm_rc_agent_t* sm = calloc(1, sizeof(sm_rc_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  *(uint16_t*)(&sm->base.ran_func_id) = SM_RC_ID; 

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[RAN_CTRL_STATS_V1_03];
  sm->base.io.read_setup = io.read_setup_tbl[RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[RAN_CONTROL_CTRL_V1_03];
  sm->base.io.write_subs = io.write_subs_tbl[RAN_CTRL_SUBS_V1_03];

  sm->base.free_sm = free_rc_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_rc_sm_ag;

  sm->base.proc.on_subscription = on_subscription_rc_sm_ag;
  sm->base.proc.on_indication = on_indication_rc_sm_ag;
  sm->base.proc.on_control = on_control_rc_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_rc_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_rc_sm_ag;
  sm->base.handle = NULL;

  assert(strlen(SM_RAN_CTRL_SHORT_NAME) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name, SM_RAN_CTRL_SHORT_NAME, strlen(SM_RAN_CTRL_SHORT_NAME)); 

  return &sm->base;
}

uint16_t id_rc_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}

