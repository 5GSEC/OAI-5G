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

#ifndef MAP_RIC_ID_H
#define MAP_RIC_ID_H 

#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../../lib/ap/e2ap_types/common/e2ap_global_node_id.h"

#include "xapp_ric_id.h"
#include <pthread.h>

typedef struct{
  global_e2_node_id_t e2_node_id;
  uint16_t ric_req_id;
} e2_node_ric_req_t;

typedef struct
{
//  assoc_rb_tree_t tree; // key: ric_req_id | value:   xapp_ric_id_t

  bi_map_t bimap; // left: key:   e2_node_ric_req_t | value: xapp_ric_id_t
                  // right: key:  xapp_ric_id_t | value: e2_node_ric_req_t  

  pthread_mutex_t mtx;
} map_ric_id_t;


void init_map_ric_id(map_ric_id_t* map);

void free_map_ric_id( map_ric_id_t* map);

void add_map_ric_id(map_ric_id_t* map, e2_node_ric_req_t* node, xapp_ric_id_t* x);

void rm_map_ric_id(map_ric_id_t* map, e2_node_ric_req_t* node); // uint16_t ric_req_id);

xapp_ric_id_t find_xapp_map_ric_id(map_ric_id_t* map, uint16_t ric_req_id);

e2_node_ric_req_t find_ric_req_map_ric_id(map_ric_id_t* map, xapp_ric_id_t* x);

#endif

