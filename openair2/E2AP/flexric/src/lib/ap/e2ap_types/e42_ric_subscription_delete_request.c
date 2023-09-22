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


#include "e42_ric_subscription_delete_request.h"

#include <assert.h>
#include <stdlib.h>

bool  eq_e42_ric_subscription_delete_request(const e42_ric_subscription_delete_request_t* m0, const e42_ric_subscription_delete_request_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->xapp_id != m1->xapp_id) 
    return false;

  return eq_ric_subscription_delete_request(&m0->sdr, &m1->sdr);
}


