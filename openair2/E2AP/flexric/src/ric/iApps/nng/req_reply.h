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



#ifndef REQUEST_REPLY_SERVER
#define REQUEST_REPLY_SERVER 

#include "msgs/xapp_msgs.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  char url[64];
  uint64_t (*init_fp)(xapp_init_msg_t const* msg, void* data);
  bool (*keep_alive_fp)(xapp_keep_alive_msg_t const*, void* data);
  xapp_request_answer_e (*request_fp)(xapp_req_msg_t const*, void* data);
  void* data;
} req_reply_server_arg_t;


void init_req_reply_server_ping(req_reply_server_arg_t* arg);

void init_req_reply_server_msg(req_reply_server_arg_t* arg);


void stop_req_reply_server_ping(void);

void stop_req_reply_server_msg(void);

#endif

