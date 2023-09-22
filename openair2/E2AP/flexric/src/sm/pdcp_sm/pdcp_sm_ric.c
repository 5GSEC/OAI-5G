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


#include "pdcp_sm_ric.h"
#include "pdcp_sm_id.h"

#include <assert.h>
#include <stdlib.h>
#include "../../util/alg_ds/alg/defer.h"

#include "enc/pdcp_enc_generic.h"
#include "dec/pdcp_dec_generic.h"

typedef struct{
  sm_ric_t base;

#ifdef ASN
  pdcp_enc_asn_t enc;
#elif FLATBUFFERS 
  pdcp_enc_fb_t enc;
#elif PLAIN
  pdcp_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif
} sm_pdcp_ric_t;


static
sm_subs_data_t on_subscription_pdcp_sm_ric(sm_ric_t const* sm_ric, void* cmd)
{
  assert(sm_ric != NULL); 
  sm_pdcp_ric_t* sm = (sm_pdcp_ric_t*)sm_ric;  

  pdcp_sub_data_t pdcp = {0}; 

  const int max_str_sz = 10;
  if(strncmp(cmd, "1_ms", max_str_sz) == 0 ){
  pdcp.et.ms = 1;
  } else if (strncmp(cmd, "2_ms", max_str_sz) == 0 ) {
   pdcp.et.ms = 2;
  } else if (strncmp(cmd, "5_ms", max_str_sz) == 0 ) {
    pdcp.et.ms = 5;
  } else if (strncmp(cmd, "10_ms", max_str_sz) == 0 ) {
    pdcp.et.ms = 10;
  } else {
    assert(0 != 0 && "Invalid input");
  }
  
  const byte_array_t ba = pdcp_enc_event_trigger(&sm->enc, &pdcp.et); 

  sm_subs_data_t data = {0}; 
  
  // Event trigger IE
  data.event_trigger = ba.buf;
  data.len_et = ba.len;

  // Action Definition IE
  data.action_def = NULL;
  data.len_ad = 0;

  return data;
}

static
sm_ag_if_rd_ind_t on_indication_pdcp_sm_ric(sm_ric_t const* sm_ric, sm_ind_data_t const* data)
{
  assert(sm_ric != NULL); 
  assert(data != NULL); 
  sm_pdcp_ric_t* sm = (sm_pdcp_ric_t*)sm_ric;  

  sm_ag_if_rd_ind_t rd_if = {.type = PDCP_STATS_V0 };

  rd_if.pdcp.hdr = pdcp_dec_ind_hdr(&sm->enc, data->len_hdr, data->ind_hdr);
  rd_if.pdcp.msg = pdcp_dec_ind_msg(&sm->enc, data->len_msg, data->ind_msg);

  return rd_if;
}

static
sm_ctrl_req_data_t on_control_req_pdcp_sm_ric(sm_ric_t const* sm_ric, void* ctrl)
{
  assert(sm_ric != NULL); 
  assert(ctrl != NULL); 

  sm_pdcp_ric_t* sm = (sm_pdcp_ric_t*)sm_ric;  

  pdcp_ctrl_req_data_t const* req =  ( pdcp_ctrl_req_data_t const*)ctrl;

  sm_ctrl_req_data_t ret_data = {0};  

  byte_array_t ba = pdcp_enc_ctrl_hdr(&sm->enc, &req->hdr);
  ret_data.ctrl_hdr = ba.buf;
  ret_data.len_hdr = ba.len;

  ba = pdcp_enc_ctrl_msg(&sm->enc, &req->msg);
  ret_data.ctrl_msg = ba.buf;
  ret_data.len_msg = ba.len;
  return ret_data;
}

static  
sm_ag_if_ans_ctrl_t on_control_out_pdcp_sm_ric(sm_ric_t const* sm_ric, sm_ctrl_out_data_t const* out)
{
  assert(sm_ric != NULL);
  assert(out != NULL);
  sm_pdcp_ric_t* sm = (sm_pdcp_ric_t*)sm_ric;  
  assert(out->len_out > 0);
  assert(out->ctrl_out != NULL);

  sm_ag_if_ans_ctrl_t ans = { 0 }; 
  ans.type = PDCP_AGENT_IF_CTRL_ANS_V0;
  ans.pdcp = pdcp_dec_ctrl_out(&sm->enc, out->len_out, out->ctrl_out);

  return ans;
}

