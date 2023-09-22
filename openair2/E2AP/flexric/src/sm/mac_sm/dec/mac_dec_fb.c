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



#include "mac_dec_fb.h"

//#include "../ie/fb/e2sm_mac_stats_v00_builder.h"
//#include "../ie/fb/e2sm_mac_stats_v00_verifier.h"

#include <assert.h>
#include <stdio.h>


mac_event_trigger_t mac_dec_event_trigger_fb(size_t len, uint8_t const ev_tr[len])
{
  assert(len > 0);

  assert(0!=0 && "Not implemented");

/*
  assert(E2SM_MACStats_EventTrigger_verify_as_root(ev_tr, len) == 0 && "Invalid event trigger");

//  int rc = E2SM_MACStats_EventTrigger_verify_as_root(ev_tr, len);
//  printf("EventTrigger is invalid: %s\n", flatcc_verify_error_string(rc));

  E2SM_MACStats_EventTrigger_table_t ev_tr_fb = E2SM_MACStats_EventTrigger_as_root(ev_tr);
  assert(ev_tr_fb != NULL);

  const uint8_t val = E2SM_MACStats_EventTrigger_trig(ev_tr_fb);

  mac_event_trigger_t  ret = {0};
  if(val == E2SM_MACStats_TriggerNature_oneMs){
    ret.ms = 1;
  } else if(val == E2SM_MACStats_TriggerNature_twoMs){
    ret.ms = 2;
  } else if(val == E2SM_MACStats_TriggerNature_fiveMs){
    ret.ms = 5;
  } else {
    assert(0!=0 && "Not foreseen case");
  }
  return ret;
  */

}

mac_action_def_t mac_dec_action_def_fb(size_t len, uint8_t const action_def[len])
{

  assert(0!=0 && "Not implemented");
}

mac_ind_hdr_t mac_dec_ind_hdr_fb(size_t len, uint8_t const ind_hdr[len])
{

  assert(0!=0 && "Not implemented");
}

mac_ind_msg_t mac_dec_ind_msg_fb(size_t len, uint8_t const ind_msg[len])
{
  assert(len > 0);

  assert(0!=0 && "Not implemented");

  /*
  assert(E2SM_MACStats_IndicationMessage_verify_as_root(ind_msg, len) == 0 && "Invalid event trigger");

//  int rc = E2SM_MACStats_EventTrigger_verify_as_root(ev_tr, len);
//  printf("EventTrigger is invalid: %s\n", flatcc_verify_error_string(rc));


  E2SM_MACStats_IndicationMessage_table_t ind_msg_fb = E2SM_MACStats_IndicationMessage_as_root(ind_msg);
  assert(ind_msg_fb != NULL);

  mac_ind_msg_t ret = {0};
  ret.tx_bytes = E2SM_MACStats_IndicationMessage_tx_bytes(ind_msg_fb); 
  ret.tx_pkts = E2SM_MACStats_IndicationMessage_tx_pkts(ind_msg_fb); 
  return ret;
  */
}

mac_call_proc_id_t mac_dec_call_proc_id_fb(size_t len, uint8_t const call_proc_id[len])
{

  assert(0!=0 && "Not implemented");
}

mac_ctrl_hdr_t mac_dec_ctrl_hdr_fb(size_t len, uint8_t const ctrl_hdr[len])
{

  assert(0!=0 && "Not implemented");
}

mac_ctrl_msg_t mac_dec_ctrl_msg_fb(size_t len, uint8_t const ctrl_msg[len])
{

  assert(0!=0 && "Not implemented");
}


mac_ctrl_out_t mac_dec_ctrl_out_fb(size_t len, uint8_t const ctrl_out[len]) 
{

  assert(0!=0 && "Not implemented");
}

mac_func_def_t mac_dec_func_def_fb(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
}


