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



#include "stdout.h"
#include "../../sm/mac_sm/ie/mac_data_ie.h"    // for mac_ind_msg_t
#include "../../sm/pdcp_sm/ie/pdcp_data_ie.h"  // for pdcp_ind_msg_t
#include "../../sm/rlc_sm/ie/rlc_data_ie.h"    // for rlc_ind_msg_t
#include "string_parser.h"                               // for to_string_ma..

#include "../../util/time_now_us.h"

#include <assert.h>                                      // for assert
#include <stdint.h>                                      // for uint32_t
#include <stdio.h>                                       // for NULL, fputs
#include <stdlib.h>                                      // for atexit
#include <string.h>                                      // for memset
#include <time.h>
#include <pthread.h>                                     // for pthread_once

static
FILE *fp = NULL;

const char* file_path = "log.txt";


static 
pthread_once_t init_fp_once = PTHREAD_ONCE_INIT;


static
void close_fp(void)
{
  assert(fp != NULL);

  int const rc = fclose(fp);
  assert(rc == 0);
}

/*
static
void init_fp(FILE** fp, const char* path)
{
  assert(*fp == NULL);
  assert(path != NULL);

//  const char* append = "a"; 
  const char* write = "w"; 

  *fp = fopen(path, write);
  assert(*fp != NULL);

  atexit(close_fp);
}
*/

static
void init_fp(void)
{
  assert(fp == NULL);
  assert(file_path != NULL);

//  const char* append = "a"; 
  const char* write = "w"; 

  fp = fopen(file_path, write);
  assert(fp != NULL);

  atexit(close_fp);
}


static
void print_mac_stats(mac_ind_msg_t const* msg )
{
  assert(msg != NULL);

  pthread_once(&init_fp_once, init_fp);
  assert(fp != NULL);

  int64_t now = time_now_us();
  printf("Time diff at iApp = %ld \n", now - msg->tstamp);

  for(uint32_t i = 0; i < msg->len_ue_stats; ++i){
    char stats[1024] = {0};
    to_string_mac_ue_stats(&msg->ue_stats[i], msg->tstamp, stats, 1024);
    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }
}


static
void print_rlc_stats(rlc_ind_msg_t const* rlc)
{
  assert(rlc != NULL);
  pthread_once(&init_fp_once, init_fp);
  assert(fp != NULL);

  for(uint32_t i = 0; i < rlc->len; ++i){
    char stats[1024] = {0};
    to_string_rlc_rb(&rlc->rb[i], rlc->tstamp , stats , 1024);

    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }
}

static
void print_pdcp_stats(pdcp_ind_msg_t const* pdcp)
{
  assert(pdcp != NULL);
  pthread_once(&init_fp_once, init_fp);
  assert(fp != NULL);

  for(uint32_t i = 0; i < pdcp->len; ++i){

    char stats[512] = {0};
    pdcp_radio_bearer_stats_t* rb = &pdcp->rb[i];

    to_string_pdcp_rb(rb, pdcp->tstamp, stats, 512);
    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }
}

static
void print_slice_stats(slice_ind_msg_t const* slice)
{
  assert(slice != NULL);
  pthread_once(&init_fp_once, init_fp);
  assert(fp != NULL);

  char stats[2048] = {0};
  to_string_slice(slice, slice->tstamp, stats, 2048);

  int const rc = fputs(stats , fp);
  // Edit: The C99 standard §7.19.1.3 states:
  // The macros are [...]
  // EOF which expands to an integer constant expression, 
  // with type int and a negative value, that is returned by 
  // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
  assert(rc > -1);

}

static
void print_gtp_stats(gtp_ind_msg_t const* gtp)
{
  assert(gtp != NULL);
  pthread_once(&init_fp_once, init_fp);
  assert(fp != NULL);

  for(uint32_t i = 0; i < gtp->len; ++i){
    char stats[1024] = {0};
    to_string_gtp_ngu(&gtp->ngut[i], gtp->tstamp , stats , 1024);

    int const rc = fputs(stats , fp);
    // Edit: The C99 standard §7.19.1.3 states:
    // The macros are [...]
    // EOF which expands to an integer constant expression, 
    // with type int and a negative value, that is returned by 
    // several functions to indicate end-of-ﬁle, that is, no more input from a stream;
    assert(rc > -1);
  }

}



