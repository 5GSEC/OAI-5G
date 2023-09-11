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

# include "intertask_interface.h"
# include "create_tasks.h"
# include "common/utils/LOG/log.h"
# include "executables/lte-softmodem.h"
# include "common/ran_context.h"

    #include "sctp_eNB_task.h"
    #include "x2ap_eNB.h"
    #include "s1ap_eNB.h"
    #include "openair3/ocp-gtpu/gtp_itf.h"
    #include "m2ap_eNB.h"
    #include "m2ap_MCE.h"
    #include "m3ap_MME.h"
    #include "m3ap_MCE.h"
  #if ENABLE_RAL
    #include "lteRALue.h"
    #include "lteRALenb.h"
  #endif
  #include "RRC/LTE/rrc_defs.h"
# include "enb_app.h"
# include "mce_app.h"
# include "mme_app.h"

#include <openair3/ocp-gtpu/gtp_itf.h>
//extern RAN_CONTEXT_t RC;

int create_tasks_mbms(uint32_t enb_nb) {
 // LOG_D(ENB_APP, "%s(enb_nb:%d\n", __FUNCTION__, enb_nb);
  AssertFatal(!get_softmodem_params()->nsa, "In NSA mode\n");
  int rc;

  if (enb_nb == 0) return 0;

  if(!EPC_MODE_ENABLED){
    rc = itti_create_task(TASK_SCTP, sctp_eNB_task, NULL);
    AssertFatal(rc >= 0, "Create task for SCTP failed\n");
  }


  LOG_I(MME_APP, "Creating MME_APP eNB Task\n");
  rc = itti_create_task (TASK_MME_APP, MME_app_task, NULL);
  AssertFatal(rc >= 0, "Create task for MME APP failed\n");

  if (is_m3ap_MME_enabled()) {
	  rc = itti_create_task(TASK_M3AP_MME, m3ap_MME_task, NULL);
	  AssertFatal(rc >= 0, "Create task for M3AP MME failed\n");
  }

  LOG_I(MCE_APP, "Creating MCE_APP eNB Task\n");
  rc = itti_create_task (TASK_MCE_APP, MCE_app_task, NULL);
  AssertFatal(rc >= 0, "Create task for MCE APP failed\n");

    if(!EPC_MODE_ENABLED){
   // rc = itti_create_task(TASK_SCTP, sctp_eNB_task, NULL);
   // AssertFatal(rc >= 0, "Create task for SCTP failed\n");
    rc = itti_create_task(TASK_GTPV1_U, gtpv1uTask, NULL);
    AssertFatal(rc >= 0, "Create task for GTPV1U failed\n");
    }

  if (is_m3ap_MCE_enabled()) {
     rc = itti_create_task(TASK_M3AP_MCE, m3ap_MCE_task, NULL);
     AssertFatal(rc >= 0, "Create task for M3AP MCE failed\n");

  }
  if (is_m2ap_MCE_enabled()) {
     rc = itti_create_task(TASK_M2AP_MCE, m2ap_MCE_task, NULL);
     AssertFatal(rc >= 0, "Create task for M2AP failed\n");
  }

  if (is_m2ap_eNB_enabled()) {
     rc = itti_create_task(TASK_M2AP_ENB, m2ap_eNB_task, NULL);
     AssertFatal(rc >= 0, "Create task for M2AP failed\n");
  }

   return 0;
}
