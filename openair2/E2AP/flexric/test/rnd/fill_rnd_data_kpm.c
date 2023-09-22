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

#include "fill_rnd_data_kpm.h"
#include "../../src/sm/kpm_sm_v03.00/kpm_sm_id.h"
#include "../../src/util/time_now_us.h"

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

kpm_event_trigger_def_t fill_kpm_event_trigger_def(void)
{
  kpm_event_trigger_def_t dst = {0};

  dst.type = rand()%END_FORMAT_RIC_EVENT_TRIGGER;

  if(dst.type == FORMAT_1_RIC_EVENT_TRIGGER  ){
    // Normally the period is not free, need to check in the specs
    dst.kpm_ric_event_trigger_format_1.report_period_ms = 2;  // rand()%1000;
  } else {
    assert(0 != 0 && "unknown type");
  }

  return dst;
}

static bin_range_def_t fill_bin_range_def(void)
{
  bin_range_def_t bin_range_def = {0};

  bin_range_def.bin_x_lst_len = 3;  // (rand() % 65535) + 1;

  bin_range_def.bin_x_lst = calloc(bin_range_def.bin_x_lst_len, sizeof(bin_distr_t));
  assert(bin_range_def.bin_x_lst != NULL && "Memory exhausted");

  for (size_t j = 0; j<bin_range_def.bin_x_lst_len; j++)
  {
    bin_range_def.bin_x_lst[j].bin_index = (rand() % 65535) + 0;
    bin_range_def.bin_x_lst[j].start_value.value = rand()%END_BIN_RANGE;


    // start value
    switch (bin_range_def.bin_x_lst[j].start_value.value)
    {
    case INTEGER_BIN_RANGE:
      bin_range_def.bin_x_lst[j].start_value.int_value = (rand() % 4294967296) + 0;
      break;

    case REAL_BIN_RANGE:
      bin_range_def.bin_x_lst[j].start_value.real_value = (rand() % 256) + 0;
    
    default:
      break;
    }

    // end value
    bin_range_def.bin_x_lst[j].end_value.value = rand()%END_BIN_RANGE;

    switch (bin_range_def.bin_x_lst[j].end_value.value)
    {
    case INTEGER_BIN_RANGE:
      bin_range_def.bin_x_lst[j].end_value.int_value = (rand() % 4294967296) + 0;
      break;

    case REAL_BIN_RANGE:
      bin_range_def.bin_x_lst[j].end_value.real_value = (rand() % 256) + 0;

    default:
      break;
    }
  }
    
  bin_range_def.bin_y_lst_len = 0;
  bin_range_def.bin_y_lst = NULL;

  bin_range_def.bin_z_lst_len = 0;
  bin_range_def.bin_z_lst = NULL;

  return bin_range_def;
}

static kpm_act_def_format_1_t fill_kpm_action_def_frm_1(void)
{
  kpm_act_def_format_1_t action_def_frm_1 = {0};

  // Measurement Information
  // [1, 65535]
  // mir: Here there is a bug. uncomment the following line i.e., (rand() % 65535)
  action_def_frm_1.meas_info_lst_len = 3; //(rand() % 65535) + 1;
                                            
  action_def_frm_1.meas_info_lst = calloc(action_def_frm_1.meas_info_lst_len, sizeof(meas_info_format_1_lst_t));
  assert(action_def_frm_1.meas_info_lst != NULL && "Memory exhausted" );
  
  for (size_t i = 0; i < action_def_frm_1.meas_info_lst_len; i++)
  {
    meas_info_format_1_lst_t* meas_info = &action_def_frm_1.meas_info_lst[i];
    
    // Measurement Type
    meas_info->meas_type.type = rand()%END_MEAS_TYPE;

    switch (meas_info->meas_type.type)
    {
    case NAME_MEAS_TYPE:
      meas_info->meas_type.type = NAME_MEAS_TYPE;
      meas_info->meas_type.name.buf = calloc(strlen("test") + 1, sizeof(char));
      memcpy(meas_info->meas_type.name.buf, "test", strlen("test"));
      meas_info->meas_type.name.len = strlen("test");
      break;
    
    case ID_MEAS_TYPE:
      meas_info->meas_type.type = ID_MEAS_TYPE;
      meas_info->meas_type.id = (rand() % 65536) + 1;
      break;

    default:
      assert(false && "Unknown Measurement Type");
    }
  

    // Label Information
    // [1, 2147483647]
    meas_info->label_info_lst_len = 2 ; // With the real number, we can run out of memory i.e., 2147483647
    meas_info->label_info_lst = calloc(meas_info->label_info_lst_len, sizeof(label_info_lst_t));
    assert(meas_info->label_info_lst != NULL && "Memory exhausted" );
    
    for (size_t j = 0; j < meas_info->label_info_lst_len; j++)
    {
      if(j%2){
      meas_info->label_info_lst[j].plmn_id = malloc(sizeof(e2sm_plmn_t));
      assert(meas_info->label_info_lst[j].plmn_id  != NULL);

      *meas_info->label_info_lst[j].plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
      } else {
       action_def_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = malloc(sizeof(enum_value_e));
       *action_def_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = TRUE_ENUM_VALUE;
      }
    }
  }
  
  // Granularity Period
  action_def_frm_1.gran_period_ms = (rand() % 4294967295) + 0;
  
  // Cell Global ID - OPTIONAL
  action_def_frm_1.cell_global_id = calloc(1, sizeof(*action_def_frm_1.cell_global_id));
  action_def_frm_1.cell_global_id->type = rand()%END_CGI_RAT_TYPE;
  
  switch (action_def_frm_1.cell_global_id->type)
  {
  case NR_CGI_RAT_TYPE:
    action_def_frm_1.cell_global_id->nr_cgi.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    action_def_frm_1.cell_global_id->nr_cgi.nr_cell_id = (rand() % 2^36) + 1;
    break;

  case EUTRA_CGI_RAT_TYPE:
    action_def_frm_1.cell_global_id->eutra.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    action_def_frm_1.cell_global_id->eutra.eutra_cell_id = (rand() % 2^28) + 0;
    break;
  
  default:
    assert(false && "Unknown Cell Global ID Type");
  }
  
  // Measurement Bin Range - OPTIONAL
  // not yet implemented in ASN.1 - possible extension
  action_def_frm_1.meas_bin_range_info_lst_len = 3;  // (rand() % 65535) + 0;
  action_def_frm_1.meas_bin_info_lst = calloc(action_def_frm_1.meas_bin_range_info_lst_len, sizeof(meas_bin_range_info_lst_t));
  assert(action_def_frm_1.meas_bin_info_lst != NULL && "Memory exhausted");

  for (size_t i = 0; i<action_def_frm_1.meas_bin_range_info_lst_len; i++)
  {
    // Measurement Type
    action_def_frm_1.meas_bin_info_lst[i].meas_type.type = rand()%END_MEAS_TYPE;

    switch (action_def_frm_1.meas_bin_info_lst[i].meas_type.type)
    {
    case NAME_MEAS_TYPE:
      action_def_frm_1.meas_bin_info_lst[i].meas_type.type = NAME_MEAS_TYPE;
      action_def_frm_1.meas_bin_info_lst[i].meas_type.name.buf = calloc(strlen("test") + 1, sizeof(char));
      memcpy(action_def_frm_1.meas_bin_info_lst[i].meas_type.name.buf, "test", strlen("test"));
      action_def_frm_1.meas_bin_info_lst[i].meas_type.name.len = strlen("test");
      break;
    
    case ID_MEAS_TYPE:
      action_def_frm_1.meas_bin_info_lst[i].meas_type.type = ID_MEAS_TYPE;
      action_def_frm_1.meas_bin_info_lst[i].meas_type.id = (rand() % 65535) + 1;
      break;

    default:
      assert(false && "Unknown Measurement Type");
    }

    // Bin Range Definition
    action_def_frm_1.meas_bin_info_lst[i].bin_range_def = fill_bin_range_def();

  }

  return action_def_frm_1;
}