void print_kpm_ind_msg_frm_1(const kpm_ind_msg_format_1_t message, uint32_t truncated_ts)
{

  char stats[1024] = {0};
  int max = 1024;

  for(size_t i = 0; i < message.meas_data_lst_len; i++){
    meas_data_lst_t * curMeasData = &message.meas_data_lst[i];


    if (i == 0 && message.gran_period_ms){
      int rc = snprintf(stats, max,  "kpm_stats: "
                      "tstamp=%u"
                      ",granulPeriod=%u"
                      ",MeasData_len=%zu"
                      , truncated_ts
                      , *(message.gran_period_ms)
                      , message.meas_data_lst_len
                      );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }else if(i == 0 && message.gran_period_ms == NULL){
      int rc = snprintf(stats, max,  "kpm_stats: "
                      "tstamp=%u"
                      ",granulPeriod=NULL"
                      ",MeasData_len=%zu"
                      , truncated_ts
                      , message.meas_data_lst_len
                      );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }

    memset(stats, 0, sizeof(stats));
    int rc = snprintf(stats, max,",MeasData[%zu]=(incompleteFlag=%p, Record_len=%zu ",
                                  i, (void *)curMeasData->incomplete_flag, curMeasData->meas_record_len);
    assert(rc < (int)max && "Not enough space in the char array to write all the data");
    rc = fputs(stats , fp);
    assert(rc > -1);

    for(size_t j = 0; j < curMeasData->meas_record_len; j++){
      meas_record_lst_t* curMeasRecord = &curMeasData->meas_record_lst[j];
      memset(stats, 0, sizeof(stats));
      to_string_kpm_measRecord(curMeasRecord, j, stats, max);
      int rc = fputs(stats , fp);
      assert(rc > -1);
    }
  }
  int rc = snprintf(stats, max, ",MeasInfo_len=%zu", message.meas_info_lst_len);
  assert(rc < (int)max && "Not enough space in the char array to write all the data");
  rc = fputs(stats , fp);
  assert(rc > -1);

  for(size_t i = 0; i < message.meas_info_lst_len; i++){
    meas_info_format_1_lst_t* curMeasInfo = &message.meas_info_lst[i];

    if (curMeasInfo->meas_type.type == NAME_MEAS_TYPE){
      memset(stats, 0, sizeof(stats));
      int rc = snprintf(stats, max, ",MeasInfo[%zu]=(type=ID, content=%hhn)", i, curMeasInfo->meas_type.name.buf);
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    } else if (curMeasInfo->meas_type.type == ID_MEAS_TYPE){
      memset(stats, 0, sizeof(stats));
      int rc = snprintf(stats, max, ",MeasInfo[%zu]=(type=NAME, content=%u)", i, curMeasInfo->meas_type.id);
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }

    for(size_t j = 0; j < curMeasInfo->label_info_lst_len; ++j){
      label_info_lst_t* curLabelInfo = &curMeasInfo->label_info_lst[j];
      memset(stats, 0, sizeof(stats));
      to_string_kpm_labelInfo(curLabelInfo, j, stats, max);
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = fputs(stats , fp);
      assert(rc > -1);
    }
  }
  fputs("\n", fp);
}

void print_kpm_stats(kpm_ind_data_t const* kpm)
{
  assert(kpm != NULL);
  pthread_once(&init_fp_once, init_fp);
  assert(fp != NULL);

  

  // From Indication Header : Collect Start Time
  // To be defined better, switch/case for specific format
  uint64_t truncated_ts = (uint64_t)kpm->hdr.kpm_ric_ind_hdr_format_1.collectStartTime;



  switch (kpm->msg.type)
  {
  case FORMAT_1_INDICATION_MESSAGE: 
    print_kpm_ind_msg_frm_1(kpm->msg.frm_1, truncated_ts);
    break;

  // case FORMAT_2_INDICATION_MESSAGE:
  //   kpm_ind_msg_format_2_t * message = calloc(1, sizeof(kpm_ind_msg_format_2_t));
  //   message = &kpm->kpm_ind_msg.frm_2;
  //   print_kpm_ind_msg_frm_2(&message);
  //   break;

  // case FORMAT_3_INDICATION_MESSAGE:
  //   kpm_ind_msg_format_3_t * message = calloc(1, sizeof(kpm_ind_msg_format_3_t));
  //   message = &kpm->kpm_ind_msg.frm_3;
  //   print_kpm_ind_msg_frm_3(&message);
  //   break;
  
  default:
    assert(false && "Unknown Indication Message Type");
  }

}

void notify_stdout_listener(sm_ag_if_rd_ind_t const* data)
{
  assert(data != NULL);
  if(data->type == MAC_STATS_V0)  
    print_mac_stats(&data->mac.msg);
  else if (data->type == RLC_STATS_V0)
    print_rlc_stats(&data->rlc.msg);
  else if (data->type == PDCP_STATS_V0)
    print_pdcp_stats(&data->pdcp.msg);
  else if (data->type == SLICE_STATS_V0)
    print_slice_stats(&data->slice.msg);
  else if (data->type == GTP_STATS_V0)
    print_gtp_stats(&data->gtp.msg);
  else if (data->type == TC_STATS_V0){
    // assert(0!=0 && "Not implemented");
    //print_tc_stats(&data->gtp.msg);
  }else if (data->type == KPM_STATS_V3_0){
    // assert(0!=0 && "Not implemented");
//    print_kpm_stats(&data->kpm_ind);
  } else if (data->type == RAN_CTRL_STATS_V1_03){
    // assert(0!=0 && "Not implemented");
  } else {
    assert(0 != 0 && "Unknown data type");
  }
}

