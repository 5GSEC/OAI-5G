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

/*! \file PHY/LTE_TRANSPORT/lte_mcs.c
* \brief Some support routines for MCS computations
* \author R. Knopp
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr
* \note
* \warning
*/

#include "PHY/defs_common.h"
#include "PHY/phy_extern.h"
#include "PHY/LTE_TRANSPORT/transport_common_proto.h"
#include "openair1/PHY/LTE_TRANSPORT/transport_vars.h"

unsigned char get_Qm(unsigned char I_MCS) {
  if (I_MCS < 10)
    return(2);
  else if (I_MCS < 17)
    return(4);
  else
    return(6);
}

unsigned char get_Qm_ul(unsigned char I_MCS) {
  if (I_MCS < 11)
    return(2);
  else if (I_MCS < 21)
    return(4);
  else
    return(6);
}

unsigned char get_I_TBS(unsigned char I_MCS) {
  if (I_MCS < 10)
    return(I_MCS);
  else if (I_MCS == 10)
    return(9);
  else if (I_MCS < 17)
    return(I_MCS-1);
  else if (I_MCS == 17)
    return(15);
  else return(I_MCS-2);
}

unsigned char get_I_TBS_UL(unsigned char I_MCS) {
  if (I_MCS <= 10)
    return(I_MCS);
  else if (I_MCS == 10)
    return(10);
  else if (I_MCS < 21)
    return(I_MCS-1);
  else return(I_MCS-2);
}

unsigned char I_TBS2I_MCS(unsigned char I_TBS) {
  unsigned char I_MCS = -1;

  ///note: change from <= to < to go from choosing higher modulation rather than high code-rate
  if (I_TBS <= 9)
    I_MCS = I_TBS;
  else if (I_TBS <= 15)
    I_MCS = I_TBS + 1;
  else if (I_TBS > 15 && I_TBS <= 26)
    I_MCS = I_TBS + 2;

#ifdef OUTPUT_DEBUG
  printf("I_MCS: %d, from mod_order %d and I_TBS %d\n",I_MCS,modOrder(I_MCS,I_TBS),I_TBS);

  if (I_MCS == 0xFF) getchar();

#endif
  return I_MCS;
}

uint16_t find_nb_rb_DL(uint8_t mcs, uint32_t bytes, uint16_t nb_rb_max, uint16_t rb_gran) {
  if (bytes == 0 || mcs > 28 || nb_rb_max == 0)
    return 0;
  const uint32_t bits = bytes << 3;
  const unsigned char I_TBS = get_I_TBS(mcs);

  uint32_t TBS = TBStable[I_TBS][nb_rb_max - 1];
  if (bits >= TBS)     // is nb_rb_max too small?
    return nb_rb_max;

  TBS = TBStable[I_TBS][rb_gran - 1];
  if (bits <= TBS)     // is rb_gran RB enough?
    return rb_gran;

  nb_rb_max += nb_rb_max % rb_gran; // round up to full RBG
  uint16_t hi = nb_rb_max/rb_gran - 1;
  uint16_t lo = 0;
  uint16_t p = (hi + lo) / 2;
  for (; lo + 1 != hi; p = (hi + lo) / 2) {
    const uint16_t rbi = (p + 1) * rb_gran - 1;  // convert "RBG" -> RB
    TBS = TBStable[I_TBS][rbi];
    if (bits <= TBS)  // need less RBs
      hi = p;
    else              // need more RBs
      lo = p;
  }
  return (hi + 1) * rb_gran;
}

uint32_t get_TBS_DL(uint8_t mcs, uint16_t nb_rb) {
  uint32_t TBS;

  if ((nb_rb > 0) && (mcs < 29)) {
    TBS = TBStable[get_I_TBS(mcs)][nb_rb-1];
    TBS = TBS>>3;
    return(TBS);
  } else {
    return(uint32_t)0;
  }
}

uint32_t get_TBS_UL(uint8_t mcs, uint16_t nb_rb) {
  uint32_t TBS = 0;

  if ((nb_rb > 0) && (mcs < 29)) {
    TBS = TBStable[get_I_TBS_UL(mcs)][nb_rb-1];
    TBS = TBS>>3;
    return(TBS);
  } else {
    return(uint32_t)0;
  }
}

