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

#include "endpoint_agent.h"
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
void init_sctp_conn_client(e2ap_ep_ag_t* ep, const char* addr, int port)
{
  int sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  assert(sock_fd != -1);

  struct sockaddr_in servaddr = { .sin_family = AF_INET,
                                  .sin_port = htons(port),
                                  .sin_addr.s_addr = htonl(INADDR_ANY)}; 

  int rc = inet_pton(AF_INET, addr, &servaddr.sin_addr);
  assert(rc == 1);

  struct sctp_event_subscribe evnts = { .sctp_data_io_event = 1,
                                        .sctp_shutdown_event = 1}; 

  rc = setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof (evnts));
  assert(rc == 0);

  const int close_time = 0; // No automatic close https://www.rfc-editor.org/rfc/pdfrfc/rfc6458.txt.pdf p. 65
  rc = setsockopt(sock_fd, IPPROTO_SCTP, SCTP_AUTOCLOSE, &close_time, sizeof(close_time));
  assert(rc == 0);

  const int no_delay = 1;
  rc = setsockopt(sock_fd, IPPROTO_SCTP, SCTP_NODELAY, &no_delay, sizeof(no_delay));
  assert(rc == 0);

  ep->to = servaddr;
  *(int*)(&ep->base.port) = port; 
  *(int*)(&ep->base.fd) = sock_fd;
  strncpy((char*)(&ep->base.addr), addr, 15);
  ((char*)(&ep->base.addr))[15] = '\0';
}

void e2ap_init_ep_agent(e2ap_ep_ag_t* ep, const char* addr, int port)
{
  assert(ep != NULL);
  assert(addr != NULL);
  assert(strlen(addr) < 16);
  assert(port > 0 && port < 65535);
  init_sctp_conn_client(ep, addr, port);
}

/*
static
bool eq_sockaddr(struct sockaddr_in* m0, struct sockaddr_in* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->sin_addr.s_addr != m1->sin_addr.s_addr)
    return false;

  if(m0->sin_port != m1->sin_port)
    return false;

  if(m0->sin_family != m1->sin_family)
    return false;

  return true;
}
*/

sctp_msg_t e2ap_recv_msg_agent(e2ap_ep_ag_t* ep)
{
  assert(ep != NULL);

  sctp_msg_t rcv = e2ap_recv_sctp_msg(&ep->base);// , &ba);
  return rcv;
}

void e2ap_send_bytes_agent(e2ap_ep_ag_t* ep, byte_array_t ba)
{
  assert(ep != NULL);
  assert(ba.buf && ba.len > 0);

  sctp_msg_t msg = { .info.addr = ep->to,
                     .info.sri = ep->sri,
                     .ba = ba};

  e2ap_send_sctp_msg(&ep->base, &msg);
}

void e2ap_free_ep_agent(e2ap_ep_ag_t* ep)
{
  assert(ep != NULL);
  assert(0!=0 && "Not implemented!");
}
