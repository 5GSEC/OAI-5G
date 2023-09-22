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

#include "../util/alg_ds/ds/lock_guard/lock_guard.h"
#include "sync_ui.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>

void init_sync_ui(sync_ui_t* s)
{
  assert(s != NULL);

  const pthread_condattr_t* cond_attr = NULL;
  int rc = pthread_cond_init(&s->cv_sync, cond_attr);
  assert(rc == 0);

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK; 
#endif

  rc = pthread_mutex_init(&s->mtx_sync, mtx_attr );
  assert(rc == 0);

  s->wait_ms = 5000;

  s->flag_sync = false;
  s->msg_ack = false;
}

void free_sync_ui(sync_ui_t* s)
{
  assert(s != NULL);
  // Liberate the thread??? if waiting
  s->flag_sync = true; 

  int rc = pthread_cond_destroy(&s->cv_sync);
  assert(rc == 0);

  rc = pthread_mutex_destroy(&s->mtx_sync);
  assert(rc == 0);
}

void cond_wait_sync_ui(sync_ui_t* s, uint32_t wait_ms)
{
  assert(s != NULL);
  assert(wait_ms > 0);

  pthread_mutex_lock(&s->mtx_sync);

  s->flag_sync = false;
  s->msg_ack = false;
  struct timespec ts = {0};
  int rc = clock_gettime(CLOCK_REALTIME, &ts);
  assert(rc == 0);
  ts.tv_sec += wait_ms/1000 + 1;

  while(s->flag_sync == false && rc == 0) {
    rc = pthread_cond_timedwait(&s->cv_sync, &s->mtx_sync, &ts);
  }

  assert(rc != ETIMEDOUT && "Timeout. No response from the E2 Node received, and neither from epoll. Unforeseen path");
  assert(rc == 0);

  pthread_mutex_unlock(&s->mtx_sync);

  assert(s->msg_ack == true && "No response to subscription from the RIC received\n" );
}

void signal_sync_ui(sync_ui_t* s)
{
  assert(s != NULL);

  lock_guard(&s->mtx_sync);
  s->flag_sync = true;
  s->msg_ack = true;
  pthread_cond_signal(&s->cv_sync); 
}

