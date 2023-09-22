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


#include "gtp_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static inline
size_t next_pow2(size_t x)
{
  static_assert(sizeof(x) == 8, "Need this size to work correctly");
  x -= 1;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	x |= (x >> 32);
	
	return x + 1;

}

gtp_event_trigger_t gtp_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  gtp_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

gtp_action_def_t gtp_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  gtp_action_def_t act_def;// = {0};
  return act_def;
}

gtp_ind_hdr_t gtp_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(len == sizeof(gtp_ind_hdr_t)); 
  gtp_ind_hdr_t ret;
  memcpy(&ret, ind_hdr, len);
  return ret;
}

gtp_ind_msg_t gtp_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{
  assert(next_pow2(len) >= sizeof(gtp_ind_msg_t) - sizeof(gtp_ngu_t_stats_t*) && "Less bytes than the case where there are not active Radio bearers! Next pow2 trick used for aligned struct");
  gtp_ind_msg_t ret = {0};

  memcpy(&ret.len, ind_msg, sizeof(ret.len));
  if(ret.len > 0){
    ret.ngut = calloc(ret.len, sizeof(gtp_ngu_t_stats_t) );
    assert(ret.ngut != NULL && "memory exhausted");
  }

  void const* it = ind_msg + sizeof(ret.len);
  for(uint32_t i = 0; i < ret.len; ++i){
  memcpy(&ret.ngut[i], it, sizeof(ret.ngut[i]) );
  it += sizeof(ret.ngut[i]); 
  }
  
  memcpy(&ret.tstamp, it, sizeof(ret.tstamp));
  it += sizeof(ret.tstamp);

//  memcpy(&ret.slot, it, sizeof(ret.slot));
//  it += sizeof(ret.slot);
 
  assert(it == &ind_msg[len] && "Mismatch of data layout");

  return ret;
}

gtp_call_proc_id_t gtp_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
}

gtp_ctrl_hdr_t gtp_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len == sizeof(gtp_ctrl_hdr_t)); 
  gtp_ctrl_hdr_t ret;
  memcpy(&ret, ctrl_hdr, len);
  return ret;
}

gtp_ctrl_msg_t gtp_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  assert(len == sizeof(gtp_ctrl_msg_t)); 
  gtp_ctrl_msg_t ret;
  memcpy(&ret, ctrl_msg, len);
  return ret;
}

gtp_ctrl_out_t gtp_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(0!=0 && "Not implemented");
  assert(ctrl_out!= NULL);
}

gtp_func_def_t gtp_dec_func_def_plain(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(func_def != NULL);
}

