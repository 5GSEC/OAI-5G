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



#include "mac_dec_asn.h"

#include <assert.h>

//#include "../ie/asn/E2SM-MACStats-EventTriggerDefinition.h"
//#include "../ie/asn/E2SM-MACStats-EventTriggerDefinition-Format1.h"
//#include "../ie/asn/E2SM-MACStats-IndicationMessage.h"



mac_event_trigger_t mac_dec_event_trigger_asn(size_t len, uint8_t const buf[len])
{

  assert(0!=0 && "Not implemented");

/*
  E2SM_MACStats_EventTriggerDefinition_t ev_tr = {0};
  E2SM_MACStats_EventTriggerDefinition_t* ev_tr_ptr = &ev_tr;

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_MACStats_EventTriggerDefinition, (void **) &ev_tr_ptr, buf, len);
  assert(rval.code == RC_OK);

  MACStats_TriggerNature_t val = ev_tr.choice.eventDefinition_Format1->triggerNature;
  mac_event_trigger_t ret = {0};
  if(val == MACStats_TriggerNature_oneMs	){
    ret.ms = 1; 
  }else if( val == MACStats_TriggerNature_twoMs	){
    ret.ms = 2; 
  } else if (val == MACStats_TriggerNature_fiveMs	) {
    ret.ms = 5; 
  } else {
    assert(0!=0 && "Not implemented case");
  }

  // Despite its name, it frees the elements allocated by asn_decode
  ASN_STRUCT_RESET(asn_DEF_E2SM_MACStats_EventTriggerDefinition, &ev_tr );

  return ret;
  */

}

mac_action_def_t mac_dec_action_def_asn(size_t len, uint8_t const action_def[len])
{

  assert(0!=0 && "Not implemented");
}

mac_ind_hdr_t mac_dec_ind_hdr_asn(size_t len, uint8_t const ind_hdr[len])
{
  assert(0!=0 && "Not implemented");

}

mac_ind_msg_t mac_dec_ind_msg_asn(size_t len, uint8_t const ind_msg[len])
{

  assert(0!=0 && "Not implemented");
  /*
  E2SM_MACStats_IndicationMessage_t ind_msg_asn = {0};
  E2SM_MACStats_IndicationMessage_t* ind_msg_ptr = &ind_msg_asn; 

  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_MACStats_IndicationMessage, (void **) &ind_msg_ptr, ind_msg, len);
  assert(rval.code == RC_OK);


 mac_ind_msg_t ret = {.tx_bytes = ind_msg_asn.txBytes, .tx_pkts = ind_msg_asn.txPkts};
  ASN_STRUCT_RESET(asn_DEF_E2SM_MACStats_IndicationMessage, &ind_msg_asn);
  return ret;
  */
}

mac_call_proc_id_t mac_dec_call_proc_id_asn(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");

}

mac_ctrl_hdr_t mac_dec_ctrl_hdr_asn(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(0!=0 && "Not implemented");

}

mac_ctrl_msg_t mac_dec_ctrl_msg_asn(size_t len, uint8_t const ctrl_msg[len])
{
  assert(0!=0 && "Not implemented");

}


mac_ctrl_out_t mac_dec_ctrl_out_asn(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(0!=0 && "Not implemented");

}

mac_func_def_t mac_dec_func_def_asn(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");

}

