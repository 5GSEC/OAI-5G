/*
 * Implementation agent side of SM KPM

 * Note that the agent side functionalities at E2AP level is already implemented with generic code
 * given that the framework is the same for all SM, it changes only the content of the IE at applicative layer SM, i.e. SM-KPM, SM-PDCP
 * 
 * I.e. ON SUBSCRIPTION, implemented in src/agent/msg_handler_agent.c:e2ap_handle_subscription_request_agent()) 
 * and following O-RAN.WG3.E2AP-v01.01, $8.2.1.2, the functionality is expressed below:
 * "
 * At reception of the RIC SUBSCRIPTION REQUEST message the target E2 Node shall:
 * - Determine the target function using the information in the RAN Function ID IE and configure the requested
 *   event trigger using information in the RIC Subscription Details IE (part of RIC SUBSCRIPTION REQUEST sent by server RIC) 
 * - If one or more Report, Insert and/or Policy RIC service actions are included in the RIC Subscription Details IE
 *   then the target function shall validate the event trigger and requested action sequence and, if accepted, store the
 *   required RIC Request ID, RIC Event Trigger Definition IE and sequence of RIC Action ID IE, RIC Action Type
 *   IE, RIC Action Definition IE, if included, and RIC Subsequent Action IE, if included.
 * If the requested trigger and at least one required action are accepted by the target E2 Node, the target E2 Node shall
 * reserve necessary resources and send the RIC SUBSCRIPTION RESPONSE message back to the Near-RT RIC. The
 * target E2 Node shall include in the response message the actions for which resources have been prepared at the target
 * E2 Node in the RIC Actions Admitted List IE. The target E2 Node shall include the actions that have not been admitted
 * in the RIC Actions Not Admitted List IE with an appropriate cause value.
 * "
 * In this case, you would need just to implement in this source code the IE above mentioned (RAN Function ID IE, RIC Event Trigger 
 * Definition etc..)
 */
#include "kpm_sm_agent.h"
#include "kpm_sm_id.h"
#include "ie/kpm_data_ie.h"
#include "enc/kpm_enc_generic.h"
#include "dec/kpm_dec_generic.h"
#include "../../util/alg_ds/alg/defer.h"
#include "../../util/conversions.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

  #ifdef ASN
    kpm_enc_asn_t enc;
  #elif FLATBUFFERS 
    //pdcp_enc_fb_t enc;
    static_assert(false, "Encryption FLATBUFFERS not implemented yet");
  #elif PLAIN
    kpm_enc_plain_t enc;
  #else
    static_assert(false, "No encryption type selected");
  #endif
} sm_kpm_agent_t;

static
subscribe_timer_t on_subscription_kpm_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{ 
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_kpm_agent_t* sm = (sm_kpm_agent_t*)sm_agent;

  kpm_sub_data_t sub_data = {0};
  defer({free_kpm_sub_data(&sub_data);}); 

  sub_data.ev_trg_def = kpm_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);
  assert(sub_data.ev_trg_def.kpm_ric_event_trigger_format_1.report_period_ms > 0);

  subscribe_timer_t timer = { .type = KPM_V3_0_SUB_DATA_ENUM ,
    .ms = sub_data.ev_trg_def.kpm_ric_event_trigger_format_1.report_period_ms};
  // Only 1 supported
  kpm_act_def_t* tmp = calloc(1, sizeof(kpm_act_def_t));
  assert(tmp != NULL && "Memory exhausted");
  *tmp = kpm_dec_action_def(&sm->enc, data->len_ad, data->action_def);

  timer.act_def = tmp; 

  return timer;
}

