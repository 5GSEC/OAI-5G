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

/*!\file nrLDPC_mPass.h
 * \brief Defines the functions for message passing
 *
*/
#ifndef __NR_LDPC_MPASS__H__
#define __NR_LDPC_MPASS__H__

#include <string.h>
#include "common/utils/utils.h"
#include "nrLDPCdecoder_defs.h"
//#include <omp.h>
/**
   \brief Circular memcpy1
                |<- rem->|<- circular shift ->|
   (src) str2 = |--------xxxxxxxxxxxxxxxxxxxxx|
                         \_______________
                                         \
   (dst) str1 =     |xxxxxxxxxxxxxxxxxxxxx---------|
   \param str1 Pointer to the start of the destination buffer
   \param str2 Pointer to the source buffer
   \param Z Lifting size
   \param cshift Circular shift
*/

#define arrPos(a, b) a.d + b* a.dim2
static inline void *nrLDPC_inv_circ_memcpy(int8_t *str1, const int8_t *str2, uint16_t Z, uint16_t cshift)
{
    uint16_t rem = Z - cshift;
    memcpy(str1+cshift, str2    , rem);
    memcpy(str1       , str2+rem, cshift);

    return(str1);
}

/**
   \brief Inverse circular memcpy
                |<- circular shift ->|<- rem->|
   (src) str2 = |xxxxxxxxxxxxxxxxxxxx\--------|
                                      \
   (dst) str1 =               |--------xxxxxxxxxxxxxxxxxxxxx|
   \param str1 Pointer to the start of the destination buffer
   \param str2 Pointer to the source buffer
   \param Z Lifting size
   \param cshift Circular shift
*/
static inline void *nrLDPC_circ_memcpy(int8_t *str1, const int8_t *str2, uint16_t Z, uint16_t cshift)
{
    uint16_t rem = Z - cshift;
    memcpy(str1     , str2+cshift, rem);
    memcpy(str1+rem , str2       , cshift);

    return(str1);
}

/**
   \brief Copies the input LLRs to their corresponding place in the LLR processing buffer.
   Example: BG2
             | 0| 0| LLRs -->                                    |
   BN Groups |22|23|10| 5| 5|14| 7|13| 6| 8| 9|16| 9|12|1|1|...|1|
              ^---------------------------------------/----     /
                            _________________________/    |    /
                           /  ____________________________|___/
                          /  /                            \
   LLR Proc Buffer (BNG) | 1| 5| 6| 7| 8| 9|10|12|13|14|16|22|23|
   Number BN in BNG(R15) |38| 2| 1| 1| 1| 2| 1| 1| 1| 1| 1| 1| 1|
   Idx:                  0  ^                             ^  ^
          38*384=14592 _____|   ...                       |  |
          50*384=19200 -----------------------------------   |
          51*384=19584 --------------------------------------

   \param p_lut Pointer to decoder LUTs
   \param llr Pointer to input LLRs
   \param p_procBuf Pointer the processing buffers
   \param Z Lifting size
   \param BG Base graph
*/
static inline void nrLDPC_llr2llrProcBuf(t_nrLDPC_lut* p_lut, int8_t* llr, int8_t* llrProcBuf, uint16_t Z, uint8_t BG)
{
  const uint8_t numBn2CnG1 = p_lut->numBnInBnGroups[0];
  uint32_t startColParity = (BG == 1) ? (NR_LDPC_START_COL_PARITY_BG1) : (NR_LDPC_START_COL_PARITY_BG2);

  uint32_t colG1 = startColParity * Z;

  const uint16_t* lut_llr2llrProcBufAddr = p_lut->llr2llrProcBufAddr;
  const uint8_t* lut_llr2llrProcBufBnPos = p_lut->llr2llrProcBufBnPos;

  // Copy LLRs connected to 1 CN
  if (numBn2CnG1 > 0) {
    memcpy(&llrProcBuf[0], &llr[colG1], numBn2CnG1 * Z);
  }

  // First 2 columns might be set to zero directly if it's true they always belong to the groups with highest number of connected CNs...
  for (int i = 0; i < startColParity; i++) {
    const int idxBn = lut_llr2llrProcBufAddr[i] + lut_llr2llrProcBufBnPos[i] * Z;
    memcpy(&llrProcBuf[idxBn], llr, Z);
    llr += Z;
  }
}

