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


#include "ind_event.h"

#include <assert.h>
#include <stdlib.h>

int cmp_ind_event(void const* m0_v, void const* m1_v)
{
  assert(m0_v != NULL);
  assert(m1_v != NULL);
  ind_event_t* m0 = (ind_event_t*)m0_v;
  ind_event_t* m1 = (ind_event_t*)m1_v;

  int cmp = cmp_ric_gen_id(&m0->ric_id, &m1->ric_id);
  if(cmp != 0)
    return cmp;

  // It is foreseen that not all the struct is
  // scanned. This is basically a bad trick, 
  // but works as expected and "simplifies?"
  // the code

  return cmp;
}

bool eq_ind_event_ric_req_id(const void* value, const void* key)
{
  assert(value != NULL);
  assert(key != NULL);
  
  uint32_t* ric_id = (uint32_t*)value; 
  ind_event_t* ind_ev = (ind_event_t*)key;
  bool eq = (*ric_id == ind_ev->ric_id.ric_req_id);
  return eq;
}


bool eq_ind_event(const void* value, const void* key)
{
  assert(value != NULL);
  assert(key != NULL);
  
  ric_gen_id_t* ric_id = (ric_gen_id_t*)value; 
  ind_event_t* ind_ev = (ind_event_t*)key;
  bool eq = eq_ric_gen_id(ric_id, &ind_ev->ric_id );
  return eq;
}


/*
void free_ind_event(ind_event_t* src)
{
  assert(src != NULL);

//  ric_gen_id_t ric_id;
  // Non-owning ptr
//  sm_agent_t* sm;
//  uint8_t action_id;
  free_subscribe_timer(&src->sub);
}
*/

