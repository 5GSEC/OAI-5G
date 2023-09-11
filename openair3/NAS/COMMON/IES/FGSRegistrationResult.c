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

/*! \file FGSRegistrationResult.c

\brief 5GS Registration result for registration request procedures
\author Yoshio INOUE, Masayuki HARADA
\email: yoshio.inoue@fujitsu.com,masayuki.harada@fujitsu.com
\date 2020
\version 0.1
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "FGSRegistrationResult.h"


int decode_fgs_registration_result(FGSRegistrationResult *fgsregistrationresult, uint8_t iei, uint16_t value, uint32_t len)
{
  int decoded = 0;
  uint16_t *buffer = &value;
  fgsregistrationresult->registrationresult = *buffer & 0x7;
  fgsregistrationresult->smsallowed = *buffer & 0x8;
  decoded = decoded+2;
  return decoded;
}

uint16_t encode_fgs_registration_result(FGSRegistrationResult *fgsregistrationresult)
{
  uint16_t bufferReturn;
  uint16_t *buffer = &bufferReturn;
  uint8_t encoded = 0;
  *(buffer + encoded) = 0x00 | (fgsregistrationresult->smsallowed & 0x8) |
                        (fgsregistrationresult->registrationresult & 0x7);
  encoded= encoded+2;

  return bufferReturn;
}

