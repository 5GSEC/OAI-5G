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
/* 
 * Some assumptions/tradeoffs on the implementation:
 * - we assume same CPU architecture among nodes (client/server) little endian/big endian. Most of the CPUs nowadays are little endian, 
 *   so  we relax a bit this assumption relying on kpm_data_ie.h macro that checks that and fails if endianess is not little endian.
 * - Tradeoff on buffer length checks: We control buffer length at the end of the buffer; not needed complete check before walking the data stack.
 * - We can't have just a cast to the output data structure from the byte array as we would pass the burden of memory management 
 *   to the caller of the function.
 * - memory alignment: you have a concern of memory alignment here. Three solutions available: padding/packing/do nothing. We choose to do nothing as this data
 *   is serialized and deserialized to buffer (byte array) using our code. In a general case this does not work if you have to serialize
 *   for data exchange with third party tools.
 */
#include "kpm_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

kpm_event_trigger_t kpm_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  kpm_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

// TO BE IMPLEMENTES, it is just a stub
kpm_action_def_t kpm_dec_action_def_plain(size_t len, uint8_t const action_def[len]) 
{
  assert(0!=0 && "Not implemented");
  
  kpm_action_def_t adef ={0}; 
  
  uint8_t *ptr = action_def;
  memcpy(&adef.ric_style_type, ptr, sizeof(adef.ric_style_type));
  ptr += (sizeof(adef.ric_style_type) < padding)? padding : sizeof(adef.ric_style_type);
  
  memcpy(&adef.adf_dtype, ptr, sizeof (adef.adf_dtype));
  ptr += sizeof(adef.adf_dtype);

  switch(adef.adf_dtype)
  {
  case ADF_TYPE1:

    adf1_t *d = &adef.adf.adf1;

    memcpy(&(d->MeasInfo_len), ptr, sizeof(d->MeasInfo_len));
    ptr += sizeof(d->MeasInfo_len);
    assert(d->MeasInfo_len <= maxnoofMeasurementInfo && \
      "action definition 1: MeasInfo_len %d bigger than maximum allowed", d->MeasInfo_len);
    
    d->MeasInfo = (MeasInfo_item_t *)calloc(d->MeasInfo_len, sizeof(d->MeasInfo));
    assert(d->MeasInfo != NULL && "Memory exhausted");
    memcpy(&d->MeasInfo, ptr, d->MeasInfo_len * sizeof(d->MeasInfo));

    // we skip test on measType, it is not useful
    
    /** 
     * TODO:
     * adf1_t{
     *   size_t            MeasInfo_len; // <  maxnoofMeasurementInfo
     *   MeasInfo_item_t   *MeasInfo;    // list implemented as array of length MeasInfo_len 
     *   uint32_t          granularity_period;
     *   struct CGI_t      *cellGlobalID; // optional. If NULL means not present
     * }
     * 
     * typedef struct MeasInfo_item_t{
     *   MeasurementType_t	measType;
	   *  LabelInfoList_t	  *labelInfo;   // list implemented as array, maxnoofLabelInfo
     *   size_t            labelInfo_len;// length of the array labelInfo
     *  }MeasInfo_item_t; 
     * 
    */

    break;
  case ADF_TYPE2: 
    break;
  case ADF_TYPE3:
    break;
  case ADF_TYPE4:
    break;
  case ADF_TYPE5:
    break;
  default:
    assert("unexpected adf_type: %d", adef.adf_dtype);
  }
 
  return adef;
}