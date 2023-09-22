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

#include "../../util/alg_ds/alg/alg.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "map_ric_id.h"
#include "xapp_ric_id.h"

#include <assert.h>

static inline
int cmp_uint16(const void* m0_v, const void* m1_v)
{
  uint16_t* m0 = (uint16_t*)m0_v;
  uint16_t* m1 = (uint16_t*)m1_v;
  if(*m0 < *m1)
    return -1;
  else if(*m0 == * m1)
    return 0;

  return 1;
}

static
void free_xapp_ric_gen_id(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  xapp_ric_id_t* id = (xapp_ric_id_t*)value;
  free(id);
}

static
void free_e2_node_ric_req(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  e2_node_ric_req_t* n = ( e2_node_ric_req_t* )value;

  free_global_e2_node_id(&n->e2_node_id);
  free(n);
}


static inline
int cmp_e2_node_ric_req_wrapper(void const* m0_v, void const* m1_v)
{
  assert(m0_v != NULL);
  assert(m1_v != NULL);

  e2_node_ric_req_t* m0 = (e2_node_ric_req_t*)m0_v;
  e2_node_ric_req_t* m1 = (e2_node_ric_req_t*)m1_v;

  return cmp_uint16(&m0->ric_req_id, &m1->ric_req_id);
} 

void init_map_ric_id(map_ric_id_t* map)
{
  assert(map != NULL);

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  int rc = pthread_mutex_init(&map->mtx, mtx_attr);
  assert(rc == 0);

  size_t key_sz_1 = sizeof(e2_node_ric_req_t);
  size_t key_sz_2 = sizeof(xapp_ric_id_t); //);

  bi_map_init(&map->bimap, key_sz_1, key_sz_2, cmp_e2_node_ric_req_wrapper, cmp_xapp_ric_gen_id_wrapper, free_xapp_ric_gen_id, free_e2_node_ric_req);
}

void free_map_ric_id(map_ric_id_t* map)
{
  assert(map != NULL);

  int rc = pthread_mutex_destroy(&map->mtx);
  assert(rc == 0);

  bi_map_free(&map->bimap);
//  assoc_free(&map->tree);
}

static inline
bool eq_uint16(void const* m0, void const* m1)
{
  return *(uint16_t*)m0 == *(uint16_t*)m1;
}

static inline
bool eq_e2_node_ric_req(void const* m0_v, void const* m1_v)
{
  assert(m0_v != NULL);
  assert(m1_v != NULL);

  e2_node_ric_req_t* m0 = (e2_node_ric_req_t*)m0_v;
  e2_node_ric_req_t* m1 = (e2_node_ric_req_t*)m1_v;

  return m0->ric_req_id == m1->ric_req_id;
}

void add_map_ric_id(map_ric_id_t* map, e2_node_ric_req_t* node, xapp_ric_id_t* x)
{
  assert(map != NULL);
  assert(node != NULL);
  assert(x != NULL);

  // The lock must be already acquired
//  lock_guard(&map->mtx);

  assoc_rb_tree_t* left = &map->bimap.left; 

  void* it = assoc_front(left);
  void* end = assoc_end(left);
  it = find_if(left, it, end, node, eq_e2_node_ric_req);
  assert(it == end && "ric_req_id already in the map");

  bi_map_insert(&map->bimap, node, sizeof(e2_node_ric_req_t), x, sizeof(xapp_ric_id_t));
}

void rm_map_ric_id(map_ric_id_t* map, e2_node_ric_req_t* node)
{
  assert(map != NULL);
  assert(node != NULL);

  lock_guard(&map->mtx);

  xapp_ric_id_t* id = bi_map_extract_left(&map->bimap, node, sizeof( e2_node_ric_req_t )); //  &ric_req_id, sizeof(uint16_t));
//  xapp_ric_id_t* id = assoc_extract(&map->tree ,&ric_req_id);

  free(id);
}

xapp_ric_id_t find_xapp_map_ric_id(map_ric_id_t* map, uint16_t ric_req_id)
{
  assert(map != NULL);
  assert(ric_req_id > 0 );

  lock_guard(&map->mtx);

  assoc_rb_tree_t* left = &map->bimap.left; 

  e2_node_ric_req_t dummy_node = { //  global_e2_node_id_t e2_node_id;
                                      .ric_req_id = ric_req_id
                                  }; 

  void* it = assoc_front(left);
  void* end = assoc_end(left);
  it = find_if(left , it, end, &dummy_node, eq_e2_node_ric_req );
  assert(it != end && "Not found RIC Request ID");

  xapp_ric_id_t* id = assoc_value(left, it);
  return *id;
}

e2_node_ric_req_t find_ric_req_map_ric_id(map_ric_id_t* map, xapp_ric_id_t* x)
{
  assert(map != NULL);

  lock_guard(&map->mtx);

  assoc_rb_tree_t* r = &map->bimap.right; 

  void* it = assoc_front(r);
  void* end = assoc_end(r);
  it = find_if(r , it, end, x , eq_xapp_ric_gen_id_wrapper);
  assert(it != end && "Not found xApp RIC ID ");

  e2_node_ric_req_t* id = assoc_value(r, it);
  return *id;
}

