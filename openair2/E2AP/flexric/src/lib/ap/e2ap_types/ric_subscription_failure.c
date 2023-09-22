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


#include "ric_subscription_failure.h"

bool eq_ric_subscritption_failure(const ric_subscription_failure_t* m0, const ric_subscription_failure_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(eq_ric_gen_id(&m0->ric_id, &m1->ric_id) == false)
    return false;

  if(m0->len_na != m1->len_na)
    return false;

  for(size_t i = 0; i < m0->len_na; ++i){
    if(eq_ric_action_not_admitted(&m0->not_admitted[i], &m1->not_admitted[i]) == false){
      return false;
    }
  }

  if(eq_criticality_diagnostics(m0->crit_diag, m1->crit_diag) == false)
    return false;

  return true;
}

