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



#ifndef E2AP_AGENT_H
#define E2AP_AGENT_H

#include "lib/ap/e2ap_ap.h"                                      // for e2ap...
#include "lib/ap/e2ap_types/e2_setup_request.h"                  // for e2_s...
#include "lib/ap/e2ap_types/ric_control_ack.h"                   // for ric_...
#include "lib/ap/e2ap_types/ric_control_failure.h"               // for ric_...
#include "lib/ap/e2ap_types/ric_indication.h"                    // for ric_...
#include "lib/ap/e2ap_types/ric_subscription_delete_failure.h"   // for ric_...
#include "lib/ap/e2ap_types/ric_subscription_delete_response.h"  // for ric_...
#include "lib/ap/e2ap_types/ric_subscription_failure.h"          // for ric_...
#include "lib/ap/e2ap_types/ric_subscription_response.h"         // for ric_...
#include "lib/ap/type_defs.h"                                    // for e2ap...
#include "util/byte_array.h"                                     // for byte...

typedef struct e2ap_agent_s 
{
  e2ap_ap_t base;

} e2ap_agent_t;

void e2ap_msg_free_ag(e2ap_agent_t* ag, e2ap_msg_t* msg);

/////////////
// Encoding
//////////////

byte_array_t e2ap_msg_enc_ag(e2ap_agent_t* ap, const e2ap_msg_t* msg); 

byte_array_t e2ap_enc_setup_request_ag(e2ap_agent_t* ap, e2_setup_request_t* sr);

byte_array_t e2ap_enc_subscription_response_ag(e2ap_agent_t* ap, const ric_subscription_response_t* sr);

byte_array_t e2ap_enc_subscription_failure_ag(e2ap_agent_t* ap,const ric_subscription_failure_t* sf);

byte_array_t e2ap_enc_indication_ag(e2ap_agent_t* ap, const ric_indication_t* ind);

byte_array_t e2ap_enc_subscription_delete_response_ag(e2ap_agent_t* ap, const ric_subscription_delete_response_t*  sdr);

byte_array_t e2ap_enc_subscription_delete_failure_ag(e2ap_agent_t* ap, const ric_subscription_delete_failure_t*  sdf);

byte_array_t e2ap_enc_control_acknowledge_ag(e2ap_agent_t* ap, const ric_control_acknowledge_t* ca);

byte_array_t e2ap_enc_control_failure_ag(e2ap_agent_t* ap, const ric_control_failure_t* cf);


//
// Decoding
//

e2ap_msg_t e2ap_msg_dec_ag(e2ap_agent_t* ap, byte_array_t ba); 

#endif