static gnb_e2sm_t fill_gnb_data(void)
{
  gnb_e2sm_t gnb = {0};

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  gnb.amf_ue_ngap_id = (rand() % 2^40) + 0;

  // Mandatory
  //GUAMI 6.2.3.17 
  gnb.guami.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  
  gnb.guami.amf_region_id = (rand() % 2^8) + 0;
  gnb.guami.amf_set_id = (rand() % 2^10) + 0;
  gnb.guami.amf_ptr = (rand() % 2^6) + 0;

  // gNB-CU UE F1AP ID List
  // C-ifCUDUseparated 
  gnb.gnb_cu_ue_f1ap_lst_len = (rand() % 4) + 1;
  gnb.gnb_cu_ue_f1ap_lst = calloc(gnb.gnb_cu_ue_f1ap_lst_len, sizeof(uint32_t));
  for (size_t i = 0; i < gnb.gnb_cu_ue_f1ap_lst_len; i++)
  {
    gnb.gnb_cu_ue_f1ap_lst[i] = (rand() % 4294967296) + 0;
  }

  //gNB-CU-CP UE E1AP ID List
  //C-ifCPUPseparated 
  gnb.gnb_cu_cp_ue_e1ap_lst_len = 3; //(rand() % 65535) + 1;
  gnb.gnb_cu_cp_ue_e1ap_lst = calloc(gnb.gnb_cu_cp_ue_e1ap_lst_len, sizeof(uint32_t));
  for (size_t i = 0; i < gnb.gnb_cu_cp_ue_e1ap_lst_len; i++)
  {
    gnb.gnb_cu_cp_ue_e1ap_lst[i] = (rand() % 4294967296) + 0;
  }

  // RAN UE ID
  // Optional
  // 6.2.3.25
  gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(gnb.ran_ue_id != NULL && "Memory exhausted");
  *gnb.ran_ue_id = 14294967296; // (rand() % 2^64) + 0;

  //  M-NG-RAN node UE XnAP ID
  // C- ifDCSetup
  // 6.2.3.19
  gnb.ng_ran_node_ue_xnap_id = calloc(1, sizeof(uint32_t));
  assert(gnb.ng_ran_node_ue_xnap_id != NULL && "Memory exhausted");
  *gnb.ng_ran_node_ue_xnap_id = (rand() % 4294967296) + 0;

  // Global gNB ID
  // 6.2.3.3
  // Optional
  // This IE shall not be used. Global NG-RAN Node ID IE shall replace this IE 
  gnb.global_gnb_id = calloc(1, sizeof(global_gnb_id_t));
  assert(gnb.global_gnb_id != NULL && "Memory exhausted");
  gnb.global_gnb_id->type = GNB_TYPE_ID;
  gnb.global_gnb_id->plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  gnb.global_gnb_id->gnb_id = (rand() % 4294967296) + 0;

  // Global NG-RAN Node ID
  // C-ifDCSetup
  // 6.2.3.2
  gnb.global_ng_ran_node_id = calloc(1, sizeof(*gnb.global_ng_ran_node_id));
  gnb.global_ng_ran_node_id->type = rand()%END_GLOBAL_TYPE_ID;

  switch (gnb.global_ng_ran_node_id->type)
  {
  case GNB_GLOBAL_TYPE_ID:
    gnb.global_ng_ran_node_id->global_gnb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    gnb.global_ng_ran_node_id->global_gnb_id.type = GNB_TYPE_ID;
    gnb.global_ng_ran_node_id->global_gnb_id.gnb_id = (rand() % 4294967296) + 0;
    break;
  
  case NG_ENB_GLOBAL_TYPE_ID:
    gnb.global_ng_ran_node_id->global_ng_enb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    gnb.global_ng_ran_node_id->global_ng_enb_id.type = rand()%END_NG_ENB_TYPE_ID;

    switch (gnb.global_ng_ran_node_id->global_ng_enb_id.type)
    {
    case MACRO_NG_ENB_TYPE_ID:
      gnb.global_ng_ran_node_id->global_ng_enb_id.macro_ng_enb_id = (rand() % 2^20) + 0;
      break;

    case SHORT_MACRO_NG_ENB_TYPE_ID:
      gnb.global_ng_ran_node_id->global_ng_enb_id.short_macro_ng_enb_id = (rand() % 2^18) + 0;
      break;

    case LONG_MACRO_NG_ENB_TYPE_ID:
      gnb.global_ng_ran_node_id->global_ng_enb_id.long_macro_ng_enb_id = (rand() % 2^21) + 0;
      break;
    
    default:
      assert(false && "Unknown Global NG eNB ID Type");
    }
    break;

  default:
    assert(false && "Unknown Global NG-RAN Node ID Type");
  }

  return gnb;
}

static gnb_du_e2sm_t fill_gnb_du_data(void)
{
  gnb_du_e2sm_t gnb_du = {0};
  gnb_du.gnb_cu_ue_f1ap = (rand() % 4294967296) + 0;

  gnb_du.ran_ue_id = calloc(1, sizeof(*gnb_du.ran_ue_id));
  *gnb_du.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_du;
}

static gnb_cu_up_e2sm_t fill_gnb_cu_up_data(void)
{
  gnb_cu_up_e2sm_t gnb_cu_up = {0};
  gnb_cu_up.gnb_cu_cp_ue_e1ap = (rand() % 4294967296) + 0;

  gnb_cu_up.ran_ue_id = calloc(1, sizeof(*gnb_cu_up.ran_ue_id));
  *gnb_cu_up.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_cu_up;
}

static ng_enb_e2sm_t fill_ng_enb_data(void)
{
  ng_enb_e2sm_t ng_enb = {0};

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  ng_enb.amf_ue_ngap_id = (rand() % 2^40) + 0;

  // 6.2.3.17
  // Mandatory
  // GUAMI
  ng_enb.guami.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  
  ng_enb.guami.amf_region_id = (rand() % 2^8) + 0;
  ng_enb.guami.amf_set_id = (rand() % 2^10) + 0;
  ng_enb.guami.amf_ptr = (rand() % 2^6) + 0;

  // 6.2.3.22
  // C-if CU DU separated
  // ng-eNB-CU UE W1AP ID
  ng_enb.ng_enb_cu_ue_w1ap_id = calloc(1, sizeof(uint32_t));
  assert(ng_enb.ng_enb_cu_ue_w1ap_id != NULL && "Memory exhausted");
  *ng_enb.ng_enb_cu_ue_w1ap_id = (rand() % 4294967296) + 0;

  // 6.2.3.19
  // C- ifDCSetup
  // M-NG-RAN node UE XnAP ID
  ng_enb.ng_ran_node_ue_xnap_id = calloc(1, sizeof(uint32_t));
  assert(ng_enb.ng_ran_node_ue_xnap_id != NULL && "Memory exhausted");
  *ng_enb.ng_ran_node_ue_xnap_id = (rand() % 4294967296) + 0;

  // OPTIONAL
  // This IE shall not be used. Global NG-RAN Node ID IE shall replace this IE
  ng_enb.global_ng_enb_id = calloc(1, sizeof(*ng_enb.global_ng_enb_id));
  assert(ng_enb.global_ng_enb_id != NULL && "Memory exhausted");
  ng_enb.global_ng_enb_id->plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};

  ng_enb.global_ng_enb_id->type = rand()%END_NG_ENB_TYPE_ID;

  switch (ng_enb.global_ng_enb_id->type)
  {
  case MACRO_NG_ENB_TYPE_ID:
    ng_enb.global_ng_enb_id->macro_ng_enb_id = (rand() % 2^20) + 0;
    break;

  case SHORT_MACRO_NG_ENB_TYPE_ID:
    ng_enb.global_ng_enb_id->short_macro_ng_enb_id = (rand() % 2^18) + 0;
    break;

  case LONG_MACRO_NG_ENB_TYPE_ID:
    ng_enb.global_ng_enb_id->long_macro_ng_enb_id = (rand() % 2^21) + 0;
    break;
  
  default:
    assert(false && "Unknown Global NG eNB ID Type");
  }


  // Global NG-RAN Node ID
  // C-ifDCSetup
  // 6.2.3.2
  ng_enb.global_ng_ran_node_id = calloc(1, sizeof(*ng_enb.global_ng_ran_node_id));
  assert(ng_enb.global_ng_ran_node_id != NULL && "Memory exhausted");
  ng_enb.global_ng_ran_node_id->type = rand()%END_GLOBAL_TYPE_ID;

  switch (ng_enb.global_ng_ran_node_id->type)
  {
  case GNB_GLOBAL_TYPE_ID:
    ng_enb.global_ng_ran_node_id->global_gnb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    ng_enb.global_ng_ran_node_id->global_gnb_id.type = GNB_TYPE_ID;
    ng_enb.global_ng_ran_node_id->global_gnb_id.gnb_id = (rand() % 4294967296) + 0;
    break;
  
  case NG_ENB_GLOBAL_TYPE_ID:
    ng_enb.global_ng_ran_node_id->global_ng_enb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    ng_enb.global_ng_ran_node_id->global_ng_enb_id.type = rand()%END_NG_ENB_TYPE_ID;

    switch (ng_enb.global_ng_ran_node_id->global_ng_enb_id.type)
    {
    case MACRO_NG_ENB_TYPE_ID:
      ng_enb.global_ng_ran_node_id->global_ng_enb_id.macro_ng_enb_id = (rand() % 2^20) + 0;
      break;

    case SHORT_MACRO_NG_ENB_TYPE_ID:
      ng_enb.global_ng_ran_node_id->global_ng_enb_id.short_macro_ng_enb_id = (rand() % 2^18) + 0;
      break;

    case LONG_MACRO_NG_ENB_TYPE_ID:
      ng_enb.global_ng_ran_node_id->global_ng_enb_id.long_macro_ng_enb_id = (rand() % 2^21) + 0;
      break;
    
    default:
      assert(false && "Unknown Global NG eNB ID Type");
    }
    break;

  default:
    assert(false && "Unknown Global NG-RAN Node ID Type");
  }

  return ng_enb;
}