/**
   \brief Copies the input LLRs to their corresponding place in the CN processing buffer for BG1.
   \param p_lut Pointer to decoder LUTs
   \param llr Pointer to input LLRs
   \param p_procBuf Pointer to the processing buffers
   \param Z Lifting size
*/
static inline void nrLDPC_llr2CnProcBuf_BG1(t_nrLDPC_lut* p_lut, int8_t* llr, int8_t* cnProcBuf, uint16_t Z)
{
  const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
  const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

  uint32_t bitOffsetInGroup;

  // =====================================================================
  // CN group with 3 BNs

  bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[0] * NR_LDPC_ZMAX;

  for (int j = 0; j < 3; j++) {
    const uint16_t* lut_circShift_CNG3 = p_lut->circShift[0].d;
    int8_t* p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];
    const uint8_t* lut_posBnInCnProcBuf_CNG3 = p_lut->posBnInCnProcBuf[0].d;

    const int idxBn = lut_posBnInCnProcBuf_CNG3[j] * Z;

    nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift_CNG3[j]);
  }

  // =====================================================================
  // CN group with BNs
  int BNs[] = {3, 4, 5, 6, 7, 8, 9, 10, 19};
  for (int bn = 1; bn < sizeofArray(BNs); bn++) {
    bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[bn] * NR_LDPC_ZMAX;
    for (int j = 0; j < BNs[bn]; j++) {
      int8_t* p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[bn] + j * bitOffsetInGroup];
      const uint16_t* lut_circShift = arrPos(p_lut->circShift[bn], j);
      const uint8_t* lut_posBnInCnProcBuf = arrPos(p_lut->posBnInCnProcBuf[bn], j);

      for (int i = 0; i < lut_numCnInCnGroups[bn]; i++) {
        const int idxBn = lut_posBnInCnProcBuf[i] * Z;

        nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift[i]);

        p_cnProcBuf += Z;
      }
    }
  }
}

/**
   \brief Copies the input LLRs to their corresponding place in the CN processing buffer for BG2.
   Example: BG2
             | 0| 0| LLRs -->                                    |
   BN Groups |22|23|10| 5| 5|14| 7|13| 6| 8| 9|16| 9|12|1|1|...|1|


   CN Processing Buffer (CNGs) | 3| 4| 5| 6| 8|10|
   Number of CN per CNG (R15)  | 6|20| 9| 3| 2| 2|
                               0  ^     ^\  \
            3*6*384=6912 _________|     ||   \_____________
            (3*6+4*20+5*9)*384=54912____||                 \
                                     Bit | 1| 2| 3| 4| 5| 6|
                                 3*Z CNs>|  |<
                                            ^
                         54912 + 3*384______|

   \param p_lut Pointer to decoder LUTs
   \param llr Pointer to input LLRs
   \param p_procBuf Pointer to the processing buffers
   \param Z Lifting size
*/
static inline void nrLDPC_llr2CnProcBuf_BG2(t_nrLDPC_lut* p_lut, int8_t* llr, int8_t* cnProcBuf, uint16_t Z)
{
  const uint8_t* lut_numCnInCnGroups = p_lut->numCnInCnGroups;
  const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

  uint32_t bitOffsetInGroup;

  // =====================================================================
  // CN group with  BNs
  int BNs[] = {3, 4, 5, 6, 8, 10};
  for (int bn = 0; bn < sizeofArray(BNs); bn++) {
    bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[bn] * NR_LDPC_ZMAX;

    for (int j = 0; j < BNs[bn]; j++) {
      int8_t* p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[bn] + j * bitOffsetInGroup];
      const uint16_t* lut_circShift = arrPos(p_lut->circShift[bn], j);
      const uint8_t* lut_posBnInCnProcBuf = arrPos(p_lut->posBnInCnProcBuf[bn], j);

      for (int i = 0; i < lut_numCnInCnGroups[bn]; i++) {
        const int idxBn = lut_posBnInCnProcBuf[i] * Z;
        nrLDPC_circ_memcpy(p_cnProcBuf, &llr[idxBn], Z, lut_circShift[i]);
        p_cnProcBuf += Z;
      }
    }
  }
}

/**
   \brief Copies the values in the CN processing results buffer to their corresponding place in the BN processing buffer for BG2.
   \param p_lut Pointer to decoder LUTs
   \param p_procBuf Pointer to the processing buffers
   \param Z Lifting size
*/
static inline void nrLDPC_cn2bnProcBuf_BG2(t_nrLDPC_lut* p_lut, int8_t* cnProcBufRes, int8_t* bnProcBuf, uint16_t Z)
{
    const uint8_t*  lut_numCnInCnGroups = p_lut->numCnInCnGroups;
    const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

    uint32_t bitOffsetInGroup;

    // =====================================================================
    // CN group with 3 BNs
    int BNs[] = {3, 4, 5, 6, 8, 10};
    for (int bn = 0; bn < sizeofArray(BNs); bn++) {
      bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[bn] * NR_LDPC_ZMAX;

      for (int j = 0; j < BNs[bn]; j++) {
        int8_t* p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[bn] + j * bitOffsetInGroup];
        const uint16_t* lut_circShift = arrPos(p_lut->circShift[bn], j);
        const uint32_t* lut_startAddrBnProcBuf = arrPos(p_lut->startAddrBnProcBuf[bn], j);
        const uint8_t* lut_bnPosBnProcBuf = arrPos(p_lut->bnPosBnProcBuf[bn], j);

        for (int i = 0; i < lut_numCnInCnGroups[bn]; i++) {
          const int idxBn = lut_startAddrBnProcBuf[i] + lut_bnPosBnProcBuf[i] * Z;
          nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift[i]);
          p_cnProcBufRes += Z;
        }
      }
    }
}

