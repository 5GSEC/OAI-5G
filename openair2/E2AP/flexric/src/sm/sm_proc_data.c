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


#include "sm_proc_data.h"

#include <assert.h>
#include <stdlib.h>

void free_sm_subs_data(sm_subs_data_t* data)
{
  assert(data != NULL);

  if(data->action_def != NULL){
    assert(data->len_ad != 0);
    free(data->action_def);
  }

  if(data->event_trigger != NULL){
    assert(data->len_et != 0);
    free(data->event_trigger);
  }

}

void free_sm_ind_data(sm_ind_data_t* data)
{
  assert(data != NULL);

  if(data->ind_hdr != NULL){
    assert(data->len_hdr != 0);
    free(data->ind_hdr);
  }

  if(data->ind_msg != NULL){
    assert(data->len_msg != 0);
    free(data->ind_msg);
  }

  if(data->call_process_id != NULL){
    assert(data->len_cpid != 0);
    free(data->call_process_id);
  }

}

void free_sm_ctrl_req_data(sm_ctrl_req_data_t* data)
{
  assert(data != NULL);

  if(data->ctrl_hdr != NULL){
    assert(data->len_hdr != 0);
    free(data->ctrl_hdr);
  }

  if(data->ctrl_msg != NULL){
    assert(data->len_msg != 0);
    free(data->ctrl_msg);
  }
}

void free_sm_ctrl_out_data(sm_ctrl_out_data_t* data)
{
  assert(data != NULL);

  if(data->len_out > 0){
    assert(data->ctrl_out != NULL);
    free(data->ctrl_out);
  }
}

void free_sm_e2_setup(sm_e2_setup_data_t* data)
{
  assert(data != NULL);

  if(data->ran_fun_def != NULL){
    assert(data->len_rfd != 0);
    free(data->ran_fun_def);
  }

  free_ran_function(&data->rf);
} 

void free_sm_ric_service_update(sm_ric_service_update_data_t* data)
{
  assert(data != NULL);
  
  if(data->ran_fun_def != NULL){
    assert(data->len_rfd != 0);
    free(data->ran_fun_def);
  }

}

