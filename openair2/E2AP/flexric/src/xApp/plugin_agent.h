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



#ifndef E2_PLUGIN_AGENT_H
#define E2_PLUGIN_AGENT_H

#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>

#include "util/alg_ds/ds/assoc_container/assoc_generic.h"

#include "sm/sm_agent.h"
#include "sm/sm_io.h"


typedef struct
{
  sm_io_ag_ran_t io;
  const char* dir_path;

  // Registered SMs
  assoc_rb_tree_t sm_ds; // key: ran_func_id, value: sm_agent_t* 
  pthread_mutex_t sm_ds_mtx;

  int sockfd;
  pthread_t thread_rx;

  atomic_bool flag_shutdown;
} plugin_ag_t;


void init_plugin_ag(plugin_ag_t* p, const char* path, sm_io_ag_ran_t io);

void free_plugin_ag(plugin_ag_t* p);

void load_plugin_ag(plugin_ag_t* p, const char* path);

void unload_plugin_ag(plugin_ag_t* p, uint16_t key);

sm_agent_t* sm_plugin_ag(plugin_ag_t* p, uint16_t key);

size_t size_plugin_ag(plugin_ag_t* p);


#endif

