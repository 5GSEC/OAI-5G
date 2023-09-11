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

#include "tools_defs.h"

// Approximate 10*log10(x) in fixed point : x = 0...(2^32)-1

static const int8_t dB_table[256] = {
    0,  3,  5,  6,  7,  8,  8,  9,  10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15,
    15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18,
    18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24};

static const int16_t dB_table_times10[256] = {
    0,   30,  47,  60,  69,  77,  84,  90,  95,  100, 104, 107, 111, 114, 117, 120, 123, 125, 127, 130, 132, 134, 136, 138,
    139, 141, 143, 144, 146, 147, 149, 150, 151, 153, 154, 155, 156, 157, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
    169, 169, 170, 171, 172, 173, 174, 174, 175, 176, 177, 177, 178, 179, 179, 180, 181, 181, 182, 183, 183, 184, 185, 185,
    186, 186, 187, 188, 188, 189, 189, 190, 190, 191, 191, 192, 192, 193, 193, 194, 194, 195, 195, 196, 196, 197, 197, 198,
    198, 199, 199, 200, 200, 200, 201, 201, 202, 202, 202, 203, 203, 204, 204, 204, 205, 205, 206, 206, 206, 207, 207, 207,
    208, 208, 208, 209, 209, 210, 210, 210, 211, 211, 211, 212, 212, 212, 213, 213, 213, 213, 214, 214, 214, 215, 215, 215,
    216, 216, 216, 217, 217, 217, 217, 218, 218, 218, 219, 219, 219, 219, 220, 220, 220, 220, 221, 221, 221, 222, 222, 222,
    222, 223, 223, 223, 223, 224, 224, 224, 224, 225, 225, 225, 225, 226, 226, 226, 226, 226, 227, 227, 227, 227, 228, 228,
    228, 228, 229, 229, 229, 229, 229, 230, 230, 230, 230, 230, 231, 231, 231, 231, 232, 232, 232, 232, 232, 233, 233, 233,
    233, 233, 234, 234, 234, 234, 234, 235, 235, 235, 235, 235, 235, 236, 236, 236, 236, 236, 237, 237, 237, 237, 237, 238,
    238, 238, 238, 238, 238, 239, 239, 239, 239, 239, 239, 240, 240, 240, 240, 240};

static const uint32_t bit_seach_mask[32] = {0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000,
                                            0x01000000, 0x00800000, 0x00400000, 0x00200000, 0x00100000, 0x00080000, 0x00040000,
                                            0x00020000, 0x00010000, 0x00008000, 0x00004000, 0x00002000, 0x00001000, 0x00000800,
                                            0x00000400, 0x00000200, 0x00000100, 0x00000080, 0x00000040, 0x00000020, 0x00000010,
                                            0x00000008, 0x00000004, 0x00000002, 0x00000001};
static const uint32_t bit_seach_res[32] = {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,
                                           15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0};
static const uint32_t dB_fix_x10_tbl[128] = {
    0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  9,
    10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17,
    18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24,
    24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 30, 30, 30};
static const uint32_t dB_fix_x10_tbl_low[128] = {
    0,   0,   30,  48,  60,  70,  78,  85,  90,  95,  100, 104, 108, 111, 115, 118, 120, 123, 126, 128, 130, 132,
    134, 136, 138, 140, 141, 143, 145, 146, 148, 149, 151, 152, 153, 154, 156, 157, 158, 159, 160, 161, 162, 163,
    164, 165, 166, 167, 168, 169, 170, 171, 172, 172, 173, 174, 175, 176, 176, 177, 178, 179, 179, 180, 181, 181,
    182, 183, 183, 184, 185, 185, 186, 186, 187, 188, 188, 189, 189, 190, 190, 191, 191, 192, 192, 193, 193, 194,
    194, 195, 195, 196, 196, 197, 197, 198, 198, 199, 199, 200, 200, 200, 201, 201, 202, 202, 203, 203, 203, 204,
    204, 205, 205, 205, 206, 206, 206, 207, 207, 208, 208, 208, 209, 209, 209, 210, 210, 210};

