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



#include "e42_iapp_api.h"

#include <assert.h>                                        // for assert                   
#include <pthread.h>                                       // for pthread_cr...            
#include <stdlib.h>
#include <stdio.h>                                         // for NULL                     
#include "e42_iapp.h"
#include "../../lib/ap/e2ap_types/common/e2ap_global_node_id.h"  // for global_e2_...
#include "../../lib/ap/e2ap_types/common/e2ap_plmn.h"            // for plmn_t
#include "../../util/ngran_types.h"                              // for ngran_gNB                
#include "../../util/conf_file.h"

static
e42_iapp_t* iapp = NULL;

static
pthread_t thrd_iapp;

static inline
void* static_start_iapp(void* a)
{
  (void)a;
  // Blocking...
  start_e42_iapp(iapp);
  return NULL;
}

void init_iapp_api(const char* addr, near_ric_if_t ric_if)
{
  assert(iapp == NULL);

  iapp = init_e42_iapp(addr, ric_if);
  assert(iapp->io.efd < 1024);

  // Spawn a new thread for the iapp
  const int rc = pthread_create(&thrd_iapp, NULL, static_start_iapp, NULL);
  assert(rc == 0);
}

void stop_iapp_api(void)
{
  assert(iapp != NULL);
  free_e42_iapp(iapp);
  int const rc = pthread_join(thrd_iapp,NULL);
  assert(rc == 0);
}

void add_e2_node_iapp_api(global_e2_node_id_t* id, size_t len, ran_function_t const ran_func[len])
{
  assert(iapp != NULL);
  assert(len > 0 && "E2 Node with no RAN functions??\n");
  add_e2_node_iapp(iapp, id, len , ran_func );
}

void rm_e2_node_iapp_api(global_e2_node_id_t* id)
{
  assert(iapp != NULL);
  assert(id != NULL);

  rm_e2_node_iapp(iapp, id);
}

void notify_msg_iapp_api(e2ap_msg_t const* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  notify_msg_iapp(iapp, msg);
}

