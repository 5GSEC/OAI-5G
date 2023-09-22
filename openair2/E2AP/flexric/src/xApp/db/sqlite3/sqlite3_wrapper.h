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


#ifndef SQLITE3_WRAPPER_XAPP_H
#define SQLITE3_WRAPPER_XAPP_H 

#include "../../../sm/agent_if/read/sm_ag_if_rd.h"
#include "../../../lib/ap/e2ap_types/common/e2ap_global_node_id.h" 

#include "sqlite3.h"

void init_db_sqlite3(sqlite3** db, char const* db_filename);

void close_db_sqlite3(sqlite3* db);

void write_db_sqlite3(sqlite3* db, global_e2_node_id_t const* id, sm_ag_if_rd_t const* rd);

#endif

