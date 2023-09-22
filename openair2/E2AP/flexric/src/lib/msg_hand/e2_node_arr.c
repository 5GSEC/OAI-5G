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

#include "e2_node_arr.h"

#include <assert.h>
#include <stdlib.h>

void free_e2_node_arr(e2_node_arr_t* xapp)
{
  assert(xapp != NULL);

  for(size_t i = 0; i < xapp->len; ++i){
    for(size_t j = 0; j < xapp->n[i].len_rf; ++j){
      free_ran_function(&xapp->n[i].ack_rf[j]);
    }
    if(xapp->n[i].len_rf > 0)
      free(xapp->n[i].ack_rf);

    free_global_e2_node_id(&xapp->n[i].id);
  }
 
  if(xapp->len > 0){
    free(xapp->n);
  }
}

