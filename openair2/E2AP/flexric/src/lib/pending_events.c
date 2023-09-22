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



#include "pending_events.h"
#include <assert.h>
#include <stdlib.h>

int cmp_pending_event(void const* pend_v1, void const* pend_v2)
{
  assert(pend_v1 != NULL);
  assert(pend_v2 != NULL);

  pending_event_t* ev1 = (pending_event_t*)pend_v1; 
  pending_event_t* ev2 = (pending_event_t*)pend_v2; 

  if(*ev1 < *ev2) return 1;
  if(*ev1 == *ev2) return 0;
  return -1;
}


bool valid_pending_event(pending_event_t ev)
{
  assert(ev == SETUP_REQUEST_PENDING_EVENT
          || ev == SUBSCRIPTION_REQUEST_PENDING_EVENT
          || ev == SUBSCRIPTION_DELETE_REQUEST_PENDING_EVENT
          || ev == E42_SETUP_REQUEST_PENDING_EVENT
          );
  return true;
}

