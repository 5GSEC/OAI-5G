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


#include "e2ap_xapp.h"
#include <assert.h>                           // for assert
#include <stdio.h>                            // for NULL
#include "lib/ap/dec/e2ap_msg_dec_generic.h"  // for e2ap_msg_dec_gen
#include "lib/ap/enc/e2ap_msg_enc_generic.h"  // for e2ap_enc_control_reques...
#include "lib/ap/free/e2ap_msg_free.h"        // for e2ap_free_control_request

//#include "msg_handler_ric.h"
//#include "lib/ap/free/e2ap_msg_free.h"
//#include "lib/ep/e2ap_ep.h"
//#include "lib/ap/dec/e2ap_msg_dec_generic.h"
//#include "lib/ap/enc/e2ap_msg_enc_generic.h"

//#include <assert.h>
//#include <stdio.h>

void e2ap_msg_free_xapp(e2ap_xapp_t* ap, e2ap_msg_t* msg)
{
  assert(ap != NULL);
  assert(msg != NULL);
  const e2_msg_type_t msg_type = msg->type;   
  if(msg_type == NONE_E2_MSG_TYPE)
    return;

  ap->base.type.free_msg[msg_type](msg);
}

void e2ap_free_subscription_request_xapp(e2ap_xapp_t* ap, ric_subscription_request_t* sr)
{
  assert(ap != NULL);
  assert(sr != NULL);
  e2ap_free_subscription_request(sr);
}

void e2ap_free_control_request_xapp(e2ap_xapp_t* ap, ric_control_request_t* ctrl_req)
{
  assert(ap != NULL);
  assert(ctrl_req != NULL);
  e2ap_free_control_request(ctrl_req);
}


e2ap_msg_t e2ap_msg_dec_xapp(e2ap_xapp_t* ap, byte_array_t ba)
{
  assert(ap != NULL);
  e2ap_msg_t msg = e2ap_msg_dec_gen(&ap->base.type, ba);
  return msg;
}

byte_array_t e2ap_msg_enc_xapp(e2ap_xapp_t* ap, e2ap_msg_t* msg)
{
  assert(ap != NULL);
  assert(msg != NULL);
  const e2_msg_type_t msg_type = msg->type;   
  assert(msg_type < NONE_E2_MSG_TYPE);

  byte_array_t ba = ap->base.type.enc_msg[msg_type](msg);
  return ba;
}

byte_array_t e2ap_enc_subscription_request_xapp(e2ap_xapp_t* ap, ric_subscription_request_t* sr)
{
  assert(ap != NULL);
  assert(sr != NULL);
  return e2ap_enc_subscription_request_gen(&ap->base.type, sr);
}

byte_array_t e2ap_enc_subscription_delete_request_xapp(e2ap_xapp_t* ap, ric_subscription_delete_request_t* sd)
{
  assert(ap != NULL);
  assert(sd != NULL);
  return e2ap_enc_subscription_delete_request_gen(&ap->base.type, sd);
}

byte_array_t e2ap_enc_setup_request_xapp(e2ap_xapp_t* ap, e2_setup_request_t* sr)
{
  assert(ap != NULL);
  assert(sr != NULL);
  return e2ap_enc_setup_request_gen(&ap->base.type, sr);
}

byte_array_t e2ap_enc_control_request_xapp(e2ap_xapp_t* ap, ric_control_request_t* ctrl_req)
{
  assert(ap != NULL);
  assert(ctrl_req != NULL);
  return e2ap_enc_control_request_gen(&ap->base.type,ctrl_req);
}

byte_array_t e2ap_enc_e42_setup_request_xapp(e2ap_xapp_t* ap, e42_setup_request_t* sr)
{
  assert(ap != NULL);
  assert(sr != NULL);
  return e2ap_enc_e42_setup_request_gen(&ap->base.type, sr);
}


byte_array_t e2ap_enc_e42_subscription_request_xapp(e2ap_xapp_t* ap, e42_ric_subscription_request_t* sr)
{
  assert(ap != NULL);
  assert(sr != NULL);
  return e2ap_enc_e42_subscription_request_gen(&ap->base.type, sr);
}

byte_array_t e2ap_enc_ric_subscription_delete_xapp(e2ap_xapp_t* ap, ric_subscription_delete_request_t* sdr)
{
  assert(ap != NULL);
  assert(sdr != NULL);
  return e2ap_enc_subscription_delete_request_gen(&ap->base.type, sdr);
}


byte_array_t e2ap_enc_e42_ric_subscription_delete_xapp(e2ap_xapp_t* ap, e42_ric_subscription_delete_request_t* sdr)
{
  assert(ap != NULL);
  assert(sdr != NULL);
  return e2ap_enc_e42_subscription_delete_request_gen(&ap->base.type, sdr);
}



byte_array_t e2ap_enc_e42_control_request_xapp(e2ap_xapp_t* ap, e42_ric_control_request_t* cr)
{
  assert(ap != NULL);
  assert(cr != NULL);
  return e2ap_enc_e42_control_request_gen(&ap->base.type, cr);
}


