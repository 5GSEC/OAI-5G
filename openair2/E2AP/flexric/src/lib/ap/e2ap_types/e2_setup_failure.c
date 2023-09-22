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



#include "e2_setup_failure.h"

bool eq_e2_setup_failure(const e2_setup_failure_t* m0, const e2_setup_failure_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(eq_cause(&m0->cause, &m1->cause) == false)
    return false;

  if(eq_time_to_wait(m0->time_to_wait_ms, m1->time_to_wait_ms) == false)
    return false;

  if(eq_criticality_diagnostics(m0->crit_diag, m1->crit_diag) == false)
    return false;

  if(eq_transport_layer_information(m0->tl_info, m1->tl_info) == false)
    return false;

  return true;
}
