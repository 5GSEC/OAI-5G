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

#include "openair2/RRC/NR_UE/rrc_proto.h"


void nr_rrc_set_T304(NR_UE_Timers_Constants_t *tac, NR_ReconfigurationWithSync_t *reconfigurationWithSync)
{
  if(reconfigurationWithSync) {
    switch (reconfigurationWithSync->t304) {
      case NR_ReconfigurationWithSync__t304_ms50 :
        tac->T304_k = 50;
        break;
      case NR_ReconfigurationWithSync__t304_ms100 :
        tac->T304_k = 100;
        break;
      case NR_ReconfigurationWithSync__t304_ms150 :
        tac->T304_k = 150;
        break;
      case NR_ReconfigurationWithSync__t304_ms200 :
        tac->T304_k = 200;
        break;
      case NR_ReconfigurationWithSync__t304_ms500 :
        tac->T304_k = 500;
        break;
      case NR_ReconfigurationWithSync__t304_ms1000 :
        tac->T304_k = 1000;
        break;
      case NR_ReconfigurationWithSync__t304_ms2000 :
        tac->T304_k = 2000;
        break;
      case NR_ReconfigurationWithSync__t304_ms10000 :
        tac->T304_k = 10000;
        break;
      default :
        AssertFatal(false, "Invalid T304 %ld\n", reconfigurationWithSync->t304);
    }
  }
}

