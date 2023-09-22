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



#ifndef SERVICE_MODEL_STRING_PARSER_H
#define SERVICE_MODEL_STRING_PARSER_H 

#include <stddef.h>                            // for size_t
#include <stdint.h>                            // for int64_t
#include "../../sm/mac_sm/ie/mac_data_ie.h"
#include "../../sm/rlc_sm/ie/rlc_data_ie.h"
#include "../../sm/pdcp_sm/ie/pdcp_data_ie.h"
#include "../../sm/slice_sm/ie/slice_data_ie.h"
#include "../../sm/gtp_sm/ie/gtp_data_ie.h"
#include "../../sm/kpm_sm_v03.00/ie/kpm_data_ie.h"


void to_string_mac_ue_stats(mac_ue_stats_impl_t* stats, int64_t tstamp, char* out, size_t out_len);

void to_string_rlc_rb(rlc_radio_bearer_stats_t* rlc, int64_t tstamp, char* out, size_t out_len);

void to_string_pdcp_rb(pdcp_radio_bearer_stats_t* pdcp, int64_t tstamp, char* out, size_t out_len);

void to_string_slice(slice_ind_msg_t const* slice, int64_t tstamp, char* out, size_t out_len);

void to_string_gtp_ngu(gtp_ngu_t_stats_t const* gtp, int64_t tstamp, char* out, size_t out_len);

void to_string_kpm_measRecord(meas_record_lst_t const* measRecord, size_t idx, char*out, size_t out_len);

void to_string_kpm_labelInfo(label_info_lst_t const* labelInfo, size_t idx, char*out, size_t out_len);

#endif
