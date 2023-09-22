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


#include "rlc_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

void free_rlc_event_trigger(rlc_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

rlc_event_trigger_t cp_rlc_event_trigger( rlc_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 

  rlc_event_trigger_t et = {0};
  return et;
}

bool eq_rlc_event_trigger(rlc_event_trigger_t* m0, rlc_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

void free_rlc_action_def(rlc_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

rlc_action_def_t cp_rlc_action_def(rlc_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  rlc_action_def_t ad = {0};
  return ad;
}

bool eq_rlc_action_def(rlc_event_trigger_t* m0,  rlc_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_rlc_ind_hdr(rlc_ind_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
}

rlc_ind_hdr_t cp_rlc_ind_hdr(rlc_ind_hdr_t const* src)
{
  assert(src != NULL);
  rlc_ind_hdr_t dst = {0}; 
  dst.dummy = src->dummy;
  return dst;
}

bool eq_rlc_ind_hdr(rlc_ind_hdr_t* m0, rlc_ind_hdr_t* m1)
{
  assert(m0 != 0);
  assert(m1 != 0);

  if(m0->dummy != m1->dummy)
    return false;
  return true;
}






//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////

void free_rlc_ind_msg(rlc_ind_msg_t* src)
{
  assert(src != NULL);

  if(src->len > 0){
    assert(src->rb != NULL);
    free(src->rb);
  }
}

rlc_ind_msg_t cp_rlc_ind_msg(rlc_ind_msg_t const* src)
{
  assert(src != NULL);

  rlc_ind_msg_t cp = {.len = src->len, .tstamp = src->tstamp}; 

  if(cp.len > 0){
    cp.rb = calloc(cp.len, sizeof(rlc_radio_bearer_stats_t));
    assert(cp.rb != NULL && "memory exhausted");
  }

  memcpy(cp.rb, src->rb, sizeof(rlc_radio_bearer_stats_t)*cp.len);

  return cp;
}

bool eq_rlc_ind_msg(rlc_ind_msg_t* m0, rlc_ind_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->tstamp != m1->tstamp ||
      m0->len != m1->len)
    return false;

  for(uint32_t i =0; i < m0->len; ++i){
 rlc_radio_bearer_stats_t const* rb0 = &m0->rb[i];
 rlc_radio_bearer_stats_t const* rb1 = &m1->rb[i];
 if(
     rb0->txpdu_pkts != rb1->txpdu_pkts   ||
     rb0->txpdu_bytes != rb1->txpdu_bytes ||  
     rb0->txpdu_wt_ms != rb1->txpdu_wt_ms  ||
     rb0->txpdu_dd_pkts != rb1->txpdu_dd_pkts ||
     rb0->txpdu_dd_bytes != rb1->txpdu_dd_bytes ||
     rb0->txpdu_retx_pkts != rb1->txpdu_retx_pkts ||
     rb0->txpdu_retx_bytes != rb1->txpdu_retx_bytes ||
     rb0->txpdu_segmented != rb1->txpdu_segmented ||
     rb0->txpdu_status_pkts != rb1->txpdu_status_pkts ||
     rb0->txpdu_status_bytes != rb1->txpdu_status_bytes ||
     rb0->txbuf_occ_bytes != rb1-> txbuf_occ_bytes ||
     rb0->txbuf_occ_pkts != rb1->txbuf_occ_pkts ||
     rb0->rxpdu_pkts != rb1->rxpdu_pkts ||
     rb0->rxpdu_bytes != rb1->rxpdu_bytes ||
     rb0->rxpdu_dup_pkts != rb1->rxpdu_dup_pkts ||
     rb0->rxpdu_dup_bytes != rb1->rxpdu_dup_bytes ||
     rb0->rxpdu_dd_pkts != rb1->rxpdu_dd_pkts ||
     rb0->rxpdu_dd_bytes != rb1->rxpdu_dd_bytes ||
     rb0->rxpdu_ow_pkts != rb1->rxpdu_ow_pkts ||
     rb0->rxpdu_ow_bytes != rb1->rxpdu_ow_bytes ||
     rb0->rxpdu_status_pkts != rb1->rxpdu_status_pkts ||
     rb0->rxpdu_status_bytes != rb1->rxpdu_status_bytes ||
     rb0->rxbuf_occ_bytes != rb1->rxbuf_occ_bytes ||
     rb0->rxbuf_occ_pkts != rb1->rxbuf_occ_pkts ||
     rb0->txsdu_pkts != rb1->txsdu_pkts ||
     rb0->txsdu_bytes != rb1->txsdu_bytes ||
     rb0->rxsdu_pkts != rb1->rxsdu_pkts ||
     rb0->rxsdu_bytes != rb1->rxsdu_bytes ||
     rb0->rxsdu_dd_pkts != rb1->rxsdu_dd_pkts ||
     rb0->rxsdu_dd_bytes != rb1->rxsdu_dd_bytes ||
     rb0->rnti != rb1->rnti ||
     rb0->mode != rb1->mode ||
     rb0->rbid != rb1->rbid 
     )
  return false;
  }
    return true;
}

//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

void free_rlc_call_proc_id(rlc_call_proc_id_t* src)
{
  // Note that the src could be NULL
  free(src);
}

rlc_call_proc_id_t cp_rlc_call_proc_id( rlc_call_proc_id_t* src)
{
  assert(src != NULL); 
  rlc_call_proc_id_t dst = {0};

  dst.dummy = src->dummy;

  return dst;
}

bool eq_rlc_call_proc_id(rlc_call_proc_id_t* m0, rlc_call_proc_id_t* m1)
{
  if(m0 == NULL && m1 == NULL)
    return true;
  if(m0 == NULL)
    return false;
  if(m1 == NULL)
    return false;

  if(m0->dummy != m1->dummy)
    return false;

  return true;
}


//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////

void free_rlc_ctrl_hdr( rlc_ctrl_hdr_t* src)
{

  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

rlc_ctrl_hdr_t cp_rlc_ctrl_hdr(rlc_ctrl_hdr_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
  rlc_ctrl_hdr_t ret = {0};
  return ret;
}

bool eq_rlc_ctrl_hdr(rlc_ctrl_hdr_t* m0, rlc_ctrl_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////


void free_rlc_ctrl_msg( rlc_ctrl_msg_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

rlc_ctrl_msg_t cp_rlc_ctrl_msg(rlc_ctrl_msg_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  rlc_ctrl_msg_t ret = {0};
  return ret;
}

bool eq_rlc_ctrl_msg(rlc_ctrl_msg_t* m0, rlc_ctrl_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

void free_rlc_ctrl_out(rlc_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

rlc_ctrl_out_t cp_rlc_ctrl_out(rlc_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  rlc_ctrl_out_t ret = {0}; 
  return ret;
}

bool eq_rlc_ctrl_out(rlc_ctrl_out_t* m0, rlc_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

void free_rlc_func_def( rlc_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

rlc_func_def_t cp_rlc_func_def(rlc_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  rlc_func_def_t ret = {0};
  return ret;
}

bool eq_rlc_func_def(rlc_func_def_t* m0, rlc_func_def_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 
  return true;
}


///////////////
// RIC Indication
///////////////

void free_rlc_ind_data(rlc_ind_data_t* ind)
{
  assert(ind != NULL);
  
  free_rlc_ind_hdr(&ind->hdr);
  free_rlc_ind_msg(&ind->msg);
  free_rlc_call_proc_id(ind->proc_id); 
}

rlc_ind_data_t cp_rlc_ind_data(rlc_ind_data_t const* src)
{
  assert(src != NULL);
  rlc_ind_data_t dst = {0};

  dst.hdr = cp_rlc_ind_hdr(&src->hdr);
  dst.msg = cp_rlc_ind_msg(&src->msg);

  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(rlc_call_proc_id_t));
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_rlc_call_proc_id(src->proc_id);
  }

  return dst;
}


