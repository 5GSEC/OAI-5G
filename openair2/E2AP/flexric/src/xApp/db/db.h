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

#ifndef DATABASE_XAPP_H
#define DATABASE_XAPP_H 

#include "../../lib/ap/e2ap_types/common/e2ap_global_node_id.h"
#include "../../sm/agent_if/read/sm_ag_if_rd.h"
#include "../../util/alg_ds/ds/tsn_queue/tsn_queue.h"

#include <pthread.h>

#ifdef SQLITE3_XAPP
  #include "sqlite3/sqlite3.h"
#endif

typedef struct{

#ifdef SQLITE3_XAPP
  sqlite3* handler;
#else
  static_assert(0!=0, "Unknown DB selected for the xApp"); 
#endif

  pthread_t p;
  tsnq_t q;
} db_xapp_t;

void init_db_xapp(db_xapp_t* db, char const* db_filename);

void close_db_xapp(db_xapp_t* db);

void write_db_xapp(db_xapp_t* db, global_e2_node_id_t const* id, sm_ag_if_rd_t const* rd);

#endif

