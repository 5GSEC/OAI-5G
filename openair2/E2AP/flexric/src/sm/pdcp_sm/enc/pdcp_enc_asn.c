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



#include "pdcp_enc_asn.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

byte_array_t pdcp_enc_event_trigger_asn(pdcp_event_trigger_t const* event_trigger)
{
  assert(0!=0 && "Not implemented");

  assert(event_trigger != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t pdcp_enc_action_def_asn(pdcp_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t pdcp_enc_ind_hdr_asn(pdcp_ind_hdr_t const* ind_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ind_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t pdcp_enc_ind_msg_asn(pdcp_ind_msg_t const* ind_msg)
{
  assert(0!=0 && "Not implemented");

  assert(ind_msg != NULL);
  byte_array_t  ba = {0};
  return ba;
}


byte_array_t pdcp_enc_call_proc_id_asn(pdcp_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t pdcp_enc_ctrl_hdr_asn(pdcp_ctrl_hdr_t const* ctrl_hdr)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_hdr != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t pdcp_enc_ctrl_msg_asn(pdcp_ctrl_msg_t const* ctrl_msg)
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_msg != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t pdcp_enc_ctrl_out_asn(pdcp_ctrl_out_t const* ctrl_out) 
{
  assert(0!=0 && "Not implemented");

  assert(ctrl_out != NULL );
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t pdcp_enc_func_def_asn(pdcp_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

