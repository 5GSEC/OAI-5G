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


#ifndef E2_NODE_RIC_STRUCT_H
#define E2_NODE_RIC_STRUCT_H 

#include <stddef.h>

#include "../lib/ap/e2ap_types/common/e2ap_global_node_id.h"
#include "../lib/ap/e2ap_types/e2_setup_response.h"


typedef struct{

  global_e2_node_id_t id;

  accepted_ran_function_t* accepted;
  size_t len_acc;

} e2_node_t ;


void init_e2_node(e2_node_t* n, global_e2_node_id_t const* id, size_t len_acc, accepted_ran_function_t accepted[len_acc]);

void free_e2_node(e2_node_t* n);

void free_e2_node_void(void *n);

e2_node_t cp_e2_node(e2_node_t const* n);

#endif

