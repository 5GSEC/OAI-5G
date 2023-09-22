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



#include "influx.h"

#include <assert.h>                                      // for assert
#include <netinet/in.h>                                  // for sockaddr_in
#include <pthread.h>                                     // for pthread_once
#include <stdint.h>                                      // for uint32_t
#include <stdlib.h>                                      // for exit, EXIT_F...
#include <string.h>                                      // for strlen, memset
#include <stdio.h>
#include <sys/socket.h>                                  // for sendto, MSG_...

#include "../../sm/mac_sm/ie/mac_data_ie.h"    // for mac_ind_msg_t
#include "../../sm/pdcp_sm/ie/pdcp_data_ie.h"  // for pdcp_ind_msg_t
#include "../../sm/rlc_sm/ie/rlc_data_ie.h"    // for rlc_ind_msg_t
#include "../../sm/agent_if/read/sm_ag_if_rd.h"
#include "string_parser.h"                               // for to_string_ma...

static
pthread_once_t init_socket = PTHREAD_ONCE_INIT;

static
int sockfd = 0;

static
struct sockaddr_in servaddr;

static
int const PORT = 8094;


void init_udp_socket()
{
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    assert(0!=0 && "Error creating the socket");
    exit(EXIT_FAILURE);
  }
  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

}

/*
static
void notify_kpm_ind_msg_frm_1(const kpm_ind_msg_format_1_t message, uint32_t truncated_ts)
{

  char stats[1024] = {0};
  int max = 1024;

  for(size_t i = 0; i < message.meas_data_lst_len; i++){
      meas_data_lst_t* curMeasData = &message.meas_data_lst[i];
      
      if (i == 0 && message.gran_period_ms){
        int rc = snprintf(stats, max,  "kpm_stats: "
                        "tstamp=%u"
                        ",granulPeriod=%u"
                        ",kpm_MeasData"
                        ",kpm->MeasData_len=%zu"
                        , truncated_ts
                        , *(message.gran_period_ms)
                        , message.meas_data_lst_len
                        );
        assert(rc < (int)max && "Not enough space in the char array to write all the data");
        rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        assert(rc != -1);
      }else if(i == 0 && message.gran_period_ms == NULL){
        int rc = snprintf(stats, max,  "kpm_stats: "
                        "tstamp=%u"
                        ",granulPeriod="
                        ",kpm_MeasData"
                        ",kpm->MeasData_len=%zu"
                        , truncated_ts
                        , message.meas_data_lst_len
                        );
        assert(rc < (int)max && "Not enough space in the char array to write all the data");
        rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        assert(rc != -1);
      }

      memset(stats, 0, sizeof(stats));
      int rc = snprintf(stats, max,
                        ",kpm_measData[%zu]"
                        ",MeasData->incompleteFlag=%p"
                        ",MeasData->measRecord_len=%zu"
                        , i
                        , (void *)curMeasData->incomplete_flag
                        , curMeasData->meas_record_len
                        );
      assert(rc < (int)max && "Not enough space in the char array to write all the data");
      rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
      assert(rc != -1);

      for(size_t j = 0; j < curMeasData->meas_record_len; j++){
        meas_record_lst_t* curMeasRecord = &(curMeasData->meas_record_lst[j]);
        memset(stats, 0, sizeof(stats));
        to_string_kpm_measRecord(curMeasRecord, j, stats, max);
        rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        assert(rc != -1);
      }
    }

    for(size_t i = 0; i < message.meas_info_lst_len; i++){
      meas_info_format_1_lst_t* curMeasInfo = &message.meas_info_lst[i];
      if (i == 0){
        memset(stats, 0, sizeof(stats));
        int rc = snprintf(stats, max,
                        ",kpm_MeasInfo"
                        ",kpm->MeasInfo_len=%zu",
                        i
                        );
        assert(rc < (int)max && "Not enough space in the char array to write all the data");
        rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        assert(rc != -1);
      }

      if (curMeasInfo->meas_type.type == ID_MEAS_TYPE){
        memset(stats, 0, sizeof(stats));
        int rc = snprintf(stats, max,
                          ",MeasInfo[%zu]"
                          ",measType=%d"
                          ",measID=%d"
                          , i
                          , curMeasInfo->meas_type.type
                          , curMeasInfo->meas_type.id
                          );
        assert(rc < (int)max && "Not enough space in the char array to write all the data");
        rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        assert(rc != -1);
      } else if (curMeasInfo->meas_type.type == NAME_MEAS_TYPE){
        memset(stats, 0, sizeof(stats));
        int rc = snprintf(stats, max,
                          ",MeasInfo[%zu]"
                          ",measType=%d"
                          ",measName=%s"
                          , i
                          , curMeasInfo->meas_type.type
                          , (char *)(curMeasInfo->meas_type.name.buf)
                          );
        assert(rc < (int)max && "Not enough space in the char array to write all the data");
        rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        assert(rc != -1);
      }

      for(size_t j = 0; j < curMeasInfo->label_info_lst_len; ++j){
        label_info_lst_t* curLabelInfo = &curMeasInfo->label_info_lst[j];
        memset(stats, 0, sizeof(stats));
        to_string_kpm_labelInfo(curLabelInfo, j, stats, max);
        int rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        assert(rc != -1);
      }
    }

}
*/