/**
   \brief Copies the values in the CN processing results buffer to their corresponding place in the BN processing buffer for BG1.
   \param p_lut Pointer to decoder LUTs
   \param p_procBuf Pointer to the processing buffers
   \param Z Lifting size
*/
static inline void nrLDPC_cn2bnProcBuf_BG1(t_nrLDPC_lut* p_lut, int8_t* cnProcBufRes, int8_t* bnProcBuf, uint16_t Z)
{
    const uint8_t*  lut_numCnInCnGroups = p_lut->numCnInCnGroups;
    const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;
    uint32_t bitOffsetInGroup;

    // =====================================================================
    // CN group with 3 BNs

    bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[0]*NR_LDPC_ZMAX;

    for (int j = 0; j < 3; j++) {
      const uint16_t* lut_circShift_CNG3 = arrPos(p_lut->circShift[0], j);
      const uint32_t* lut_startAddrBnProcBuf_CNG3 = arrPos(p_lut->startAddrBnProcBuf[0], j);
      int8_t* p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];
      nrLDPC_inv_circ_memcpy(&bnProcBuf[lut_startAddrBnProcBuf_CNG3[0]], p_cnProcBufRes, Z, lut_circShift_CNG3[0]);
    }

    // =====================================================================
    // CN group with 4 BNs
    // CN group with 3 BNs
    int BNs[] = {3, 4, 5, 6, 7, 8, 9, 10, 19};
    for (int bn = 1; bn < sizeofArray(BNs); bn++) {
      bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[bn] * NR_LDPC_ZMAX;

      for (int j = 0; j < BNs[bn]; j++) {
        int8_t* p_cnProcBufRes = &cnProcBufRes[lut_startAddrCnGroups[bn] + j * bitOffsetInGroup];
        const uint16_t* lut_circShift_CNG = arrPos(p_lut->circShift[bn], j);
        const uint32_t* lut_startAddrBnProcBuf_CNG = arrPos(p_lut->startAddrBnProcBuf[bn], j);
        const uint8_t* lut_bnPosBnProcBuf_CNG = arrPos(p_lut->bnPosBnProcBuf[bn], j);

        for (int i = 0; i < lut_numCnInCnGroups[bn]; i++) {
          const int idxBn = lut_startAddrBnProcBuf_CNG[i] + lut_bnPosBnProcBuf_CNG[i] * Z;
          nrLDPC_inv_circ_memcpy(&bnProcBuf[idxBn], p_cnProcBufRes, Z, lut_circShift_CNG[i]);
          p_cnProcBufRes += Z;
        }
      }
    }
}

/**
   \brief Copies the values in the BN processing results buffer to their corresponding place in the CN processing buffer for BG2.
   \param p_lut Pointer to decoder LUTs
   \param p_procBuf Pointer to the processing buffers
   \param Z Lifting size
*/
static inline void nrLDPC_bn2cnProcBuf_BG2(t_nrLDPC_lut* p_lut, int8_t* bnProcBufRes, int8_t* cnProcBuf, uint16_t Z)
{
    const uint8_t*  lut_numCnInCnGroups = p_lut->numCnInCnGroups;
    const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;

    uint32_t bitOffsetInGroup;

    // For CN groups 3 to 6 no need to send the last BN back since it's single edge
    // and BN processing does not change the value already in the CN proc buf

    // =====================================================================
    // CN group with 3 BNs
    int BNs[] = {2, 3, 4, 5, 8, 10};
    for (int bn = 0; bn < sizeofArray(BNs); bn++) {
      bitOffsetInGroup = lut_numCnInCnGroups_BG2_R15[bn] * NR_LDPC_ZMAX;

      for (int j = 0; j < BNs[bn]; j++) {
        int8_t* p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[bn] + j * bitOffsetInGroup];
        const uint16_t* lut_circShift_CNG = arrPos(p_lut->circShift[bn], j);
        const uint32_t* lut_startAddrBnProcBuf_CNG = arrPos(p_lut->startAddrBnProcBuf[bn], j);
        const uint8_t* lut_bnPosBnProcBuf_CNG = arrPos(p_lut->bnPosBnProcBuf[bn], j);

        for (int i = 0; i < lut_numCnInCnGroups[bn]; i++) {
          const int idxBn = lut_startAddrBnProcBuf_CNG[i] + lut_bnPosBnProcBuf_CNG[i] * Z;
          nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG[i]);
          p_cnProcBuf += Z;
        }
      }
    }
}


