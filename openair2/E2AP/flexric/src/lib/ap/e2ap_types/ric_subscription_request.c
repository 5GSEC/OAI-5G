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


#include "ric_subscription_request.h"
#include <assert.h>

bool eq_ric_subscritption_request(const ric_subscription_request_t* m0, const ric_subscription_request_t* m1)
{
  //compare pointers
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  if(eq_ric_gen_id(&m0->ric_id, &m1->ric_id) == false)
    return false;

  if(eq_byte_array(&m0->event_trigger, &m1->event_trigger) == false)
    return false;

  if(m0->len_action != m1->len_action)
    return false;

  for(size_t i =0 ; i < m0->len_action; ++i){
    if(eq_ric_action(&m0->action[i], &m1->action[i]) == false)
      return false;
  }

  return true;
}

ric_subscription_request_t mv_ric_subscription_request( ric_subscription_request_t* sr)
{
  assert(sr != NULL);
  ric_subscription_request_t ans = *sr;
  memset(sr, 0, sizeof(ric_subscription_request_t) );

  return ans;
}


