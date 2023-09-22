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



#include "pdcp_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

pdcp_event_trigger_t pdcp_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  pdcp_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

pdcp_action_def_t pdcp_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  pdcp_action_def_t act_def;// = {0};
  return act_def;
}

pdcp_ind_hdr_t pdcp_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(len == sizeof(pdcp_ind_hdr_t)); 
  pdcp_ind_hdr_t ret;
  memcpy(&ret, ind_hdr, len);
  return ret;
}

pdcp_ind_msg_t pdcp_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{
  assert(len >= sizeof(uint32_t) + sizeof(int64_t));
//      uint16_t) + sizeof(uint8_t)); // pdcp_ind_msg_t)); 
  pdcp_ind_msg_t ret;

  void* it = (void*)ind_msg; 
  memcpy(&ret.len, ind_msg, sizeof(ret.len)); 
  it += sizeof(ret.len);

  if(ret.len > 0){
    ret.rb = calloc(ret.len, sizeof(pdcp_radio_bearer_stats_t));
    assert(ret.rb != NULL && "Memory exhausted!");
  }

  for(uint32_t i = 0; i < ret.len; ++i){
    memcpy(&ret.rb[i] , it, sizeof(pdcp_radio_bearer_stats_t));
    it += sizeof(pdcp_radio_bearer_stats_t);
  }

  memcpy(&ret.tstamp, it, sizeof(ret.tstamp));
  it += sizeof(ret.tstamp);

//  memcpy(&ret.slot, it, sizeof(ret.slot));
//  it += sizeof(ret.slot);

  assert(it == ind_msg + len && "Data layout mismatch");

  return ret;
}

pdcp_call_proc_id_t pdcp_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
}

pdcp_ctrl_hdr_t pdcp_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len == sizeof(pdcp_ctrl_hdr_t)); 
  pdcp_ctrl_hdr_t ret;
  memcpy(&ret, ctrl_hdr, len);
  return ret;
}

pdcp_ctrl_msg_t pdcp_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  assert(len == sizeof(pdcp_ctrl_msg_t)); 
  pdcp_ctrl_msg_t ret;
  memcpy(&ret, ctrl_msg, len);
  return ret;
}

pdcp_ctrl_out_t pdcp_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(len > 0);
  assert(ctrl_out!= NULL);
  assert(len == sizeof( pdcp_ctrl_out_t ) );
  pdcp_ctrl_out_t out = {0}; 
  memcpy(&out, ctrl_out, len);

  return out;
}

pdcp_func_def_t pdcp_dec_func_def_plain(size_t len, uint8_t const func[len])
{
  assert(0!=0 && "Not implemented");
  assert(func != NULL);
}


