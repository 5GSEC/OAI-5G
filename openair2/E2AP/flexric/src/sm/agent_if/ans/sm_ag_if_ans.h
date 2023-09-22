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

#ifndef SM_ANSWER_INTERFACE_H
#define SM_ANSWER_INTERFACE_H

#include "../../mac_sm/ie/mac_data_ie.h"
#include "../../rlc_sm/ie/rlc_data_ie.h"
#include "../../pdcp_sm/ie/pdcp_data_ie.h"
#include "../../slice_sm/ie/slice_data_ie.h"
#include "../../tc_sm/ie/tc_data_ie.h"
#include "../../gtp_sm/ie/gtp_data_ie.h"
#include "../../rc_sm/ie/rc_data_ie.h"

////////////////////
// Control Outcome
////////////////////

typedef enum{
  MAC_AGENT_IF_CTRL_ANS_V0, 
  RLC_AGENT_IF_CTRL_ANS_V0, 
  PDCP_AGENT_IF_CTRL_ANS_V0, 
  SLICE_AGENT_IF_CTRL_ANS_V0, 
  TC_AGENT_IF_CTRL_ANS_V0,
  GTP_AGENT_IF_CTRL_ANS_V0,
  RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0,

  SM_AGENT_IF_CTRL_ANS_V0_END,
} sm_ag_if_ans_ctrl_e;

typedef struct{
  sm_ag_if_ans_ctrl_e type;
  union {
    mac_ctrl_out_t mac;
    rlc_ctrl_out_t rlc;
    pdcp_ctrl_out_t pdcp;
    slice_ctrl_out_t slice;
    tc_ctrl_out_t tc;
    gtp_ctrl_out_t gtp;
    e2sm_rc_ctrl_out_t rc;
  };
} sm_ag_if_ans_ctrl_t;

typedef enum {
  CTRL_OUTCOME_SM_AG_IF_ANS_V0,
  NONE_SM_AG_IF_ANS_V0,

  END_SM_AG_IF_ANS_V0,
} sm_ag_if_ans_e;

typedef struct{
  sm_ag_if_ans_e type; 
  union{
    sm_ag_if_ans_ctrl_t ctrl_out;
  };
} sm_ag_if_ans_t; 

#endif

