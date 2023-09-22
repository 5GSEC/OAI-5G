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



#ifndef E2_AGENT_H
#define E2_AGENT_H

#include "util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "util/alg_ds/ds/assoc_container/bimap.h"
#include "util/alg_ds/ds/tsq/tsq.h"

#include "util/conf_file.h"
#include "util/ngran_types.h"

#include "lib/ap/global_consts.h"
#include "lib/ap/type_defs.h"

#include "asio_agent.h"
#include "e2ap_agent.h"
#include "endpoint_agent.h"
#include "plugin_agent.h"
#include "sm/sm_io.h"

#include <stdatomic.h>
#include <stdbool.h>

typedef struct e2_agent_s e2_agent_t;

typedef e2ap_msg_t (*handle_msg_fp_agent)(struct e2_agent_s*, const e2ap_msg_t* msg) ;

typedef struct e2_agent_s 
{
  e2ap_ep_ag_t ep; 
  e2ap_agent_t ap;
  asio_agent_t io;
  handle_msg_fp_agent handle_msg[30]; // 26 E2AP + 4 E42AP note that not all the slots will be occupied

  // Registered SMs
  plugin_ag_t plugin;

  // Registered Periodic Indication events
  pthread_mutex_t mtx_ind_event;
  bi_map_t ind_event; // key1:int fd, key2:ind_event_t 

  // Pending events
  bi_map_t pending;  // left: fd, right: pending_event_t 

  global_e2_node_id_t global_e2_node_id;

  // Aperiodic Indication events
  tsq_t aind; // aind_event_t Events that occurred 

  atomic_bool stop_token;
  atomic_bool agent_stopped;
} e2_agent_t;

e2_agent_t* e2_init_agent(const char* addr, int port, global_e2_node_id_t ge2nid, sm_io_ag_ran_t io, char const*  libs_dir);

// Blocking call
void e2_start_agent(e2_agent_t* ag);

void e2_free_agent(e2_agent_t* ag);
     
void e2_async_event_agent(e2_agent_t* ag, uint32_t ric_req_id, void* ind_data);

///////////////////////////////////////////////
// E2AP AGENT FUNCTIONAL PROCEDURES MESSAGES //
///////////////////////////////////////////////

void e2_send_subscription_response(e2_agent_t* ag, const ric_subscription_response_t* sr);

void e2_send_subscription_failure(e2_agent_t* ag, const ric_subscription_failure_t* sf);

void e2_send_subscription_delete_response(e2_agent_t* ag, const ric_subscription_delete_response_t* sdr);

void e2_send_subscription_delete_failure(e2_agent_t* ag, const ric_subscription_delete_failure_t* sdf);

void e2_send_indication_agent(e2_agent_t* ag, const ric_indication_t* indication);

void e2_send_control_acknowledge(e2_agent_t* ag, const ric_control_acknowledge_t* ca);

void e2_send_control_failure(e2_agent_t* ag, const ric_control_failure_t* cf);

////////////////////////////////////////////////


#endif

