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



#include "endpoint_xapp.h"
#include <arpa/inet.h>       // for inet_pton
#include <assert.h>          // for assert
#include <netinet/sctp.h>
#include <netinet/in.h>      // for sockaddr_in, IPPROTO_SCTP, htonl, htons
#include <stdio.h>           // for NULL
#include <stdlib.h>          // for malloc
#include <string.h>          // for strlen, strncpy
#include <strings.h>         // for bzero
#include <sys/socket.h>      // for setsockopt, AF_INET, socket, SOCK_SEQPACKET
#include "lib/ep/e2ap_ep.h"  // for e2ap_ep_t, e2ap_recv_bytes, e2ap_send_bytes

static
void init_sctp_conn_client(e2ap_ep_xapp_t* ep, const char* addr, int port)
{
  int sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  assert(sock_fd != -1);

  struct sockaddr_in servaddr; 
  bzero(&servaddr, sizeof (servaddr) ) ;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (port);
  int rc = inet_pton(AF_INET, addr, &servaddr.sin_addr);
  assert(rc == 1);

  struct sctp_event_subscribe evnts; 
  bzero(&evnts, sizeof (evnts)) ;
  evnts.sctp_data_io_event = 1 ;
  setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof (evnts));

  const int no_delay = 1;
  setsockopt(sock_fd, IPPROTO_SCTP, SCTP_NODELAY, &no_delay, sizeof(no_delay));

  ep->to = servaddr;
  *(int*)(&ep->base.port) = port; 
  *(int*)(&ep->base.fd) = sock_fd;
  strncpy((char*)(&ep->base.addr), addr, 16);
}

void e2ap_init_ep_xapp(e2ap_ep_xapp_t* ep, const char* addr, int port)
{
  assert(ep != NULL);
  assert(addr != NULL);
  assert(strlen(addr) < 16);
  assert(port > 0 && port < 65535);
  init_sctp_conn_client(ep, addr, port);
}

byte_array_t e2ap_recv_msg_xapp(e2ap_ep_xapp_t* ep)
{
  assert(ep != NULL);

  sctp_msg_t rcv = e2ap_recv_sctp_msg(&ep->base); //, &ba);

//sctp_msg_t e2ap_recv_sctp_msg(e2ap_ep_t* ep);
  return rcv.ba;
//  e2ap_msg_t msg = e2ap_msg_dec(&enc->type, ba);
  //printf("Message received in the iapp\n");
//  return msg;
}

void e2ap_send_bytes_xapp(e2ap_ep_xapp_t* ep, byte_array_t ba)
{
  assert(ep != NULL);
  assert(ba.buf && ba.len > 0);

  sctp_msg_t msg = { .ba = ba,
                      .info.addr = ep->to,
                      .info.sri = ep->sri
              };


  e2ap_send_sctp_msg(&ep->base, &msg);
  //ep->base.to, ba);
}

void e2ap_free_ep_xapp(e2ap_ep_xapp_t* ep)
{
  assert(ep != NULL);
  assert(0!=0 && "Not implememented!");
}
