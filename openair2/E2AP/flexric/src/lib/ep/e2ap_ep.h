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

#ifndef E2AP_EP
#define E2AP_EP

#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> 

#include "util/byte_array.h"
#include "sctp_msg.h"


typedef struct{
  const char addr[16]; // only ipv4 supported
  const int port;
  const int fd;
  pthread_mutex_t mtx;
} e2ap_ep_t;

void e2ap_ep_init(e2ap_ep_t* ep);

void e2ap_ep_free(e2ap_ep_t* ep);

void e2ap_send_sctp_msg(const e2ap_ep_t* ep, sctp_msg_t* msg);

sctp_msg_t e2ap_recv_sctp_msg(e2ap_ep_t* ep);

#endif

