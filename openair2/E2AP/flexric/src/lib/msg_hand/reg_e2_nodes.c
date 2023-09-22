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


#include "reg_e2_nodes.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../util/alg_ds/alg/find.h"
#include "../../util/alg_ds/alg/alg.h" 

#include "../../lib/ap/e2ap_types/common/e2ap_global_node_id.h"


#include <assert.h>
#include <pthread.h>

static
void free_ran_function_pointer(void* rf_p)
{
  assert(rf_p != NULL);
  ran_function_t* rf  = *(ran_function_t**)rf_p;
  free_ran_function_wrapper(rf);
  free(rf);  
}


static inline
void free_e2_nodes(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;
  seq_arr_t* arr_node = (seq_arr_t*)value;
  //assert(seq_size(arr_node) == 2);
  seq_free_func f =  free_ran_function_pointer; 
  seq_arr_free(arr_node, f);
  free(arr_node);

  global_e2_node_id_t* e2node = (global_e2_node_id_t*) key;
  free_global_e2_node_id(e2node);
}


void init_reg_e2_node(reg_e2_nodes_t* i)
{
  assert(i != NULL);

  assoc_init(&i->node_to_rf, sizeof(global_e2_node_id_t), cmp_global_e2_node_id_wrapper, free_e2_nodes);

  pthread_mutexattr_t* attr = NULL;
#ifdef DEBUG
  *attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  int const rc = pthread_mutex_init(&i->mtx, attr);
  assert(rc == 0);
}

void free_reg_e2_node(reg_e2_nodes_t* i)
{
  assert(i != NULL);
  assoc_free(&i->node_to_rf);
//  assoc_free(&i->rf_to_node);

  int const rc = pthread_mutex_destroy(&i->mtx);
  assert(rc == 0);
}

void add_reg_e2_node(reg_e2_nodes_t* i, global_e2_node_id_t const* id, size_t len, ran_function_t const ran_func[len])
{
  assert(i != NULL);
  assert(id != NULL);
  assert(len > 0);
  assert(ran_func != NULL);

  seq_arr_t* arr = calloc(1, sizeof(seq_arr_t));
  seq_init(arr, sizeof(ran_function_t*));

  for(size_t i = 0; i < len; ++i){
    ran_function_t* tmp = calloc(1, sizeof(ran_function_t) );
    *tmp = cp_ran_function(&ran_func[i]);
    seq_push_back(arr, &tmp, sizeof(ran_function_t*) );
  }

  lock_guard(&i->mtx);

  if(assoc_size(&i->node_to_rf) > 0){
    void* it_node = assoc_front(&i->node_to_rf);
    void* end_node = assoc_end(&i->node_to_rf);

    it_node = find_if(&i->node_to_rf, it_node, end_node, id, eq_global_e2_node_id_wrapper );
    assert(it_node == end_node && "Trying to add an already existing E2 Node");
  }

//  if(it_node != end_node) {
//    seq_arr_t* old_arr = assoc_extract(&i->node_to_rf,(global_e2_node_id_t*)id);
//    seq_free(old_arr, free_ran_function_wrapper);
//    free(old_arr);
//  }

  assoc_insert(&i->node_to_rf, id, sizeof(global_e2_node_id_t), arr);

//  void* it_n = assoc_front(&i->node_to_rf);
//  seq_arr_t* arr_tmp = assoc_value(&i->node_to_rf, it_n); 
//  assert(seq_size(arr_tmp) == 2 && "Only for current test valid");
}

static inline
assoc_rb_tree_t available_e2node(reg_e2_nodes_t* n, size_t len, ran_function_t rf[len])
{
  assert(n != NULL);
  assert(len > 0);
  assert(rf != NULL);

  assoc_rb_tree_t ret = {0}; 
  assoc_init(&ret, sizeof(global_e2_node_id_t), cmp_global_e2_node_id_wrapper, free_e2_nodes);


  lock_guard(&n->mtx);

  void* it = assoc_front(&n->node_to_rf);
  void* end = assoc_end(&n->node_to_rf);

  while(it != end){ // per E2 Node
    seq_arr_t* arr_v = assoc_value(&n->node_to_rf, it); 
    const size_t sz = seq_size(arr_v); 
    for(size_t i = 0; i < len; ++i){
      for(size_t j = 0; j < sz ; ++j){
        ran_function_t* r = *(ran_function_t**)seq_at(arr_v, j); 
        // Add the RAN functions that have permission
        if(eq_ran_function(r,&rf[i]) == true){
          global_e2_node_id_t* id = assoc_key(&ret, it);

          void* it_ret = assoc_front(&ret);
          void* end_ret = assoc_end(&ret);

          it_ret = find_if(&ret, it_ret, end_ret, id, eq_global_e2_node_id_wrapper); 
          seq_arr_t* arr = NULL; 
          if(it_ret == end_ret){
            arr = calloc(1, sizeof(seq_arr_t));
            assert(arr != NULL && "memory exhasuted");
            seq_init(arr,sizeof(ran_function_t*));
            assoc_insert(&ret, id, sizeof(global_e2_node_id_t), arr);
          } else{
            arr = assoc_value(&ret, it_ret);
          }
          ran_function_t* tmp = calloc(1, sizeof(ran_function_t));
          assert(tmp != NULL);
          *tmp = cp_ran_function(r);
          seq_push_back(arr, tmp, sizeof(ran_function_t*));
        }
      }
    }
    it = assoc_next(&n->node_to_rf, it);
  }

  return ret;
}

