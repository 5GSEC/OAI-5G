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



#ifndef ENDPOINT_XAPP
#define ENDPOINT_XAPP

#include "lib/ep/e2ap_ep.h"   // for e2ap_ep_t
#include "util/byte_array.h"  // for byte_array_t

typedef struct e2ap_xapp_xapp
{
  e2ap_ep_t base;

  // Only one connection supported 
  struct sockaddr_in to; 
  struct sctp_sndrcvinfo sri;
  int msg_flags;
} e2ap_ep_xapp_t;

void e2ap_init_ep_xapp(e2ap_ep_xapp_t* ep, const char* addr, int port);

void e2ap_free_ep_xapp(e2ap_ep_xapp_t* ep);

byte_array_t e2ap_recv_msg_xapp(e2ap_ep_xapp_t* ep);

void e2ap_send_bytes_xapp(e2ap_ep_xapp_t* ep, byte_array_t ba);

#endif

