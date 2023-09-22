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

#if defined(__clang__) || defined (__GNUC__)
# define ATTRIBUTE_NO_SANITIZE_THREAD  __attribute__((no_sanitize("thread"))) 
#else
# define ATTRIBUTE_NO_SANITIZE_THREAD 
#endif


#include "../../../src/agent/e2_agent_api.h"
#include "sm_mac.h"
#include "sm_rlc.h"
#include "sm_pdcp.h"
#include "sm_gtp.h"
#include "sm_slice.h"
#include "sm_tc.h"
#include "sm_kpm.h"
#include "sm_rc.h"
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include <unistd.h>

static
void init_read_ind_tbl(read_ind_fp (*read_ind_tbl)[SM_AGENT_IF_READ_V0_END])
{
  (*read_ind_tbl)[MAC_STATS_V0] = read_mac_sm;
  (*read_ind_tbl)[RLC_STATS_V0] = read_rlc_sm ;
  (*read_ind_tbl)[PDCP_STATS_V0] = read_pdcp_sm ;
  (*read_ind_tbl)[SLICE_STATS_V0] = read_slice_sm ;
  (*read_ind_tbl)[TC_STATS_V0] = read_tc_sm ;
  (*read_ind_tbl)[GTP_STATS_V0] = read_gtp_sm ; 
  (*read_ind_tbl)[KPM_STATS_V3_0] = read_kpm_sm ; 
  (*read_ind_tbl)[RAN_CTRL_STATS_V1_03] = read_rc_sm;
}

static
void init_read_setup_tbl(read_e2_setup_fp (*read_setup_tbl)[SM_AGENT_IF_E2_SETUP_ANS_V0_END])
{
  (*read_setup_tbl)[MAC_AGENT_IF_E2_SETUP_ANS_V0] = read_mac_setup_sm;
  (*read_setup_tbl)[RLC_AGENT_IF_E2_SETUP_ANS_V0] = read_rlc_setup_sm ;
  (*read_setup_tbl)[PDCP_AGENT_IF_E2_SETUP_ANS_V0] = read_pdcp_setup_sm ;
  (*read_setup_tbl)[SLICE_AGENT_IF_E2_SETUP_ANS_V0] = read_slice_setup_sm ;
  (*read_setup_tbl)[TC_AGENT_IF_E2_SETUP_ANS_V0] = read_tc_setup_sm ;
  (*read_setup_tbl)[GTP_AGENT_IF_E2_SETUP_ANS_V0] = read_gtp_setup_sm ; 
  (*read_setup_tbl)[KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0] = read_kpm_setup_sm ; 
  (*read_setup_tbl)[RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0] = read_rc_setup_sm;
}

static
void init_write_ctrl( write_ctrl_fp (*write_ctrl_tbl)[SM_AGENT_IF_WRITE_CTRL_V0_END])
{
  (*write_ctrl_tbl)[MAC_CTRL_REQ_V0] = write_ctrl_mac_sm;
  (*write_ctrl_tbl)[RLC_CTRL_REQ_V0] =  write_ctrl_rlc_sm;
  (*write_ctrl_tbl)[PDCP_CTRL_REQ_V0] =  write_ctrl_pdcp_sm;
  (*write_ctrl_tbl)[SLICE_CTRL_REQ_V0] =  write_ctrl_slice_sm;
  (*write_ctrl_tbl)[TC_CTRL_REQ_V0] =  write_ctrl_tc_sm;
  (*write_ctrl_tbl)[GTP_CTRL_REQ_V0] =  write_ctrl_gtp_sm;
  (*write_ctrl_tbl)[RAN_CONTROL_CTRL_V1_03] =  write_ctrl_rc_sm;
}