/*
int8_t dB_fixed(int x) {

  int i=0,adj=0;
  int8_t log10=0;

  // find MSB
  for (i=31;i>=0;i--) {

    if ((x & (1<<i)) >= 1) {
      log10 = 3*i;
      i=0;
    }

  }

  // look at next 2 MSBs and adjust between 0-2
    if (i>1) {
      adj = (x>>(i-2))&3;
      if (adj == 1)
  log10 += 1;
      else if ((adj == 2) || (adj == 3))
  log10 += 2;
    }
    else
      log10 += (x&1 == 1) ? 1 : 0;

    return(log10);
}
*/

int16_t dB_fixed_x10(uint32_t x) {
  int16_t dB_power = 0;

  //for new algorithm
  uint32_t cnt;
  uint32_t Exponent;
  uint32_t Mantissa;
  uint32_t shift_right;
  uint32_t tbl_resolution = 7;
  uint32_t tbl_addr_mask;

  if (x < 128){ //OAI alogrithm
    dB_power = dB_fix_x10_tbl_low[x];
  }
  else { //new algorithm
    tbl_addr_mask = 0x0000007Fu; // (1 << tbl_resolution) - 1;//i.e. 0x0000007F
    Mantissa = 0;
    Exponent = 0;
    for (cnt = 0; cnt < 32; cnt++) {
      if ((bit_seach_mask[cnt] & x) != 0) {
        Exponent = bit_seach_res[cnt];
        Mantissa = x & (~bit_seach_mask[cnt]);
        break;
      }
    }
    shift_right = Exponent - tbl_resolution;
    Mantissa = (Mantissa >> shift_right) & tbl_addr_mask;
    dB_power = dB_fix_x10_tbl[Mantissa] + Exponent * 30u;
  }
  return dB_power;
}

int16_t dB_fixed_times10(uint32_t x)
{
  int16_t dB_power=0;

  if (x==0) {
    dB_power = 0;
  } else if ( (x&0xff000000) != 0 ) {
    dB_power = dB_table_times10[((x>>24)&255)-1];
    dB_power += 3*dB_table_times10[255];
  } else if ( (x&0x00ff0000) != 0 ) {
    dB_power = dB_table_times10[((x>>16)&255)-1];
    dB_power += 2*dB_table_times10[255];
  } else if ( (x&0x0000ff00) != 0 ) {
    dB_power = dB_table_times10[((x>>8)&255)-1];
    dB_power += dB_table_times10[255];
  } else {
    dB_power = dB_table_times10[(x&255)-1];
  }

  if (dB_power > 900)
    return(900);

  return dB_power;
}


int8_t dB_fixed(uint32_t x)
{
  int8_t dB_power=0;

  if (x==0) {
    dB_power = 0;
  } else if ( (x&0xff000000) != 0 ) {
    dB_power = dB_table[((x>>24)&255)-1];
    dB_power += 3*dB_table[255];
  } else if ( (x&0x00ff0000) != 0 ) {
    dB_power = dB_table[((x>>16)&255)-1];
    dB_power += 2*dB_table[255];
  } else if ( (x&0x0000ff00) != 0 ) {
    dB_power = dB_table[((x>>8)&255)-1];
    dB_power += dB_table[255];
  } else {
    dB_power = dB_table[(x&255)-1];
  }

  if (dB_power > 90)
    return(90);

  return dB_power;
}


uint8_t dB_fixed64(uint64_t x)
{
  if ((x<(((uint64_t)1)<<32))) return(dB_fixed((uint32_t)x));
  else                         return(4*dB_table[255]+dB_fixed((uint32_t)(x>>32)));
}


int8_t dB_fixed2(uint32_t x,
                 uint32_t y)
{

  if ((x>0) && (y>0) )
    if (x>y)
      return(dB_fixed(x/y));
    else
      return(-dB_fixed(y/x));
  else if (y==0)
    return(127);
  else if (x==0)
    return(-128);

  return(0);
}
