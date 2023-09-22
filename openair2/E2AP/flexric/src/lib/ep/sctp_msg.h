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

#ifndef SCTP_MSG_H
#define SCTP_MSG_H

#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> 

#include <stdbool.h>
#include "util/byte_array.h"

typedef enum {
  SCTP_MSG_PAYLOAD,
  SCTP_MSG_NOTIFICATION
} sctp_msg_type_t;

typedef struct{
  struct sockaddr_in addr; 
  struct sctp_sndrcvinfo sri;
} sctp_info_t ;

int cmp_sctp_info_wrapper(void const* m0, void const* m1);

bool eq_sctp_info_wrapper(void const* m0, void const* m1);

typedef struct{
  sctp_msg_type_t type;
  sctp_info_t info;
  union{
    byte_array_t ba;
    union sctp_notification notif;
  };
} sctp_msg_t;

void free_sctp_msg(sctp_msg_t* rcv);

#endif

