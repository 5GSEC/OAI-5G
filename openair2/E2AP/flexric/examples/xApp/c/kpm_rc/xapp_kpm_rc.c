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
#include "../../../../src/sm/rc_sm/ie/ir/ran_param_struct.h"
#include "../../../../src/sm/rc_sm/ie/ir/ran_param_list.h"
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

static
byte_array_t copy_str_to_ba(const char* str)
{
  assert(str != NULL);

  size_t const sz = strlen(str);
  byte_array_t dst = {.len = sz }; 
  dst.buf = calloc(sz ,sizeof(uint8_t) );
  assert(dst.buf != NULL);

  memcpy(dst.buf, str, sz);

  return dst;
}

static
ue_id_e2sm_t ue_id;

static
pthread_mutex_t mtx;

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0); 

  kpm_ind_data_t const* kpm = &rd->ind.kpm.ind;

  int64_t now = time_now_us();
  printf("KPM ind_msg latency = %ld μs\n", now - kpm->hdr.kpm_ric_ind_hdr_format_1.collectStartTime);
  printf("Sojourn time %lf \n",kpm->msg.frm_3.meas_report_per_ue[0].ind_msg_format_1.meas_data_lst[0].meas_record_lst[0].real_val);

  {
    lock_guard(&mtx);
    free_ue_id_e2sm(&ue_id); 
    ue_id = cp_ue_id_e2sm(&kpm->msg.frm_3.meas_report_per_ue[0].ue_meas_report_lst);
  }
  printf("UE ID %ld \n ", ue_id.gnb.amf_ue_ngap_id);
}

static
kpm_event_trigger_def_t gen_ev_trig(uint64_t period)
{
  kpm_event_trigger_def_t dst = {0};

  dst.type = FORMAT_1_RIC_EVENT_TRIGGER;
  dst.kpm_ric_event_trigger_format_1.report_period_ms = period;

  return dst;
}

static
meas_info_format_1_lst_t gen_meas_info_format_1_lst(const char* action)
{
  meas_info_format_1_lst_t dst = {0}; 

  dst.meas_type.type = NAME_MEAS_TYPE;
  // ETSI TS 128 552
  dst.meas_type.name = copy_str_to_ba(  action );

  dst.label_info_lst_len = 1;
  dst.label_info_lst = calloc(1, sizeof(label_info_lst_t));
  assert(dst.label_info_lst != NULL && "Memory exhausted");
  dst.label_info_lst[0].noLabel = calloc(1, sizeof(enum_value_e));
  assert(dst.label_info_lst[0].noLabel != NULL && "Memory exhausted");
  *dst.label_info_lst[0].noLabel = TRUE_ENUM_VALUE;

  return dst;
}

static
kpm_act_def_format_1_t gen_act_def_frmt_1(const char* action)
{
  kpm_act_def_format_1_t dst = {0};

  dst.gran_period_ms = 100;

  // [1, 65535]
  dst.meas_info_lst_len = 1;
  dst.meas_info_lst = calloc(1, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  *dst.meas_info_lst = gen_meas_info_format_1_lst(action);
 
  return dst;
}

static
kpm_act_def_format_4_t gen_act_def_frmt_4(const char* action)
{
  kpm_act_def_format_4_t dst = {0};

  // [1, 32768]
  dst.matching_cond_lst_len = 1;  

  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t));
  assert(dst.matching_cond_lst != NULL && "Memory exhausted");
 
  // Hack. Subscribe to all UEs with CQI greater than 0 to get a list of all available UEs in the RAN
  dst.matching_cond_lst[0].test_info_lst.test_cond_type = CQI_TEST_COND_TYPE;
  dst.matching_cond_lst[0].test_info_lst.CQI = TRUE_TEST_COND_TYPE;
  
  dst.matching_cond_lst[0].test_info_lst.test_cond = calloc(1, sizeof(test_cond_e));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond = GREATERTHAN_TEST_COND;

  dst.matching_cond_lst[0].test_info_lst.test_cond_value = calloc(1, sizeof(test_cond_value_e)); 
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond_value != NULL && "Memory exhausted"); 
  *dst.matching_cond_lst[0].test_info_lst.test_cond_value =  INTEGER_TEST_COND_VALUE;
  dst.matching_cond_lst[0].test_info_lst.int_value = malloc(sizeof(int64_t));
  assert(dst.matching_cond_lst[0].test_info_lst.int_value != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.int_value = 0; 

  // Action definition Format 1 
  dst.action_def_format_1 = gen_act_def_frmt_1(action);  // 8.2.1.2.1

  return dst;
}


static
kpm_act_def_t gen_act_def(const char* act)
{
  kpm_act_def_t dst = {0}; 

  dst.type = FORMAT_4_ACTION_DEFINITION; 
  dst.frm_4 = gen_act_def_frmt_4(act);
  return dst;
}