size_t sz_reg_e2_node(reg_e2_nodes_t* n)
{
  assert(n != NULL);

  lock_guard(&n->mtx);
  return assoc_size(&n->node_to_rf);
}


assoc_rb_tree_t cp_reg_e2_node(reg_e2_nodes_t* n)
{
  assert(n != 0);

  // key:global_e2_node_id_t | value: seq_arr_t* of ran_function_t
  assoc_rb_tree_t ans = {0};
  assoc_init(&ans, sizeof(global_e2_node_id_t), cmp_global_e2_node_id_wrapper, free_e2_nodes);

  lock_guard(&n->mtx);

  void* it = assoc_front(&n->node_to_rf);
  void* end = assoc_end(&n->node_to_rf);

  while(it != end){
    global_e2_node_id_t* tmp = assoc_key(&n->node_to_rf, it );
    global_e2_node_id_t cp_tmp = cp_global_e2_node_id(tmp);

    seq_arr_t* arr = assoc_value(&n->node_to_rf, it);
    seq_arr_t* new_arr = calloc(1, sizeof(seq_arr_t)); 
    assert(new_arr != NULL);
    seq_init(new_arr, sizeof(ran_function_t*));
    void* it_arr = seq_front(arr);
    void* end_arr = seq_end(arr);
    while(it_arr != end_arr){
      ran_function_t* r = *(ran_function_t**)it_arr; 
      ran_function_t* tmp_r = calloc(1, sizeof(ran_function_t));
      assert(tmp_r != NULL);
      *tmp_r = cp_ran_function(r);
      seq_push_back(new_arr, &tmp_r, sizeof(ran_function_t*));

      it_arr = seq_next(arr, it_arr);
    }
    assoc_insert(&ans,&cp_tmp, sizeof(global_e2_node_id_t), new_arr);

    it = assoc_next(&n->node_to_rf, it);
  }

  assert(assoc_size(&ans) == assoc_size(&n->node_to_rf) );

  return ans;
}

e2_node_arr_t generate_e2_node_arr(reg_e2_nodes_t* n)
{ 
  assoc_rb_tree_t t = cp_reg_e2_node(n);
  defer({ assoc_free(&t); }; );

  e2_node_arr_t ans = {0};
  ans.len = assoc_size(&t);

  if(ans.len > 0){
    ans.n = calloc(ans.len, sizeof(e2_node_connected_t) );
    assert(ans.n != NULL && "Memory exhausted" );
  }

  uint32_t i = 0;
  void* it = assoc_front(&t);
  void* end = assoc_end(&t);
  while(it != end){
   global_e2_node_id_t* tmp_id = assoc_key(&t, it);        
   ans.n[i].id = cp_global_e2_node_id(tmp_id);

   seq_arr_t* tmp_arr = assoc_value(&t, it);
   assert(tmp_arr->elt_size == sizeof(ran_function_t*));
   
   const size_t sz = seq_size(tmp_arr);

   ans.n[i].len_rf = sz;
   if(sz > 0){
    ans.n[i].ack_rf = calloc(sz, sizeof(ran_function_t));
    assert(ans.n[i].ack_rf != NULL && "memory exhausted");
   }
    
   for(size_t j = 0; j < sz; ++j){
    ran_function_t* r = *(ran_function_t**)seq_at(tmp_arr, j);
    ans.n[i].ack_rf[j] = cp_ran_function(r);
   }

    i += 1;
    it = assoc_next(&t, it);
  }

  return ans;
}

void rm_reg_e2_node(reg_e2_nodes_t* n, global_e2_node_id_t const* id)
{
  assert(n != NULL);
  assert(id != NULL);

  printf("[NEAR-RIC]: Removing E2 Node MCC %d MNC %d NB_ID %u \n", id->plmn.mcc, id->plmn.mnc, id->nb_id);

  {
    lock_guard(&n->mtx);

    void* it = assoc_front(&n->node_to_rf);
    void* end = assoc_end(&n->node_to_rf);
    it = find_if(&n->node_to_rf, it, end, id, eq_global_e2_node_id_wrapper );
    assert(it != end && "Not registed e2 Node passed");

    seq_arr_t* arr = assoc_extract(&n->node_to_rf, (global_e2_node_id_t*)id);;

    free_e2_nodes((void*)id , arr);
  }
}

