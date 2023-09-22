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

#ifndef ASYNC_EVENT_H
#define ASYNC_EVENT_H

#include "aind_event.h"
#include "ind_event.h"
#include "pending_events.h"
#include "ep/sctp_msg.h"

typedef enum
{
  CHECK_STOP_TOKEN_EVENT,
  SCTP_CONNECTION_SHUTDOWN_EVENT,
  SCTP_MSG_ARRIVED_EVENT, 
  INDICATION_EVENT,
  APERIODIC_INDICATION_EVENT,
  PENDING_EVENT,

  UNKNOWN_EVENT,
} async_event_e;

typedef struct
{
  async_event_e type;
  int fd;
  union{
    pending_event_t* p_ev;
    ind_event_t* i_ev;
    aind_event_t ai_ev;
    sctp_msg_t msg; 
  };
} async_event_t;

#endif