static ng_enb_du_e2sm_t fill_ng_enb_du_data(void)
{
  ng_enb_du_e2sm_t ng_enb_du = {0};

  // 6.2.3.22
  // C-if CU DU separated
  // ng-eNB-CU UE W1AP ID
  ng_enb_du.ng_enb_cu_ue_w1ap_id = (rand() % 4294967296) + 0;

  return ng_enb_du;
}

static en_gnb_e2sm_t fill_en_gnb_data(void)
{
  en_gnb_e2sm_t en_gnb = {0};

  // 6.2.3.23
  // Mandatory
  // MeNB UE X2AP ID
  en_gnb.enb_ue_x2ap_id = rand() % 4096;

  // 6.2.3.24
  // OPTIONAL
  // MeNB UE X2AP ID Extension
  en_gnb.enb_ue_x2ap_id_extension = calloc(1, sizeof(uint16_t));
  assert(en_gnb.enb_ue_x2ap_id_extension != NULL && "Memory exhausted");
  *en_gnb.enb_ue_x2ap_id_extension = rand() % 4096;

  // 6.2.3.9
  // Mandatory
  // Global eNB ID
  en_gnb.global_enb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};

  en_gnb.global_enb_id.type = rand()%SHORT_MACRO_ENB_TYPE_ID;

  switch (en_gnb.global_enb_id.type)
  {
  case MACRO_ENB_TYPE_ID:
    en_gnb.global_enb_id.macro_enb_id = (rand() % 2^20) + 0;
    break;
  
  case HOME_ENB_TYPE_ID:
    en_gnb.global_enb_id.home_enb_id = (rand() % 2^28) + 0;
    break;

  /* Possible extensions: */
  // case SHORT_MACRO_ENB_TYPE_ID:
  //   en_gnb.global_enb_id.short_macro_enb_id = (rand() % 2^18) + 0;
  //   break;

  // case LONG_MACRO_ENB_TYPE_ID:
  //   en_gnb.global_enb_id.long_macro_enb_id = (rand() % 2^21) + 0;
  //   break;

  default:
    break;
  }

  // 6.2.3.21
  // gNB-CU UE F1AP ID
  // C-ifCUDUseparated 
  en_gnb.gnb_cu_ue_f1ap_lst = calloc(1, sizeof(uint32_t));
  assert(en_gnb.gnb_cu_ue_f1ap_lst != NULL && "Memory exhausted");
  *en_gnb.gnb_cu_ue_f1ap_lst = (rand() % 4294967296) + 0;

  // gNB-CU-CP UE E1AP ID List
  // C-ifCPUPseparated 
  en_gnb.gnb_cu_cp_ue_e1ap_lst_len = 3;
  en_gnb.gnb_cu_cp_ue_e1ap_lst = calloc(en_gnb.gnb_cu_cp_ue_e1ap_lst_len, sizeof(uint32_t));
  for (size_t i = 0; i < en_gnb.gnb_cu_cp_ue_e1ap_lst_len; i++)
  {
    en_gnb.gnb_cu_cp_ue_e1ap_lst[i] = (rand() % 4294967296) + 0;
  }

  // RAN UE ID
  // Optional
  // 6.2.3.25
  en_gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(en_gnb.ran_ue_id != NULL && "Memory exhausted");
  *en_gnb.ran_ue_id = (rand() % 2^64) + 0;

  return en_gnb;
}

static enb_e2sm_t fill_enb_data(void)
{
  enb_e2sm_t enb = {0};

  // 6.2.3.26
  // Mandatory
  // MME UE S1AP ID
  enb.mme_ue_s1ap_id = (rand() % 4294967296) + 0;

  // 6.2.3.18
  // Mandatory
  // GUMMEI
  enb.gummei.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  enb.gummei.mme_group_id = (rand() % 2^16) + 0;
  enb.gummei.mme_code = (rand() % 2^8) + 0;

  // 6.2.3.23
  // C-ifDCSetup
  // MeNB UE X2AP ID
  enb.enb_ue_x2ap_id = calloc(1, sizeof(uint16_t));
  assert(enb.enb_ue_x2ap_id != NULL && "Memory exhausted");
  *enb.enb_ue_x2ap_id = rand() % 4096;

  // 6.2.3.24
  // C-ifDCSetup
  // MeNB UE X2AP ID Extension
  enb.enb_ue_x2ap_id_extension = calloc(1, sizeof(uint16_t));
  assert(enb.enb_ue_x2ap_id_extension != NULL && "Memory exhausted");
  *enb.enb_ue_x2ap_id_extension = rand() % 4096;

  // 6.2.3.9
  // C-ifDCSetup
  // Global eNB ID
  enb.global_enb_id = calloc(1, sizeof(*enb.global_enb_id));
  assert(enb.global_enb_id != NULL && "Memory exhausted");

  enb.global_enb_id->plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};

  enb.global_enb_id->type = rand()%SHORT_MACRO_ENB_TYPE_ID;

  switch (enb.global_enb_id->type)
  {
  case MACRO_ENB_TYPE_ID:
    enb.global_enb_id->macro_enb_id = (rand() % 2^20) + 0;
    break;
  
  case HOME_ENB_TYPE_ID:
    enb.global_enb_id->home_enb_id = (rand() % 2^28) + 0;
    break;

  /* Possible extensions: */
  // case SHORT_MACRO_ENB_TYPE_ID:
  //   enb.global_enb_id->short_macro_enb_id = (rand() % 2^18) + 0;
  //   break;

  // case LONG_MACRO_ENB_TYPE_ID:
  //   enb.global_enb_id->long_macro_enb_id = (rand() % 2^21) + 0;
  //   break;

  default:
    break;
  }

  return enb;
}