/**
   \brief Copies the values in the BN processing results buffer to their corresponding place in the CN processing buffer for BG1.
   \param p_lut Pointer to decoder LUTs
   \param p_procBuf Pointer to the processing buffers
   \param Z Lifting size
*/
static inline void nrLDPC_bn2cnProcBuf_BG1(t_nrLDPC_lut* p_lut, int8_t* bnProcBufRes, int8_t* cnProcBuf, uint16_t Z)
{
    const uint8_t*  lut_numCnInCnGroups = p_lut->numCnInCnGroups;
    const uint32_t* lut_startAddrCnGroups = p_lut->startAddrCnGroups;
    uint32_t bitOffsetInGroup;

    // For CN groups 3 to 19 no need to send the last BN back since it's single edge
    // and BN processing does not change the value already in the CN proc buf

    // =====================================================================
    // CN group with 3 BNs

    bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[0]*NR_LDPC_ZMAX;

    for (int j = 0; j < 2; j++) {
      const uint16_t* lut_circShift_CNG3 = arrPos(p_lut->circShift[0], j);
      const uint32_t* lut_startAddrBnProcBuf_CNG3 = arrPos(p_lut->startAddrBnProcBuf[0], j);
      int8_t* p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[0] + j * bitOffsetInGroup];
      nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[lut_startAddrBnProcBuf_CNG3[0]], Z, lut_circShift_CNG3[0]);
    }

    // =====================================================================
    // CN group with 4 BNs
    int BNs[] = {2, 3, 4, 5, 6, 7, 8, 9, 19};
    for (int bn = 1; bn < sizeofArray(BNs); bn++) {
      bitOffsetInGroup = lut_numCnInCnGroups_BG1_R13[bn] * NR_LDPC_ZMAX;

      for (int j = 0; j < BNs[bn]; j++) {
        int8_t* p_cnProcBuf = &cnProcBuf[lut_startAddrCnGroups[bn] + j * bitOffsetInGroup];
        const uint16_t* lut_circShift_CNG = arrPos(p_lut->circShift[bn], j);
        const uint32_t* lut_startAddrBnProcBuf_CNG = arrPos(p_lut->startAddrBnProcBuf[bn], j);
        const uint8_t* lut_bnPosBnProcBuf_CNG = arrPos(p_lut->bnPosBnProcBuf[bn], j);

        for (int i = 0; i < lut_numCnInCnGroups[bn]; i++) {
          const int idxBn = lut_startAddrBnProcBuf_CNG[i] + lut_bnPosBnProcBuf_CNG[i] * Z;
          nrLDPC_circ_memcpy(p_cnProcBuf, &bnProcBufRes[idxBn], Z, lut_circShift_CNG[i]);
          p_cnProcBuf += Z;
        }
      }
    }
}

/**
   \brief Copies the values in the LLR results buffer to their corresponding place in the output LLR vector.
   \param p_lut Pointer to decoder LUTs
   \param llrOut Pointer to output LLRs
   \param p_procBuf Pointer to the processing buffers
   \param Z Lifting size
   \param BG Base graph
*/
static inline void nrLDPC_llrRes2llrOut(t_nrLDPC_lut* p_lut, int8_t* llrOut, int8_t* llrRes, uint16_t Z, uint8_t BG)
{
  const uint8_t numBn2CnG1 = p_lut->numBnInBnGroups[0];
  uint32_t startColParity = (BG == 1) ? (NR_LDPC_START_COL_PARITY_BG1) : (NR_LDPC_START_COL_PARITY_BG2);

  uint32_t colG1 = startColParity * Z;

  const uint16_t* lut_llr2llrProcBufAddr = p_lut->llr2llrProcBufAddr;
  const uint8_t* lut_llr2llrProcBufBnPos = p_lut->llr2llrProcBufBnPos;

  int8_t* p_llrOut = &llrOut[0];

  // Copy LLRs connected to 1 CN
  if (numBn2CnG1 > 0) {
    memcpy(&llrOut[colG1], llrRes, numBn2CnG1 * Z);
  }

  for (int i = 0; i < startColParity; i++) {
    const int idxBn = lut_llr2llrProcBufAddr[i] + lut_llr2llrProcBufBnPos[i] * Z;
    memcpy(p_llrOut, &llrRes[idxBn], Z);
    p_llrOut += Z;
  }
}

#endif




