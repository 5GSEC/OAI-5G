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

#include "../../../../src/xApp/e42_xapp_api.h"
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/sm/kpm_sm_v03.00/kpm_sm_id.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type ==INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V0);

  const kpm_ind_hdr_t* hdr = &rd->kpm_stats.hdr;
  printf("received KPM indication at %d (sender '%s', type '%s', vendor '%s')\n",
         hdr->collectStartTime, (char *)(hdr->sender_name.buf), (char *)(hdr->sender_type.buf), (char *)(hdr->vendor_name.buf));

  const kpm_ind_msg_t* msg = &rd->kpm_stats.msg;
  assert(msg->MeasInfo_len == msg->MeasData_len);
  for (size_t i = 0; i < msg->MeasInfo_len; ++i) {
    MeasInfo_t* mi = &msg->MeasInfo[i];
    assert(mi->meas_type == KPM_V2_MEASUREMENT_TYPE_NAME);
    printf("%s ", (char *)(mi->meas_name.buf));

    if (msg->MeasData[i].measRecord_len == 2) {
      const MeasRecord_t* mdi = msg->MeasData[i].measRecord;
      const long ctx = mdi[0].int_val;
      const long crx = mdi[1].int_val;
      if (i % 2 == 0){
        static long tx[10] = {0};
        static long rx[10] = {0};
        const float thrtx = (float) (ctx - tx[i]) * 8 / 1e6;
        tx[i] = ctx;
        const float thrrx = (float) (crx - rx[i]) * 8 / 1e6;
        rx[i] = crx;
        printf(" TX %7.3f RX %7.3f (Mbps)\n                         ", thrtx, thrrx);
      }
      printf(" TX %7ld RX %7ld", ctx, crx);
    } else {
      for (size_t j = 0; j < msg->MeasData[i].measRecord_len; ++j) {
        const MeasRecord_t* mdi = &msg->MeasData[i].measRecord[j];
        printf(" [%ld] %7ld", j, mdi->int_val);
      }
    }

    assert(mi->labelInfo_len == 1);
    if (mi->labelInfo[0].plmn_id != NULL) {
      const plmn_t* plmn = mi->labelInfo[0].plmn_id;
      printf(" (PLMN %03d.%0*d)",
             plmn->mcc, plmn->mnc_digit_len, plmn->mnc);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });

  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len);

  // KPM indication
  inter_xapp_e i_0 = ms_1000;
  sm_ans_xapp_t* kpm_handle = NULL;

  if(nodes.len > 0){
    kpm_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) ); 
    assert(kpm_handle  != NULL);
  }

  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_t* n = &nodes.n[i];
    for (size_t j = 0; j < n->len_rf; j++)
      printf("Registered node %d ran func id = %d \n ", i, n->ack_rf[j].id);

    kpm_handle[i] = report_sm_xapp_api(&nodes.n[i].id, SM_KPM_ID, i_0, sm_cb_kpm);
    assert(kpm_handle[i].success == true);
  }

  static bool run_xapp = true;
  void handle_sig(int signum) {
    if (signum != SIGINT && signum != SIGTERM)
      fprintf(stderr, "WARNING: received signal %d, stopping\n", signum);
    run_xapp = false;
  }
  signal(SIGINT, handle_sig);
  signal(SIGTERM, handle_sig);
  while(run_xapp) usleep(100000);


  for(int i = 0; i < nodes.len; ++i){
    // Remove the handle previously returned
    rm_report_sm_xapp_api(kpm_handle[i].u.handle);
  }

  if(nodes.len > 0){
    free(kpm_handle);
  }

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}