/*
typedef enum{
  DRB_QoS_Configuration_7_6_2_1 = 1,
  QoS_flow_mapping_configuration_7_6_2_1 = 2,
  Logical_channel_configuration_7_6_2_1 = 3,
  Radio_admission_control_7_6_2_1 = 4,
  DRB_termination_control_7_6_2_1 = 5,
  DRB_split_ratio_control_7_6_2_1 = 6,
  PDCP_Duplication_control_7_6_2_1 = 7,
} rc_ctrl_service_style_1_e;


static
e2sm_rc_ctrl_hdr_frmt_1_t gen_rc_ctrl_hdr_frmt_1(void)
{
  e2sm_rc_ctrl_hdr_frmt_1_t dst = {0};

  // 6.2.2.6
  // ue_id_e2sm_t ue_id;
  dst.ue_id = ;

  // CONTROL Service Style 1: Radio Bearer Control
  dst.ric_style_type = 1;

  // QoS flow mapping conf 
  dst.ctrl_act_id = QoS_flow_mapping_configuration_7_6_2_1 ;

  return dst;
}

static
e2sm_rc_ctrl_hdr_t gen_rc_ctrl_hdr(void)
{
  e2sm_rc_ctrl_hdr_t dst = {0};
  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_HDR;
  dst.frmt_1 = gen_rc_ctrl_hdr_frmt_1();
  return dst;
}


typedef enum{
  DRB_ID_Bearer_Context_Setup = 26101,
  CHOICE_DRB_Type_Bearer_Context_Setup = 26102, 

  NG_RAN_DRB_Bearer_Context_Setup = 26103,
  E_UTRA_DRB_Bearer_Context_Setup = 26104,

} bearer_context_setup_e ;

typedef struct{
  size_t sz;
  seq_ran_param_t* p;
} pair_rps_t;

typedef enum{
  FIVE_QI_8_1_1_5 = 14001,
  PACKET_DELAY_BUDGET_8_1_1_5 = 14002,
  PACKET_ERROR_RATE_8_1_1_5 = 14003,
  NG_RAN_DRB_ALLOCATION_AND_RETENTION_PRIORITY_8_1_1_5 = 14004,
  PRIORITY_LEVEL_8_1_1_5 = 14005,
  PRE_EMPTION_CAPABILITY_8_1_1_5 = 14006,
  PRE_EMPTION_VULNERABILITY_8_1_1_5 = 14007,
  PRIORITY_LEVEL_OF_THE_MAPPED_QOS_FLOWS_8_1_1_5 = 14008,
  QOS_PARAMETERS_FOR_GBR_FLOWS_IN_NG_RAN_BEARER_8_1_1_5 = 14009,
  MAXIMUM_FLOW_BIT_RATE_DOWNLINK_8_1_1_5 = 14010,
  GUARANTEED_FLOW_BIT_RATE_DOWNLINK_8_1_1_5 = 14011,
  MAXIMUM_PACKET_LOSS_RATE_DOWNLINK_8_1_1_5 = 14012 ,
  MAXIMUM_FLOW_BIT_RATER_UPLINK_8_1_1_5 = 14013 ,
  GUARANTEED_FLOW_BIT_RATE_UPLINK_8_1_1_5 = 14014,
  MAXIMUM_PACKET_LOSS_RATE_UPLINK_8_1_1_5 = 14015,
  QOS_MONITORING_ENABLE_REQUEST_8_1_1_5 = 14016,
  QOS_MONITORING_REPORTING_FREQUENCY_8_1_1_5 = 14017,
  QOS_MONITORING_DISABLED_8_1_1_5 = 14018,
  REFLECTIVE_QOS_MAPPING_8_1_1_5 = 14019,
  LIST_OF_CELL_GROUPS_TO_BE_ADDED_8_1_1_5 = 14101 ,
  CELL_GROUP_ITEM_8_1_1_5 = 14102 ,
  CELL_GROUP_ID_8_1_1_5 = 14103,
  CELL_GROUP_8_1_1_5 = 14104,
  SDAP_CONFIGURATION_8_1_1_5 = 14201,
  PDU_SESSION_ID_8_1_1_5 = 14202,
  DEFAULT_DRB_8_1_1_5 = 14203,
  PDU_SESSION_8_1_1_5 = 14210,
  PDCP_CONFIGURATION_8_1_1_5 = 14301,
  RLC_MODE_8_1_1_5 = 14302,
  PDCP_DUPLICATION_8_1_1_5 = 14303,
  UL_DATA_SPLIT_THRESHOLD_8_1_1_5 = 14304,
  PDCP_RE_ESTABLISHMENT_8_1_1_5 = 14305,
  PDCP_DATA_RECOVERY_8_1_1_5 = 14306,
  OUT_OF_ORDER_DELIVERY_8_1_1_5 = 14307,
  PDCP_STATUS_REPORT_INDICATION_8_1_1_5 = 14308,
  NUMBER_OF_PDCP_DUPLICATION_8_1_1_5 = 14309,
  UL_MORE_THAN_ONE_RLC_8_1_1_5 = 14310,
  PRIMARY_PATH_8_1_1_5 = 14311,
  CELL_GROUP_ID_8_1_1_5 = 14312,
  LOGICAL_CHANNEL_ID_8_1_1_5 = 14313,
  UL_MORE_THAN_TWO_RLC_8_1_1_5 = 14314,
  SPLIT_SECONDARY_PATH_8_1_1_5 = 14315,
  DUPLICATION_STATE_8_1_1_5 = 14316,

  // Cell Group
  UL_CONFIGURATION_CELL_GROUP = 15502 ,
  RAT_TYPE_CELL_GROUP  = 15503 ,
  NUMBER_OF_TUNNELS_CELL_GROUP = 15504 ,



} ng_ran_data_radio_bearer_e;

static
seq_ran_param_t gen_false_ng_ran_drb(ng_ran_data_radio_bearer_e id , int64_t val)
{
  seq_ran_param_t dst = {0};
  dst.ran_param_id = id;
  dst.ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(dst.ran_param_val.flag_false != NULL && "Memory exhausted");

  dst.ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE ; 
  dst.ran_param_val.flag_false->int_ran = val;

  return dst;
}

static
seq_ran_param_t gen_true_ng_ran_drb(ng_ran_data_radio_bearer_e id , int64_t val)
{
  seq_ran_param_t dst = {0};
  dst.ran_param_id = id;
  dst.ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t));
  assert(dst.ran_param_val.flag_true != NULL && "Memory exhausted");

  dst.ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE ; 
  dst.ran_param_val.flag_false->int_ran = val;

  return dst;
}



static
seq_ran_param_t gen_5qi(int five_qi)
{
  // TS 38.463
  // TS 38.463 INTEGER (0..255, …)
  assert(five_qi > - 1 && five_qi < 256); 
  return gen_false_ng_ran_drb(FIVE_QI_8_1_1_5, five_qi);
}

static
seq_ran_param_t gen_pdb(int64_t pdb)
{
  // TS 38.463
 // INTEGER (0..1023, …)
  assert(pdb > -1 && pdb < 1024);
  return gen_false_ng_ran_drb(PACKET_DELAY_BUDGET_8_1_1_5, pdb);
}

static
seq_ran_param_t gen_per(int32_t scalar, int32_t exp)
{
  // TS 38.463
  // Possible BUG in the standard
  // packet error rate is represented with 2 integers
  // Scalar [0-9]
  // Exponent [0-9]
  assert(scalar > -1 &&  scalar < 10);
  assert(exp > -1 && exp < 10);
 
  // The packet error rate is
  // expressed as Scalar x 10-k
  // where k is the Exponent.
  int64_t per = scalar*10+exp;
  return gen_false_ng_ran_drb(PACKET_ERROR_RATE_8_1_1_5, per);
}

static
seq_ran_param_t gen_plqf(int32_t plqf)
{
  // TS 38.463
  // INTEGER (1..127,…)
  assert(plqf > 0 && plqf < 128);
  return gen_false_ng_ran_drb(PRIORITY_LEVEL_8_1_1_5, plqf);
}

static
seq_ran_param_t gen_mon_req(mon_req)
{
  // TS 38.463
//  ENUMERATED (UL, DL, Both, …)
//  Bug ???
  assert(mon_req < 3);
  return gen_false_ng_ran_drb(QOS_MONITORING_ENABLE_REQUEST_8_1_1_5, mon_req);
}

static
seq_ran_param_t gen_mon_rep(int64_t mon_rep)
{
  // TS 38.463
  //  INTEGER (1..1800, …
  assert(mon_rep > 0 && mon_rep < 1801);
  return gen_false_ng_ran_drb(QOS_MONITORING_REPORTING_FREQUENCY_8_1_1_5, mon_rep);
}

static
seq_ran_param_t gen_mon_dis(int64_t mon_dis)
{
  // TS 38.463
  //  ENUMERATED (true, ...)
  assert(mon_dis == 0);
  return gen_false_ng_ran_drb(QOS_MONITORING_DISABLED_8_1_1_5, mon_dis);
}

static
seq_ran_param_t gen_ref_qos_map(int64_t ref_qos_map)
{
  // TS 38.463
  // ENUMERATED (enabled,…)
  assert(ref_qos_map == 0);
  return gen_false_ng_ran_drb(REFLECTIVE_QOS_MAPPING_8_1_1_5, ref_qos_map);
}

static
seq_ran_param_t gen_prio_level(int64_t prio_lev)
{
  // TS 38.463
  // INTEGER (1..15)
  assert(prio_lev > 0 && prio_lev < 16);
  return gen_false_ng_ran_drb(PRIORITY_LEVEL_8_1_1_5, prio_lev);
}

static
seq_ran_param_t gen_preemp_cap(int64_t preemp_cap)
{
  // TS 38.463
  // ENUMERATED (shall not trigger pre-emption, may trigger pre-emption)
  assert(preemp_cap == 0 || preemp_cap == 1);
  return gen_false_ng_ran_drb(PRE_EMPTION_CAPABILITY_8_1_1_5, preemp_cap);
}

static
seq_ran_param_t gen_preemp_vul(int64_t preemp_vul)
{
  // TS 38.463
  // ENUMERATED (not pre-emptable, pre-emptable)
  assert(preemp_vul == 0 || preemp_vul == 1);
  return gen_false_ng_ran_drb(PRE_EMPTION_VULNERABILITY_8_1_1_5, preemp_vul);
}

static
ran_param_val_type_t gen_structure_ran_parameter_val_type(void)
{
  ran_param_val_type_t dst = {0};
  dst.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  dst.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(dst.strct != NULL && "Memory exhausted");
  return dst;
}

static
seq_ran_param_t gen_allo_ret_prio()
{
  seq_ran_param_t dst = {0}; 
  dst.ran_param_id = NG_RAN_DRB_ALLOCATION_AND_RETENTION_PRIORITY_8_1_1_5;
  dst.ran_param_val = gen_structure_ran_parameter_val_type();
  dst.ran_param_val.strct->sz_ran_param_struct = 3; 
  dst.ran_param_val.strct->ran_param_struct = calloc(dst.ran_param_val.strct->sz_ran_param_struct, sizeof(seq_ran_param_t));
  assert(dst.ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

  // Priority Level
  int64_t prio_lev = 1;
  dst.ran_param_val.strct->ran_param_struct[0] = gen_prio_level(prio_lev);

  // Pre-emption Capability
  int64_t preemp_cap = 0;
  dst.ran_param_val.strct->ran_param_struct[1] = gen_preemp_cap(preemp_cap);

  // Pre-emption Vulnerability
  int64_t preemp_vul = 0;
  dst.ran_param_val.strct->ran_param_struct[2] = gen_preemp_vul(preemp_vul);

  return dst;
}

static
seq_ran_param_t gen_mfbrd(int64_t mfbrd)
{
// TS 38.463
// INTEGER (0..4,000,000,000,000,…)
// unit is in bit/s
  assert(mfbrd > -1 && mfbrd < 4000000000001);
  return gen_false_ng_ran_drb(MAXIMUM_FLOW_BIT_RATE_DOWNLINK_8_1_1_5, mfbrd);
}

static
seq_ran_param_t gen_gfbrd(int64_t gfbrd)
{
// TS 38.463
// INTEGER (0..4,000,000,000,000,…)
// unit is in bit/s
  assert(gfbrd > -1 && gfbrd < 4000000000001);
  return gen_false_ng_ran_drb(GUARANTEED_FLOW_BIT_RATE_DOWNLINK_8_1_1_5, gfbrd);
}

static
seq_ran_param_t gen_mplrd(int64_t mplrd)
{
  // Ratio of lost packets per number
  // of packets sent, expressed in
  // tenth of percent.
  // INTEGER (0..1000,…)
  assert(mplrd > -1 &&  mplrd < 1001);
  return gen_false_ng_ran_drb(MAXIMUM_PACKET_LOSS_RATE_DOWNLINK_8_1_1_5, mplrd);
}

static
seq_ran_param_t gen_mfbru(int64_t mfbru)
{
// TS 38.463
// INTEGER (0..4,000,000,000,000,…)
// unit is in bit/s
  assert(mfbru > -1 && mfbru < 4000000000001);
  return gen_false_ng_ran_drb(MAXIMUM_FLOW_BIT_RATER_UPLINK_8_1_1_5, mfbru);
}

static
seq_ran_param_t gen_gfbru(int64_t gfbru)
{
// TS 38.463
// INTEGER (0..4,000,000,000,000,…)
// unit is in bit/s
  assert(gfbru > -1 && gfbru < 4000000000001);
  return gen_false_ng_ran_drb(GUARANTEED_FLOW_BIT_RATE_UPLINK_8_1_1_5, gfbru);
}

static
seq_ran_param_t gen_mplru(int64_t mplru)
{
  // Ratio of lost packets per number
  // of packets sent, expressed in
  // tenth of percent.
  // INTEGER (0..1000,…)
  assert(mplru> -1 &&  mplru < 1001);
  return gen_false_ng_ran_drb(MAXIMUM_PACKET_LOSS_RATE_UPLINK_8_1_1_5, mplru);
}


static
seq_ran_param_t gen_qpgf(void)
{
  seq_ran_param_t dst = {0}; 
  dst.ran_param_id = QOS_PARAMETERS_FOR_GBR_FLOWS_IN_NG_RAN_BEARER_8_1_1_5;
  dst.ran_param_val = gen_structure_ran_parameter_val_type();

  dst.ran_param_val.strct->sz_ran_param_struct = 6; 
  dst.ran_param_val.strct->ran_param_struct = calloc( dst.ran_param_val.strct->sz_ran_param_struct, sizeof(seq_ran_param_t));
  assert(dst.ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

  seq_ran_param_t* rps = dst.ran_param_val.strct->ran_param_struct; 

  // Maximum Flow Bit
  // Rate Downlink
  int64_t mfbrd;
  rps[0] = gen_mfbrd(mfbrd);

  // Guaranteed Flow
  // Bit Rate Downlink
  int64_t gfbrd;
  rps[1] = gen_gfbrd(gfbrd);

  // Maximum Packet
  // Loss Rate Downlink
  int64_t mplrd = rand()%1000;
  rps[2] = gen_mplrd(mplrd);

  // Maximum Flow Bit
  // Rate Uplink
  int64_t mfbru = rand()%1024;
  rps[3] = gen_mfbru(mfbru);

  // Guaranteed Flow
  // Bit Rate Uplink
  int64_t gfbru = rand()%1024;
  rps[4] = gen_gfbru(gfbru);

  // Maximum Packet
  // Loss Rate Uplink
  int64_t mplru = rand()%1000;
  rps[5] = gen_mplru(mplru);

  return dst;
}

static
seq_ran_param_t gen_cgid(int64_t cell_gid)
{
  // TS 38.463
  // INTEGER (0..3, …)
  assert(cell_gid > -1 && cell_gid < 4);
  return gen_true_ng_ran_drb(CELL_GROUP_ID_8_1_1_5, cell_gid);
}

static
seq_ran_param_t gen_ulc(int64_t ulc)
{
//  ENUMERATED (no-data, shared,only, ..)
  assert(ulc == 0 || ulc == 1 || ulc == 2);
  return gen_false_ng_ran_drb(UL_CONFIGURATION_CELL_GROUP , ulc);
}

static
seq_ran_param_t gen_rat_type(int64_t rat_type)
{
  // ENUMERATED (E-UTRA, NR,…) 
  assert(rat_type == 0 || rat_type == 1);
  return gen_false_ng_ran_drb(RAT_TYPE_CELL_GROUP, rat_type);
}

static
seq_ran_param_t gen_num_tun(int64_t num_tun)
{
  // INTEGER (1..4, …)
  assert(num_tun > 0 && num_tun < 5);
  return gen_false_ng_ran_drb( NUMBER_OF_TUNNELS_CELL_GROUP, num_tun);
}

static
seq_ran_param_t gen_cgs(void)
{
  seq_ran_param_t dst = {0};
  dst.ran_param_id = CELL_GROUP_8_1_1_5;
  dst.ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(dst.ran_param_val.strct != NULL && "Memory exhausted");

  // 8.1.1.7
  dst.ran_param_val.strct->sz_ran_param_struct = 3;
  dst.ran_param_val.strct->ran_param_struct = calloc(3, sizeof(seq_ran_param_t));
  assert(dst.ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

  seq_ran_param_t* rps = dst.ran_param_val.strct->ran_param_struct; 

  // UL Configuration
  int64_t ulc;
  rps[0] = gen_ulc(ulc);

  // RAT Type
  int64_t rat;
  rps[1] = gen_rat_type(rat);

  // Number of Tunnels
  int64_t num_tun;
  rps[2] = gen_num_tun(num_tun);

  return dst;
}

static
seq_ran_param_t gen_lst_cell_group_add(void)
{
  seq_ran_param_t dst = {0};
  dst.ran_param_id = LIST_OF_CELL_GROUPS_TO_BE_ADDED_8_1_1_5;
  dst.ran_param_val.type = LIST_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param_val.lst = calloc(1, sizeof(ran_param_list_t));
  assert(dst.ran_param_val.lst != NULL && "Memory exhausted");

  dst.ran_param_val.lst->sz_lst_ran_param = 1; 
  dst.ran_param_val.lst->lst_ran_param = calloc(1, sizeof(lst_ran_param_t));
  assert(dst.ran_param_val.lst->lst_ran_param != NULL && "Memory exhausted");
  lst_ran_param_t* lst_ran_param = dst.ran_param_val.lst->lst_ran_param; 

  lst_ran_param->ran_param_id = CELL_GROUP_ITEM_8_1_1_5 ; 
  lst_ran_param->ran_param_struct.sz_ran_param_struct = 2;
  lst_ran_param->ran_param_struct.ran_param_struct = calloc(2, sizeof(seq_ran_param_t));
  assert(lst_ran_param->ran_param_struct.ran_param_struct != NULL && "Memory exhausted");

  seq_ran_param_t* rps = lst_ran_param->ran_param_struct.ran_param_struct ;

  // Cell Group ID
  // ELEMENT TRUE
  int64_t cell_gid = 2;
  rps[0] = gen_cgid(cell_gid);

  // Cell Group 
  // Structure
  rps[1] = gen_cgs();

  return dst;
}

static
seq_ran_param_t gen_pdu_sid(int64_t pdu_sid)
{
  // TS 38.331
  // INTEGER (0..255)
  assert(pdu_sid > -1 && pdu_sid < 256);
  return gen_true_ng_ran_drb(PDU_SESSION_ID_8_1_1_5, pdu_sid);
}

static
seq_ran_param_t gen_ddrb(int64_t ddrb)
{
  // TS 38.331
  // ENUMERATED
  // (True, False, …)
  assert(ddrb == 0 || ddrb == 1);
  return gen_false_ng_ran_drb(DEFAULT_DRB_8_1_1_5, ddrb);
}

static
seq_ran_param_t gen_pdu_ses(int64_t pdu_ses)
{



}


static
seq_ran_param_t gen_sdap_conf(void)
{
seq_ran_param_t dst = {0}; 
dst.ran_param_id = SDAP_CONFIGURATION_8_1_1_5 ;
dst.ran_param_val.type =STRUCTURE_RAN_PARAMETER_VAL_TYPE ;
dst.ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
assert(dst.ran_param_val.strct != NULL && "Memory exhausted");

dst.ran_param_val.strct->sz_ran_param_struct = 3;
dst.ran_param_val.strct->ran_param_struct = calloc(3, sizeof(seq_ran_param_t));
assert(dst.ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

seq_ran_param_t* rps = dst.ran_param_val.strct->ran_param_struct; 

// PDU Session ID
int64_t pdu_sid = 3;
rps[0] = gen_pdu_sid(pdu_sid);

// Default DRB
int64_t ddrb;
rps[1] = gen_ddrb(ddrb);

// PDU Session
int64_t pdu_ses;
rps[2] = gen_pdu_ses(pdu_ses);

return dst;
}


// 8.1.1.5 NG-RAN Data Radio Bearer
static
ran_param_val_type_t gen_ng_ran_drb(void)
{
  ran_param_val_type_t dst = {0}; 

  dst.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  dst.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(dst.strct != NULL && "Memory exhausted");
  dst.strct->sz_ran_param_struct = 1; 
  dst.strct->ran_param_struct = calloc(dst.strct->sz_ran_param_struct, sizeof(seq_ran_param_t));
  assert(dst.strct->ran_param_struct != NULL && "Memory exhausted");
  dst.strct->ran_param_struct[0].ran_param_id = NG_RAN_DRB_Bearer_Context_Setup; 
  dst.strct->ran_param_struct[0].ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE; 

  dst.strct->ran_param_struct[0].ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t) );
  assert(dst.strct->ran_param_struct[0].ran_param_val.strct != NULL && "Memory exhausted");
  dst.strct->ran_param_struct[0].ran_param_val.strct->sz_ran_param_struct = 13; 
  dst.strct->ran_param_struct[0].ran_param_val.strct->ran_param_struct = calloc(13, sizeof(seq_ran_param_t)); 
  assert(dst.strct->ran_param_struct[0].ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");
  
  seq_ran_param_t* rps = dst.strct->ran_param_struct[0].ran_param_val.strct->ran_param_struct;

  // 5QI
  int five_qi = 10;
  rps[0] = gen_5qi(five_qi);

  // Packet Delay Budget 
  int pdb = 1000;
  rps[1] = gen_pdb(pdb);

  // Packet Error Rate
  int scalar = 1;
  int exp = 3;
  rps[2] = gen_per(scalar, exp );

  // NG-RAN DRB
  // Allocation and
  // Retention Priority 
  rps[3] = gen_allo_ret_prio(); 

  // Priority Level of the
  // mapped QoS flows
  int64_t plqf = 5;
  rps[4] = gen_plqf( plqf ); 

  // QoS parameters
  // for GBR flows in
  // NG-RAN Bearer
  rps[5] = gen_qpgf();

  // QoS Monitoring
  // Enable Request
  int64_t mon_req = 0;
  rps[6] = gen_mon_req(mon_req); 

  // QoS Monitoring
  // Reporting
  // Frequency
  int64_t mon_rep = 5;
  rps[7] = gen_mon_rep(mon_rep); 

  // QoS Monitoring
  // Disabled
  int64_t mon_dis = 0; 
  rps[8] = gen_mon_dis(mon_dis);

  // Reflective QoS
  // Mapping
  int64_t ref_qos_map = 0;
  rps[9] = gen_ref_qos_map(ref_qos_map);

  // List of cell groups
  // to be added
  rps[10] = gen_lst_cell_group_add();

  // SDAP
  // Configuration
  rps[11] = gen_sdap_conf();

  // PDCP
  // Configuration
  rps[12] = gen_pdcp_conf();

  return dst;
}

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1(void)
{
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0};

  // 8.1.2.2.1 Bearer Context Setup
  dst.sz_ran_param = 9;
  dst.ran_param = calloc(dst.sz_ran_param, sizeof(seq_ran_param_t));
  assert(dst.ran_param != NULL && "Memory exhausted");

  // DRB ID
  dst.ran_param[0].ran_param_id = DRB_ID_Bearer_Context_Setup;
  dst.ran_param[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t)) ;
  assert(dst.ran_param[0].ran_param_val.flag_true != NULL && "Memory exhausted");
  // DRB-Identity defined in TS 38.331
  // drb-Identity add/mod
  // Let's suppose that 1 is the default bearer
  dst.ran_param[0].ran_param_val.flag_true->int_ran = 2; // [1-32] 

  // CHOICE DRB Type
  dst.ran_param[1].ran_param_id = CHOICE_DRB_Type_Bearer_Context_Setup;
  dst.ran_param[1].ran_param_val = gen_ng_ran_drb();

  return dst;
}

typedef enum {
  DRB_ID_8_4_2_2 = 1,
  LIST_OF_QOS_FLOWS_MOD_IN_DRB_8_4_2_2 = 2,
  QOS_FLOW_ITEM_8_4_2_2 = 3,
  QOS_FLOW_ID_8_4_2_2 = 4,
  QOS_FLOW_MAPPING_IND_8_4_2_2 = 5,
} qos_flow_mapping_conf_e;

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1_qos_flow_map()
{
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0}; 

  // 8.4.2.2 QoS flow mapping configuration
  dst.sz_ran_param = 2;
  dst.ran_param = calloc(1, sizeof(seq_ran_param_t));
  assert(dst.ran_param != NULL && "Memory exhausted");

  dst.ran_param[0].ran_param_id = DRB_ID_8_4_2_2;
  dst.ran_param[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t)) ;
  assert(dst.ran_param[0].ran_param_val.flag_true != NULL && "Memory exhausted");

  // Let's suppose that it is the DRB 5 
  dst.ran_param[0].ran_param_val.flag_true->type = INTEGER_RAN_PARAMETER_VALUE; 
  dst.ran_param[0].ran_param_val.flag_true->int_ran = 5; 

  // List of QoS Flows to be modified in DRB
  dst.ran_param[1].ran_param_id = LIST_OF_QOS_FLOWS_MOD_IN_DRB_8_4_2_2;
  dst.ran_param[1].ran_param_val.type = LIST_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[1].ran_param_val.lst = calloc(1, sizeof(ran_param_list_t));
  assert(dst.ran_param[1].ran_param_val.lst != NULL && "Memory exhausted");
  ran_param_list_t* rpl = dst.ran_param[1].ran_param_val.lst;

  rpl->sz_lst_ran_param = 1; 
  rpl->lst_ran_param = calloc(1, sizeof(lst_ran_param_t));

  // QoS Flow Item
  rpl->lst_ran_param[0].ran_param_id = QOS_FLOW_ITEM_8_4_2_2; 
  rpl->lst_ran_param[0].ran_param_struct.sz_ran_param_struct = 2;
  rpl->lst_ran_param[0].ran_param_struct.ran_param_struct = calloc(2, sizeof(seq_ran_param_t));
  assert(rpl->lst_ran_param[0].ran_param_struct.ran_param_struct != NULL && "Memory exhausted");
  seq_ran_param_t* rps = rpl->lst_ran_param[0].ran_param_struct.ran_param_struct ;

  // QoS Flow Identifier
  rps[0].ran_param_id = QOS_FLOW_ID_8_4_2_2;
  rps[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  rps[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps[0].ran_param_val.flag_true != NULL && "Memory exhausted");
  rps[0].ran_param_val.flag_true->type = INTEGER_RAN_PARAMETER_VALUE; 
  // Let's suppose that we have QFI 10
  rps[0].ran_param_val.flag_true->int_ran = 10; 

  // QoS Flow Mapping Indication
  rps[1].ran_param_id = QOS_FLOW_MAPPING_IND_8_4_2_2;
  rps[1].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE; 
  rps[1].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps[1].ran_param_val.flag_false != NULL && "Memory exhausted"); 

  // ENUMERATED (ul, dl, ...) 
  rps[1].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  rps[1].ran_param_val.flag_false->int_ran = 1; 

  return dst;
}



static
e2sm_rc_ctrl_msg_t gen_rc_ctrl_msg(void)
{
  e2sm_rc_ctrl_msg_t dst = {0}; 
  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_MSG;
  //dst.frmt_1 = gen_rc_ctrl_msg_frmt_1();
  dst.frmt_1 = gen_rc_ctrl_msg_frmt_1_qos_flow_map();
 
  return dst;
}
*/

