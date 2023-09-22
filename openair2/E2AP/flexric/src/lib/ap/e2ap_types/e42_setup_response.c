
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


#include "e42_setup_response.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

e42_setup_response_t cp_e42_setup_response(const e42_setup_response_t* src)
{
  assert(src != NULL);

  e42_setup_response_t dst = {.xapp_id = src->xapp_id};

  dst.len_e2_nodes_conn = src->len_e2_nodes_conn;

  if( dst.len_e2_nodes_conn > 0){
    dst.nodes = calloc( dst.len_e2_nodes_conn, sizeof(e2_node_connected_t) );
    assert(dst.nodes != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i <  dst.len_e2_nodes_conn; ++i){
    dst.nodes[i] = cp_e2_node_connected(&src->nodes[i]);
  }

  return dst;
}

bool eq_e42_setup_response(const e42_setup_response_t* m0, const e42_setup_response_t* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL)
    return false;

  if(m1 == NULL)
    return false;

  if(m0->xapp_id != m1->xapp_id)
    return false;

  if(m0->len_e2_nodes_conn != m1->len_e2_nodes_conn)
    return false;

  for(size_t i = 0; i < m0->len_e2_nodes_conn; ++i){
    if( eq_e2_node_connected(&m0->nodes[i], &m1->nodes[i] ) == false)
      return false;
  }

  return true;
}