static
sm_ag_if_rd_e2setup_t on_e2_setup_pdcp_sm_ric(sm_ric_t const* sm_ric, sm_e2_setup_data_t const* setup)
{
  assert(sm_ric != NULL); 
  assert(setup == NULL); 
  //  sm_pdcp_ric_t* sm = (sm_pdcp_ric_t*)sm_ric;  

  assert(0!=0 && "Not implemented");
  sm_ag_if_rd_e2setup_t dst = {0}; 
  return dst;
}

static
sm_ag_if_rd_rsu_t on_ric_service_update_pdcp_sm_ric(sm_ric_t const* sm_ric, sm_ric_service_update_data_t const* data)
{
  assert(sm_ric != NULL); 
  assert(data != NULL); 
  //  sm_pdcp_ric_t* sm = (sm_pdcp_ric_t*)sm_ric;  

  assert(0!=0 && "Not implemented");
  sm_ag_if_rd_rsu_t dst = {0}; 
  return dst;
}

static
void free_pdcp_sm_ric(sm_ric_t* sm_ric)
{
  assert(sm_ric != NULL);
  sm_pdcp_ric_t* sm = (sm_pdcp_ric_t*)sm_ric;
  free(sm);
}

//
// Allocation SM functions. The memory malloc by the SM is also freed by it.
//

static
void free_subs_data_pdcp_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_ind_data_pdcp_sm_ric(void* msg)
{
  assert(msg != NULL);

  sm_ag_if_rd_ind_t* rd_ind  = (sm_ag_if_rd_ind_t*)msg;
  assert(rd_ind->type == PDCP_STATS_V0);

  pdcp_ind_data_t* ind = &rd_ind->pdcp; 
  free_pdcp_ind_hdr(&ind->hdr); 
  free_pdcp_ind_msg(&ind->msg); 
}

static
void free_ctrl_req_data_pdcp_sm_ric(void* msg)
{
  assert(msg != NULL);

  sm_ctrl_req_data_t* ctrl = (sm_ctrl_req_data_t*)msg;
  free_sm_ctrl_req_data(ctrl);
}

static
void free_ctrl_out_data_pdcp_sm_ric(void* msg)
{
  assert(msg != NULL);

  sm_ctrl_out_data_t* ctrl = (sm_ctrl_out_data_t*)msg;
  free_sm_ctrl_out_data(ctrl);
}

static
void free_e2_setup_pdcp_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}

static
void free_ric_service_update_pdcp_sm_ric(void* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");
}


sm_ric_t* make_pdcp_sm_ric(void /* sm_io_ric_t io */)
{
  sm_pdcp_ric_t* sm = calloc(1, sizeof(sm_pdcp_ric_t));
  assert(sm != NULL && "Memory exhausted");

  *((uint16_t*)&sm->base.ran_func_id) = SM_PDCP_ID; 

  sm->base.free_sm = free_pdcp_sm_ric;

  // Memory (De)Allocation
  sm->base.alloc.free_subs_data_msg = free_subs_data_pdcp_sm_ric; 
  sm->base.alloc.free_ind_data = free_ind_data_pdcp_sm_ric ; 
  sm->base.alloc.free_ctrl_req_data = free_ctrl_req_data_pdcp_sm_ric; 
  sm->base.alloc.free_ctrl_out_data = free_ctrl_out_data_pdcp_sm_ric; 
  sm->base.alloc.free_e2_setup = free_e2_setup_pdcp_sm_ric; 
  sm->base.alloc.free_ric_service_update = free_ric_service_update_pdcp_sm_ric; 

  // ORAN E2SM 5 Procedures
  sm->base.proc.on_subscription = on_subscription_pdcp_sm_ric; 
  sm->base.proc.on_indication = on_indication_pdcp_sm_ric;
  // Control needs two calls
  sm->base.proc.on_control_req = on_control_req_pdcp_sm_ric;
  sm->base.proc.on_control_out = on_control_out_pdcp_sm_ric;

  sm->base.proc.on_e2_setup = on_e2_setup_pdcp_sm_ric;
  sm->base.proc.on_ric_service_update = on_ric_service_update_pdcp_sm_ric; 

  assert(strlen(SM_PDCP_STR) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name, SM_PDCP_STR, strlen(SM_PDCP_STR)); 

  return &sm->base;
}

