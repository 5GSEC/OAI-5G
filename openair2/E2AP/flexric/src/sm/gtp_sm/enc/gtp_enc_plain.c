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



#include "gtp_enc_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

byte_array_t gtp_enc_event_trigger_plain(gtp_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
  byte_array_t  ba = {0};
 
  ba.len = sizeof(event_trigger->ms);
  ba.buf = malloc(ba.len);
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, &event_trigger->ms, ba.len);

  return ba;
}

byte_array_t gtp_enc_action_def_plain(gtp_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t gtp_enc_ind_hdr_plain(gtp_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);

  byte_array_t ba = {0};

  ba.len = sizeof(gtp_ind_hdr_t);
  ba.buf = malloc(sizeof(gtp_ind_msg_t));
  assert(ba.buf != NULL && "memory exhausted");
  memcpy(ba.buf, ind_hdr, sizeof(gtp_ind_hdr_t));

  return ba;
}

byte_array_t gtp_enc_ind_msg_plain(gtp_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  byte_array_t ba = {0};

  size_t const sz = sizeof(ind_msg->len) + 
                  sizeof(gtp_ngu_t_stats_t)*ind_msg->len + 
                  sizeof(ind_msg->tstamp);

//  printf("Size of the byte array = %lu\n", sz);

  ba.buf = malloc(sz); 
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, &ind_msg->len, sizeof(ind_msg->len));

  void* it = ba.buf + sizeof(ind_msg->len);
  for(uint32_t i = 0; i < ind_msg->len ; ++i){
    memcpy(it, &ind_msg->ngut[i], sizeof(ind_msg->ngut[i]));
    it += sizeof(ind_msg->ngut[i]);
  }

  memcpy(it, &ind_msg->tstamp, sizeof(ind_msg->tstamp));
  it += sizeof(ind_msg->tstamp);

//  memcpy(it, &ind_msg->slot, sizeof(ind_msg->slot));
//  it += sizeof(ind_msg->slot);

  assert(it == ba.buf + sz && "Mismatch of data layout");

  ba.len = sz;
  return ba;
}

byte_array_t gtp_enc_call_proc_id_plain(gtp_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t gtp_enc_ctrl_hdr_plain(gtp_ctrl_hdr_t const* ctrl_hdr)
{
  assert(ctrl_hdr != NULL);
  byte_array_t  ba = {0};
  ba.buf = malloc(sizeof(gtp_ind_msg_t)); 
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, ctrl_hdr, sizeof(gtp_ctrl_hdr_t));

  ba.len = sizeof(gtp_ctrl_hdr_t);
  return ba;
}

byte_array_t gtp_enc_ctrl_msg_plain(gtp_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  byte_array_t  ba = {0};
  ba.buf = malloc(sizeof(gtp_ctrl_msg_t)); 
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, ctrl_msg, sizeof(gtp_ctrl_msg_t));

  ba.len = sizeof(gtp_ctrl_hdr_t);
  return ba;
}

byte_array_t gtp_enc_ctrl_out_plain(gtp_ctrl_out_t const* ctrl) 
{
  assert(0!=0 && "Not implemented");

  assert( ctrl != NULL );
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t gtp_enc_func_def_plain(gtp_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

