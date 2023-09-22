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


#include "ric_action_not_admitted.h"

#include <assert.h>
#include <stddef.h>

bool eq_ric_action_not_admitted(const ric_action_not_admitted_t* m0, const ric_action_not_admitted_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL) 
    return false;

  if(m0->ric_act_id != m1->ric_act_id) 
    return false;

  if(eq_cause(&m0->cause, &m1->cause) == false)
    return false;

  return true;
}

ric_action_not_admitted_t cp_ric_action_not_admitted(ric_action_not_admitted_t* src)
{
  assert(src != NULL);
  ric_action_not_admitted_t dst = { .ric_act_id = src->ric_act_id,
                                    .cause = cp_cause(&src->cause) };

  return dst;
}


