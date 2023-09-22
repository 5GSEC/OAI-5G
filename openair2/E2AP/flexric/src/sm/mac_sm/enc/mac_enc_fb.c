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



#include "mac_enc_fb.h"

#include <assert.h>
#include <stdio.h>


//#include "../ie/fb/e2sm_mac_stats_v00_builder.h"
//#include "../ie/fb/e2sm_mac_stats_v00_verifier.h"



byte_array_t mac_enc_event_trigger_fb(mac_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);

  assert(0!=0 && "Not implemented");
  byte_array_t ba = {0};
  return ba;

/*
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);

  E2SM_MACStats_EventTrigger_start_as_root(&builder); 

  if(event_trigger->ms == 1){
    E2SM_MACStats_EventTrigger_trig_add(&builder, E2SM_MACStats_TriggerNature_oneMs );
  } else if(event_trigger->ms == 2){
    E2SM_MACStats_EventTrigger_trig_add(&builder, E2SM_MACStats_TriggerNature_twoMs );
  } else if(event_trigger->ms == 5){
    E2SM_MACStats_EventTrigger_trig_add(&builder, E2SM_MACStats_TriggerNature_fiveMs );
  } else {
    assert(0!=0 && "Not foreseen state");
  }


  E2SM_MACStats_EventTrigger_end_as_root(&builder);


  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(&builder, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = E2SM_MACStats_EventTrigger_verify_as_root(buf, size))) {
    printf("Event trigger is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(&builder);

  return ba;
  */
}


byte_array_t mac_enc_action_def_fb(mac_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ind_hdr_fb(mac_ind_hdr_t const* ind_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ind_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ind_msg_fb(mac_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);
  assert(0!=0 && "Not implemented");
  byte_array_t ba = {0};
  return ba;
/*
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);

  E2SM_MACStats_IndicationMessage_start_as_root(&builder); 
  E2SM_MACStats_IndicationMessage_tx_bytes_add(&builder, ind_msg->tx_bytes); 
  E2SM_MACStats_IndicationMessage_tx_pkts_add(&builder, ind_msg->tx_pkts); 

  E2SM_MACStats_EventTrigger_end_as_root(&builder);

  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(&builder, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if ((ret = E2SM_MACStats_IndicationMessage_verify_as_root(buf, size))) {
    printf("Event trigger is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(&builder);

  return ba;
  */
}

byte_array_t mac_enc_call_proc_id_fb(mac_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ctrl_hdr_fb(mac_ctrl_hdr_t const* ctrl_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ctrl_msg_fb(mac_ctrl_msg_t const* ctrl_msg)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_msg != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ctrl_out_fb(mac_ctrl_out_t const* ctrl) 
{
  assert(0!=0 && "Not implemented");

  assert(ctrl != NULL );
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_func_def_fb(mac_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