void set_rlf_sib1_timers_and_constants(NR_UE_Timers_Constants_t *tac, NR_SIB1_t *sib1)
{
  if(sib1 && sib1->ue_TimersAndConstants) {
    switch (sib1->ue_TimersAndConstants->t301) {
      case NR_UE_TimersAndConstants__t301_ms100 :
        tac->T301_k = 100;
        break;
      case NR_UE_TimersAndConstants__t301_ms200 :
        tac->T301_k = 200;
        break;
      case NR_UE_TimersAndConstants__t301_ms300 :
        tac->T301_k = 300;
        break;
      case NR_UE_TimersAndConstants__t301_ms400 :
        tac->T301_k = 400;
        break;
      case NR_UE_TimersAndConstants__t301_ms600 :
        tac->T301_k = 600;
        break;
      case NR_UE_TimersAndConstants__t301_ms1000 :
        tac->T301_k = 1000;
        break;
      case NR_UE_TimersAndConstants__t301_ms1500 :
        tac->T301_k = 1500;
        break;
      case NR_UE_TimersAndConstants__t301_ms2000 :
        tac->T301_k = 2000;
        break;
      default :
        AssertFatal(false, "Invalid T301 %ld\n", sib1->ue_TimersAndConstants->t301);
    }
    switch (sib1->ue_TimersAndConstants->t310) {
      case NR_UE_TimersAndConstants__t310_ms0 :
        tac->T310_k = 0;
        break;
      case NR_UE_TimersAndConstants__t310_ms50 :
        tac->T310_k = 50;
        break;
      case NR_UE_TimersAndConstants__t310_ms100 :
        tac->T310_k = 100;
        break;
      case NR_UE_TimersAndConstants__t310_ms200 :
        tac->T310_k = 200;
        break;
      case NR_UE_TimersAndConstants__t310_ms500 :
        tac->T310_k = 500;
        break;
      case NR_UE_TimersAndConstants__t310_ms1000 :
        tac->T310_k = 1000;
        break;
      case NR_UE_TimersAndConstants__t310_ms2000 :
        tac->T310_k = 2000;
        break;
      default :
        AssertFatal(false, "Invalid T310 %ld\n", sib1->ue_TimersAndConstants->t310);
    }
    switch (sib1->ue_TimersAndConstants->t311) {
      case NR_UE_TimersAndConstants__t311_ms1000 :
        tac->T311_k = 1000;
        break;
      case NR_UE_TimersAndConstants__t311_ms3000 :
        tac->T311_k = 3000;
        break;
      case NR_UE_TimersAndConstants__t311_ms5000 :
        tac->T311_k = 5000;
        break;
      case NR_UE_TimersAndConstants__t311_ms10000 :
        tac->T311_k = 10000;
        break;
      case NR_UE_TimersAndConstants__t311_ms15000 :
        tac->T311_k = 15000;
        break;
      case NR_UE_TimersAndConstants__t311_ms20000 :
        tac->T311_k = 20000;
        break;
      case NR_UE_TimersAndConstants__t311_ms30000 :
        tac->T311_k = 30000;
        break;
      default :
        AssertFatal(false, "Invalid T311 %ld\n", sib1->ue_TimersAndConstants->t311);
    }
    switch (sib1->ue_TimersAndConstants->n310) {
      case NR_UE_TimersAndConstants__n310_n1 :
        tac->N310_k = 1;
        break;
      case NR_UE_TimersAndConstants__n310_n2 :
        tac->N310_k = 2;
        break;
      case NR_UE_TimersAndConstants__n310_n3 :
        tac->N310_k = 3;
        break;
      case NR_UE_TimersAndConstants__n310_n4 :
        tac->N310_k = 4;
        break;
      case NR_UE_TimersAndConstants__n310_n6 :
        tac->N310_k = 6;
        break;
      case NR_UE_TimersAndConstants__n310_n8 :
        tac->N310_k = 8;
        break;
      case NR_UE_TimersAndConstants__n310_n10 :
        tac->N310_k = 10;
        break;
      case NR_UE_TimersAndConstants__n310_n20 :
        tac->N310_k = 20;
        break;
      default :
        AssertFatal(false, "Invalid N310 %ld\n", sib1->ue_TimersAndConstants->n310);
    }
    switch (sib1->ue_TimersAndConstants->n311) {
      case NR_UE_TimersAndConstants__n311_n1 :
        tac->N311_k = 1;
        break;
      case NR_UE_TimersAndConstants__n311_n2 :
        tac->N311_k = 2;
        break;
      case NR_UE_TimersAndConstants__n311_n3 :
        tac->N311_k = 3;
        break;
      case NR_UE_TimersAndConstants__n311_n4 :
        tac->N311_k = 4;
        break;
      case NR_UE_TimersAndConstants__n311_n5 :
        tac->N311_k = 5;
        break;
      case NR_UE_TimersAndConstants__n311_n6 :
        tac->N311_k = 6;
        break;
      case NR_UE_TimersAndConstants__n311_n8 :
        tac->N311_k = 8;
        break;
      case NR_UE_TimersAndConstants__n311_n10 :
        tac->N311_k = 10;
        break;
      default :
        AssertFatal(false, "Invalid N311 %ld\n", sib1->ue_TimersAndConstants->n311);
    }
  }
  else
    LOG_E(NR_RRC,"SIB1 should not be NULL and neither UE_Timers_Constants\n");
}

