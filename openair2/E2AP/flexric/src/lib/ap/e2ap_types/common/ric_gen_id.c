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


#include "ric_gen_id.h"
#include <assert.h>
#include <stdlib.h>

bool eq_ric_gen_id(const ric_gen_id_t* m0, const  ric_gen_id_t* m1)
{
  return m0->ric_req_id == m1->ric_req_id 
    && m0->ric_inst_id == m1->ric_inst_id
    && m0->ran_func_id == m1->ran_func_id;
}

int cmp_ric_gen_id(const ric_gen_id_t* m0, const  ric_gen_id_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->ran_func_id < m1->ran_func_id) return 1;
  if(m0->ran_func_id > m1->ran_func_id) return -1;

  if(m0->ric_inst_id < m1->ric_inst_id) return 1;
  if(m0->ric_inst_id > m1->ric_inst_id) return -1;

  if(m0->ric_req_id < m1->ric_req_id) return 1;
  if(m0->ric_req_id > m1->ric_req_id) return -1;

  return 0;
}