int adjust_G2(int Ncp, int frame_type, int N_RB_DL, uint32_t *rb_alloc, uint8_t mod_order, uint8_t subframe, uint8_t symbol) {
  int rb,re_pbch_sss=0;
  int rb_alloc_ind,nsymb;
  nsymb = (Ncp==NORMAL) ? 14 : 12;

  //      printf("adjust_G2 : symbol %d, subframe %d\n",symbol,subframe);
  if ((subframe!=0) && (subframe!=5) && (subframe!=6))  // if not PBCH/SSS or SSS
    return(0);

  //first half of slot and TDD (no adjustments in first slot except for subframe 6 - PSS)
  if ((symbol<(nsymb>>1))&&
      (frame_type == TDD)&&
      (subframe!=6))
    return(0);

  // after PBCH
  if (frame_type==TDD) { //TDD
    if ((symbol>((nsymb>>1)+3)) &&
        (symbol!=(nsymb-1)))  ///SSS
      return(0);

    if ((subframe==5) && (symbol!=(nsymb-1))) ///SSS
      return(0);

    if ((subframe==6) && (symbol!=2)) /// PSS
      return(0);
  } else { // FDD
    if ((symbol>((nsymb>>1)+3)) ||
        (symbol<((nsymb>>1)-2)))
      return(0);

    if ((subframe==5) && (symbol!=((nsymb>>1)-1))&& (symbol!=((nsymb>>1)-2)))
      return(0);

    if (subframe==6)
      return(0);
  }

  if ((N_RB_DL&1) == 1) { // ODD N_RB_DL
    for (rb=((N_RB_DL>>1)-3);
         rb<=((N_RB_DL>>1)+3);
         rb++) {
      if (rb < 32)
        rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
      else if (rb < 64)
        rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
      else if (rb < 96)
        rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
      else if (rb < 100)
        rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
      else
        rb_alloc_ind = 0;

      if (rb_alloc_ind==1) {
        if ((rb==(N_RB_DL>>1)-3) ||
            (rb==((N_RB_DL>>1)+3))) {
          re_pbch_sss += 6;
        } else
          re_pbch_sss += 12;
      }
    }
  } else {
    for (rb=((N_RB_DL>>1)-3);
         rb<((N_RB_DL>>1)+3);
         rb++) {
      if (rb < 32)
        rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
      else if (rb < 64)
        rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
      else if (rb < 96)
        rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
      else if (rb < 100)
        rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
      else
        rb_alloc_ind = 0;

      if (rb_alloc_ind==1) {
        re_pbch_sss += 12;
      }
    }
  }

  //  printf("re_pbch_sss %d\n",re_pbch_sss);
  return(re_pbch_sss);
}

int adjust_G(LTE_DL_FRAME_PARMS *frame_parms, uint32_t *rb_alloc, uint8_t mod_order, uint8_t subframe) {
  int rb,re_pbch_sss=0;
  uint8_t rb_alloc_ind;

  if ((subframe!=0) && (subframe!=5) && (subframe!=6))  // if not PBCH/SSS/PSS or SSS/PSS
    return(0);

  if ((frame_parms->N_RB_DL&1) == 1) { // ODD N_RB_DL
    for (rb=((frame_parms->N_RB_DL>>1)-3);
         rb<=((frame_parms->N_RB_DL>>1)+3);
         rb++) {
      if (rb < 32)
        rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
      else if (rb < 64)
        rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
      else if (rb < 96)
        rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
      else if (rb < 100)
        rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
      else
        rb_alloc_ind = 0;

      if (rb_alloc_ind==1) {
        if ((rb==(frame_parms->N_RB_DL>>1)-3) ||
            (rb==((frame_parms->N_RB_DL>>1)+3))) {  //rb taken by PBCH/SSS
          re_pbch_sss += 6;
        } else
          re_pbch_sss += 12;
      }
    }
  } else {
    for (rb=((frame_parms->N_RB_DL>>1)-3);
         rb<((frame_parms->N_RB_DL>>1)+3);
         rb++) {
      if (rb < 32)
        rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
      else if (rb < 64)
        rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
      else if (rb < 96)
        rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
      else if (rb < 100)
        rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
      else
        rb_alloc_ind = 0;

      if (rb_alloc_ind==1) {
        //  printf("Adjust G: rb %d\n",rb);
        re_pbch_sss += 12;
      }
    }
  }

  //    printf("re_pbch_sss %d\n",re_pbch_sss);
  if (subframe==0) {  //PBCH+SSS+PSS
    if (frame_parms->frame_type == TDD) { // TDD
      if (frame_parms->nb_antenna_ports_eNB!=1)
        //2ant so PBCH 3+2/3 symbols, SSS 1 symbol * REs => (14/3)*re_pbch_sss for normal CP,
        // 2+4/3 symbols, SSS 1 symbol => (13/3)*re_pbch_sss for ext. CP
        return((-frame_parms->Ncp+14)*re_pbch_sss * mod_order/3);
      else
        //SISO so PBCH 3+(5/6) symbols, SSS 1 symbol * REs => (29/6)*re_pbch_sss for normal CP,
        // 2+10/6 symbols, SSS 1 symbol => (28/6)*re_pbch_sss for ext. CP
        return((-frame_parms->Ncp+29)*re_pbch_sss * mod_order/6);
    } else { // FDD
      if (frame_parms->nb_antenna_ports_eNB!=1)
        //2ant so PBCH 3+2/3 symbols, PSS+SSS 2 symbols * REs => (17/3)*re_pbch_sss for normal CP,
        // 2+4/3 symbols, PSS+SSS 2 symbols => (16/3)*re_pbch_sss for ext. CP
        return((-frame_parms->Ncp+17)*re_pbch_sss * mod_order/3);
      else
        //SISO so PBCH 3+(5/6) symbols, PSS+SSS 2symbols REs => (35/6)*re_pbch_sss for normal CP,
        // 2+10/6 symbols, SSS+PSS 2 symbols => (34/6)*re_pbch_sss for ext. CP
        return((-frame_parms->Ncp+35)*re_pbch_sss * mod_order/6);
    }
  } else if (subframe == 5) // SSS+PSS for FDD, SSS for TDD
    return(((frame_parms->frame_type==FDD)?2:1)*re_pbch_sss * 1 * mod_order);
  else if ((subframe == 6)&&(frame_parms->frame_type == TDD)) // PSS for TDD
    return(re_pbch_sss * 1 * mod_order);

  return(0);
}

