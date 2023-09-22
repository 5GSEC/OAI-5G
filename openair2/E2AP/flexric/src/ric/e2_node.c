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


#include "e2_node.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void init_e2_node(e2_node_t* n, global_e2_node_id_t const* id, size_t len_acc, accepted_ran_function_t accepted[len_acc])
{
  assert(n != NULL);
  n->id.type = id->type;
  n->id.plmn = id->plmn;
  n->id.nb_id = id->nb_id;
  if (id->cu_du_id) {
    n->id.cu_du_id = calloc(1, sizeof(uint64_t));
    assert(n->id.cu_du_id != NULL && "memory exhausted");
    *n->id.cu_du_id = *id->cu_du_id;
  }
  n->len_acc = len_acc;
  if (n->len_acc > 0) {
    n->accepted = calloc(len_acc, sizeof(accepted_ran_function_t));
    assert(n->accepted != NULL);

    memcpy(n->accepted, accepted, len_acc * sizeof(accepted_ran_function_t));
  }
}

void free_e2_node(e2_node_t* n)
{
  assert(n!= NULL);

  free_global_e2_node_id(&n->id);

  if(n->len_acc > 0)
    free(n->accepted);
}

void free_e2_node_void(void *n) { free_e2_node((e2_node_t *)n); }

e2_node_t cp_e2_node(e2_node_t const* src)
{
  assert(src != NULL);
  e2_node_t dst = {0}; 

  dst.id = cp_global_e2_node_id(&src->id);

  dst.len_acc = src->len_acc;

  if(src->len_acc > 0){
    dst.accepted = calloc(src->len_acc, sizeof(accepted_ran_function_t));
    assert(dst.accepted != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < src->len_acc; ++i){
    dst.accepted[i] = src->accepted[i]; 
  }

  return dst;
}

