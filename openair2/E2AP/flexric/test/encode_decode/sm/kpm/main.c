#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/byte_array.h"
#include "../../../../src/sm/kpm_sm_v03.00/enc/kpm_enc_asn.h"
#include "../../../../src/sm/kpm_sm_v03.00/dec/kpm_dec_asn.h"
#include "../../../rnd/fill_rnd_data_kpm.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void test_kpm_ric_event_trigger_def_asn(void)
{
  kpm_event_trigger_def_t msg = fill_kpm_event_trigger_def();
  defer({ free_kpm_event_trigger_def(&msg); }); 

  byte_array_t ba = kpm_enc_event_trigger_asn(&msg);
  defer({ free_byte_array(ba); }); 

  kpm_event_trigger_def_t out = kpm_dec_event_trigger_asn(ba.len, ba.buf);
  defer({ free_kpm_event_trigger_def(&out); }); 

  assert(eq_kpm_event_trigger_def(&msg, &out ) == true);
}


void test_kpm_act_def(void)
{
  kpm_act_def_t action_def = fill_kpm_action_def();
  defer({ free_kpm_action_def(&action_def); });

  byte_array_t ba = kpm_enc_action_def_asn(&action_def);
  defer({ free_byte_array(ba); }); 

  kpm_act_def_t out = kpm_dec_action_def_asn(ba.len, ba.buf);
  defer({ free_kpm_action_def(&out); });

  assert(eq_kpm_action_def(&action_def, &out) == true);
}

void test_kpm_ind_hdr(void)
{
  kpm_ind_hdr_t hdr = fill_kpm_ind_hdr();
  defer({ free_kpm_ind_hdr(&hdr); });

  byte_array_t ba = kpm_enc_ind_hdr_asn(&hdr);
  defer({ free_byte_array(ba); }); 

  kpm_ind_hdr_t out = kpm_dec_ind_hdr_asn(ba.len, ba.buf);
  defer({ free_kpm_ind_hdr(&out); });

  assert(eq_kpm_ind_hdr(&hdr, &out) == true);
}

void test_kpm_ind_msg(void)
{
  kpm_ind_msg_t msg = fill_kpm_ind_msg();
  defer({ free_kpm_ind_msg(&msg); });

  byte_array_t ba = kpm_enc_ind_msg_asn(&msg);
  defer({ free_byte_array(ba); }); 

  kpm_ind_msg_t out = kpm_dec_ind_msg_asn(ba.len, ba.buf);
  defer({ free_kpm_ind_msg(&out); });

  assert(eq_kpm_ind_msg(&msg, &out) == true);
}

void test_kpm_ran_function(void)
{
  kpm_ran_function_def_t msg =  fill_kpm_ran_func_def();
  defer({ free_kpm_ran_function_def(&msg); });

  byte_array_t ba = kpm_enc_func_def_asn(&msg);
  defer({ free_byte_array(ba); }); 

  kpm_ran_function_def_t out = kpm_dec_func_def_asn(ba.len, ba.buf);
  defer({ free_kpm_ran_function_def(&out); });

  assert(eq_kpm_ran_function_def(&msg, &out) == true);
}

int main()
{
  time_t t;
  srand((unsigned) time(&t));

  for (size_t i = 0; i < 16; i++) {

    // Test off all the functions
    printf("KPM SM enc/dec test launched\n");

    // Event Trigger
    test_kpm_ric_event_trigger_def_asn();
    printf("KPM Event Trigger test succeeded\n");

    // Action Definition
    test_kpm_act_def();
    printf("KPM Action Definition test succeeded\n");

    // Indication Header
    test_kpm_ind_hdr();
    printf("KPM Indication Header test succeeded\n");

    // Indication Message
    test_kpm_ind_msg();
    printf("KPM Indication Message test succeeded\n");

    // RAN Function Definition
    test_kpm_ran_function();
    printf("KPM SM enc/dec test succeeded\n");
  }

  return EXIT_SUCCESS;
}