typedef enum{
  DRB_QoS_Configuration_7_6_2_1 = 1,
  QoS_flow_mapping_configuration_7_6_2_1 = 2,
  Logical_channel_configuration_7_6_2_1 = 3,
  Radio_admission_control_7_6_2_1 = 4,
  DRB_termination_control_7_6_2_1 = 5,
  DRB_split_ratio_control_7_6_2_1 = 6,
  PDCP_Duplication_control_7_6_2_1 = 7,
} rc_ctrl_service_style_1_e;

static
e2sm_rc_ctrl_hdr_frmt_1_t gen_rc_ctrl_hdr_frmt_1(void)
{
  e2sm_rc_ctrl_hdr_frmt_1_t dst = {0};

  // 6.2.2.6
  {
  lock_guard(&mtx);
  dst.ue_id = cp_ue_id_e2sm(&ue_id);
  }
  // CONTROL Service Style 1: Radio Bearer Control
  dst.ric_style_type = 1;

  // QoS flow mapping conf 
  dst.ctrl_act_id = QoS_flow_mapping_configuration_7_6_2_1 ;

  return dst;
}

static
e2sm_rc_ctrl_hdr_t gen_rc_ctrl_hdr(void)
{
  e2sm_rc_ctrl_hdr_t dst = {0};
  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_HDR;
  dst.frmt_1 = gen_rc_ctrl_hdr_frmt_1();
  return dst;
}