static ue_id_e2sm_t fill_ue_id_data(void)
{
  ue_id_e2sm_t ue_id_data = {0};

  ue_id_data.type = rand()%END_UE_ID_E2SM;

  switch (ue_id_data.type)
  {
  case GNB_UE_ID_E2SM:
    ue_id_data.gnb = fill_gnb_data();
    break;

  case GNB_DU_UE_ID_E2SM:
    ue_id_data.gnb_du = fill_gnb_du_data();
    break;
  
  case GNB_CU_UP_UE_ID_E2SM:
    ue_id_data.gnb_cu_up = fill_gnb_cu_up_data();
    break;

  case NG_ENB_UE_ID_E2SM:
    ue_id_data.ng_enb = fill_ng_enb_data();
    break;

  case NG_ENB_DU_UE_ID_E2SM:
    ue_id_data.ng_enb_du = fill_ng_enb_du_data();
    break;

  case EN_GNB_UE_ID_E2SM:
    ue_id_data.en_gnb = fill_en_gnb_data();
    break;

  case ENB_UE_ID_E2SM:
    ue_id_data.enb = fill_enb_data();
    break;
  
  default:
    assert(false && "Unknown UE ID Type");
  }


  return ue_id_data;
}

static kpm_act_def_format_2_t fill_kpm_action_def_frm_2(void)
{
  kpm_act_def_format_2_t action_def_frm_2 = {0};

  // UE ID
  action_def_frm_2.ue_id = fill_ue_id_data();
  

  // Action Definition Format 1

  action_def_frm_2.action_def_format_1 = fill_kpm_action_def_frm_1();

  return action_def_frm_2;
}

static test_info_lst_t fill_kpm_test_info(void)
{
  test_info_lst_t test_info = {0};

  test_info.test_cond_type = rand()%END_TEST_COND_TYPE_KPM_V2;

  switch (test_info.test_cond_type)
  {
  case GBR_TEST_COND_TYPE:
    test_info.GBR = TRUE_TEST_COND_TYPE;
    break;
  
  case AMBR_TEST_COND_TYPE:
    test_info.AMBR = TRUE_TEST_COND_TYPE;
    break;
  
  case IsStat_TEST_COND_TYPE:
    test_info.IsStat = TRUE_TEST_COND_TYPE;
    break;
  
  case IsCatM_TEST_COND_TYPE:
    test_info.IsCatM = TRUE_TEST_COND_TYPE;
    break;
  
  case DL_RSRP_TEST_COND_TYPE:
    test_info.DL_RSRP = TRUE_TEST_COND_TYPE;
    break;
  
  case DL_RSRQ_TEST_COND_TYPE:
    test_info.DL_RSRQ = TRUE_TEST_COND_TYPE;
    break;
  
  /* Possible extensions: */
  case UL_RSRP_TEST_COND_TYPE:
    test_info.UL_RSRP = TRUE_TEST_COND_TYPE;
    break;
  
  case CQI_TEST_COND_TYPE:
    test_info.CQI = TRUE_TEST_COND_TYPE;
    break;
  
  case fiveQI_TEST_COND_TYPE:
    test_info.fiveQI = TRUE_TEST_COND_TYPE;
    break;
  
  case QCI_TEST_COND_TYPE:
    test_info.QCI = TRUE_TEST_COND_TYPE;
    break;
  
  case S_NSSAI_TEST_COND_TYPE:
    test_info.S_NSSAI = TRUE_TEST_COND_TYPE;
    break;
  
  default:
    assert(false && "Condition type out of the range");
    break;
  }

  test_info.test_cond = NULL;

  test_info.test_cond_value = NULL;



  return test_info;
}

static kpm_act_def_format_3_t fill_kpm_action_def_frm_3(void)
{
  kpm_act_def_format_3_t action_def_frm_3 = {0};

  // Measurement Information
  // [1, 65535]
  action_def_frm_3.meas_info_lst_len = 10; //(rand() % 65535) + 1;
                                            
  action_def_frm_3.meas_info_lst = calloc(action_def_frm_3.meas_info_lst_len, sizeof(meas_info_format_3_lst_t));
  assert(action_def_frm_3.meas_info_lst != NULL && "Memory exhausted" );
  
  for (size_t i = 0; i < action_def_frm_3.meas_info_lst_len; i++)
  {
    meas_info_format_3_lst_t* meas_info = &action_def_frm_3.meas_info_lst[i];

    // Measurement Type
    meas_info->meas_type.type = rand()%END_MEAS_TYPE;

    switch (meas_info->meas_type.type)
    {
    case NAME_MEAS_TYPE:
      meas_info->meas_type.type = NAME_MEAS_TYPE;
      meas_info->meas_type.name.buf = calloc(strlen("test") + 1, sizeof(char));
      memcpy(meas_info->meas_type.name.buf, "test", strlen("test"));
      meas_info->meas_type.name.len = strlen("test");
      break;
    
    case ID_MEAS_TYPE:
      meas_info->meas_type.type = ID_MEAS_TYPE;
      meas_info->meas_type.id = (rand() % 65536) + 1;
      break;

    default:
      assert(false && "Unknown Measurement Type");
    }
      
    // Matching Condition Format 3
    // [1, 32768]
    meas_info->matching_cond_lst_len = 10;  // (rand() % 32768) + 1;

    meas_info->matching_cond_lst = calloc(meas_info->matching_cond_lst_len, sizeof(*meas_info->matching_cond_lst));
    assert(meas_info->matching_cond_lst != NULL && "Memory exhausted" );
      
    for (size_t j = 0; j < meas_info->matching_cond_lst_len; j++)
    {      
      // Matching Condition Type
      meas_info->matching_cond_lst[j].cond_type = rand()%END_INFO;
      
      switch (meas_info->matching_cond_lst[j].cond_type)
      {
      case LABEL_INFO:
        meas_info->matching_cond_lst[j].label_info_lst.plmn_id = calloc(1, sizeof(e2sm_plmn_t));
        assert(meas_info->matching_cond_lst[j].label_info_lst.plmn_id != NULL && "Memory exhausted");
        *meas_info->matching_cond_lst[j].label_info_lst.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
        break;
      
      case TEST_INFO:
        meas_info->matching_cond_lst[j].test_info_lst = fill_kpm_test_info();
        break;
      
      default:
        assert(false && "Unknown Matching Condition Type");
      }
      
      // Logical OR
      meas_info->matching_cond_lst[j].logical_OR = calloc(1, sizeof(enum_value_e));
      assert(meas_info->matching_cond_lst[j].logical_OR != NULL && "Memory exhausted");
      *meas_info->matching_cond_lst[j].logical_OR = TRUE_ENUM_VALUE;

      
    }

    // Bin Range Definition
    // not yet implemented in ASN.1 - possible extension
    meas_info->bin_range_def = calloc(1, sizeof(bin_range_def_t));
    assert(meas_info->bin_range_def != NULL && "Memory exhausted");

    meas_info->bin_range_def->bin_x_lst_len = 3;  // (rand() % 65535) + 1;

    meas_info->bin_range_def->bin_x_lst = calloc(meas_info->bin_range_def->bin_x_lst_len, sizeof(bin_distr_t));
    assert(meas_info->bin_range_def->bin_x_lst != NULL && "Memory exhausted");

    for (size_t j = 0; j<meas_info->bin_range_def->bin_x_lst_len; j++)
    {
      meas_info->bin_range_def->bin_x_lst[j].bin_index = (rand() % 65535) + 0;
      meas_info->bin_range_def->bin_x_lst[j].start_value.value = rand()%END_BIN_RANGE;


      // start value
      switch (meas_info->bin_range_def->bin_x_lst[j].start_value.value)
      {
      case INTEGER_BIN_RANGE:
        meas_info->bin_range_def->bin_x_lst[j].start_value.int_value = (rand() % 4294967296) + 0;
        break;

      case REAL_BIN_RANGE:
        meas_info->bin_range_def->bin_x_lst[j].start_value.real_value = (rand() % 256) + 0;
    
      default:
        break;
      }

      // end value
      meas_info->bin_range_def->bin_x_lst[j].end_value.value = rand()%END_BIN_RANGE;

      switch (meas_info->bin_range_def->bin_x_lst[j].end_value.value)
      {
      case INTEGER_BIN_RANGE:
        meas_info->bin_range_def->bin_x_lst[j].end_value.int_value = (rand() % 4294967296) + 0;
        break;

      case REAL_BIN_RANGE:
        meas_info->bin_range_def->bin_x_lst[j].end_value.real_value = (rand() % 256) + 0;

      default:
        break;
      }
    }
    
    meas_info->bin_range_def->bin_y_lst_len = 0;
    meas_info->bin_range_def->bin_y_lst = NULL;

    meas_info->bin_range_def->bin_z_lst_len = 0;
    meas_info->bin_range_def->bin_z_lst = NULL;

  }

  // Granularity Period
  // [0, 4294967295]
  action_def_frm_3.gran_period_ms = (rand() % 4294967295) + 0;

  // Cell Global ID
  // Optional
  action_def_frm_3.cell_global_id = calloc(1, sizeof(*action_def_frm_3.cell_global_id));
  action_def_frm_3.cell_global_id->type = rand()%END_CGI_RAT_TYPE;
  
  switch (action_def_frm_3.cell_global_id->type)
  {
  case NR_CGI_RAT_TYPE:
    action_def_frm_3.cell_global_id->nr_cgi.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    action_def_frm_3.cell_global_id->nr_cgi.nr_cell_id = (rand() % 2^36) + 1;
    break;

  case EUTRA_CGI_RAT_TYPE:
    action_def_frm_3.cell_global_id->eutra.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    action_def_frm_3.cell_global_id->eutra.eutra_cell_id = (rand() % 2^28) + 0;
    break;
  
  default:
    assert(false && "Unknown Cell Global ID Type");
  }


  return action_def_frm_3;
}

