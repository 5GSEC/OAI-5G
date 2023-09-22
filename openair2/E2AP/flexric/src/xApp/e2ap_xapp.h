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


#ifndef E2AP_XAPP_H
#define E2AP_XAPP_H

#include "lib/ap/e2ap_ap.h"                                     // for e2ap_...
#include "lib/ap/e2ap_types/e2_setup_response.h"                // for e2_se...
#include "lib/ap/e2ap_types/e42_setup_request.h"                
#include "lib/ap/e2ap_types/e42_ric_subscription_request.h"                
#include "lib/ap/e2ap_types/e42_ric_control_request.h"

#include "lib/ap/e2ap_types/ric_control_request.h"              // for ric_c...
#include "lib/ap/e2ap_types/ric_subscription_delete_request.h"  // for ric_s...
#include "lib/ap/e2ap_types/ric_subscription_request.h"         // for ric_s...
#include "lib/ap/free/../type_defs.h"                           // for e2ap_...
#include "util/byte_array.h"                                    // for byte_...


typedef struct
{
  e2ap_ap_t base;
} e2ap_xapp_t;


/////////
// Free
/////////

void e2ap_msg_free_xapp(e2ap_xapp_t* xapp, e2ap_msg_t* msg);

void e2ap_free_subscription_request_xapp(e2ap_xapp_t* ap, ric_subscription_request_t* sr);  

void e2ap_free_control_request_xapp(e2ap_xapp_t* ap, ric_control_request_t* ctrl_req);

//////////////
// Encoding
//////////////

byte_array_t e2ap_msg_enc_xapp(e2ap_xapp_t* ap, e2ap_msg_t* msg);

//byte_array_t e2ap_enc_setup_request_xapp(e2ap_xapp_t* ap, e2_setup_request_t* sr);

byte_array_t e2ap_enc_e42_setup_request_xapp(e2ap_xapp_t* ap, e42_setup_request_t* sr);

byte_array_t e2ap_enc_subscription_request_xapp(e2ap_xapp_t* ap, ric_subscription_request_t* sr);

byte_array_t e2ap_enc_subscription_delete_request_xapp(e2ap_xapp_t* ap, ric_subscription_delete_request_t* sd);

byte_array_t e2ap_enc_control_request_xapp(e2ap_xapp_t* ap, ric_control_request_t* ctrl_req);

byte_array_t e2ap_enc_e42_setup_request_xapp(e2ap_xapp_t* ap, e42_setup_request_t* sr);

byte_array_t e2ap_enc_e42_subscription_request_xapp(e2ap_xapp_t* ap, e42_ric_subscription_request_t* sr);

byte_array_t e2ap_enc_ric_subscription_delete_xapp(e2ap_xapp_t* ap, ric_subscription_delete_request_t* sdr);

byte_array_t e2ap_enc_e42_ric_subscription_delete_xapp(e2ap_xapp_t* ap, e42_ric_subscription_delete_request_t* sdr);

byte_array_t e2ap_enc_e42_control_request_xapp(e2ap_xapp_t* ap, e42_ric_control_request_t* cr);

//////////////
// Decoding
//////////////

e2ap_msg_t e2ap_msg_dec_xapp( e2ap_xapp_t* ap, byte_array_t ba);


#endif

