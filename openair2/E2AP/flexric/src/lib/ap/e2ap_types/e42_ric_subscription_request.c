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



#include "e42_ric_subscription_request.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ba_if_not_null(byte_array_t* ba)
{
  if(ba != NULL)
  {
    free_byte_array(*ba);
    free(ba);
  }
}

void free_e42_ric_subscription_request(e42_ric_subscription_request_t* e42_sr)
{
  assert(e42_sr != NULL);

  ric_subscription_request_t* sr = &e42_sr->sr; 

  free_byte_array(sr->event_trigger);
  for(size_t i = 0; i < sr->len_action; ++i){
    free_ba_if_not_null(sr->action[i].definition  );
    if(sr->action[i].subseq_action != NULL){
      if(sr->action[i].subseq_action->time_to_wait_ms != NULL)
        free(sr->action[i].subseq_action->time_to_wait_ms);
      free(sr->action[i].subseq_action);
    }
  }
  free(sr->action);
}

bool eq_e42_ric_subscritption_request(const e42_ric_subscription_request_t* m0, const e42_ric_subscription_request_t* m1)
{
  if(m0 == NULL && m1 == NULL) 
    return true;

  if(m0 == NULL)
    return false;

  if(m1 == NULL)
    return false;

  if(m0->xapp_id != m1->xapp_id)
    return false;

  bool rv = eq_global_e2_node_id(&m0->id, &m1->id );
  if(rv == false)
    return false;

  rv = eq_ric_subscritption_request(&m0->sr, &m1->sr);

  return rv;
}