static kpm_act_def_format_4_t fill_kpm_action_def_frm_4(void)
{
  kpm_act_def_format_4_t action_def_frm_4 = {0};

  // Matching Condition
  action_def_frm_4.matching_cond_lst_len = 1;  // (rand() % 32768) + 1;

  action_def_frm_4.matching_cond_lst = calloc(action_def_frm_4.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t));
  assert(action_def_frm_4.matching_cond_lst != NULL && "Memory exhausted" );

  for (size_t i = 0; i<action_def_frm_4.matching_cond_lst_len; i++)
  {
    action_def_frm_4.matching_cond_lst[i].logical_OR = NULL;

    action_def_frm_4.matching_cond_lst[i].test_info_lst = fill_kpm_test_info();
  }

  action_def_frm_4.action_def_format_1 = fill_kpm_action_def_frm_1();

  return action_def_frm_4;
}

static kpm_act_def_format_5_t fill_kpm_action_def_frm_5(void)
{
  kpm_act_def_format_5_t action_def_frm_5 = {0};

  // UE ID List
  action_def_frm_5.ue_id_lst_len = 2;  // (rand() % 65535) + 2;

  action_def_frm_5.ue_id_lst = calloc(action_def_frm_5.ue_id_lst_len, sizeof(ue_id_e2sm_t));
  assert(action_def_frm_5.ue_id_lst != NULL && "Memory exhausted");

  for (size_t i = 0; i<action_def_frm_5.ue_id_lst_len; i++)
  {
    action_def_frm_5.ue_id_lst[i] = fill_ue_id_data();
  }

  
  // Action Definition Format 1
  action_def_frm_5.action_def_format_1 = fill_kpm_action_def_frm_1();


  return action_def_frm_5;
}

kpm_act_def_t fill_kpm_action_def(void)
{
  kpm_act_def_t action_def = {0};

  action_def.type = rand()%END_ACTION_DEFINITION;

  switch (action_def.type)
  {
  case FORMAT_1_ACTION_DEFINITION:
    action_def.frm_1 = fill_kpm_action_def_frm_1();
    break;

  case FORMAT_2_ACTION_DEFINITION:
    action_def.frm_2 = fill_kpm_action_def_frm_2();
    break;

  case FORMAT_3_ACTION_DEFINITION:
    action_def.frm_3 = fill_kpm_action_def_frm_3();
    break;
  
  /* Possible extensions: */
  case FORMAT_4_ACTION_DEFINITION:
    action_def.frm_4 = fill_kpm_action_def_frm_4();
    break;

  case FORMAT_5_ACTION_DEFINITION:
    action_def.frm_5 = fill_kpm_action_def_frm_5();
    break;

  default:
    assert(false && "Unknown KPM Action Definition Format Type");
  }

  return action_def;
}

static kpm_ric_ind_hdr_format_1_t fill_kpm_ind_hdr_frm_1(void)
{
  kpm_ric_ind_hdr_format_1_t hdr_frm_1 = {0};

  int64_t t = time_now_us();
  hdr_frm_1.collectStartTime = t;
  
  hdr_frm_1.fileformat_version = NULL;
  
  hdr_frm_1.sender_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_name->buf = calloc(strlen("My OAI-CU") + 1, sizeof(char));
  memcpy(hdr_frm_1.sender_name->buf, "My OAI-CU", strlen("My OAI-CU"));
  hdr_frm_1.sender_name->len = strlen("My OAI-CU");
  
  hdr_frm_1.sender_type = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.sender_type->buf = calloc(strlen("CU") + 1, sizeof(char));
  memcpy(hdr_frm_1.sender_type->buf, "CU", strlen("CU"));
  hdr_frm_1.sender_type->len = strlen("CU");
  
  hdr_frm_1.vendor_name = calloc(1, sizeof(byte_array_t));
  hdr_frm_1.vendor_name->buf = calloc(strlen("OAI") + 1, sizeof(char));
  memcpy(hdr_frm_1.vendor_name->buf, "OAI", strlen("OAI"));
  hdr_frm_1.vendor_name->len = strlen("OAI");

  return hdr_frm_1;
}

kpm_ind_hdr_t fill_kpm_ind_hdr(void)
{
  kpm_ind_hdr_t hdr = {0};

  hdr.type = rand()%END_INDICATION_HEADER;

  switch (hdr.type)
  {
  case FORMAT_1_INDICATION_HEADER:
    hdr.kpm_ric_ind_hdr_format_1 = fill_kpm_ind_hdr_frm_1();
    break;
  
  default:
    assert(false && "Unknown KPM Indication Header Type");
  }


  return hdr;
}

