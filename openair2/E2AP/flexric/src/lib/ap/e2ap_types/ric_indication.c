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


#include "ric_indication.h"

#include <assert.h>
#include <string.h>

static
bool eq_sn(uint16_t* m0, uint16_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(*m0 != *m1) 
    return false;

  return true;
} 

bool eq_ric_indication(const ric_indication_t* m0, const ric_indication_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) 
    return false;

  if(eq_ric_gen_id(&m0->ric_id, &m1->ric_id) == false)
    return false;

  if(m0->action_id != m1->action_id)
    return false;

  if(eq_sn(m0->sn, m1->sn) == false )
    return false;

  if(m0->type != m1->type) 
    return false;

  if(eq_byte_array(&m0->hdr,&m1->hdr) == false)
    return false;

   if(eq_byte_array(&m0->msg,&m1->msg) == false)
    return false;

   if(eq_byte_array(m0->call_process_id, m1->call_process_id) == false)
    return false;

  return true;
}

ric_indication_t mv_ric_indication(ric_indication_t* src)
{
  assert(src != NULL);

  ric_indication_t dst = {0};

  dst.ric_id = src->ric_id;
  dst.action_id = src->action_id;
  dst.sn = src->sn; // optional
  dst.type = src->type;
  dst.hdr = src->hdr;
  dst.msg = src->msg;
  dst.call_process_id = src->call_process_id; // optional

  memset(src, 0, sizeof(ric_indication_t) );

  return dst;
}

