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

#include "sctp_msg.h"

void free_sctp_msg(sctp_msg_t* rcv)
{
  assert(rcv != NULL);

 if(rcv->type == SCTP_MSG_PAYLOAD)
  free_byte_array(rcv->ba);
}


static
int cmp_sockaddr_in(struct sockaddr_in const* m0, struct sockaddr_in const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->sin_port < m1->sin_port)
    return 1;

  if(m0->sin_port > m1->sin_port)
    return -1;

  if(m0->sin_addr.s_addr < m1->sin_addr.s_addr)
    return 1;

  if(m0->sin_addr.s_addr > m1->sin_addr.s_addr)
    return -1;

  if(m0->sin_family < m1->sin_family)
    return 1;

  if(m0->sin_family > m1->sin_family)
    return -1;

  return 0;
}

static
int cmp_sctp_sndrcvinfo(struct sctp_sndrcvinfo* m0, struct sctp_sndrcvinfo* m1)
{
  assert(m0 != NULL && m1 != NULL);

/*
  struct sctp_sndrcvinfo {
     uint16_t sinfo_stream;
     uint16_t sinfo_ssn;
     uint16_t sinfo_flags;
     uint32_t sinfo_ppid;
     uint32_t sinfo_context;
     uint32_t sinfo_timetolive;
     uint32_t sinfo_tsn;
     uint32_t sinfo_cumtsn;
     sctp_assoc_t sinfo_assoc_id;
   };
*/

  return 0;
}

int cmp_sctp_info_wrapper(void const* m0_v, void const* m1_v)
{
  if(m0_v == NULL && m1_v == NULL) 
    return 0;

  if(m0_v == NULL)
    return 1;

  if(m1_v == NULL)
    return -1;

  sctp_info_t* m0 = (sctp_info_t*)m0_v; 
  sctp_info_t* m1 = (sctp_info_t*)m1_v; 

  int cmp = cmp_sockaddr_in(&m0->addr, &m1->addr);

  if(cmp != 0) return cmp;

  return cmp_sctp_sndrcvinfo(&m0->sri, &m1->sri); 
}

bool eq_sctp_info_wrapper(void const* m0, void const* m1)
{
  return cmp_sctp_info_wrapper(m0,m1) == 0;
}

