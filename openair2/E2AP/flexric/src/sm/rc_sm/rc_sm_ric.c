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


#include "rc_sm_ric.h"
#include "rc_sm_id.h"

#include <assert.h>
#include <stdlib.h>

#include "enc/rc_enc_generic.h"
#include "dec/rc_dec_generic.h"

typedef struct{
  sm_ric_t base;

#ifdef ASN
  rc_enc_asn_t enc;
#elif FLATBUFFERS 
  rc_enc_fb_t enc;
#elif PLAIN
  rc_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif
} sm_rc_ric_t;


static
sm_subs_data_t on_subscription_rc_sm_ric(sm_ric_t const* sm_ric, void* cmd)
{
  assert(sm_ric != NULL); 
  assert(cmd != NULL); 
  rc_sub_data_t const* src = cmd; //&subs->rc;
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;  
  sm_subs_data_t dst = {0}; 
 
  const byte_array_t ba = rc_enc_event_trigger(&sm->enc, &src->et); 
  dst.event_trigger = ba.buf;
  dst.len_et = ba.len;

  assert(src->sz_ad > 0 && src->sz_ad == 1 && "Only one action definition supported");
  assert(src->ad != NULL); 

  const byte_array_t ba_ad = rc_enc_action_def(&sm->enc, src->ad); 
  assert(ba_ad.len < 10*1024 && "Really encoding so much data?" );
  dst.action_def = ba_ad.buf;
  dst.len_ad = ba_ad.len;

  return dst;
}

static
sm_ag_if_rd_ind_t on_indication_rc_sm_ric(sm_ric_t const* sm_ric, sm_ind_data_t const* src)
{
  assert(sm_ric != NULL); 
  assert(src != NULL); 
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;  

  sm_ag_if_rd_ind_t dst = {.type = RAN_CTRL_STATS_V1_03}; 

  dst.rc.ind.hdr = rc_dec_ind_hdr(&sm->enc, src->len_hdr, src->ind_hdr);
  dst.rc.ind.msg = rc_dec_ind_msg(&sm->enc, src->len_msg, src->ind_msg);
  assert(src->call_process_id == NULL && "Not implemented");

  return dst;
}

static
sm_ctrl_req_data_t ric_on_control_req_rc_sm_ric(sm_ric_t const* sm_ric, void* ctrl)
{
  assert(sm_ric != NULL); 
  assert(ctrl != NULL); 
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;  

  rc_ctrl_req_data_t const* req = (rc_ctrl_req_data_t const*)ctrl; 
  sm_ctrl_req_data_t dst = {0};
 
  // Header
  byte_array_t ba_hdr = rc_enc_ctrl_hdr(&sm->enc, &req->hdr);
  dst.ctrl_hdr = ba_hdr.buf;
  dst.len_hdr = ba_hdr.len;

  // Message
  byte_array_t ba_msg = rc_enc_ctrl_msg(&sm->enc, &req->msg);
  dst.ctrl_msg = ba_msg.buf;
  dst.len_msg = ba_msg.len;

  return dst;
}

static
sm_ag_if_ans_ctrl_t ric_on_control_out_rc_sm_ric(sm_ric_t const* sm_ric, const sm_ctrl_out_data_t * src)
{
  assert(sm_ric != NULL); 
  assert(src != NULL);
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;  

  sm_ag_if_ans_ctrl_t dst = {.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0}; 
  dst.rc = rc_dec_ctrl_out(&sm->enc, src->len_out, src->ctrl_out);

  return dst;
}

static
sm_ag_if_rd_e2setup_t ric_on_e2_setup_rc_sm_ric(sm_ric_t const* sm_ric, sm_e2_setup_data_t const* src)
{
  assert(sm_ric != NULL); 
  assert(src != NULL); 
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;  

  sm_ag_if_rd_e2setup_t dst = {.type = RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0};  

  dst.rc.ran_func_def = rc_dec_func_def(&sm->enc, src->len_rfd, src->ran_fun_def);

  return dst;
}

static
sm_ag_if_rd_rsu_t on_ric_service_update_rc_sm_ric(sm_ric_t const* sm_ric, sm_ric_service_update_data_t const* src)
{
  assert(sm_ric != NULL); 
  assert(src != NULL); 
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;  
  (void)sm;
  assert(0!=0 && "Not implemented");

  sm_ag_if_rd_rsu_t dst = {0}; 
  return dst;
}

static
void free_rc_sm_ric(sm_ric_t* sm_ric)
{
  assert(sm_ric != NULL);
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;
  free(sm);
}

//
// Allocation SM functions. The memory malloc by the SM is also freed by it.
//

static
void free_subs_data_rc_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_ind_data_rc_sm_ric(void* msg)
{
  assert(msg != NULL);
  
  sm_ag_if_rd_ind_t* rd_ind = (sm_ag_if_rd_ind_t*)msg;
  assert(rd_ind->type == RAN_CTRL_STATS_V1_03);

  rc_ind_data_t* ind = &rd_ind->rc.ind;
  free_e2sm_rc_ind_hdr(&ind->hdr); 
  free_e2sm_rc_ind_msg(&ind->msg); 
}

static
void free_ctrl_req_data_rc_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_ctrl_out_data_rc_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_e2_setup_rc_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_ric_service_update_rc_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}



sm_ric_t* make_rc_sm_ric(void /* sm_io_ric_t io */)
{
  sm_rc_ric_t* sm = calloc(1,sizeof(sm_rc_ric_t));
  assert(sm != NULL && "Memory exhausted");

  *((uint16_t*)&sm->base.ran_func_id) = SM_RC_ID; 

  sm->base.free_sm = free_rc_sm_ric;

  // Memory (De)Allocation
  sm->base.alloc.free_subs_data_msg = free_subs_data_rc_sm_ric; 
  sm->base.alloc.free_ind_data = free_ind_data_rc_sm_ric ; 
  sm->base.alloc.free_ctrl_req_data = free_ctrl_req_data_rc_sm_ric; 
  sm->base.alloc.free_ctrl_out_data = free_ctrl_out_data_rc_sm_ric; 
 
  sm->base.alloc.free_e2_setup = free_e2_setup_rc_sm_ric; 
  sm->base.alloc.free_ric_service_update = free_ric_service_update_rc_sm_ric; 

  // O-RAN E2SM 5 Procedures
  sm->base.proc.on_subscription = on_subscription_rc_sm_ric; 
  sm->base.proc.on_indication = on_indication_rc_sm_ric;

  sm->base.proc.on_control_req = ric_on_control_req_rc_sm_ric;
  sm->base.proc.on_control_out = ric_on_control_out_rc_sm_ric;

  sm->base.proc.on_e2_setup = ric_on_e2_setup_rc_sm_ric;
  sm->base.proc.on_ric_service_update = on_ric_service_update_rc_sm_ric; 
  sm->base.handle = NULL;

  assert(strlen(SM_RAN_CTRL_SHORT_NAME) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name,SM_RAN_CTRL_SHORT_NAME, strlen( SM_RAN_CTRL_SHORT_NAME)); 

  return &sm->base;
}

uint16_t id_sm_rc_ric(sm_ric_t const* sm_ric)
{
  assert(sm_ric != NULL);
  sm_rc_ric_t* sm = (sm_rc_ric_t*)sm_ric;
  return sm->base.ran_func_id;
}