typedef enum {
  DRB_ID_8_4_2_2 = 1,
  LIST_OF_QOS_FLOWS_MOD_IN_DRB_8_4_2_2 = 2,
  QOS_FLOW_ITEM_8_4_2_2 = 3,
  QOS_FLOW_ID_8_4_2_2 = 4,
  QOS_FLOW_MAPPING_IND_8_4_2_2 = 5,
} qos_flow_mapping_conf_e;

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1_qos_flow_map()
{
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0}; 

  // 8.4.2.2 QoS flow mapping configuration
  dst.sz_ran_param = 2;
  dst.ran_param = calloc(2, sizeof(seq_ran_param_t));
  assert(dst.ran_param != NULL && "Memory exhausted");

  dst.ran_param[0].ran_param_id = DRB_ID_8_4_2_2;
  dst.ran_param[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t)) ;
  assert(dst.ran_param[0].ran_param_val.flag_true != NULL && "Memory exhausted");

  // Let's suppose that it is the DRB 5 
  dst.ran_param[0].ran_param_val.flag_true->type = INTEGER_RAN_PARAMETER_VALUE; 
  dst.ran_param[0].ran_param_val.flag_true->int_ran = 5; 

  // List of QoS Flows to be modified in DRB
  dst.ran_param[1].ran_param_id = LIST_OF_QOS_FLOWS_MOD_IN_DRB_8_4_2_2;
  dst.ran_param[1].ran_param_val.type = LIST_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[1].ran_param_val.lst = calloc(1, sizeof(ran_param_list_t));
  assert(dst.ran_param[1].ran_param_val.lst != NULL && "Memory exhausted");
  ran_param_list_t* rpl = dst.ran_param[1].ran_param_val.lst;

  rpl->sz_lst_ran_param = 1; 
  rpl->lst_ran_param = calloc(1, sizeof(lst_ran_param_t));
  assert(rpl->lst_ran_param != NULL && "Memory exhausted");

  // QoS Flow Item
  rpl->lst_ran_param[0].ran_param_id = QOS_FLOW_ITEM_8_4_2_2; 
  rpl->lst_ran_param[0].ran_param_struct.sz_ran_param_struct = 2;
  rpl->lst_ran_param[0].ran_param_struct.ran_param_struct = calloc(2, sizeof(seq_ran_param_t));
  assert(rpl->lst_ran_param[0].ran_param_struct.ran_param_struct != NULL && "Memory exhausted");
  seq_ran_param_t* rps = rpl->lst_ran_param[0].ran_param_struct.ran_param_struct ;

  // QoS Flow Identifier
  rps[0].ran_param_id = QOS_FLOW_ID_8_4_2_2;
  rps[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  rps[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps[0].ran_param_val.flag_true != NULL && "Memory exhausted");
  rps[0].ran_param_val.flag_true->type = INTEGER_RAN_PARAMETER_VALUE; 
  // Let's suppose that we have QFI 10
  rps[0].ran_param_val.flag_true->int_ran = 10; 

  // QoS Flow Mapping Indication
  rps[1].ran_param_id = QOS_FLOW_MAPPING_IND_8_4_2_2;
  rps[1].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE; 
  rps[1].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps[1].ran_param_val.flag_false != NULL && "Memory exhausted"); 

  // ENUMERATED (ul, dl, ...) 
  rps[1].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  rps[1].ran_param_val.flag_false->int_ran = 1; 

  return dst;
}

