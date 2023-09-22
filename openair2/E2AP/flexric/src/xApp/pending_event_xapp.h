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

#ifndef PENDING_EVENT_XAPP_H
#define PENDING_EVENT_XAPP_H

#include "../lib/pending_events.h"
#include "../lib/ap/e2ap_types/common/ric_gen_id.h"

#include <pthread.h>
#include "../util/alg_ds/ds/assoc_container/bimap.h"

typedef struct{
  pending_event_t ev;
  ric_gen_id_t id;
  int wait_ms;
} pending_event_xapp_t ;

bool eq_pending_event_xapp(pending_event_xapp_t* m0, pending_event_xapp_t* m1);


typedef struct{
  bi_map_t pending; // left: fd, right: pending_event_xapp_t   
  pthread_mutex_t pend_mtx;
} pending_event_xapp_ds_t;


void init_pending_events( pending_event_xapp_ds_t* ds);

void free_pending_events( pending_event_xapp_ds_t* ds);

bool find_pending_event_fd(pending_event_xapp_ds_t* p, int fd);

bool find_pending_event_ev(pending_event_xapp_ds_t* p, pending_event_xapp_t* ev);

void add_pending_event( pending_event_xapp_ds_t* ds, int fd, pending_event_xapp_t* ev );

int* rm_pending_event_ev(pending_event_xapp_ds_t* ds, pending_event_xapp_t* ev );

void rm_pending_event_fd(pending_event_xapp_ds_t* ds, int fd);


#endif

