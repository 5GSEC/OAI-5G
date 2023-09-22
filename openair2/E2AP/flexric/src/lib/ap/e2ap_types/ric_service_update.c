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



#include "ric_service_update.h"

bool eq_ric_service_update(const ric_service_update_t* m0, const ric_service_update_t* m1)
{

  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(m0->len_added != m1->len_added)
    return false;

  for(size_t i = 0; i < m0->len_added; ++i){
    if(eq_ran_function(&m0->added[i], &m1->added[i]) == false)
      return false;
  }

  if(m0->len_modified != m1->len_modified)
    return false;

  for(size_t i = 0; i < m0->len_modified; ++i){
    if(eq_ran_function(&m0->modified[i], &m1->modified[i]))
      return false;
  }

  if(m0->len_deleted != m1->len_deleted)
    return false;

  for(size_t i = 0; i < m0->len_deleted; ++i){
    if(eq_ran_function_id_rev(&m0->deleted[i], &m1->deleted[i]) == false)
      return false;
  }
  return true;
}