void nr_rrc_set_sib1_timers_and_constants(NR_UE_Timers_Constants_t *tac, NR_SIB1_t *sib1)
{
  set_rlf_sib1_timers_and_constants(tac, sib1);
  if(sib1 && sib1->ue_TimersAndConstants) {
    switch (sib1->ue_TimersAndConstants->t300) {
      case NR_UE_TimersAndConstants__t300_ms100 :
        tac->T300_k = 100;
        break;
      case NR_UE_TimersAndConstants__t300_ms200 :
        tac->T300_k = 200;
        break;
      case NR_UE_TimersAndConstants__t300_ms300 :
        tac->T300_k = 300;
        break;
      case NR_UE_TimersAndConstants__t300_ms400 :
        tac->T300_k = 400;
        break;
      case NR_UE_TimersAndConstants__t300_ms600 :
        tac->T300_k = 600;
        break;
      case NR_UE_TimersAndConstants__t300_ms1000 :
        tac->T300_k = 1000;
        break;
      case NR_UE_TimersAndConstants__t300_ms1500 :
        tac->T300_k = 1500;
        break;
      case NR_UE_TimersAndConstants__t300_ms2000 :
        tac->T300_k = 2000;
        break;
      default :
        AssertFatal(false, "Invalid T300 %ld\n", sib1->ue_TimersAndConstants->t300);
    }
    switch (sib1->ue_TimersAndConstants->t319) {
      case NR_UE_TimersAndConstants__t319_ms100 :
        tac->T319_k = 100;
        break;
      case NR_UE_TimersAndConstants__t319_ms200 :
        tac->T319_k = 200;
        break;
      case NR_UE_TimersAndConstants__t319_ms300 :
        tac->T319_k = 300;
        break;
      case NR_UE_TimersAndConstants__t319_ms400 :
        tac->T319_k = 400;
        break;
      case NR_UE_TimersAndConstants__t319_ms600 :
        tac->T319_k = 600;
        break;
      case NR_UE_TimersAndConstants__t319_ms1000 :
        tac->T319_k = 1000;
        break;
      case NR_UE_TimersAndConstants__t319_ms1500 :
        tac->T319_k = 1500;
        break;
      case NR_UE_TimersAndConstants__t319_ms2000 :
        tac->T319_k = 2000;
        break;
      default :
        AssertFatal(false, "Invalid T319 %ld\n", sib1->ue_TimersAndConstants->t319);
    }
  }
  else
    LOG_E(NR_RRC,"SIB1 should not be NULL and neither UE_Timers_Constants\n");
}

void nr_rrc_handle_SetupRelease_RLF_TimersAndConstants(NR_UE_RRC_INST_t *rrc,
                                                       struct NR_SetupRelease_RLF_TimersAndConstants *rlf_TimersAndConstants)
{
  if(rlf_TimersAndConstants == NULL)
    return;

