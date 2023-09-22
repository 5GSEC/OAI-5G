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



#include "mac_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

mac_event_trigger_t mac_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  mac_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

mac_action_def_t mac_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  mac_action_def_t act_def;// = {0};
  return act_def;
}

mac_ind_hdr_t mac_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(len == sizeof(mac_ind_hdr_t)); 
  mac_ind_hdr_t ret;
  memcpy(&ret, ind_hdr, len);
  return ret;
}

mac_ind_msg_t mac_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{
//  assert(len == sizeof(mac_ind_msg_t)); 
  mac_ind_msg_t ret;

  static_assert(sizeof(uint32_t) == sizeof(ret.len_ue_stats), "Different sizes!");

  const size_t len_sizeof = sizeof(ret.len_ue_stats);
  memcpy(&ret.len_ue_stats, ind_msg, len_sizeof);

  if(ret.len_ue_stats > 0){
    ret.ue_stats = calloc(ret.len_ue_stats, sizeof(mac_ue_stats_impl_t));
    assert(ret.ue_stats != NULL && "Memory exhausted!");
  }
  
  void* ptr = (void*)&ind_msg[len_sizeof];
  for(uint32_t i = 0; i < ret.len_ue_stats; ++i){
    memcpy(&ret.ue_stats[i], ptr, sizeof( mac_ue_stats_impl_t) );
    ptr += sizeof( mac_ue_stats_impl_t); 
  }

  memcpy(&ret.tstamp, ptr, sizeof(ret.tstamp));

  ptr += sizeof(ret.tstamp);
  assert(ptr == ind_msg + len && "data layout mismacth");

  return ret;
}

mac_call_proc_id_t mac_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
}

mac_ctrl_hdr_t mac_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len == sizeof(mac_ctrl_hdr_t)); 
  mac_ctrl_hdr_t ret;
  memcpy(&ret, ctrl_hdr, len);
  return ret;
}

mac_ctrl_msg_t mac_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  assert(len == sizeof(mac_ctrl_msg_t)); 
  mac_ctrl_msg_t ret;
  memcpy(&ret, ctrl_msg, len);
  return ret;
}

mac_ctrl_out_t mac_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(0!=0 && "Not implemented");
  assert(ctrl_out != NULL);
}

mac_func_def_t mac_dec_func_def_plain(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(func_def != NULL);
}


