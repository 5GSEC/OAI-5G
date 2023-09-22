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



#include "mac_enc_asn.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//#include "../ie/asn/E2SM-MACStats-EventTriggerDefinition.h"
//#include "../ie/asn/E2SM-MACStats-EventTriggerDefinition-Format1.h"
//#include "../ie/asn/E2SM-MACStats-IndicationMessage.h"


//#include "asn_application.h"

/*
static
byte_array_t encode(const void* pdu, asn_TYPE_descriptor_t type)
{
  assert(pdu != NULL);
  byte_array_t b = {.buf = malloc(2048), .len = 2048};
  assert(b.buf != NULL);
 // xer_fprint(stderr, &asn_DEF_E2AP_PDU, pdu);
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &type, pdu, b.buf, b.len);
  assert(er.encoded < (ssize_t) b.len);
  if(er.encoded == -1) {
    printf("Failed the encoding in type %s and xml_type = %s\n", er.failed_type->name, er.failed_type->xml_tag);
    fflush(stdout);
    assert(0!=0 && "Failed encoding");
  }
  assert(er.encoded > -1);
  b.len = er.encoded;
  return b;
}
*/

byte_array_t mac_enc_event_trigger_asn(mac_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
 assert(0!=0 && "Not implemented");

  byte_array_t  ba = {0};
  return ba;
/*
  E2SM_MACStats_EventTriggerDefinition_t ev_tr = {0};

  ev_tr.present = E2SM_MACStats_EventTriggerDefinition_PR_eventDefinition_Format1;

  E2SM_MACStats_EventTriggerDefinition_Format1_t format = {0};

  if(event_trigger->ms == 1){
    format.triggerNature = MACStats_TriggerNature_oneMs;	
  } else if(event_trigger->ms == 2){
    format.triggerNature = MACStats_TriggerNature_twoMs;	
  } else if(event_trigger->ms == 5){
    format.triggerNature = MACStats_TriggerNature_fiveMs;	
  } else {
    assert(0!=0 && "Not valid event trigger value");
  }

  ev_tr.choice.eventDefinition_Format1 = &format;
  byte_array_t ba = encode(&ev_tr, asn_DEF_E2SM_MACStats_EventTriggerDefinition);

  return ba;
  */
}

byte_array_t mac_enc_action_def_asn(mac_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ind_hdr_asn(mac_ind_hdr_t const* ind_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ind_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ind_msg_asn(mac_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  assert(0!=0 && "Not implemented");
  byte_array_t  ba = {0};
  return ba;

  /*
  E2SM_MACStats_IndicationMessage_t ind_msg_asn = {0};
  ind_msg_asn.txBytes = ind_msg->tx_bytes;
  ind_msg_asn.txPkts = ind_msg->tx_pkts;

  byte_array_t ba = encode(&ind_msg_asn, asn_DEF_E2SM_MACStats_IndicationMessage);

  return ba;
  */
}


byte_array_t mac_enc_call_proc_id_asn(mac_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ctrl_hdr_asn(mac_ctrl_hdr_t const* ctrl_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_ctrl_msg_asn(mac_ctrl_msg_t const* ctrl_msg)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_msg != NULL);
  byte_array_t  ba = {0};
  return ba;
}


byte_array_t mac_enc_ctrl_out_asn(mac_ctrl_out_t const* ctrl) 
{
  assert(0!=0 && "Not implemented");

  assert( ctrl != NULL );
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t mac_enc_func_def_asn(mac_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

