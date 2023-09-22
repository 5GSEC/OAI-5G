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

#ifndef FILL_RND_DATA_KPM_V3_H
#define FILL_RND_DATA_KPM_V3_H

#include "../../src/sm/kpm_sm_v03.00/ie/kpm_data_ie.h"

kpm_event_trigger_def_t fill_kpm_event_trigger_def(void);
  
kpm_act_def_t fill_kpm_action_def(void);

kpm_ind_hdr_t fill_kpm_ind_hdr(void);

kpm_ind_msg_t fill_kpm_ind_msg(void);

kpm_ran_function_def_t fill_kpm_ran_func_def(void);

#endif