static kpm_ind_msg_format_1_t fill_kpm_ind_msg_frm_1(void)
{
  kpm_ind_msg_format_1_t msg_frm_1 = {0};
  
  // Measurement Data
  msg_frm_1.meas_data_lst_len = 3;  // (rand() % 65535) + 1;
  msg_frm_1.meas_data_lst = calloc(msg_frm_1.meas_data_lst_len, sizeof(*msg_frm_1.meas_data_lst));
  assert(msg_frm_1.meas_data_lst != NULL && "Memory exhausted" );
  
  for (size_t i = 0; i < msg_frm_1.meas_data_lst_len; i++)
  {
      // Incomplete Flag
      msg_frm_1.meas_data_lst[i].incomplete_flag = calloc(1, sizeof(enum_value_e));
      assert(msg_frm_1.meas_data_lst[i].incomplete_flag != NULL && "Memory exhausted");
      *msg_frm_1.meas_data_lst[i].incomplete_flag = TRUE_ENUM_VALUE;
      
      // Measurement Record
      msg_frm_1.meas_data_lst[i].meas_record_len = 4;  // (rand() % 65535) + 1;
      msg_frm_1.meas_data_lst[i].meas_record_lst = calloc(msg_frm_1.meas_data_lst[i].meas_record_len, sizeof(meas_record_lst_t));
      assert(msg_frm_1.meas_data_lst[i].meas_record_lst != NULL && "Memory exhausted" );
      
      for (size_t j = 0; j < msg_frm_1.meas_data_lst[i].meas_record_len; j++)
      {
        msg_frm_1.meas_data_lst[i].meas_record_lst[j].value = rand()%END_MEAS_VALUE;

        switch (msg_frm_1.meas_data_lst[i].meas_record_lst[j].value)
        {
        case INTEGER_MEAS_VALUE:
          msg_frm_1.meas_data_lst[i].meas_record_lst[j].int_val = (rand() % 4294967295) + 0;
          break;

        case REAL_MEAS_VALUE:
          msg_frm_1.meas_data_lst[i].meas_record_lst[j].real_val = (rand() % 256) + 0;
          break;

        case NO_VALUE_MEAS_VALUE:
          msg_frm_1.meas_data_lst[i].meas_record_lst[j].no_value = NULL;
          break;
        
        default:
          assert(false && "Unknown Measurement Record Type");
        }
          
      }
  }
  
  // Granularity Period - OPTIONAL: (1..4294967295)
  msg_frm_1.gran_period_ms = calloc(1, sizeof(*msg_frm_1.gran_period_ms));
  assert(msg_frm_1.gran_period_ms != NULL && "Memory exhausted");
  *msg_frm_1.gran_period_ms = (rand() % 4294967295) + 1;
  
  // Measurement Information - OPTIONAL
  msg_frm_1.meas_info_lst_len = 2;
  msg_frm_1.meas_info_lst = calloc(msg_frm_1.meas_info_lst_len, sizeof(meas_info_format_1_lst_t));
  assert(msg_frm_1.meas_info_lst != NULL && "Memory exhausted" );
  
  for (size_t i = 0; i < msg_frm_1.meas_info_lst_len; i++)
  {
      // Measurement Type
      msg_frm_1.meas_info_lst[i].meas_type.type = rand()%END_MEAS_TYPE;

      switch (msg_frm_1.meas_info_lst[i].meas_type.type)
      {
      case NAME_MEAS_TYPE:
        msg_frm_1.meas_info_lst[i].meas_type.type = NAME_MEAS_TYPE;
        char s[100];
        snprintf(s, 100, "RNTI %04x PrbDlUsage", (unsigned) (1111*i + 1111));
        msg_frm_1.meas_info_lst[i].meas_type.name.buf = calloc(strlen(s) + 1, sizeof(char));
        memcpy(msg_frm_1.meas_info_lst[i].meas_type.name.buf, s, strlen(s));
        msg_frm_1.meas_info_lst[i].meas_type.name.len = strlen(s);
        break;
    
      case ID_MEAS_TYPE:
        msg_frm_1.meas_info_lst[i].meas_type.type = ID_MEAS_TYPE;
        msg_frm_1.meas_info_lst[i].meas_type.id = (rand() % 65536) + 1;
        break;

      default:
        assert(false && "Unknown Measurement Type");
      }

      
      
      // Label Information
      msg_frm_1.meas_info_lst[i].label_info_lst_len = 1;
      msg_frm_1.meas_info_lst[i].label_info_lst = calloc(msg_frm_1.meas_info_lst[i].label_info_lst_len, sizeof(label_info_lst_t));
      assert(msg_frm_1.meas_info_lst[i].label_info_lst != NULL && "Memory exhausted" );
      
      for (size_t j = 0; j < msg_frm_1.meas_info_lst[i].label_info_lst_len; j++)
      {
          // msg_frm_1.meas_info_lst[i].label_info_lst[j].plmn_id = malloc(sizeof(plmn_t));
          // *msg_frm_1.meas_info_lst[i].label_info_lst[j].plmn_id = (plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
          msg_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = malloc(sizeof(enum_value_e));
          *msg_frm_1.meas_info_lst[i].label_info_lst[j].noLabel = TRUE_ENUM_VALUE;
      }
  }


  return msg_frm_1;
}

