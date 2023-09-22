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


#include "gtp_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

void free_gtp_event_trigger(gtp_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

gtp_event_trigger_t cp_gtp_event_trigger( gtp_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 

  gtp_event_trigger_t et = {0};
  return et;
}

bool eq_gtp_event_trigger(gtp_event_trigger_t* m0, gtp_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

void free_gtp_action_def(gtp_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

gtp_action_def_t cp_gtp_action_def(gtp_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  gtp_action_def_t ad = {0};
  return ad;
}

bool eq_gtp_action_def(gtp_event_trigger_t* m0,  gtp_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_gtp_ind_hdr(gtp_ind_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
}

gtp_ind_hdr_t cp_gtp_ind_hdr(gtp_ind_hdr_t const* src)
{
  assert(src != NULL);
  gtp_ind_hdr_t dst = {0}; 
  dst.dummy = src->dummy;
  return dst;
}

bool eq_gtp_ind_hdr(gtp_ind_hdr_t* m0, gtp_ind_hdr_t* m1)
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

void free_gtp_ind_msg(gtp_ind_msg_t* src)
{
  assert(src != NULL);

  if(src->len > 0){
    assert(src->ngut != NULL);
    free(src->ngut);
  }
}

gtp_ind_msg_t cp_gtp_ind_msg(gtp_ind_msg_t const* src)
{
  assert(src != NULL);

  gtp_ind_msg_t cp = {.len = src->len, .tstamp = src->tstamp}; 

  if(cp.len > 0){
    cp.ngut = calloc(cp.len, sizeof(gtp_ngu_t_stats_t));
    assert(cp.ngut != NULL && "memory exhausted");
  }

  memcpy(cp.ngut, src->ngut, sizeof(gtp_ngu_t_stats_t)*cp.len);

  return cp;
}

bool eq_gtp_ind_msg(gtp_ind_msg_t* m0, gtp_ind_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->tstamp != m1->tstamp ||
      m0->len != m1->len)
    return false;

  for(uint32_t i =0; i < m0->len; ++i){
 gtp_ngu_t_stats_t const* ngut0 = &m0->ngut[i];
 gtp_ngu_t_stats_t const* ngut1 = &m1->ngut[i];
 if(
     ngut0->rnti != ngut1->rnti   ||
     ngut0->teidgnb != ngut1->teidgnb ||  
     ngut0->qfi != ngut1->qfi  ||
     ngut0->teidupf != ngut1->teidupf
     )
  return false;
  }
    return true;
}

//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

void free_gtp_call_proc_id(gtp_call_proc_id_t* src)
{
  // Note that the src could be NULL
  free(src);
}

gtp_call_proc_id_t cp_gtp_call_proc_id( gtp_call_proc_id_t* src)
{
  assert(src != NULL); 
  gtp_call_proc_id_t dst = {0};

  dst.dummy = src->dummy;

  return dst;
}

bool eq_gtp_call_proc_id(gtp_call_proc_id_t* m0, gtp_call_proc_id_t* m1)
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

void free_gtp_ctrl_hdr( gtp_ctrl_hdr_t* src)
{

  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

gtp_ctrl_hdr_t cp_gtp_ctrl_hdr(gtp_ctrl_hdr_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
  gtp_ctrl_hdr_t ret = {0};
  return ret;
}

bool eq_gtp_ctrl_hdr(gtp_ctrl_hdr_t* m0, gtp_ctrl_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////


void free_gtp_ctrl_msg( gtp_ctrl_msg_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

gtp_ctrl_msg_t cp_gtp_ctrl_msg(gtp_ctrl_msg_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  gtp_ctrl_msg_t ret = {0};
  return ret;
}

bool eq_gtp_ctrl_msg(gtp_ctrl_msg_t* m0, gtp_ctrl_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

void free_gtp_ctrl_out(gtp_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

gtp_ctrl_out_t cp_gtp_ctrl_out(gtp_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  gtp_ctrl_out_t ret = {0}; 
  return ret;
}

bool eq_gtp_ctrl_out(gtp_ctrl_out_t* m0, gtp_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

void free_gtp_func_def( gtp_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

gtp_func_def_t cp_gtp_func_def(gtp_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  gtp_func_def_t ret = {0};
  return ret;
}

bool eq_gtp_func_def(gtp_func_def_t* m0, gtp_func_def_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 
  return true;
}


///////////////
// RIC Indication
///////////////

void free_gtp_ind_data(gtp_ind_data_t* ind)
{
  assert(ind != NULL);
  
  free_gtp_ind_hdr(&ind->hdr);
  free_gtp_ind_msg(&ind->msg);
  free_gtp_call_proc_id(ind->proc_id); 
}

gtp_ind_data_t cp_gtp_ind_data(gtp_ind_data_t const* src)
{
  assert(src != NULL);
  gtp_ind_data_t dst = {0};

  dst.hdr = cp_gtp_ind_hdr(&src->hdr);
  dst.msg = cp_gtp_ind_msg(&src->msg);

  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(gtp_call_proc_id_t));
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_gtp_call_proc_id(src->proc_id);
  }

  return dst;
}