static 
sm_ind_data_t on_indication_kpm_sm_ag(sm_agent_t const* sm_agent, void* act_def_v)
{
  assert(sm_agent != NULL);
  assert(act_def_v != NULL && "Action Definition data needed for this SM");
  sm_kpm_agent_t* sm = (sm_kpm_agent_t*)sm_agent;

  kpm_act_def_t* act_def = act_def_v;

  sm_ind_data_t ret = {0};

  // Fill Indication Message and Header
 // sm_ag_if_rd_t rd_if = {.type = INDICATION_MSG_AGENT_IF_ANS_V0};
 // sm_ag_if_rd_ind_t ind = {.type = KPM_STATS_V3_0};
  kpm_rd_ind_data_t kpm = {0}; 
  kpm.act_def = act_def;
  sm->base.io.read_ind(&kpm); 
  // Free memory allocated by the RAN. Sucks
  defer({ free_kpm_ind_data(&kpm.ind); });

  byte_array_t ba_hdr = kpm_enc_ind_hdr(&sm->enc, &kpm.ind.hdr);
  ret.ind_hdr = ba_hdr.buf;
  ret.len_hdr = ba_hdr.len;

  byte_array_t ba = kpm_enc_ind_msg(&sm->enc, &kpm.ind.msg);
  ret.ind_msg = ba.buf;
  ret.len_msg = ba.len;

  // we do not have Call Process ID
  ret.call_process_id = NULL;
  ret.len_cpid = 0;

  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_kpm_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  sm_kpm_agent_t* sm = (sm_kpm_agent_t*)sm_agent;

  // Call the RAN and fill the data  
  kpm_e2_setup_t kpm = {0};
  sm->base.io.read_setup(&kpm);
  defer({ free_kpm_ran_function_def(&kpm.ran_func_def );});

  kpm_ran_function_def_t* ran_func = &kpm.ran_func_def; 

  byte_array_t ba = kpm_enc_func_def(&sm->enc, ran_func);

  sm_e2_setup_data_t setup = {0}; 
  setup.len_rfd = ba.len;
  setup.ran_fun_def = ba.buf;

  // RAN Function
  setup.rf.def = cp_str_to_ba(SM_KPM_STR);
  setup.rf.id = SM_KPM_ID;
  setup.rf.rev = SM_KPM_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_KPM_OID);

  return setup;
}

static 
void free_e2_setup_kpm_sm_ag (void *msg)
{
  assert(msg != NULL);

  sm_e2_setup_data_t * func_def  = (sm_e2_setup_data_t *)msg;

  free(func_def->ran_fun_def);
}

static
sm_ric_service_update_data_t on_ric_service_update_kpm_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");

 sm_ric_service_update_data_t  ret; 
  return ret;
}


static void free_kpm_sm_ag(sm_agent_t *sm_agent)
{
  assert(sm_agent != NULL);
  sm_kpm_agent_t *sm = (sm_kpm_agent_t*)sm_agent;
  free(sm);
}

static
void free_act_def_kpm_sm_ag(sm_agent_t *sm_agent, void* act_def_v)
{
  assert(sm_agent != NULL);
  assert(act_def_v != NULL);

  kpm_act_def_t* act_def = act_def_v;
  free_kpm_action_def(act_def);
  free(act_def);
}

sm_agent_t *make_kpm_sm_agent(sm_io_ag_ran_t io)
{
  sm_kpm_agent_t *sm = calloc(1, sizeof(*sm));
  assert(sm != NULL && "Memory exhausted!!!");

  *(uint16_t*)(&sm->base.ran_func_id) = SM_KPM_ID; 

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[KPM_STATS_V3_0];
  sm->base.io.read_setup = io.read_setup_tbl[KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = NULL; 
  sm->base.io.write_subs = NULL; // Used only for aperiodic subscription

  sm->base.free_sm = free_kpm_sm_ag;
  sm->base.free_act_def = free_act_def_kpm_sm_ag;

  // Memory DeAllocation 
  sm->base.alloc.free_subs_data_msg = NULL;
  sm->base.alloc.free_ind_data = NULL;
  sm->base.alloc.free_ctrl_req_data = NULL;
  sm->base.alloc.free_ctrl_out_data = NULL;
  sm->base.alloc.free_e2_setup = free_e2_setup_kpm_sm_ag; 
  sm->base.alloc.free_ric_service_update = NULL;//free_ric_service_update_kpm_sm_ric; 

  // O-RAN E2SM 5 Procedures
  sm->base.proc.on_subscription       = on_subscription_kpm_sm_ag;
  sm->base.proc.on_indication         = on_indication_kpm_sm_ag;
  sm->base.proc.on_control            = NULL;
  sm->base.proc.on_ric_service_update = on_ric_service_update_kpm_sm_ag;
  sm->base.proc.on_e2_setup           = on_e2_setup_kpm_sm_ag;

  // SM name
  assert(strlen(SM_KPM_STR) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name, SM_KPM_STR, strlen(SM_KPM_STR)); 

  return &sm->base;
}