  NR_UE_Timers_Constants_t *tac = &rrc->timers_and_constants;
  NR_RLF_TimersAndConstants_t *rlf_tac = NULL;
  switch(rlf_TimersAndConstants->present){
    case NR_SetupRelease_RLF_TimersAndConstants_PR_release :
      // use values for timers T301, T310, T311 and constants N310, N311, as included in ue-TimersAndConstants received in SIB1
      set_rlf_sib1_timers_and_constants(tac, rrc->SInfo[0].sib1);
      break;
    case NR_SetupRelease_RLF_TimersAndConstants_PR_setup :
      rlf_tac = rlf_TimersAndConstants->choice.setup;
      if (rlf_tac == NULL)
        return;
      // (re-)configure the value of timers and constants in accordance with received rlf-TimersAndConstants
      switch (rlf_tac->t310) {
        case NR_RLF_TimersAndConstants__t310_ms0 :
          tac->T310_k = 0;
          break;
        case NR_RLF_TimersAndConstants__t310_ms50 :
          tac->T310_k = 50;
          break;
        case NR_RLF_TimersAndConstants__t310_ms100 :
          tac->T310_k = 100;
          break;
        case NR_RLF_TimersAndConstants__t310_ms200 :
          tac->T310_k = 200;
          break;
        case NR_RLF_TimersAndConstants__t310_ms500 :
          tac->T310_k = 500;
          break;
        case NR_RLF_TimersAndConstants__t310_ms1000 :
          tac->T310_k = 1000;
          break;
        case NR_RLF_TimersAndConstants__t310_ms2000 :
          tac->T310_k = 2000;
          break;
        case NR_RLF_TimersAndConstants__t310_ms4000 :
          tac->T310_k = 4000;
          break;
        case NR_RLF_TimersAndConstants__t310_ms6000 :
          tac->T310_k = 6000;
          break;
        default :
          AssertFatal(false, "Invalid T310 %ld\n", rlf_tac->t310);
      }
      switch (rlf_tac->n310) {
        case NR_RLF_TimersAndConstants__n310_n1 :
          tac->N310_k = 1;
          break;
        case NR_RLF_TimersAndConstants__n310_n2 :
          tac->N310_k = 2;
          break;
        case NR_RLF_TimersAndConstants__n310_n3 :
          tac->N310_k = 3;
          break;
        case NR_RLF_TimersAndConstants__n310_n4 :
          tac->N310_k = 4;
          break;
        case NR_RLF_TimersAndConstants__n310_n6 :
          tac->N310_k = 6;
          break;
        case NR_RLF_TimersAndConstants__n310_n8 :
          tac->N310_k = 8;
          break;
        case NR_RLF_TimersAndConstants__n310_n10 :
          tac->N310_k = 10;
          break;
        case NR_RLF_TimersAndConstants__n310_n20 :
          tac->N310_k = 20;
          break;
        default :
          AssertFatal(false, "Invalid N310 %ld\n", rlf_tac->n310);
      }
      switch (rlf_tac->n311) {
        case NR_RLF_TimersAndConstants__n311_n1 :
          tac->N311_k = 1;
          break;
        case NR_RLF_TimersAndConstants__n311_n2 :
          tac->N311_k = 2;
          break;
        case NR_RLF_TimersAndConstants__n311_n3 :
          tac->N311_k = 3;
          break;
        case NR_RLF_TimersAndConstants__n311_n4 :
          tac->N311_k = 4;
          break;
        case NR_RLF_TimersAndConstants__n311_n5 :
          tac->N311_k = 5;
          break;
        case NR_RLF_TimersAndConstants__n311_n6 :
          tac->N311_k = 6;
          break;
        case NR_RLF_TimersAndConstants__n311_n8 :
          tac->N311_k = 8;
          break;
        case NR_RLF_TimersAndConstants__n311_n10 :
          tac->N311_k = 10;
          break;
        default :
          AssertFatal(false, "Invalid N311 %ld\n", rlf_tac->n311);
      }
      if (rlf_tac->ext1) {
        switch (rlf_tac->ext1->t311) {
          case NR_RLF_TimersAndConstants__ext1__t311_ms1000 :
            tac->T311_k = 1000;
            break;
          case NR_RLF_TimersAndConstants__ext1__t311_ms3000 :
            tac->T311_k = 3000;
            break;
          case NR_RLF_TimersAndConstants__ext1__t311_ms5000 :
            tac->T311_k = 5000;
            break;
          case NR_RLF_TimersAndConstants__ext1__t311_ms10000 :
            tac->T311_k = 10000;
            break;
          case NR_RLF_TimersAndConstants__ext1__t311_ms15000 :
            tac->T311_k = 15000;
            break;
          case NR_RLF_TimersAndConstants__ext1__t311_ms20000 :
            tac->T311_k = 20000;
            break;
          case NR_RLF_TimersAndConstants__ext1__t311_ms30000 :
            tac->T311_k = 30000;
            break;
          default :
            AssertFatal(false, "Invalid T311 %ld\n", rlf_tac->ext1->t311);
        }
      }
      reset_rlf_timers_and_constants(tac);
      break;
    default :
      AssertFatal(false, "Invalid rlf_TimersAndConstants\n");
  }
}

void set_default_timers_and_constants(NR_UE_Timers_Constants_t *tac)
{
  // 38.331 9.2.3 Default values timers and constants
  tac->T310_k = 1000;
  tac->N310_k = 1;
  tac->T311_k = 30000;
  tac->N311_k = 1;
}

void reset_rlf_timers_and_constants(NR_UE_Timers_Constants_t *tac)
{
  // stop timer T310 for this cell group, if running
  tac->T310_active = false;
  tac->T310_cnt = 0;
  // reset the counters N310 and N311
  tac->N310_cnt = 0;
  tac->N311_cnt = 0;
}
