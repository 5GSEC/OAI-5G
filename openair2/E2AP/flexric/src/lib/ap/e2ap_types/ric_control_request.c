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



#include "ric_control_request.h"

static
bool eq_ric_control_ack_req(const  ric_control_ack_req_t* m0, const  ric_control_ack_req_t* m1)
{
  if(m0 == m1) 
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(*m0 != *m1)
    return false;

  return true;
}

bool eq_ric_control_request(const ric_control_request_t* m0, const ric_control_request_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(eq_ric_gen_id(&m0->ric_id, &m1->ric_id) == false)
    return false;

  if(eq_byte_array(m0->call_process_id, m1->call_process_id) == false)
    return false;

  if(eq_byte_array(&m0->hdr, &m1->hdr) == false)
    return false;

  if(eq_byte_array(&m0->msg, &m1->msg) == false)
    return false;

  if(eq_ric_control_ack_req(m0->ack_req, m1->ack_req) == false)
    return false;

  return true;
}