int get_G(LTE_DL_FRAME_PARMS *frame_parms,
          uint16_t nb_rb,
          uint32_t *rb_alloc,
          uint8_t mod_order,
          uint8_t Nl,
          uint8_t num_pdcch_symbols,
          int frame,
          uint8_t subframe,
          uint8_t beamforming_mode)
{
  int G_adj;

  if (is_pmch_subframe(frame,subframe,frame_parms) == 0) {
    G_adj= adjust_G(frame_parms,rb_alloc,mod_order,subframe);

    //printf("get_G subframe %d mod_order %d, nb_rb %d: rb_alloc %x,%x,%x,%x, G_adj %d\n",subframe,mod_order,nb_rb,rb_alloc[3],rb_alloc[2],rb_alloc[1],rb_alloc[0], G_adj);
    if (frame_parms->Ncp==NORMAL) { // normal prefix
      // PDDDPDD PDDDPDD - 13 PDSCH symbols, 10 full, 3 w/ pilots = 10*12 + 3*8
      // PCDDPDD PDDDPDD - 12 PDSCH symbols, 9 full, 3 w/ pilots = 9*12 + 3*8
      // PCCDPDD PDDDPDD - 11 PDSCH symbols, 8 full, 3 w/pilots = 8*12 + 3*8
      if (beamforming_mode==0 && frame_parms->nb_antenna_ports_eNB!=1)
        return((((int)nb_rb * mod_order * ((11-num_pdcch_symbols)*12 + 3*8)) - G_adj)*Nl);
      else if(beamforming_mode==7)
        return(((int)nb_rb * mod_order * ((7-num_pdcch_symbols)*12 + 3*10 + 4*9)) - G_adj);
      else //SISO
        return(((int)nb_rb * mod_order * ((11-num_pdcch_symbols)*12 + 3*10)) - G_adj);
    } else {
      // PDDPDD PDDPDD - 11 PDSCH symbols, 8 full, 3 w/ pilots = 8*12 + 3*8
      // PCDPDD PDDPDD - 10 PDSCH symbols, 7 full, 3 w/ pilots = 7*12 + 3*8
      // PCCPDD PDDPDD - 9 PDSCH symbols, 6 full, 3 w/pilots = 6*12 + 3*8
      if (frame_parms->nb_antenna_ports_eNB!=1)
        return((((int)nb_rb * mod_order * ((9-num_pdcch_symbols)*12 + 3*8)) - G_adj)*Nl);
      else if(beamforming_mode==7)
        return(((int)nb_rb * mod_order * ((5-num_pdcch_symbols)*12 + 3*8 + 4*9)) - G_adj);
      else //SISO
        return(((int)nb_rb * mod_order * ((9-num_pdcch_symbols)*12 + 3*10)) - G_adj);
    }
  } else { // This is an MBSFN subframe
    return((int)frame_parms->N_RB_DL * mod_order * 102);
  }
}

