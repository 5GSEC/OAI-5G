
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

#ifndef MAP_E2_NODE_SOCKADDR_H
#define MAP_E2_NODE_SOCKADDR_H 

#include "../lib/ap/e2ap_types/common/e2ap_global_node_id.h"
#include "../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "../util/alg_ds/ds/assoc_container/bimap.h"


#include "../lib/ep/sctp_msg.h"

#include <netinet/in.h>
#include <pthread.h>

typedef struct{
//  assoc_rb_tree_t tree; // key: global_e2_node_id_t | value: sctp_info_t     

  bi_map_t map; // left  key: global_e2_node_id_t | value: sctp_info_t 
                // right key: sctp_info_t | value: global_e2_node_id_t 

  pthread_mutex_t mtx;
} map_e2_node_sockaddr_t ; //


void init_map_e2_node_sad(map_e2_node_sockaddr_t* m);

void free_map_e2_node_sad(map_e2_node_sockaddr_t* m);

void add_map_e2_node_sad(map_e2_node_sockaddr_t* m, global_e2_node_id_t const* id, sctp_info_t const* info );

//void rm_map_e2_node_sad(map_e2_node_sockaddr_t* m, global_e2_node_id_t* id);

global_e2_node_id_t* rm_map_sad_e2_node(map_e2_node_sockaddr_t* m, sctp_info_t const* s);

sctp_info_t find_map_e2_node_sad(map_e2_node_sockaddr_t* m, global_e2_node_id_t const* id);

sctp_info_t find_map_e2_node_sad(map_e2_node_sockaddr_t * m, global_e2_node_id_t const* id);

#endif

