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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../ie/ir/e2sm_rc_ev_trg_frmt_1.h"

#include "../../../util/alg_ds/alg/defer.h"
#include "../../../util/byte_array.h"
#include "../../../sm/rc_sm/enc/rc_enc_asn.h"
#include "../../../sm/rc_sm/dec/rc_dec_asn.h"
#include "fill_rnd_data_rc.h"

#include "../ie/rc_data_ie.h"

#include "fill_rnd_data_rc.h"

#include <time.h>


void test_ric_event_trigger_rc(void)
{
  e2sm_rc_event_trigger_t msg = fill_rnd_rc_event_trigger();
  defer({ free_e2sm_rc_event_trigger(&msg); });

  byte_array_t ba = rc_enc_event_trigger_asn(&msg);
  defer({ free_byte_array(ba); });

  e2sm_rc_event_trigger_t out = rc_dec_event_trigger_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_event_trigger(&out); });

  assert(eq_e2sm_rc_event_trigger(&msg, &out ) == true);
}

 
void test_rc_act_def(void)
{
  e2sm_rc_action_def_t action_def = fill_rnd_rc_action_def();
  defer({ free_e2sm_rc_action_def(&action_def); });

  byte_array_t ba = rc_enc_action_def_asn(&action_def);
  defer({ free_byte_array(ba); });

  e2sm_rc_action_def_t out = rc_dec_action_def_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_action_def(&out); });

  assert(eq_e2sm_rc_action_def(&action_def, &out) == true);
}

void test_rc_ind_hdr(void)
{
  e2sm_rc_ind_hdr_t hdr = fill_rnd_rc_ind_hdr();
  defer({ free_e2sm_rc_ind_hdr(&hdr); });

  byte_array_t ba = rc_enc_ind_hdr_asn(&hdr);
  defer({ free_byte_array(ba); });

  e2sm_rc_ind_hdr_t out = rc_dec_ind_hdr_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_ind_hdr(&out); });

  assert(eq_e2sm_rc_ind_hdr(&hdr, &out) == true);
}


void test_rc_ind_msg(void)
{
  e2sm_rc_ind_msg_t msg = fill_rnd_rc_ind_msg();
  defer({ free_e2sm_rc_ind_msg(&msg); });

  byte_array_t ba = rc_enc_ind_msg_asn(&msg);
  defer({ free_byte_array(ba); });

  e2sm_rc_ind_msg_t out = rc_dec_ind_msg_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_ind_msg(&out); });

  assert(eq_e2sm_rc_ind_msg(&msg, &out) == true);
}

void test_rc_call_process_id()
{
  e2sm_rc_cpid_t msg = fill_rnd_rc_cpid();
  defer({ free_e2sm_rc_cpid(&msg); });

  byte_array_t ba = rc_enc_cpid_asn(&msg);
  defer({ free_byte_array(ba); });

  e2sm_rc_cpid_t out = rc_dec_cpid_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_cpid(&out); });

  assert(eq_e2sm_rc_cpid(&msg, &out) == true);
}

void test_rc_ctrl_hdr(void)
{
  e2sm_rc_ctrl_hdr_t msg = fill_rnd_rc_ctrl_hdr();
  defer({ free_e2sm_rc_ctrl_hdr(&msg); });

  byte_array_t ba = rc_enc_ctrl_hdr_asn(&msg);
  defer({ free_byte_array(ba); });

  e2sm_rc_ctrl_hdr_t out = rc_dec_ctrl_hdr_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_ctrl_hdr(&out); });

  assert(eq_e2sm_rc_ctrl_hdr(&msg, &out) == true);
}


void test_rc_ctrl_msg(void)
{
  e2sm_rc_ctrl_msg_t msg = fill_rnd_rc_ctrl_msg();
  defer({ free_e2sm_rc_ctrl_msg(&msg); });

  byte_array_t ba = rc_enc_ctrl_msg_asn(&msg);
  defer({ free_byte_array(ba); });

  e2sm_rc_ctrl_msg_t out = rc_dec_ctrl_msg_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_ctrl_msg(&out); });

  assert(eq_e2sm_rc_ctrl_msg(&msg, &out) == true);
}

void test_rc_ctrl_out(void)
{
  e2sm_rc_ctrl_out_t msg = fill_rnd_rc_ctrl_out();
  defer({ free_e2sm_rc_ctrl_out(&msg); });

  byte_array_t ba = rc_enc_ctrl_out_asn(&msg);
  defer({ free_byte_array(ba); });

  e2sm_rc_ctrl_out_t out = rc_dec_ctrl_out_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_ctrl_out(&out); });

  assert(eq_e2sm_rc_ctrl_out(&msg, &out) == true);
}

void test_rc_ran_func_def(void)
{
  e2sm_rc_func_def_t msg = fill_rnd_rc_ran_func_def();
  defer({ free_e2sm_rc_func_def(&msg); });

  byte_array_t ba = rc_enc_func_def_asn(&msg);
  defer({ free_byte_array(ba); });

  e2sm_rc_func_def_t out = rc_dec_func_def_asn(ba.len, ba.buf);
  defer({ free_e2sm_rc_func_def(&out); });

  assert(eq_e2sm_rc_func_def(&msg, &out) == true);
}





int main()
{
  time_t t;
  srand((unsigned) time(&t));

  // Test off all the functions
  printf("\nRC SM enc/dec test launched\n");

  // Event Trigger
  test_ric_event_trigger_rc();
  printf("\nRC Event Trigger test succeeded\n");

  // Action Definition
  test_rc_act_def();
  printf("\nRC Action Definition test succeeded\n");

  // Indication Header
  test_rc_ind_hdr();
  printf("\nRC Indication Header test succeeded\n");

  // Indication Message
  test_rc_ind_msg();
  printf("\nRC Indication Message test succeeded\n");

  // Call Process ID 
  test_rc_call_process_id();
  printf("\nRC Call Process ID\nn");

  // Control Header 
   test_rc_ctrl_hdr();
   printf("\nRC Control Header \n");

  // Control Message  
   test_rc_ctrl_msg();
   printf("\nRC Control Message\n");

  // Control Outcome 
  test_rc_ctrl_out();
  printf("\nRC Control Outcome\n");

  // RAN Function Definition
  test_rc_ran_func_def();
  printf("\nRC RAN Function Definition\n");

  return EXIT_SUCCESS;
}