static
void init_write_subs(write_subs_fp (*write_subs_tbl)[SM_AGENT_IF_WRITE_SUBS_V0_END])
{
  (*write_subs_tbl)[MAC_SUBS_V0] = NULL;
  (*write_subs_tbl)[RLC_SUBS_V0] = NULL;
  (*write_subs_tbl)[PDCP_SUBS_V0] = NULL;
  (*write_subs_tbl)[SLICE_SUBS_V0] = NULL;
  (*write_subs_tbl)[TC_SUBS_V0] = NULL;
  (*write_subs_tbl)[GTP_SUBS_V0] = NULL;
  (*write_subs_tbl)[KPM_SUBS_V3_0] = NULL;
  (*write_subs_tbl)[RAN_CTRL_SUBS_V1_03] = write_subs_rc_sm;
}

static
sm_io_ag_ran_t init_io_ag()
{
  sm_io_ag_ran_t io = {0};
  init_read_ind_tbl(&io.read_ind_tbl);
  init_read_setup_tbl(&io.read_setup_tbl);
  init_write_ctrl(&io.write_ctrl_tbl);
  init_write_subs(&io.write_subs_tbl);

  return io;
}

/*
static
void read_RAN(sm_ag_if_rd_t* ag_rd)
{
  assert(ag_rd->type == INDICATION_MSG_AGENT_IF_ANS_V0
      || ag_rd->type == E2_SETUP_AGENT_IF_ANS_V0);

  if(ag_rd->type == INDICATION_MSG_AGENT_IF_ANS_V0)
      read_ind_tbl[ag_rd->ind.type](&ag_rd->ind);
  else if(ag_rd->type == E2_SETUP_AGENT_IF_ANS_V0 )
      read_setup_tbl[ag_rd->e2ap.type](&ag_rd->e2ap);
  else
    assert(0!=0 && "Unknown type");
}

static
sm_ag_if_ans_t write_RAN(sm_ag_if_wr_t const* ag_wr)
{
  assert(ag_wr!= NULL);
  assert(ag_wr->type == CONTROL_SM_AG_IF_WR
        || ag_wr->type == SUBSCRIPTION_SM_AG_IF_WR);

  if(ag_wr->type == CONTROL_SM_AG_IF_WR){
    return write_ctrl_tbl[ag_wr->ctrl.type](&ag_wr->ctrl);
  }
  return write_subs_tbl[ag_wr->subs.type](&ag_wr->subs);
}

*/


ATTRIBUTE_NO_SANITIZE_THREAD static 
void stop_and_exit()
{
  // Stop the E2 Agent
  stop_agent_api();
  exit(EXIT_SUCCESS);
}

static 
pthread_once_t once = PTHREAD_ONCE_INIT;

static
void sig_handler(int sig_num)
{
  printf("\nEnding the E2 Agent with signal number = %d\n. Please, wait.", sig_num);
  // For the impatient, do not break my code
  pthread_once(&once, stop_and_exit);
}

int main(int argc, char *argv[])
{
  // Signal handler
  signal(SIGINT, sig_handler);

  // Init the Agent
  // Values defined in the CMakeLists.txt file
  const ngran_node_t ran_type = TEST_AGENT_RAN_TYPE;
  const int mcc = TEST_AGENT_MCC;
  const int mnc = TEST_AGENT_MNC;
  const int mnc_digit_len = TEST_AGENT_MNC_DIG_LEN;
  const int nb_id = TEST_AGENT_NB_ID;
  const int cu_du_id = TEST_AGENT_CU_DU_ID;

  sm_io_ag_ran_t io = init_io_ag();

  fr_args_t args = init_fr_args(argc, argv);

  if (NODE_IS_MONOLITHIC(ran_type))
    printf("[E2 AGENT]: nb_id %d, mcc %d, mnc %d, mnc_digit_len %d, ran_type %s\n", nb_id, mcc, mnc, mnc_digit_len, get_ngran_name(ran_type));
  else
    printf("[E2 AGENT]: nb_id %d, mcc %d, mnc %d, mnc_digit_len %d, ran_type %s, cu_du_id %d\n", nb_id, mcc, mnc, mnc_digit_len, get_ngran_name(ran_type), cu_du_id);

  init_agent_api(mcc, mnc, mnc_digit_len, nb_id, cu_du_id, ran_type, io, &args);

  while(1){
    poll(NULL, 0, 1000);
  }

  return EXIT_SUCCESS;
}