int get_G_khz_1dot25(LTE_DL_FRAME_PARMS *frame_parms,
                     uint16_t nb_rb,
                     uint32_t *rb_alloc,
                     uint8_t mod_order,
                     uint8_t Nl,
                     uint8_t num_pdcch_symbols,
                     int frame,
                     uint8_t subframe,
                     uint8_t beamforming_mode)
{
  //int G_adj;
  /*if (is_pmch_subframe(frame,subframe,frame_parms) == 0) {
    G_adj= adjust_G(frame_parms,rb_alloc,mod_order,subframe);

    //printf("get_G subframe %d mod_order %d, nb_rb %d: rb_alloc %x,%x,%x,%x, G_adj %d\n",subframe,mod_order,nb_rb,rb_alloc[3],rb_alloc[2],rb_alloc[1],rb_alloc[0], G_adj);
    if (frame_parms->Ncp==NORMAL) { // normal prefix
      // PDDDPDD PDDDPDD - 13 PDSCH symbols, 10 full, 3 w/ pilots = 10*12 + 3*8
      // PCDDPDD PDDDPDD - 12 PDSCH symbols, 9 full, 3 w/ pilots = 9*12 + 3*8
      // PCCDPDD PDDDPDD - 11 PDSCH symbols, 8 full, 3 w/pilots = 8*12 + 3*8
      if (beamforming_mode==0 && frame_parms->nb_antenna_ports_eNB!=1)
        return((((int)nb_rb * mod_order * ((11-num_pdcch_symbols)*12 + 3*8)) - G_adj)*Nl);
      else if(beamforming_mode==7)
        return(((int)nb_rb * mod_order * ((7-num_pdcch_symbols)*12 + 3*10 + 4*9)) - G_adj);
      else //SISO
        return(((int)nb_rb * mod_order * ((11-num_pdcch_symbols)*12 + 3*10)) - G_adj);
    } else {
      // PDDPDD PDDPDD - 11 PDSCH symbols, 8 full, 3 w/ pilots = 8*12 + 3*8
      // PCDPDD PDDPDD - 10 PDSCH symbols, 7 full, 3 w/ pilots = 7*12 + 3*8
      // PCCPDD PDDPDD - 9 PDSCH symbols, 6 full, 3 w/pilots = 6*12 + 3*8
      if (frame_parms->nb_antenna_ports_eNB!=1)
        return((((int)nb_rb * mod_order * ((9-num_pdcch_symbols)*12 + 3*8)) - G_adj)*Nl);
      else if(beamforming_mode==7)
        return(((int)nb_rb * mod_order * ((5-num_pdcch_symbols)*12 + 3*8 + 4*9)) - G_adj);
      else //SISO
        return(((int)nb_rb * mod_order * ((9-num_pdcch_symbols)*12 + 3*10)) - G_adj);
    }
  } else { // This is an MBSFN subframe
    return((int)frame_parms->N_RB_DL * mod_order * 102);
  }*/
  return((int)frame_parms->N_RB_DL * mod_order * 120);
}

// following function requires dlsch_tbs_full.h
#include "PHY/LTE_TRANSPORT/dlsch_tbs_full.h"

unsigned char SE2I_TBS(float SE, unsigned char N_PRB, unsigned char symbPerRB) {
  unsigned char I_TBS= -1;
  int throughPutGoal = 0;
  short diffOld = TBStable[0][N_PRB-1] - throughPutGoal; // always positive because of unsigned arithmetic
  short diffNew = diffOld;
  int i = 0;
  throughPutGoal = (int)(((SE*1024)*N_PRB*symbPerRB*12)/1024);
#ifdef OUTPUT_DEBUG
  printf("Throughput goal = %d, from SE = %f\n",throughPutGoal,SE);
#endif
  I_TBS = 0;

  for (i = 0; i<TBStable_rowCnt; i++) {
    diffNew = TBStable[i][N_PRB-1] - throughPutGoal; // always positive because of unsigned arithmetic

    if (diffNew <= diffOld) {
      diffOld = diffNew;
      I_TBS = i;
    } else {
#ifdef OUTPUT_DEBUG
      printf("diff neglected: %d\n",diffNew);
#endif
      break; // no need to continue, strictly increasing function...
    }

#ifdef OUTPUT_DEBUG
    printf("abs(%d - %d) = %d, --> I_TBS = %d\n",TBStable[i][N_PRB-1],throughPutGoal,diffNew,I_TBS);
#endif
  }

  return I_TBS;
}

//added for ALU icic purpose
uint8_t Get_SB_size(uint8_t n_rb_dl) {
  if(n_rb_dl<27)
    return 4;
  else if(n_rb_dl<64)
    return 6;
  else
    return 8;
}
//end ALU's algo
