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

#include "msg_generator_xapp.h"

#include <assert.h>

ric_subscription_request_t generate_subscription_request(ric_gen_id_t ric_id , sm_ric_t const* sm, void* cmd)
{
  assert(sm != NULL);
  assert(cmd != NULL);

  sm_subs_data_t data = sm->proc.on_subscription(sm, cmd);
  assert(data.len_ad < 2048);

  ric_subscription_request_t sr = {0}; 
  sr.ric_id = ric_id;
  sr.event_trigger.len = data.len_et;
  sr.event_trigger.buf = data.event_trigger;

  // We just support one action per subscription msg
  sr.len_action = 1;  
  sr.action = calloc(1, sizeof(ric_action_t));
  assert(sr.action != NULL && "Memory exhausted");

  sr.action[0].id = 0;
  sr.action[0].type = RIC_ACT_REPORT;
  if(data.action_def != NULL){
    sr.action[0].definition = malloc(sizeof(byte_array_t));
    assert(sr.action[0].definition != NULL && "Memory exhausted");
    sr.action[0].definition->buf = data.action_def;
    sr.action[0].definition->len = data.len_ad;
  }

  // Only fulfilled when the type is RIC_ACT_INSERT  
  sr.action[0].subseq_action = NULL;

  return sr; 
}

e42_ric_subscription_request_t generate_e42_ric_subscription_request(uint16_t xapp_id, global_e2_node_id_t* id, ric_subscription_request_t* sr)
{

  e42_ric_subscription_request_t e42_sr = {
    .xapp_id = xapp_id,
    .id = cp_global_e2_node_id(id),
    .sr = mv_ric_subscription_request(sr)
  }; 

  return e42_sr;
}

e42_setup_request_t generate_e42_setup_request(e42_xapp_t* xapp)
{
  assert(xapp != NULL);

  const size_t len_rf = assoc_size(&xapp->plugin_ag.sm_ds);
  assert(len_rf > 0 && "No RAN function/service model registered. Check if the Service Models are at the /usr/lib/flexric/ path \n");

  assert(len_rf == assoc_size(&xapp->plugin_ric.sm_ds) && "Invariant violated");

  ran_function_t* ran_func = NULL;
  if(len_rf > 0){
    ran_func = calloc(len_rf, sizeof(*ran_func));
    assert(ran_func != NULL);
  }

  void* it = assoc_front(&xapp->plugin_ag.sm_ds);
  for(size_t i = 0; i < len_rf; ++i){
    sm_agent_t* sm = assoc_value(&xapp->plugin_ag.sm_ds, it);
    assert(sm->ran_func_id == *(uint16_t*)assoc_key(&xapp->plugin_ag.sm_ds, it) && "RAN function mismatch");

    sm_e2_setup_data_t def = sm->proc.on_e2_setup(sm);
    assert(sm->ran_func_id == def.rf.id);

    if(def.len_rfd > 0)
      free(def.ran_fun_def);

    ran_func[i] = def.rf;

    it = assoc_next(&xapp->plugin_ag.sm_ds ,it);
  }
  assert(it == assoc_end(&xapp->plugin_ag.sm_ds) && "Length mismatch");

  e42_setup_request_t sr = {
    .ran_func_item = ran_func,
    .len_rf = len_rf,
  };

  return sr;
}

ric_control_request_t generate_ric_control_request(ric_gen_id_t ric_id, sm_ric_t const* sm, void* ctrl_msg)
{
  assert(sm != NULL);

  sm_ctrl_req_data_t const data = sm->proc.on_control_req(sm,  ctrl_msg);
  assert(data.len_hdr < 2049 && "Check that the SM is built with the same flags as FlexRIC ");
  assert(data.len_msg < 2049 && "Check that the SM is built with the same flags as FlexRIC");

  ric_control_request_t cr = {0}; 
  cr.ric_id = ric_id;
  cr.call_process_id = NULL;
  cr.hdr.buf = data.ctrl_hdr;
  cr.hdr.len = data.len_hdr;
  cr.msg.buf = data.ctrl_msg;
  cr.msg.len = data.len_msg;
  cr.ack_req = malloc(sizeof(ric_control_ack_req_t )) ;
  assert(cr.ack_req != NULL);
  *cr.ack_req = RIC_CONTROL_REQUEST_ACK;  

  return cr;
}
