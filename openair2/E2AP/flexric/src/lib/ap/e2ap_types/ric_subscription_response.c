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


#include "ric_subscription_response.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

bool eq_ric_subscritption_response(const ric_subscription_response_t* m0, const ric_subscription_response_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(eq_ric_gen_id(&m0->ric_id, &m1->ric_id) == false)
    return false;

  if(m0->len_admitted != m1->len_admitted) 
    return false;

  for(size_t i = 0; i < m0->len_admitted; i++){
  if(eq_ric_action_admitted(&m0->admitted[i], &m1->admitted[i]) == false)
    return false;
  }

  if(m0->len_na != m1->len_na) 
    return false;

  for(size_t i = 0; i < m0->len_na; ++i){
    if(eq_ric_action_not_admitted(&m0->not_admitted[i], &m1->not_admitted[i]) == false)
      return false;
  }

  return true;
}

ric_subscription_response_t cp_ric_subscription_respponse(ric_subscription_response_t const* src)
{
  assert(src != NULL);
  
  ric_subscription_response_t dst  = {0}; 

  dst.ric_id = src->ric_id;

  dst.len_admitted = src->len_admitted;

  if(dst.len_admitted > 0){ 
    dst.admitted = calloc(dst.len_admitted, sizeof(ric_action_admitted_t) );
    assert(dst.admitted != NULL && "Memory exhausted" );
  }

  for(size_t i = 0; i < dst.len_admitted; ++i){
    dst.admitted[i] = cp_ric_action_admitted(&src->admitted[i]);
  }

  dst.len_na = src->len_na; 
  if(dst.len_na > 0){ 
    dst.not_admitted = calloc(dst.len_na, sizeof( ric_action_not_admitted_t ) );
    assert(dst.not_admitted != NULL && "Memory exhausted" );
  }

  for(size_t i = 0; i < dst.len_na; ++i){
    dst.not_admitted[i] = cp_ric_action_not_admitted(&src->not_admitted[i]); 
  }

  return dst;
}

ric_subscription_response_t mv_ric_subscription_respponse(ric_subscription_response_t const* src)
{
  ric_subscription_response_t dst = {0}; 
  dst.admitted = src->admitted;
  dst.len_admitted = src->len_admitted;
  dst.len_na =src->len_na;
  dst.not_admitted = src->not_admitted;
  dst.ric_id = src->ric_id; 

  // move ownership to the dst msg. Forgive my sinns 
  memset((ric_subscription_response_t*) src, 0, sizeof(ric_subscription_response_t ) );
  return dst;
}


