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



#include "keep_alive_timer.h"

#include <assert.h>                        // for assert
#include <bits/types/struct_itimerspec.h>  // for itimerspec
#include <pthread.h>                       // for pthread_create, pthread_join
#include <stdatomic.h>                     // for atomic_bool
#include <stdbool.h>                       // for true, false
#include <stdio.h>                         // for NULL
#include <stdlib.h>                        // for calloc, free
#include <sys/epoll.h>                     // for epoll_event, epoll_create1
#include <sys/time.h>                      // for CLOCK_MONOTONIC
#include <sys/timerfd.h>                   // for timerfd_create, timerfd_se...
#include <time.h>                          // for timespec
#include <unistd.h>                        // for close, read, ssize_t

static
atomic_bool stop_flag = true;

static
int efd = -1;

static
int tfd = -1;

static
pthread_t keep_alive_run_thread;

typedef struct
{
  uint32_t exp_time_ms;
  void (*fun)(void* data);
  void* data;
} keep_alive_t;


static
void* run_keep_alive_timer(void* arg)
{
  assert(arg != NULL);
  keep_alive_t* t_data = (keep_alive_t*)(arg); 

  // Create the timer
  const int clockid = CLOCK_MONOTONIC;
  const int flags = TFD_NONBLOCK | TFD_CLOEXEC;
  tfd = timerfd_create(clockid, flags);
  assert(tfd != -1);

  const int flags_2 = 0;
  struct itimerspec *old_value = NULL; // not interested in how the timer was previously configured
  int const t_sec = t_data->exp_time_ms / 1000; 
  int const t_nsec = (t_data->exp_time_ms % 1000) * 1000000;  
    
  const struct timespec it_interval = {.tv_sec = t_sec, .tv_nsec = t_nsec};  /* Interval for periodic timer */
  const struct timespec it_value = {.tv_sec =  t_sec, .tv_nsec = t_nsec};     /* Initial expiration */
  const struct itimerspec new_value = {.it_interval = it_interval, .it_value = it_value}; 
  int rc = timerfd_settime(tfd, flags_2, &new_value, old_value);
  assert(rc != -1);

  // Create epoll
  const int flags_3 = EPOLL_CLOEXEC; 
  efd = epoll_create1(flags_3);  
  assert(efd != -1);

  const int op = EPOLL_CTL_ADD;

  const epoll_data_t e_data = {.fd = tfd};
  const int e_events = EPOLLIN ; // open for reading
  struct epoll_event event = {.events = e_events, .data = e_data};
  rc = epoll_ctl(efd, op, tfd, &event);
  assert(rc != -1);

  const int maxevents = 32;
  struct epoll_event events[maxevents];

  // Every 1 sec. check whether the stop flag was set
  const int timeout_ms = 1000;
  while(stop_flag == false){
    const int events_ready = epoll_wait(efd, events, maxevents, timeout_ms); 
    assert(events_ready != -1);
    for(int i =0; i < events_ready; ++i){
      assert((events[i].events & EPOLLERR) == 0);
      const int cur_fd = events[i].data.fd; 
      if (cur_fd == tfd){
        uint64_t res;
        ssize_t r = read(tfd, &res, sizeof(uint64_t));
        assert(r != 0);
        if(t_data->fun != NULL)
          t_data->fun(t_data->data);
      }
    }
  }

  free(t_data);
  return NULL;
}

void init_keep_alive_timer(uint32_t exp_time_ms, void (*fun)(void*), void* data)
{
  assert(exp_time_ms > 0);
  assert(stop_flag == true && "Called twice this function? stop_flag is init as static variable to true");
  stop_flag = false;
  
  keep_alive_t* arg = calloc(1, sizeof(keep_alive_t));
  assert(arg != NULL && "Memory exhausted");
  arg->exp_time_ms = exp_time_ms;
  arg->fun = fun;
  arg->data = data;

  const pthread_attr_t *restrict attr = NULL;
  int rc = pthread_create(&keep_alive_run_thread, attr, run_keep_alive_timer, arg);
  assert(rc == 0 );
}

void stop_keep_alive_timer(void)
{
  stop_flag = true;

  void **retval = NULL;
  int rc = pthread_join(keep_alive_run_thread, retval);
  assert(rc == 0);

  rc = close(efd);
  assert(rc != -1);

	rc = close(tfd); 
  assert(rc != -1);
}

