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


#include "e2ap_plmn.h"

#include <assert.h>
#include <stddef.h>


e2ap_plmn_t cp_e2ap_plmn(const e2ap_plmn_t* src)
{
  assert(src != NULL);

  e2ap_plmn_t dst = {0};

  dst.mcc = src->mcc;
  dst.mnc = src->mnc;
  dst.mnc_digit_len = src->mnc_digit_len;

  return dst;
}

bool eq_e2ap_plmn(const e2ap_plmn_t* m0, const e2ap_plmn_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;

  if(m0->mcc != m1->mcc) 
    return false;

  if(m0->mnc != m1->mnc)
    return false;

  if(m0->mnc_digit_len != m1->mnc_digit_len)
    return false;

  return true;
}


int cmp_e2ap_plmn(const e2ap_plmn_t* m0, const e2ap_plmn_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->mcc < m1->mcc)
    return -1;
  else if(m0->mcc > m1->mcc )
    return 1;

  if(m0->mnc < m1->mnc)
    return -1;
  else if(m0->mnc > m1->mnc )
    return 1;

  if(m0->mnc_digit_len < m1->mnc_digit_len)
    return -1;
  else if(m0->mnc_digit_len  > m1->mnc_digit_len)
    return 1;

  return 0;
}

