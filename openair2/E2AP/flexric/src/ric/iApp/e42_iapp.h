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

#ifndef E42_IAPP_H
#define E42_IAPP_H 

// Like E2 Agent, but it does not generate the Setup Request
#include "../../lib/ap/global_consts.h"
#include "../../lib/ap/type_defs.h"
#include "../../lib/msg_hand/reg_e2_nodes.h"

#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../../util/alg_ds/ds/assoc_container/bimap.h"
#include "../../util/ngran_types.h"

#include "../near_ric.h"
#include "near_ric_if.h"
//#include "../../../test/iapp-xapp/near_ric_emulator.h"

#include "asio_iapp.h"
#include "e2ap_iapp.h"
#include "endpoint_iapp.h"
#include "map_ric_id.h"

#include <stdatomic.h>
#include <stdbool.h>

typedef struct e42_iapp_s e42_iapp_t;

typedef e2ap_msg_t (*handle_msg_fp_iapp)(struct e42_iapp_s*, const e2ap_msg_t* msg) ;

typedef struct e42_iapp_s 
{
  e2ap_ep_iapp_t ep; 
  e2ap_iapp_t ap;
  asio_iapp_t io;
  handle_msg_fp_iapp handle_msg[31]; // note that not all the slots will be occupied

  // Registered xApps
  uint32_t xapp_id;

  // Registered E2 Nodes 
  reg_e2_nodes_t e2_nodes;

  map_ric_id_t map_ric_id;

  near_ric_if_t ric_if;

  atomic_bool stop_token;
  atomic_bool stopped;
} e42_iapp_t;

e42_iapp_t* init_e42_iapp(const char* addr, near_ric_if_t ric_if); //, int port);

// Blocking call
void start_e42_iapp(e42_iapp_t* iapp);

void free_e42_iapp(e42_iapp_t* iapp);

void add_e2_node_iapp(e42_iapp_t* i, global_e2_node_id_t* id, size_t len, ran_function_t const ran_func[len]);

void rm_e2_node_iapp(e42_iapp_t* i, global_e2_node_id_t* id);

void notify_msg_iapp(e42_iapp_t* iapp, e2ap_msg_t const* msg);

#endif

