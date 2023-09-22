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



#ifndef E2_PLUGIN_RIC_H
#define E2_PLUGIN_RIC_H 

#include <stddef.h>
#include <stdint.h>

#include "util/alg_ds/ds/assoc_container/assoc_generic.h"

#include "sm/sm_ric.h"
#include "sm/sm_io.h"

typedef struct
{
  const char* dir_path;

  // Registered SMs
  assoc_rb_tree_t sm_ds; // key: ran_func_id, value: sm_ric_t* 

} plugin_ric_t;

void init_plugin_ric(plugin_ric_t* p, const char* dir_path);

void free_plugin_ric(plugin_ric_t* p);

void load_plugin_ric(plugin_ric_t* p, const char* file_path);

void unload_plugin_ric(plugin_ric_t* p, uint16_t key);

sm_ric_t* sm_plugin_ric(plugin_ric_t* p, uint16_t key);

size_t size_plugin_ric(plugin_ric_t* p);

void tx_plugin_ric(plugin_ric_t* p, size_t len, char const file_path[len]);

#endif