static kpm_ind_msg_format_2_t fill_kpm_ind_msg_frm_2(void)
{
  kpm_ind_msg_format_2_t msg_frm_2 = {0};

  // Measurement Data
  msg_frm_2.meas_data_lst_len = 2;  // (rand() % 65535) + 1;
  msg_frm_2.meas_data_lst = calloc(msg_frm_2.meas_data_lst_len, sizeof(*msg_frm_2.meas_data_lst));
  assert(msg_frm_2.meas_data_lst != NULL && "Memory exhausted" );
  
  for (size_t i = 0; i < msg_frm_2.meas_data_lst_len; i++)
  {
      // Incomplete Flag
      msg_frm_2.meas_data_lst[i].incomplete_flag = calloc(1, sizeof(enum_value_e));
      assert(msg_frm_2.meas_data_lst[i].incomplete_flag != NULL && "Memory exhausted");
      *msg_frm_2.meas_data_lst[i].incomplete_flag = TRUE_ENUM_VALUE;
      
      // Measurement Record
      msg_frm_2.meas_data_lst[i].meas_record_len = 1;  // (rand() % 65535) + 1;
      msg_frm_2.meas_data_lst[i].meas_record_lst = calloc(msg_frm_2.meas_data_lst[i].meas_record_len, sizeof(meas_record_lst_t));
      assert(msg_frm_2.meas_data_lst[i].meas_record_lst != NULL && "Memory exhausted" );
      
      for (size_t j = 0; j < msg_frm_2.meas_data_lst[i].meas_record_len; j++)
      {
        msg_frm_2.meas_data_lst[i].meas_record_lst[j].value = rand()%END_MEAS_VALUE;

        switch (msg_frm_2.meas_data_lst[i].meas_record_lst[j].value)
        {
        case INTEGER_MEAS_VALUE:
          msg_frm_2.meas_data_lst[i].meas_record_lst[j].int_val = (rand() % 4294967295) + 0;
          break;

        case REAL_MEAS_VALUE:
          msg_frm_2.meas_data_lst[i].meas_record_lst[j].real_val = (rand() % 256) + 0;
          break;

        case NO_VALUE_MEAS_VALUE:
          msg_frm_2.meas_data_lst[i].meas_record_lst[j].no_value = NULL;
          break;
        
        default:
          assert(false && "Unknown Measurement Record Type");
        }
      }
  }

  // Granularity Period - OPTIONAL
  msg_frm_2.gran_period_ms = calloc(1, sizeof(*msg_frm_2.gran_period_ms));
  assert(msg_frm_2.gran_period_ms != NULL && "Memory exhausted");
  *msg_frm_2.gran_period_ms = (rand() % 4294967295) + 1;

  // Measurement Information Condition UE
  // [1, 65535]
  msg_frm_2.meas_info_cond_ue_lst_len = 1;  // (rand() % 65535) + 1;
  msg_frm_2.meas_info_cond_ue_lst = calloc(msg_frm_2.meas_info_cond_ue_lst_len, sizeof(meas_info_cond_ue_lst_t));
  assert(msg_frm_2.meas_info_cond_ue_lst != NULL && "Memory exhausted");

  for (size_t i = 0; i<msg_frm_2.meas_info_cond_ue_lst_len; i++)
  {
    meas_info_cond_ue_lst_t * cond_ue = &msg_frm_2.meas_info_cond_ue_lst[i];

    // Measurement Type
    cond_ue->meas_type.type = rand()%END_MEAS_TYPE;

    switch (cond_ue->meas_type.type)
    {
    case NAME_MEAS_TYPE:
      cond_ue->meas_type.type = NAME_MEAS_TYPE;
      cond_ue->meas_type.name.buf = calloc(strlen("condition UE measurement") + 1, sizeof(char));
      memcpy(cond_ue->meas_type.name.buf, "condition UE measurement", strlen("condition UE measurement"));
      cond_ue->meas_type.name.len = strlen("condition UE measurement");
      break;
    
    case ID_MEAS_TYPE:
      cond_ue->meas_type.type = ID_MEAS_TYPE;
      cond_ue->meas_type.id = (rand() % 65536) + 1;
      break;

    default:
      assert(false && "Unknown Measurement Type");
    }

    
    // Matching Condition Format 3
    // [1, 32768]
    cond_ue->matching_cond_lst_len = 1; // (rand() % 32768) + 1;
    cond_ue->matching_cond_lst = calloc(cond_ue->matching_cond_lst_len, sizeof(*cond_ue->matching_cond_lst));
    assert(cond_ue->matching_cond_lst != NULL && "Memory exhausted" );
      
    for (size_t j = 0; j < cond_ue->matching_cond_lst_len; j++)
    {      
      // Matching Condition Type
      cond_ue->matching_cond_lst[j].cond_type = rand()%END_INFO;
      
      switch (cond_ue->matching_cond_lst[j].cond_type)
      {
      case LABEL_INFO:
        cond_ue->matching_cond_lst[j].label_info_lst.plmn_id = calloc(1, sizeof(e2sm_plmn_t));
        assert(cond_ue->matching_cond_lst[j].label_info_lst.plmn_id != NULL && "Memory exhausted");
        *cond_ue->matching_cond_lst[j].label_info_lst.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
        break;
      
      case TEST_INFO:
        cond_ue->matching_cond_lst[j].test_info_lst = fill_kpm_test_info();
        break;
      
      default:
        assert(false && "Unknown Matching Condition Type");
      }
      
      // Logical OR
      cond_ue->matching_cond_lst[j].logical_OR = calloc(1, sizeof(enum_value_e));
      assert(cond_ue->matching_cond_lst[j].logical_OR != NULL && "Memory exhausted");
      *cond_ue->matching_cond_lst[j].logical_OR = TRUE_ENUM_VALUE;
    }

    // List of matched UE IDs
    // Optional [0, 65535]
    cond_ue->ue_id_matched_lst_len = 1;  // (rand() % 65535) + 0;
    cond_ue->ue_id_matched_lst = calloc(cond_ue->ue_id_matched_lst_len, sizeof(ue_id_e2sm_t));
    assert(cond_ue->ue_id_matched_lst != NULL && "Memory exhausted");

    for (size_t j = 0; j<cond_ue->ue_id_matched_lst_len; j++)
    {
      cond_ue->ue_id_matched_lst[j] = fill_ue_id_data();
    }

    // Sequence of Matched UE IDs for Granularity Periods
    // not yet implemented in ASN.1 - possible extension
    cond_ue->ue_id_gran_period_lst_len = 3;  // (rand() % 65535) + 0;
    cond_ue->ue_id_gran_period_lst = calloc(cond_ue->ue_id_gran_period_lst_len, sizeof(ue_id_gran_period_lst_t));
    assert(cond_ue->ue_id_gran_period_lst != NULL && "Memory exhausted");

    for (size_t j = 0; j<cond_ue->ue_id_gran_period_lst_len; j++)
    {
      cond_ue->ue_id_gran_period_lst[j].num_matched_ue = rand()%END_MATCHED_UE;

      switch (cond_ue->ue_id_gran_period_lst[j].num_matched_ue)
      {
      case NONE_MATCHED_UE:
      {
        cond_ue->ue_id_gran_period_lst[j].no_matched_ue = TRUE_ENUM_VALUE;
        break;
      }

      case ONE_OR_MORE_MATCHED_UE:
      {
        cond_ue->ue_id_gran_period_lst[j].matched_ue_lst.ue_lst_len = 5;  // (rand() % 65535) + 1;
        cond_ue->ue_id_gran_period_lst[j].matched_ue_lst.ue_lst = calloc(cond_ue->ue_id_gran_period_lst[j].matched_ue_lst.ue_lst_len, sizeof(ue_id_e2sm_t));
        assert(cond_ue->ue_id_gran_period_lst[j].matched_ue_lst.ue_lst != NULL && "Memory exhausted");

        for (size_t z = 0; z<cond_ue->ue_id_gran_period_lst[j].matched_ue_lst.ue_lst_len; z++)
        {
          cond_ue->ue_id_gran_period_lst[j].matched_ue_lst.ue_lst[z] = fill_ue_id_data();
        }
        break;
      }
      
      default:
        break;
      }
    }

  }

  return msg_frm_2;
}

static 
kpm_ind_msg_format_3_t fill_kpm_ind_msg_frm_3(void)
{
  kpm_ind_msg_format_3_t msg_frm_3 = {0};

  msg_frm_3.ue_meas_report_lst_len = 3;  // (rand() % 65535) + 1;

  msg_frm_3.meas_report_per_ue = calloc(msg_frm_3.ue_meas_report_lst_len, sizeof(meas_report_per_ue_t));
  assert(msg_frm_3.meas_report_per_ue != NULL && "Memory exhausted");

  for (size_t i = 0; i<msg_frm_3.ue_meas_report_lst_len; i++)
  {
    msg_frm_3.meas_report_per_ue[i].ue_meas_report_lst = fill_ue_id_data();
    msg_frm_3.meas_report_per_ue[i].ind_msg_format_1 = fill_kpm_ind_msg_frm_1();
  }

  return msg_frm_3;
}



kpm_ind_msg_t fill_kpm_ind_msg(void)
{
  kpm_ind_msg_t msg = {0};

  msg.type = FORMAT_3_INDICATION_MESSAGE; // rand()%END_INDICATION_MESSAGE;
  
  switch (msg.type)
  {
  case FORMAT_1_INDICATION_MESSAGE:
    msg.frm_1 = fill_kpm_ind_msg_frm_1();
    break;

  case FORMAT_2_INDICATION_MESSAGE:
    msg.frm_2 = fill_kpm_ind_msg_frm_2();
    break;

  /* Possible extensions: */
  case FORMAT_3_INDICATION_MESSAGE:
    msg.frm_3 = fill_kpm_ind_msg_frm_3();
    break;
  
  default:
    assert(false && "Unknown KPM Indication Message Format Type");
  }
  return msg;
}

