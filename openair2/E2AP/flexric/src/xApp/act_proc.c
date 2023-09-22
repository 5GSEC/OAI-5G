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


#include "act_proc.h"
#include "../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../util/alg_ds/alg/find.h"


#include <assert.h>
#include <pthread.h>

void init_act_proc(act_proc_t* p)
{
  assert(p != NULL);
  assoc_reg_init(&p->reg, sizeof(act_proc_val_t ));

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  int rc = pthread_mutex_init(&p->mtx, mtx_attr );
  assert(rc == 0);
}

void free_act_proc(act_proc_t* p)
{
  assert(p != NULL);

  assoc_reg_free(&p->reg);

  int rc = pthread_mutex_destroy(&p->mtx);
  assert(rc == 0);
}

void free_act_proc_val(void* value)
{
  assert(value != NULL);
  act_proc_val_t* v = (act_proc_val_t*) value;
  free_global_e2_node_id(&v->e2_node);
}

static
bool valid_proc_type(act_proc_val_e type)
{
  if(type == RIC_SUBSCRIPTION_PROCEDURE_ACTIVE
//     || type == RIC_SUBSCRIPTION_DELETE_PROCEDURE_ACTIVE
//     || type == RIC_INDICATION_PROCEDURE_ACTIVE
     || type == RIC_CONTROL_PROCEDURE_ACTIVE
   )
    return true;
  return false;
}

uint32_t add_act_proc(act_proc_t* p, act_proc_val_e type, ric_gen_id_t id, global_e2_node_id_t const* e2_node, void(*sm_cb)(sm_ag_if_rd_t const *))
{
  assert(p != NULL);
  assert(valid_proc_type(type) == true );

  lock_guard(&p->mtx);

  act_proc_val_t val = {  .type = type, 
                          .id = id,
                          .sm_cb = sm_cb,
                          .e2_node = cp_global_e2_node_id(e2_node)
                        };

  uint32_t const ric_req_id = assoc_reg_push_back(&p->reg, &val, sizeof(act_proc_val_t));
  return ric_req_id; 
}

void rm_act_proc(act_proc_t* p, uint16_t ric_req_id )
{
  assert(p != NULL);
  lock_guard(&p->mtx);

  void* it = assoc_reg_front(&p->reg);
  void* end = assoc_reg_end(&p->reg);

  it = find_reg(&p->reg, it, end, ric_req_id );
  assert(it != end && "ric_req_id key value not found in the registry" );
  void* next = assoc_reg_next(&p->reg, it);
  assoc_reg_erase(&p->reg, it, next, free_act_proc_val);
}

act_proc_ans_t find_act_proc(act_proc_t* act, uint16_t ric_req_id)
{
  assert(act != NULL);
  lock_guard(&act->mtx);

  void* it = assoc_reg_front(&act->reg);
  void* end = assoc_reg_end(&act->reg);

  it = find_reg(&act->reg, it, end, ric_req_id );

  if(it == end){
    act_proc_ans_t ans = {.ok = false,
                          .error = "ric_req_id not found in the registry" };     
    return ans;
  }


  assert(it != end && "ric_req_id key value not found in the registry" );

  act_proc_val_t* val = (act_proc_val_t*)assoc_reg_value(&act->reg ,it);
  val->id.ric_req_id = ric_req_id;
 
  act_proc_ans_t ans = {.ok = true,
                        .val = *val };     
  return ans;
}

