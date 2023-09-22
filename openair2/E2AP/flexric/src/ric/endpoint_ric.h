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


#ifndef E2AP_ENDPOINT_RIC_H
#define E2AP_ENDPOINT_RIC_H

#include "lib/ep/e2ap_ep.h"   // for e2ap_ep_t
#include "util/byte_array.h"  // for byte_array_t
#include "map_e2_node_sockaddr.h"

typedef struct{
  e2ap_ep_t base;
  
  // Multi-connection supported
  // Global E2 Node <-> sctp_info_t  
  map_e2_node_sockaddr_t e2_nodes; 

} e2ap_ep_ric_t;

void e2ap_init_ep_ric(e2ap_ep_ric_t* ep, const char* addr, int port);

void e2ap_free_ep_ric(e2ap_ep_ric_t* ep);

sctp_msg_t e2ap_recv_msg_ric(e2ap_ep_ric_t* ep);

void e2ap_send_bytes_ric(const e2ap_ep_ric_t* ep, global_e2_node_id_t const* id, byte_array_t ba);

void e2ap_send_sctp_msg_ric(const e2ap_ep_ric_t* ep, sctp_msg_t* msg);

void e2ap_reg_sock_addr_ric(e2ap_ep_ric_t* ric, global_e2_node_id_t* id, sctp_info_t* s);

global_e2_node_id_t* e2ap_rm_sock_addr_ric(e2ap_ep_ric_t* ric, sctp_info_t const* s);

#endif

