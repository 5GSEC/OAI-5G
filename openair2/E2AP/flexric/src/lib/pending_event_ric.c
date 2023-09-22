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



#include "pending_event_ric.h"

#include <assert.h>
#include <stdlib.h>

int cmp_pending_event_ric(void const* p_v1, void const* p_v2)
{
  assert(p_v1 != NULL);
  assert(p_v2 != NULL);

  pending_event_ric_t* p1 = (pending_event_ric_t*)p_v1; 
  pending_event_ric_t* p2 = (pending_event_ric_t*)p_v2; 
  int cmp_ev = cmp_pending_event(p1, p2);
  if(cmp_ev != 0) 
    return cmp_ev;

  return cmp_ric_gen_id(&p1->id, &p2->id);
}



