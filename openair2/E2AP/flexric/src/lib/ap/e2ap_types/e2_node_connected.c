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

#include "e2_node_connected.h"

#include <assert.h>
#include <stdlib.h>

e2_node_connected_t cp_e2_node_connected(const e2_node_connected_t* src)
{
  assert(src != NULL);

  e2_node_connected_t dst = {0};

  dst.id = cp_global_e2_node_id(&src->id);

  dst.len_rf = src->len_rf;
  if(dst.len_rf > 0){
    dst.ack_rf = calloc(dst.len_rf, sizeof(ran_function_t));
    assert(dst.ack_rf != NULL && "Memory exhausted");
  }

  for(size_t i =0; i < dst.len_rf; ++i){
    dst.ack_rf[i] = cp_ran_function( &src->ack_rf[i]); 
  }
  return dst;
}

void free_e2_node_connected(e2_node_connected_t* src)
{
  assert(src != NULL);

  free_global_e2_node_id(&src->id);

  for(size_t i = 0; i < src->len_rf; ++i){
    ran_function_t* rf = &src->ack_rf[i]; 
    free_ran_function(rf);
  }

  free(src->ack_rf);
}

bool eq_e2_node_connected(const e2_node_connected_t* m0, const e2_node_connected_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL)
    return false;

  if(m1 == NULL)
    return false;

  if( eq_global_e2_node_id(&m0->id, &m1->id) == false)
    return false;

  if(m0->len_rf != m1->len_rf)
    return false;

  for(size_t i = 0; i < m0->len_rf; ++i){
    if(eq_ran_function(&m0->ack_rf[i], &m1->ack_rf[i] ) == false)
      return false;
  }

  return true;
}


