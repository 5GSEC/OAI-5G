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


#ifndef ASYNC_INPUT_OUTPUT_AGENT_H
#define ASYNC_INPUT_OUTPUT_AGENT_H

typedef struct{
  int r;
  int w;
} fd_pair_t;

typedef struct{
  // epoll based fd
  int efd; 
  // Aperiodic events
  // pipe fd, for communication with epoll
  fd_pair_t pipe; 
} asio_agent_t;

void init_asio_agent(asio_agent_t* io);

void add_fd_asio_agent(asio_agent_t* io, int fd);

void rm_fd_asio_agent(asio_agent_t* io, int fd);

int create_timer_ms_asio_agent(asio_agent_t* io, long initial_ms, long interval_ms);

int event_asio_agent(asio_agent_t const* io);

#endif

