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
e* For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */


#ifndef E2_NEAR_RIC_H
#define E2_NEAR_RIC_H

#include "asio_ric.h"
#include "e2ap_ric.h"
#include "endpoint_ric.h"
#include "util/alg_ds/ds/seq_container/seq_generic.h"
#include "util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "util/alg_ds/ds/assoc_container/bimap.h"
#include "util/conf_file.h"
#include "sm/sm_ric.h"
#include "plugin_ric.h"
#include "map_e2_node_sockaddr.h"

//#include "../ric/iApp/iapp_if.h"

#include <stdatomic.h>

struct near_ric_s;
typedef struct e2ap_msg_s (*e2ap_handle_msg_fp_ric)(struct near_ric_s* ric, const struct e2ap_msg_s* msg);

typedef struct near_ric_s
{
  e2ap_ep_ric_t ep;
  e2ap_ric_t ap; 
  asio_ric_t io;
  e2ap_handle_msg_fp_ric handle_msg[30]; // 26 E2AP + 4 E42AP note that not all the slots will be occupied

  // Registered SMs
  plugin_ric_t plugin;

  // Publish/Subscribed update function pointers per sm 
  assoc_rb_tree_t pub_sub; // seq_arr_t per SM 
 
  // Connected E2 Nodes
  seq_arr_t conn_e2_nodes; // e2_node_t 
  pthread_mutex_t conn_e2_nodes_mtx;

  // Monotonically increasing RIC request ID
  atomic_int req_id;

  // Pending events
  bi_map_t pending; // left: fd, right: pending_event_ric_t   
  pthread_mutex_t pend_mtx;

  atomic_bool server_stopped;
  atomic_bool stop_token;
} near_ric_t;

near_ric_t* init_near_ric(fr_args_t const* args);

// Blocking call
void start_near_ric(near_ric_t* ric);

void free_near_ric(near_ric_t* ric);

//////

seq_arr_t conn_e2_nodes(near_ric_t* ric); 

//size_t num_conn_e2_nodes(near_ric_t* ric);


uint16_t report_service_near_ric(near_ric_t* ric, global_e2_node_id_t const* id, uint16_t ran_func_id, void* cmd);

void rm_report_service_near_ric(near_ric_t* ric, global_e2_node_id_t const* id, uint16_t ran_func_id, uint16_t act_id);

void control_service_near_ric(near_ric_t* ric, global_e2_node_id_t const* id, uint16_t ran_func_id, void* ctrl);

// Plug-ins functions

void load_sm_near_ric(near_ric_t* ric, const char* file_path); 


// iApp interface used by the xApps


void start_near_ric_iapp(near_ric_t* ric); 

void stop_near_ric_iapp();

uint16_t fwd_ric_subscription_request(near_ric_t* ric,  global_e2_node_id_t const* id, ric_subscription_request_t const* sr, void (*f)(e2ap_msg_t const* msg));

void fwd_ric_subscription_request_delete(near_ric_t* ric, global_e2_node_id_t const* id,  ric_subscription_delete_request_t const* sdr, void (*f)(e2ap_msg_t const* msg));

uint16_t fwd_ric_control_request(near_ric_t* ric, global_e2_node_id_t const* id, ric_control_request_t const* cr,  void (*f)(e2ap_msg_t const* msg));

#endif

