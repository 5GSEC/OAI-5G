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


#ifndef DATABASE_XAPP_GENERIC_H
#define DATABASE_XAPP_GENERIC_H 


#include "sqlite3/sqlite3_wrapper.h"



#define init_db_gen(T,U) _Generic ((T), \
                                    sqlite3*:  init_db_sqlite3, \
                                    default:   init_db_sqlite3) (T,U)

#define close_db_gen(T) _Generic ((T),\
                                    sqlite3*: close_db_sqlite3, \
                                    default:  close_db_sqlite3) (T)


#define write_db_gen(T,U,V) _Generic ((T),\
                                    sqlite3*:   write_db_sqlite3, \
                                    default:    write_db_sqlite3) (T,U,V)

#endif

