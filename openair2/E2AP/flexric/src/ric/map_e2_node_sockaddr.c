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

#include "map_e2_node_sockaddr.h"

#include "../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../util/alg_ds/alg/alg.h"
#include "../lib/ep/sctp_msg.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

static inline
void free_sctp_info(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  sctp_info_t* s = (sctp_info_t*)value;
  free(s);
}

static
void free_global_e2_node(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  global_e2_node_id_t* id = (global_e2_node_id_t*)value;
  free(id);
}

void init_map_e2_node_sad(map_e2_node_sockaddr_t* m)
{
  assert(m != NULL);

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  int rc = pthread_mutex_init(&m->mtx, mtx_attr);
  assert(rc == 0);

//  const size_t key_sz = sizeof(global_e2_node_id_t);
//  assoc_init(&m->tree, key_sz, cmp_global_e2_node_id_wrapper, free_sctp_info);

  const size_t key_sz_1 = sizeof(global_e2_node_id_t);
  const size_t key_sz_2 = sizeof(sctp_info_t);

  bi_map_init(&m->map, key_sz_1, key_sz_2, cmp_global_e2_node_id_wrapper, cmp_sctp_info_wrapper, free_sctp_info, free_global_e2_node);

}

void free_map_e2_node_sad(map_e2_node_sockaddr_t* m)
{
  assert(m != NULL);

  int rc = pthread_mutex_destroy(&m->mtx);
  assert(rc == 0);

  bi_map_free(&m->map);
}

void add_map_e2_node_sad(map_e2_node_sockaddr_t* m, global_e2_node_id_t const* id, sctp_info_t const* s)
{
  assert(m != NULL);
  assert(id != NULL);
  assert(s != NULL);

  lock_guard(&m->mtx);

#if DEBUG
  assoc_rb_tree_t* left = &m->map.left;  

  void* it = assoc_front(left);
  void* end = assoc_end(left);

  it = find_if(left, it, end, id, eq_global_e2_node_id_wrapper);
  assert(it == end && "E2 Node already registered in the tree");

  assoc_rb_tree_t* right = &m->map.right;  

  it = assoc_front(right);
  end = assoc_end(right);

  it = find_if(right, it, end, id, eq_sctp_info_wrapper);
  assert(it == end && "SCTP info already registered in the tree");
#endif
  
  bi_map_insert(&m->map, id, sizeof(global_e2_node_id_t), s, sizeof(sctp_info_t));
}

sctp_info_t* rm_map_e2_node_sad(map_e2_node_sockaddr_t* m, global_e2_node_id_t* id)
{
  assert(m != NULL);
  assert(id != NULL);

  lock_guard(&m->mtx);


  sctp_info_t* s = bi_map_extract_left(&m->map, id, sizeof(global_e2_node_id_t));
  return s;

//  sctp_info_t* s = assoc_extract(&m->tree, id);
//  free(s);
}

global_e2_node_id_t* rm_map_sad_e2_node(map_e2_node_sockaddr_t* m, sctp_info_t const* s)
{
  assert(m != NULL);
  assert(s != NULL);

  lock_guard(&m->mtx);

  global_e2_node_id_t* id = bi_map_extract_right(&m->map, (sctp_info_t*) s, sizeof(sctp_info_t));
  return id;
}

sctp_info_t find_map_e2_node_sad(map_e2_node_sockaddr_t* m, global_e2_node_id_t const* id)
{
  assert(m != NULL);
  assert(id != NULL);

  lock_guard(&m->mtx);

  assoc_rb_tree_t* tree = &m->map.left;  

  void* it = assoc_front(tree);
  void* end = assoc_end(tree);

  it = find_if(tree, it, end, (global_e2_node_id_t*)id, eq_global_e2_node_id_wrapper);
  assert(it != end && "E2 Node not found in the tree");

  sctp_info_t* s = assoc_value(tree, it);  

  printf("[NEAR-RIC]: nb_id %d port = %d  \n", id->nb_id, s->addr.sin_port);

  return *s;
}

