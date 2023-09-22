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

#ifndef ACTIVE_PROCEDURES_H
#define ACTIVE_PROCEDURES_H 

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#include "../lib/ap/e2ap_types/common/e2ap_global_node_id.h"
#include "../lib/ap/e2ap_types/common/ric_gen_id.h"
#include "../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../sm/agent_if/read/sm_ag_if_rd.h"

typedef enum{
  // Near-RT RIC Functional Procedures
  RIC_SUBSCRIPTION_PROCEDURE_ACTIVE,
  // Use the same ric__id as the subscription procedure
//  RIC_SUBSCRIPTION_DELETE_PROCEDURE_ACTIVE,
  // Use the same ric__id as the subscription procedure
//  RIC_INDICATION_PROCEDURE_ACTIVE,
  RIC_CONTROL_PROCEDURE_ACTIVE,

  // Global Procedures do NOT have a ric_gen_id_t
  //E2_SETUP_PROCEDURE_ACTIVE,
  //RESET_PROCEDURE_ACTIVE,
  //RIC_SERVICE_UPDATE_PROCEDURE_ACTIVE,
  //E2_NODE_CONFIGURATION_UPDATE_PROCEDURE_ACTIVE,
  //E2_CONNECTION_UPDATE_PROCEDURE_ACTIVE
} act_proc_val_e ;

typedef struct{
  act_proc_val_e type;
  ric_gen_id_t id; 
  void (*sm_cb)(sm_ag_if_rd_t const*);
  global_e2_node_id_t e2_node;
} act_proc_val_t;

typedef struct{
  assoc_reg_t reg; // key: uint32_t | value: act_subs_val_t 
  pthread_mutex_t mtx; //act_subs_mtx;
} act_proc_t;

void init_act_proc(act_proc_t* proc);

void free_act_proc(act_proc_t* proc);

void free_act_proc_val(void* val);

uint32_t add_act_proc(act_proc_t* proc, act_proc_val_e type, ric_gen_id_t id, global_e2_node_id_t const* e2_node, void(*sm_cb)(sm_ag_if_rd_t const *));

void rm_act_proc(act_proc_t* act, uint16_t ric_req_id );

typedef struct{
  bool ok; 
  union {
    const char* error;
    act_proc_val_t val; 
  };

}act_proc_ans_t;


act_proc_ans_t find_act_proc(act_proc_t* proc, uint16_t ric_req_id);

#endif