kpm_ran_function_def_t fill_kpm_ran_func_def(void)
{
  kpm_ran_function_def_t ran_function = {0};

  // RAN Function Name
  ran_function.name.description.buf = calloc(strlen(SM_KPM_DESCRIPTION) + 1, sizeof(char));
  memcpy(ran_function.name.description.buf, SM_KPM_DESCRIPTION, strlen(SM_KPM_DESCRIPTION));
  ran_function.name.description.len = strlen(SM_KPM_DESCRIPTION);

  ran_function.name.name.buf = calloc(strlen(SM_KPM_STR) + 1, sizeof(char));
  memcpy(ran_function.name.name.buf, SM_KPM_STR, strlen(SM_KPM_STR));
  ran_function.name.name.len = strlen(SM_KPM_STR);

  ran_function.name.oid.buf = calloc(strlen(SM_KPM_OID) + 1, sizeof(char));
  memcpy(ran_function.name.oid.buf, SM_KPM_OID, strlen(SM_KPM_OID));
  ran_function.name.oid.len = strlen(SM_KPM_OID);

  ran_function.name.instance = NULL;

  ran_function.sz_ric_event_trigger_style_list = 0;
  ran_function.ric_event_trigger_style_list = NULL;

  ran_function.sz_ric_report_style_list = 0;
  ran_function.ric_report_style_list = NULL;


  // // RIC Event Trigger Style List
   ran_function.sz_ric_event_trigger_style_list = 3;  // (rand() % 63) + 0;
   ran_function.ric_event_trigger_style_list = calloc(ran_function.sz_ric_event_trigger_style_list, sizeof(ric_event_trigger_style_item_t));
   assert(ran_function.ric_event_trigger_style_list != NULL && "Memory exhausted");

   for (size_t i = 0; i<ran_function.sz_ric_event_trigger_style_list; i++)
   {
     // RIC Event Trigger Style
     ran_function.ric_event_trigger_style_list[i].style_type = rand()%END_STYLE_RIC_EVENT_TRIGGER;

     switch (ran_function.ric_event_trigger_style_list[i].style_type)
     {
     case STYLE_1_RIC_EVENT_TRIGGER:
     {
       // RIC Event Trigger Style Name
       ran_function.ric_event_trigger_style_list[i].style_name.buf = calloc(strlen("RIC-Event-Trigger-Style-Type-1") + 1, sizeof(char));
       memcpy(ran_function.ric_event_trigger_style_list[i].style_name.buf, "RIC-Event-Trigger-Style-Type-1", strlen("RIC-Event-Trigger-Style-Type-1"));
       ran_function.ric_event_trigger_style_list[i].style_name.len = strlen("RIC-Event-Trigger-Style-Type-1");

       // RIC Event Trigger Format
       ran_function.ric_event_trigger_style_list[i].format_type = FORMAT_1_RIC_EVENT_TRIGGER;

       break;
     }
    
     default:
       assert(false && "Unknown RIC Event Trigger Style Type");
     }
   }

   // RIC Report Style List
   ran_function.sz_ric_report_style_list = 3;  // (rand() % 63) + 0;
   ran_function.ric_report_style_list = calloc(ran_function.sz_ric_report_style_list, sizeof(ric_report_style_item_t));
   assert(ran_function.ric_report_style_list != NULL && "Memory exhausted");

   for (size_t i = 0; i<ran_function.sz_ric_report_style_list; i++)
   {

     // RIC Report Styles
     ran_function.ric_report_style_list[i].report_style_type = rand()%END_RIC_SERVICE_REPORT;

     switch (ran_function.ric_report_style_list[i].report_style_type)
     {
     case STYLE_1_RIC_SERVICE_REPORT:
     {
       ran_function.ric_report_style_list[i].act_def_format_type = FORMAT_1_ACTION_DEFINITION;
       ran_function.ric_report_style_list[i].ind_hdr_format_type = FORMAT_1_INDICATION_HEADER;
       ran_function.ric_report_style_list[i].ind_msg_format_type = FORMAT_1_INDICATION_MESSAGE;

       // RIC REPORT Style Name
       ran_function.ric_report_style_list[i].report_style_name.buf = calloc(strlen("RIC-Report-Style-Type-1") + 1, sizeof(char));
       memcpy(ran_function.ric_report_style_list[i].report_style_name.buf, "RIC-Report-Style-Type-1", strlen("RIC-Report-Style-Type-1"));
       ran_function.ric_report_style_list[i].report_style_name.len = strlen("RIC-Report-Style-Type-1");
       break;
     }

     case STYLE_2_RIC_SERVICE_REPORT:
     {
       ran_function.ric_report_style_list[i].act_def_format_type = FORMAT_2_ACTION_DEFINITION;
       ran_function.ric_report_style_list[i].ind_hdr_format_type = FORMAT_1_INDICATION_HEADER;
       ran_function.ric_report_style_list[i].ind_msg_format_type = FORMAT_1_INDICATION_MESSAGE;

       // RIC REPORT Style Name
       ran_function.ric_report_style_list[i].report_style_name.buf = calloc(strlen("RIC-Report-Style-Type-2") + 1, sizeof(char));
       memcpy(ran_function.ric_report_style_list[i].report_style_name.buf, "RIC-Report-Style-Type-2", strlen("RIC-Report-Style-Type-2"));
       ran_function.ric_report_style_list[i].report_style_name.len = strlen("RIC-Report-Style-Type-2");
       break;
     }

     case STYLE_3_RIC_SERVICE_REPORT:
     {
       ran_function.ric_report_style_list[i].act_def_format_type = FORMAT_3_ACTION_DEFINITION;
       ran_function.ric_report_style_list[i].ind_hdr_format_type = FORMAT_1_INDICATION_HEADER;
       ran_function.ric_report_style_list[i].ind_msg_format_type = FORMAT_2_INDICATION_MESSAGE;

  //     // RIC REPORT Style Name
       ran_function.ric_report_style_list[i].report_style_name.buf = calloc(strlen("RIC-Report-Style-Type-3") + 1, sizeof(char));
       memcpy(ran_function.ric_report_style_list[i].report_style_name.buf, "RIC-Report-Style-Type-3", strlen("RIC-Report-Style-Type-3"));
       ran_function.ric_report_style_list[i].report_style_name.len = strlen("RIC-Report-Style-Type-3");
       break;
     }

     case STYLE_4_RIC_SERVICE_REPORT:
     {
       ran_function.ric_report_style_list[i].act_def_format_type = FORMAT_4_ACTION_DEFINITION;
       ran_function.ric_report_style_list[i].ind_hdr_format_type = FORMAT_1_INDICATION_HEADER;
       ran_function.ric_report_style_list[i].ind_msg_format_type = FORMAT_3_INDICATION_MESSAGE;

       // RIC REPORT Style Name
       ran_function.ric_report_style_list[i].report_style_name.buf = calloc(strlen("RIC-Report-Style-Type-4") + 1, sizeof(char));
       memcpy(ran_function.ric_report_style_list[i].report_style_name.buf, "RIC-Report-Style-Type-4", strlen("RIC-Report-Style-Type-4"));
       ran_function.ric_report_style_list[i].report_style_name.len = strlen("RIC-Report-Style-Type-4");
       break;
     }

     case STYLE_5_RIC_SERVICE_REPORT:
     {
       ran_function.ric_report_style_list[i].act_def_format_type = FORMAT_5_ACTION_DEFINITION;
       ran_function.ric_report_style_list[i].ind_hdr_format_type = FORMAT_1_INDICATION_HEADER;
       ran_function.ric_report_style_list[i].ind_msg_format_type = FORMAT_3_INDICATION_MESSAGE;

       // RIC REPORT Style Name
       ran_function.ric_report_style_list[i].report_style_name.buf = calloc(strlen("RIC-Report-Style-Type-5") + 1, sizeof(char));
       memcpy(ran_function.ric_report_style_list[i].report_style_name.buf, "RIC-Report-Style-Type-5", strlen("RIC-Report-Style-Type-5"));
       ran_function.ric_report_style_list[i].report_style_name.len = strlen("RIC-Report-Style-Type-5");
       break;
     }
    
     default:
       assert(false && "Unknown RIC REPORT Style Type");
     }


     // Measurement Information for Action
     ran_function.ric_report_style_list[i].meas_info_for_action_lst_len = 3;  // (rand() % 65535) + 0;
     ran_function.ric_report_style_list[i].meas_info_for_action_lst = calloc(ran_function.ric_report_style_list[i].meas_info_for_action_lst_len, sizeof(meas_info_for_action_lst_t));
     assert(ran_function.ric_report_style_list[i].meas_info_for_action_lst != NULL && "Memory exhausted");

     for (size_t j = 0; j<ran_function.ric_report_style_list[i].meas_info_for_action_lst_len; j++)
     {
       // Measurement Type Name
       ran_function.ric_report_style_list[i].meas_info_for_action_lst[j].name.buf = calloc(strlen("Name_for_action") + 1, sizeof(char));
       memcpy(ran_function.ric_report_style_list[i].meas_info_for_action_lst[j].name.buf, "Name_for_action", strlen("Name_for_action"));
       ran_function.ric_report_style_list[i].meas_info_for_action_lst[j].name.len = strlen("Name_for_action");

       // Measurement Type ID. Optional
       //ran_function.ric_report_style_list[i].meas_info_for_action_lst[j].id = calloc(1, sizeof(uint16_t));
       //assert(ran_function.ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL && "Memory exhausted");
       //*ran_function.ric_report_style_list[i].meas_info_for_action_lst[j].id = (rand() % 65536) + 0;

       // Bin Range Definition
       // not yet implemented in ASN.1
       ran_function.ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def = NULL;
     }
   }

  return ran_function;
}


