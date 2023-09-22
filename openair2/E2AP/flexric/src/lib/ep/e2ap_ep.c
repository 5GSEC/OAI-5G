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

#include "e2ap_ep.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

#include <pthread.h>

void e2ap_ep_init(e2ap_ep_t* ep)
{
  int rc = pthread_mutex_init(&ep->mtx, NULL);
  assert(rc == 0);
}

void e2ap_ep_free(e2ap_ep_t* ep)
{
  assert(ep != NULL);
  int rc = pthread_mutex_destroy(&ep->mtx);
  assert(rc == 0);
}

void e2ap_send_sctp_msg(const e2ap_ep_t* ep, sctp_msg_t* msg)
{
  assert(ep != NULL);
  assert(msg->ba.buf && msg->ba.len > 0);

  struct sockaddr_in const* addr = &msg->info.addr; 
  struct sctp_sndrcvinfo const* sri = &msg->info.sri;
  byte_array_t const ba = msg->ba;

  lock_guard(&((e2ap_ep_t*)ep)->mtx);

  const int rc = sctp_sendmsg(ep->fd, (void*)ba.buf, ba.len, (struct sockaddr *)addr, sizeof(*addr), sri->sinfo_ppid, sri->sinfo_flags, sri->sinfo_stream, 0, 0) ;
  assert(rc != 0);
}


static
struct sctp_shutdown_event cp_sn_shutdown_event(struct sctp_shutdown_event const* src)
{
  struct sctp_shutdown_event dst = { .sse_type = src->sse_type,
                                     .sse_flags = src->sse_flags,
                                     .sse_length = src->sse_length,
                                     .sse_assoc_id = src->sse_assoc_id }; 
  return dst;
}

static
struct sctp_assoc_change cp_sn_assoc_change( struct sctp_assoc_change const* src)
{
  struct sctp_assoc_change dst = {.sac_state = src->sac_state}; 

  switch(src->sac_state) {
    case SCTP_COMM_UP:
      assert(0 !=0 && "Not implemented");
      break;

    case SCTP_COMM_LOST:
      assert(0 !=0 && "Not implemented");
      break;

    case SCTP_RESTART:
      assert(0 !=0 && "Not implemented");
      break;

    case SCTP_SHUTDOWN_COMP:
      assert(0 !=0 && "Not implemented");
      break;

    case SCTP_CANT_STR_ASSOC:
      assert(0 !=0 && "Not implemented");
      break;
    default:
      assert(0!=0 && "Impossible data path. enum sctp_sac_state only has 5 states" );
  }

  return dst;
}




static
union sctp_notification cp_sctp_notification(union sctp_notification const* src, size_t len)
{
  assert(src != NULL);
  assert(sizeof(((union sctp_notification*)NULL)->sn_header) <= len);

  union sctp_notification dst = {.sn_header.sn_type = src-> sn_header.sn_type};

  switch(src->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE:   //This tag indicates that an association has either been opened or closed. Refer to Section 6.1.1 for details.
                              assert(sizeof(struct sctp_assoc_change) >= len && "Error notification msg size is smaller than struct sctp_assoc_change size\n");
                              dst.sn_assoc_change = cp_sn_assoc_change(&src->sn_assoc_change);
                              break;
                              
    case SCTP_PEER_ADDR_CHANGE: //This tag indicates that an address that is part of an existing association has experienced a change of state (e.g., a failure or return to service of the reachability of an endpoint via a specific transport address). Please see Section 6.1.2 for data structure details.
                              assert(0!=0 && "Not implemented");
                              break;

    case SCTP_REMOTE_ERROR: // The attached error message is an Operation Error message received from the remote peer. It includes the complete TLV sent by the remote endpoint. See Section 6.1.3 for the detailed format.
                              assert(0!=0 && "Not implemented");
                              break;

    case SCTP_SEND_FAILED: //The attached datagram could not be sent to the remote endpoint. This structure includes the original SCTP_SNDINFO that was used in sending this message; i.e., this structure uses the sctp_sndinfo per Section 6.1.11.
                              assert(0!=0 && "Not implemented");
                              break;

    case SCTP_SHUTDOWN_EVENT: // The peer has sent a SHUTDOWN. No further data should be sent on this socket.
                               assert( sizeof(struct sctp_shutdown_event) >= len);
                               dst.sn_shutdown_event = cp_sn_shutdown_event(&src->sn_shutdown_event);
                               break;
                             
    case SCTP_ADAPTATION_INDICATION: //This notification holds the peer’s indicated adaptation layer. Please see Section 6.1.6.
                              assert(0!=0 && "Not implemented");
                              break;

    case SCTP_PARTIAL_DELIVERY_EVENT: //This notification is used to tell a receiver that the partial delivery has been aborted. This may indicate that the association is about to be aborted. Please see Section 6.1.7.
                             assert(0!=0 && "Not implemented");
                             break;

    //case SCTP_AUTHENTICATION_EVENT: //This notification is used to tell a receiver that either an error occurred on authentication, or a new key was made active. See Section 6.1.8.
    //                         assert(0!=0 && "Not implemented");
    //                         break;

    case SCTP_SENDER_DRY_EVENT:
                             assert(0!=0 && "Not implemented");
                             break;

    default:
                             assert(0!=0 && "Not implemented");
                             break;

  }

  return dst;
}

sctp_msg_t e2ap_recv_sctp_msg(e2ap_ep_t* ep)
{
  assert(ep != NULL);

  sctp_msg_t from = {0}; 

  from.ba.len = 32*1024;
  from.ba.buf = malloc(32*1024);

  assert(from.ba.buf != NULL && "Memory exhausted");

  socklen_t len = sizeof(from.info.addr);
  int msg_flags = 0;

  lock_guard(&((e2ap_ep_t*)ep)->mtx);
  int const rc = sctp_recvmsg(ep->fd, from.ba.buf, from.ba.len, (struct sockaddr*)&from.info.addr, &len, &from.info.sri, &msg_flags);
  assert(rc > -1 && rc != 0 && rc < (int)from.ba.len);

  if(msg_flags & MSG_NOTIFICATION){
    assert((msg_flags & MSG_EOR) && "Notification received but the buffer is not large enough");
    uint8_t buf[2048] = {0};
    memcpy(buf, from.ba.buf, 2048);
    free(from.ba.buf); 

    from.type = SCTP_MSG_NOTIFICATION;
    from.notif = cp_sctp_notification((union sctp_notification*) buf, rc); 
  } else {
    from.type = SCTP_MSG_PAYLOAD;
    from.ba.len = rc; // set actually received number of bytes
  }

  return from;
}

