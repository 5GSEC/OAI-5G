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


#include "ric_subsequent_action.h"

#include <stddef.h>

bool eq_ric_subsequent_action(const ric_subsequent_action_t* m0, const ric_subsequent_action_t* m1)
{
  if(m0 == m1) 
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->type != m1->type) 
    return false;

  if(m0->time_to_wait_ms == m1->time_to_wait_ms)
    return true;

  if(m0->time_to_wait_ms == NULL || m1 == NULL)
    return false;

  if(*m0->time_to_wait_ms != *m1->time_to_wait_ms)
    return false;

  return true;

}