static
e2sm_rc_ctrl_msg_t gen_rc_ctrl_msg(void)
{
  e2sm_rc_ctrl_msg_t dst = {0}; 

  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_MSG;
  //dst.frmt_1 = gen_rc_ctrl_msg_frmt_1();
  dst.frmt_1 = gen_rc_ctrl_msg_frmt_1_qos_flow_map();

  return dst;
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

  sm_ans_xapp_t* h = calloc(nodes.len, sizeof(sm_ans_xapp_t)); 
  assert(h != NULL && "Memory exhausted");


  pthread_mutexattr_t attr = {0};
  int rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);


  //////////// 
  // START KPM 
  //////////// 
  kpm_sub_data_t kpm_sub = {0};
  defer({ free_kpm_sub_data(&kpm_sub); });

  // KPM Event Trigger
  uint64_t period_ms = 100;
  kpm_sub.ev_trg_def = gen_ev_trig(period_ms);

  // KPM Action Definition
  kpm_sub.sz_ad = 1;
  kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
  assert(kpm_sub.ad != NULL && "Memory exhausted");
  const char act[] = "DRB.RlcSduDelayDl";
  *kpm_sub.ad = gen_act_def(act); 

  const int KPM_ran_function = 2;

  for(size_t i =0; i < nodes.len; ++i){ 
    h[i] = report_sm_xapp_api(&nodes.n[i].id, KPM_ran_function, &kpm_sub, sm_cb_kpm);
    assert(h[i].success == true);
  } 

  //////////// 
  // END KPM 
  //////////// 

  sleep(5);

  //////////// 
  // START RC 
  //////////// 

  // RC On Demand report
  //  rc_sub_data_t rc_sub = {0};
  //  defer({ free_rc_sub_data(&rc_sub); });
  //  sm_ans_xapp_t h_2 = report_sm_xapp_api(&nodes.n[0].id, RC_ran_function, &rc_sub, sm_cb_rc);
  //  assert(h_2.success == true);


  // RC Control 
  rc_ctrl_req_data_t rc_ctrl = {0};
  defer({ free_rc_ctrl_req_data(&rc_ctrl); });

  rc_ctrl.hdr = gen_rc_ctrl_hdr();
  rc_ctrl.msg = gen_rc_ctrl_msg();

  const int RC_ran_function = 3;

  for(size_t i =0; i < nodes.len; ++i){ 
    control_sm_xapp_api(&nodes.n[i].id, RC_ran_function, &rc_ctrl);
  }

  //////////// 
  // END RC 
  //////////// 

  sleep(5);

  for(int i = 0; i < nodes.len; ++i){
    // Remove the handle previously returned
    rm_report_sm_xapp_api(h[i].u.handle);
  }

  sleep(1);
  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  free(h);

  rc = pthread_mutex_destroy(&mtx);
  assert(rc == 0);

  printf("Test xApp run SUCCESSFULLY\n");
}