void notify_influx_listener(sm_ag_if_rd_ind_t const* data)
{
  assert(data != NULL);

  assert(data->type == MAC_STATS_V0 || data->type == RLC_STATS_V0 || data->type == PDCP_STATS_V0 
      || data->type == SLICE_STATS_V0 || data->type == KPM_STATS_V3_0 || data->type == GTP_STATS_V0
      || data->type == TC_STATS_V0 || data->type == RAN_CTRL_STATS_V1_03);

  return;

  // Not supported


  pthread_once(&init_socket, init_udp_socket);

//  printf("Influx db data called!!!\n");

  


  if(data->type == MAC_STATS_V0){
    mac_ind_msg_t const* ind =  &data->mac.msg;

    for(uint32_t i = 0; i < ind->len_ue_stats; ++i){
      char stats[1024] = {0};
      to_string_mac_ue_stats(&ind->ue_stats[i], ind->tstamp, stats, 1024);
      int const rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
      assert(rc != -1);
    }
  } else if (data->type == RLC_STATS_V0){
    rlc_ind_msg_t const* rlc = &data->rlc.msg;

    for(uint32_t i = 0; i < rlc->len; ++i){

      char stats[1024] = {0};
      to_string_rlc_rb(&rlc->rb[i], rlc->tstamp, stats, 1024);

     int const rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
     assert(rc != -1);
    }

  } else if (data->type == PDCP_STATS_V0){
    pdcp_ind_msg_t const* pdcp = &data->pdcp.msg;

    for(uint32_t i = 0; i < pdcp->len; ++i){
      char stats[512] = {0};
      pdcp_radio_bearer_stats_t* rb = &pdcp->rb[i];

      to_string_pdcp_rb(rb, pdcp->tstamp, stats, 512);
      int const rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
      assert(rc != -1);
    } 
  } else if(data->type == SLICE_STATS_V0){
    slice_ind_msg_t const* slice = &data->slice.msg;

    char stats[2048] = {0};

    to_string_slice(slice, slice->tstamp, stats, 2048);
    int const rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    assert(rc != -1);
  } else if (data->type == GTP_STATS_V0){
    gtp_ind_msg_t const* gtp = &data->gtp.msg;

    for(uint32_t i = 0; i < gtp->len; ++i){
      char stats[512] = {0};
      gtp_ngu_t_stats_t* ngut = &gtp->ngut[i];

      to_string_gtp_ngu(ngut, gtp->tstamp, stats, 512);
      int const rc = sendto(sockfd, stats, strlen(stats),  MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
      assert(rc != -1);
    }
  } else if(data->type == KPM_STATS_V3_0){
    assert(0!=0 && "Not implemented");

    // kpm_ind_data_t const* kpm = &data->kpm;

    // From Indication Header : Collect Start Time
    // To be defined better, switch/case for specific format
    // uint64_t truncated_ts = (uint64_t)kpm->hdr.kpm_ric_ind_hdr_format_1.collectStartTime;

   // switch (kpm->msg.type)
   // {
   // case FORMAT_1_INDICATION_MESSAGE:
   //   notify_kpm_ind_msg_frm_1(kpm->msg.frm_1, truncated_ts);
   //   break;

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

   // default:
   //   assert(false && "Unknown Indication Message Type");
   // }

  } else {
    assert(0 != 0 || "invalid data type ");
  }

}

