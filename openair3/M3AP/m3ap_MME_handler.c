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

/*! \file m3ap_eNB_handler.c
 * \brief m3ap handler procedures for eNB
 * \author Javier Morgade <javier.morgade@ieee.org>
 * \date 2019
 * \version 0.1
 */

#include <stdint.h>

#include "intertask_interface.h"

#include "oai_asn1.h"

#include "m3ap_common.h"
#include "m3ap_MME_defs.h"
#include "m3ap_handler.h"
#include "m3ap_decoder.h"
#include "m3ap_ids.h"

#include "m3ap_MME_management_procedures.h"
//#include "m3ap_MCE_generate_messages.h"

#include "m3ap_MME_interface_management.h"

#include "assertions.h"
#include "conversions.h"

/* Handlers matrix. Only eNB related procedure present here */
static const m3ap_MCE_message_decoded_callback m3ap_MME_messages_callback[][3] = {
    {0, MME_handle_MBMS_SESSION_START_RESPONSE, 0}, /* MBMSSessionStart  */
    {0, MME_handle_MBMS_SESSION_STOP_RESPONSE, 0}, /* MBMSSessionStop */
    {0, 0, 0}, /* Error Indication */
    {0, 0, 0}, /* Reset */
    {0, 0, 0}, /* ??? */
    {0, MME_handle_MBMS_SESSION_UPDATE_RESPONSE, 0}, /* MBMSSessionUpdate */
    {0, 0, 0}, /* MCEConfigurationUpdate */
    {MME_handle_M3_SETUP_REQUEST, 0, 0} /* M3 Setup */
};

static char *m3ap_direction2String(int m3ap_dir) {
static char *m3ap_direction_String[] = {
  "", /* Nothing */
  "Originating message", /* originating message */
  "Successfull outcome", /* successfull outcome */
  "UnSuccessfull outcome", /* successfull outcome */
};
return(m3ap_direction_String[m3ap_dir]);
}


int m3ap_MME_handle_message(instance_t instance, uint32_t assoc_id, int32_t stream,
                            const uint8_t * const data, const uint32_t data_length)
{
  M3AP_M3AP_PDU_t pdu;
  int ret;

  DevAssert(data != NULL);

  memset(&pdu, 0, sizeof(pdu));

 
  if (m3ap_decode_pdu(&pdu, data, data_length) < 0) {
    LOG_E(M3AP, "Failed to decode PDU\n");
    return -1;
  }

  /* Checking procedure Code and direction of message */
  if (pdu.choice.initiatingMessage.procedureCode > sizeof(m3ap_MME_messages_callback) / (3 * sizeof(
        m3ap_message_decoded_callback))
      || (pdu.present > M3AP_M3AP_PDU_PR_unsuccessfulOutcome)) {
    LOG_E(M3AP, "[SCTP %d] Either procedureCode %ld or direction %d exceed expected\n",
               assoc_id, pdu.choice.initiatingMessage.procedureCode, pdu.present);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_M3AP_M3AP_PDU, &pdu);
    return -1;
  }

  /* No handler present.
   * This can mean not implemented or no procedure for eNB (wrong direction).
   */
  if (m3ap_MME_messages_callback[pdu.choice.initiatingMessage.procedureCode][pdu.present - 1] == NULL) {
    LOG_E(M3AP, "[SCTP %d] No handler for procedureCode %ld in %s\n",
                assoc_id, pdu.choice.initiatingMessage.procedureCode,
               m3ap_direction2String(pdu.present - 1));
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_M3AP_M3AP_PDU, &pdu);
    return -1;
  }
  
  /* Calling the right handler */
  LOG_I(M3AP, "Calling handler with instance %ld\n",instance);
  ret = (*m3ap_MME_messages_callback[pdu.choice.initiatingMessage.procedureCode][pdu.present - 1])
        (instance, assoc_id, stream, &pdu);
  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_M3AP_M3AP_PDU, &pdu);
  return ret;
}


