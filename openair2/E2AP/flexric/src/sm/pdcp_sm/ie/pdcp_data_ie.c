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


#include "pdcp_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>



//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

void free_pdcp_event_trigger(pdcp_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

pdcp_event_trigger_t cp_pdcp_event_trigger( pdcp_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 

  pdcp_event_trigger_t et = {0};
  return et;
}

bool eq_pdcp_event_trigger(pdcp_event_trigger_t* m0, pdcp_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

void free_pdcp_action_def(pdcp_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

pdcp_action_def_t cp_pdcp_action_def(pdcp_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  pdcp_action_def_t ad = {0};
  return ad;
}

bool eq_pdcp_action_def(pdcp_event_trigger_t* m0,  pdcp_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_pdcp_ind_hdr(pdcp_ind_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
}

pdcp_ind_hdr_t cp_pdcp_ind_hdr(pdcp_ind_hdr_t const* src)
{
  assert(src != NULL);
  pdcp_ind_hdr_t dst = {0}; 
  dst.dummy = src->dummy;
  return dst;
}

bool eq_pdcp_ind_hdr(pdcp_ind_hdr_t* m0, pdcp_ind_hdr_t* m1)
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





static
bool check_pdcp_mode(uint8_t mode)
{
  assert(mode == 0 || mode == 1 || mode == 2);
  return true;
}

static
bool check_pdcp_rbid(uint8_t rbid)
{
  assert(rbid < 11); // [0,2] for srb and [3,10] for drb
  return true;
}

static
bool check_pdcp_bearer(pdcp_radio_bearer_stats_t* rb)
{
  assert(rb != NULL);

  // Pkts need to be less than bytes since a packet can only have [1,MTU+header)  
  assert(rb->txpdu_pkts <= rb->txpdu_bytes); 
  assert(rb->rxpdu_pkts <= rb->rxpdu_bytes);
  assert(rb->txsdu_pkts <= rb->txsdu_bytes);
  assert(rb->rxsdu_pkts <= rb->rxsdu_bytes);

  return true;
}

static
bool check_pdcp_invariants(pdcp_ind_msg_t const* msg)
{
  assert(msg != NULL);

  for(uint32_t i = 0; i < msg->len; ++i){
    pdcp_radio_bearer_stats_t* rb = &msg->rb[i];
    check_pdcp_mode(rb->mode);  
    check_pdcp_rbid(rb->rbid);
    check_pdcp_bearer(rb);
  }

  return true;
}

void free_pdcp_ind_msg(pdcp_ind_msg_t* msg)
{
  assert(msg != NULL);
  assert(check_pdcp_invariants(msg));

  if(msg->len > 0)
    free(msg->rb);
}

pdcp_ind_msg_t cp_pdcp_ind_msg(pdcp_ind_msg_t const* src)
{
  assert(src != NULL);
  assert(check_pdcp_invariants(src));

  pdcp_ind_msg_t dst = {0}; 

  dst.tstamp = src->tstamp;
//  dst.frame = src->frame;
//  dst.slot = src->slot;
  dst.len = src->len;


  if(dst.len > 0){
    dst.rb = calloc(dst.len, sizeof(*dst.rb) );
    assert(dst.rb != NULL && "Memory exhausted"); 
  }

  memcpy(dst.rb, src->rb, sizeof(*dst.rb)*src->len);
  return dst;
}

bool eq_pdcp_ind_msg(pdcp_ind_msg_t* m0, pdcp_ind_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  assert(check_pdcp_invariants(m0));
  assert(check_pdcp_invariants(m1));

  if(m0->tstamp != m1->tstamp ||
      m0->len != m1->len ){
    assert(0 != 0 && "Impossible!");
    return false;
  }

  for(uint32_t i =0; i < m0->len; ++i){
    pdcp_radio_bearer_stats_t* rb0 = &m0->rb[i]; 
    pdcp_radio_bearer_stats_t* rb1 = &m1->rb[i]; 

    if( rb0->txpdu_pkts != rb1->txpdu_pkts || 
        rb0->txpdu_bytes != rb1->txpdu_bytes ||
        rb0->txpdu_sn !=   rb1->txpdu_sn ||
        rb0->rxpdu_pkts != rb1->rxpdu_pkts ||
        rb0->rxpdu_bytes != rb1->rxpdu_bytes ||
        rb0->rxpdu_sn != rb1->rxpdu_sn ||  
        rb0->rxpdu_oo_pkts != rb1->rxpdu_oo_pkts ||  
        rb0->rxpdu_oo_bytes != rb1->rxpdu_oo_bytes ||
        rb0->rxpdu_dd_pkts != rb1->rxpdu_dd_pkts ||
        rb0->rxpdu_dd_bytes != rb1->rxpdu_dd_bytes ||
        rb0->rxpdu_ro_count != rb1->rxpdu_ro_count || 
        rb0->txsdu_pkts != rb1->txsdu_pkts || 
        rb0->txsdu_bytes != rb1->txsdu_bytes ||
        rb0->rxsdu_pkts != rb1->rxsdu_pkts ||  
        rb0->rxsdu_bytes != rb1->rxsdu_bytes ||
        rb0->rnti !=  rb1->rnti ||
        rb0->mode !=  rb1->mode ||
        rb0->rbid != rb1->rbid){

    assert(0 != 0 && "Impossible!");
      return false;
    }
  }
  return true;
} 

//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

void free_pdcp_call_proc_id(pdcp_call_proc_id_t* src)
{
  // Note that the src could be NULL
  free(src);
}

pdcp_call_proc_id_t cp_pdcp_call_proc_id( pdcp_call_proc_id_t* src)
{
  assert(src != NULL); 
  pdcp_call_proc_id_t dst = {0};

  dst.dummy = src->dummy;

  return dst;
}

bool eq_pdcp_call_proc_id(pdcp_call_proc_id_t* m0, pdcp_call_proc_id_t* m1)
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

void free_pdcp_ctrl_hdr( pdcp_ctrl_hdr_t* src)
{
  assert(src != NULL);
  // No memory allocated
  (void)src;
}

pdcp_ctrl_hdr_t cp_pdcp_ctrl_hdr(pdcp_ctrl_hdr_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
  pdcp_ctrl_hdr_t ret = {0};
  return ret;
}

bool eq_pdcp_ctrl_hdr(pdcp_ctrl_hdr_t* m0, pdcp_ctrl_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////


void free_pdcp_ctrl_msg( pdcp_ctrl_msg_t* src)
{
  assert(src != NULL);
  // No memory allocated, so nothing to free
  (void)src;
}

pdcp_ctrl_msg_t cp_pdcp_ctrl_msg(pdcp_ctrl_msg_t* src)
{
  assert(src != NULL);

  pdcp_ctrl_msg_t ret = {0};

  ret.action = src->action;

  return ret;
}

bool eq_pdcp_ctrl_msg(pdcp_ctrl_msg_t* m0, pdcp_ctrl_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  
  if(m0->action == m1->action)
    return true;

  return false;
}


//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

void free_pdcp_ctrl_out(pdcp_ctrl_out_t* src)
{
  assert(src != NULL);
  (void)src;
  // No heap memory allocated
}

pdcp_ctrl_out_t cp_pdcp_ctrl_out(pdcp_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  pdcp_ctrl_out_t ret = {0}; 
  return ret;
}

bool eq_pdcp_ctrl_out(pdcp_ctrl_out_t* m0, pdcp_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

void free_pdcp_func_def( pdcp_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

pdcp_func_def_t cp_pdcp_func_def(pdcp_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  pdcp_func_def_t ret = {0};
  return ret;
}

bool eq_pdcp_func_def(pdcp_func_def_t* m0, pdcp_func_def_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 
  return true;
}

///////////////
// RIC Indication
///////////////

void free_pdcp_ind_data(pdcp_ind_data_t* ind)
{
  assert(ind != NULL);
  
  free_pdcp_ind_hdr(&ind->hdr);
  free_pdcp_ind_msg(&ind->msg);
  free_pdcp_call_proc_id(ind->proc_id);
}

pdcp_ind_data_t cp_pdcp_ind_data(pdcp_ind_data_t const* src)
{
  assert(src != NULL);

  pdcp_ind_data_t dst = {0}; 

  dst.hdr = cp_pdcp_ind_hdr(&src->hdr);
  dst.msg = cp_pdcp_ind_msg(&src->msg);

  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(*dst.proc_id) );
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_pdcp_call_proc_id(src->proc_id);
  }

  return dst;
}

