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

/* \file config_ue.c
 * \brief common utility functions for NR (gNB and UE)
 * \author R. Knopp,
 * \date 2019
 * \version 0.1
 * \company Eurecom
 * \email: knopp@eurecom.fr
 * \note
 * \warning
 */

#include <stdint.h>
#include "assertions.h"
#include "nr_common.h"

const char *duplex_mode[]={"FDD","TDD"};

int tables_5_3_2[5][12] = {
  {25, 52, 79, 106, 133, 160, 216, 270, -1, -1, -1, -1}, // 15 FR1
  {11, 24, 38, 51, 65, 78, 106, 133, 162, 217, 245, 273},// 30 FR1
  {-1, 11, 18, 24, 31, 38, 51, 65, 79, 107, 121, 135},   // 60 FR1
  {66, 132, 264, -1 , -1, -1, -1, -1, -1, -1, -1, -1},   // 60 FR2
  {32, 66, 132, 264, -1, -1, -1, -1, -1, -1, -1, -1}     // 120FR2
};

int get_supported_band_index(int scs, int band, int n_rbs)
{
  int scs_index = scs;
  if (band > 256)
    scs_index++;
  for (int i = 0; i < 12; i++) {
    if(n_rbs == tables_5_3_2[scs_index][i])
      return i;
  }
  return (-1); // not found
}


// Table 5.2-1 NR operating bands in FR1 & FR2 (3GPP TS 38.101)
// Table 5.4.2.3-1 Applicable NR-ARFCN per operating band in FR1 & FR2 (3GPP TS 38.101)
// Notes:
// - N_OFFs for bands from 80 to 89 and band 95 is referred to UL
// - Frequencies are expressed in KHz
// - col: NR_band ul_min  ul_max  dl_min  dl_max  step  N_OFFs_DL  deltaf_raster
const nr_bandentry_t nr_bandtable[] = {
  {1,   1920000, 1980000, 2110000, 2170000, 20, 422000, 100},
  {2,   1850000, 1910000, 1930000, 1990000, 20, 386000, 100},
  {3,   1710000, 1785000, 1805000, 1880000, 20, 361000, 100},
  {5,    824000,  849000,  869000,  894000, 20, 173800, 100},
  {7,   2500000, 2570000, 2620000, 2690000, 20, 524000, 100},
  {8,    880000,  915000,  925000,  960000, 20, 185000, 100},
  {12,   698000,  716000,  729000,  746000, 20, 145800, 100},
  {14,   788000,  798000,  758000,  768000, 20, 151600, 100},
  {18,   815000,  830000,  860000,  875000, 20, 172000, 100},
  {20,   832000,  862000,  791000,  821000, 20, 158200, 100},
  {25,  1850000, 1915000, 1930000, 1995000, 20, 386000, 100},
  {26,   814000,  849000,  859000,  894000, 20, 171800, 100},
  {28,   703000,  758000,  758000,  813000, 20, 151600, 100},
  {29,      000,     000,  717000,  728000, 20, 143400, 100},
  {30,  2305000, 2315000, 2350000, 2360000, 20, 470000, 100},
  {34,  2010000, 2025000, 2010000, 2025000, 20, 402000, 100},
  {38,  2570000, 2620000, 2570000, 2630000, 20, 514000, 100},
  {39,  1880000, 1920000, 1880000, 1920000, 20, 376000, 100},
  {40,  2300000, 2400000, 2300000, 2400000, 20, 460000, 100},
  {41,  2496000, 2690000, 2496000, 2690000,  3, 499200,  15},
  {41,  2496000, 2690000, 2496000, 2690000,  6, 499200,  30},
  {47,  5855000, 5925000, 5855000, 5925000,  1, 790334,  15},
  {48,  3550000, 3700000, 3550000, 3700000,  1, 636667,  15},
  {48,  3550000, 3700000, 3550000, 3700000,  2, 636668,  30},
  {50,  1432000, 1517000, 1432000, 1517000, 20, 286400, 100},
  {51,  1427000, 1432000, 1427000, 1432000, 20, 285400, 100},
  {53,  2483500, 2495000, 2483500, 2495000, 20, 496700, 100},
  {65,  1920000, 2010000, 2110000, 2200000, 20, 422000, 100},
  {66,  1710000, 1780000, 2110000, 2200000, 20, 422000, 100},
  {70,  1695000, 1710000, 1995000, 2020000, 20, 399000, 100},
  {71,   663000,  698000,  617000,  652000, 20, 123400, 100},
  {74,  1427000, 1470000, 1475000, 1518000, 20, 295000, 100},
  {75,      000,     000, 1432000, 1517000, 20, 286400, 100},
  {76,      000,     000, 1427000, 1432000, 20, 285400, 100},
  {77,  3300000, 4200000, 3300000, 4200000,  1, 620000,  15},
  {77,  3300000, 4200000, 3300000, 4200000,  2, 620000,  30},
  {78,  3300000, 3800000, 3300000, 3800000,  1, 620000,  15},
  {78,  3300000, 3800000, 3300000, 3800000,  2, 620000,  30},
  {79,  4400010, 5000000, 4400010, 5000000,  1, 693334,  15},
  {79,  4400010, 5000000, 4400010, 5000000,  2, 693334,  30},
  {80,  1710000, 1785000,     000,     000, 20, 342000, 100},
  {81,   880000,  915000,     000,     000, 20, 176000, 100},
  {82,   832000,  862000,     000,     000, 20, 166400, 100},
  {83,   703000,  748000,     000,     000, 20, 140600, 100},
  {84,  1920000, 1980000,     000,     000, 20, 384000, 100},
  {86,  1710000, 1785000,     000,     000, 20, 342000, 100},
  {89,   824000,  849000,     000,     000, 20, 342000, 100},
  {90,  2496000, 2690000, 2496000, 2690000,  3, 499200,  15},
  {90,  2496000, 2690000, 2496000, 2690000,  6, 499200,  30},
  {90,  2496000, 2690000, 2496000, 2690000, 20, 499200, 100},
  {91,   832000,  862000, 1427000, 1432000, 20, 285400, 100},
  {92,   832000,  862000, 1432000, 1517000, 20, 286400, 100},
  {93,   880000,  915000, 1427000, 1432000, 20, 285400, 100},
  {94,   880000,  915000, 1432000, 1517000, 20, 286400, 100},
  {95,  2010000, 2025000,     000,     000, 20, 402000, 100},
  {96,  5925000, 7125000, 5925000, 7125000,  1, 795000,  15},
  {257,26500020,29500000,26500020,29500000,  1,2054166,  60},
  {257,26500080,29500000,26500080,29500000,  2,2054167, 120},
  {258,24250080,27500000,24250080,27500000,  1,2016667,  60},
  {258,24250080,27500000,24250080,27500000,  2,2016667, 120},
  {260,37000020,40000000,37000020,40000000,  1,2229166,  60},
  {260,37000080,40000000,37000080,40000000,  2,2229167, 120},
  {261,27500040,28350000,27500040,28350000,  1,2070833,  60},
  {261,27500040,28350000,27500040,28350000,  2,2070833, 120}
};

int get_supported_bw_mhz(frequency_range_t frequency_range, int bw_index)
{
  if (frequency_range == FR1) {
    switch (bw_index) {
      case 0 :
        return 5; // 5MHz
      case 1 :
        return 10;
      case 2 :
        return 15;
      case 3 :
        return 20;
      case 4 :
        return 25;
      case 5 :
        return 30;
      case 6 :
        return 40;
      case 7 :
        return 50;
      case 8 :
        return 60;
      case 9 :
        return 80;
      case 10 :
        return 90;
      case 11 :
        return 100;
      default :
        AssertFatal(false, "Invalid band index for FR1 %d\n", bw_index);
    }
  }
  else {
    switch (bw_index) {
      case 0 :
        return 50; // 50MHz
      case 1 :
        return 100;
      case 2 :
        return 200;
      case 3 :
        return 400;
      default :
        AssertFatal(false, "Invalid band index for FR2 %d\n", bw_index);
    }
  }
}

bool compare_relative_ul_channel_bw(int nr_band, int scs, int nb_ul, frame_type_t frame_type)
{
  // 38.101-1 section 6.2.2
  // Relative channel bandwidth <= 4% for TDD bands and <= 3% for FDD bands
  int index = get_nr_table_idx(nr_band, scs);
  int bw_index = get_supported_band_index(scs, nr_band, nb_ul);
  int band_size_khz = get_supported_bw_mhz(nr_band > 256 ? FR2 : FR1, bw_index) * 1000;
  float limit = frame_type == TDD ? 0.04 : 0.03;
  float rel_bw = (float) (2 * band_size_khz) / (float) (nr_bandtable[index].ul_max + nr_bandtable[index].ul_min);
  return rel_bw <= limit;
}

uint16_t get_band(uint64_t downlink_frequency, int32_t delta_duplex)
{
  const int64_t dl_freq_khz = downlink_frequency / 1000;
  const int32_t  delta_duplex_khz = delta_duplex / 1000;

  uint64_t center_freq_diff_khz = UINT64_MAX; // 2^64
  uint16_t current_band = 0;

  for (int ind = 0; ind < sizeofArray(nr_bandtable); ind++) {

    if (dl_freq_khz < nr_bandtable[ind].dl_min || dl_freq_khz > nr_bandtable[ind].dl_max)
      continue;

    int32_t current_offset_khz = nr_bandtable[ind].ul_min - nr_bandtable[ind].dl_min;

    if (current_offset_khz != delta_duplex_khz)
      continue;

    int64_t center_frequency_khz = (nr_bandtable[ind].dl_max + nr_bandtable[ind].dl_min) / 2;

    if (labs(dl_freq_khz - center_frequency_khz) < center_freq_diff_khz){
      current_band = nr_bandtable[ind].band;
      center_freq_diff_khz = labs(dl_freq_khz - center_frequency_khz);
    }
  }

  printf("DL frequency %"PRIu64": band %d, UL frequency %"PRIu64"\n",
        downlink_frequency, current_band, downlink_frequency+delta_duplex);

  AssertFatal(current_band != 0, "Can't find EUTRA band for frequency %"PRIu64" and duplex_spacing %u\n", downlink_frequency, delta_duplex);

  return current_band;
}

int NRRIV2BW(int locationAndBandwidth,int N_RB) {
  int tmp = locationAndBandwidth/N_RB;
  int tmp2 = locationAndBandwidth%N_RB;
  if (tmp <= ((N_RB>>1)+1) && (tmp+tmp2)<N_RB) return(tmp+1);
  else                      return(N_RB+1-tmp);

}

int NRRIV2PRBOFFSET(int locationAndBandwidth,int N_RB) {
  int tmp = locationAndBandwidth/N_RB;
  int tmp2 = locationAndBandwidth%N_RB;
  if (tmp <= ((N_RB>>1)+1) && (tmp+tmp2)<N_RB) return(tmp2);
  else                      return(N_RB-1-tmp2);
}

/* TS 38.214 ch. 6.1.2.2.2 - Resource allocation type 1 for DL and UL */
int PRBalloc_to_locationandbandwidth0(int NPRB,int RBstart,int BWPsize) {
  AssertFatal(NPRB>0 && (NPRB + RBstart <= BWPsize),"Illegal NPRB/RBstart Configuration (%d,%d) for BWPsize %d\n",NPRB,RBstart,BWPsize);

  if (NPRB <= 1+(BWPsize>>1)) return(BWPsize*(NPRB-1)+RBstart);
  else                        return(BWPsize*(BWPsize+1-NPRB) + (BWPsize-1-RBstart));
}

int PRBalloc_to_locationandbandwidth(int NPRB,int RBstart) {
  return(PRBalloc_to_locationandbandwidth0(NPRB,RBstart,275));
}

int cce_to_reg_interleaving(const int R, int k, int n_shift, const int C, int L, const int N_regs) {

  int f;  // interleaving function
  if(R==0)
    f = k;
  else {
    int c = k/R;
    int r = k % R;
    f = (r * C + c + n_shift) % (N_regs / L);
  }
  return f;
}

void get_coreset_rballoc(uint8_t *FreqDomainResource,int *n_rb,int *rb_offset) {

  uint8_t count=0, start=0, start_set=0;

  uint64_t bitmap = (((uint64_t)FreqDomainResource[0])<<37)|
    (((uint64_t)FreqDomainResource[1])<<29)|
    (((uint64_t)FreqDomainResource[2])<<21)|
    (((uint64_t)FreqDomainResource[3])<<13)|
    (((uint64_t)FreqDomainResource[4])<<5)|
    (((uint64_t)FreqDomainResource[5])>>3);

  for (int i=0; i<45; i++)
    if ((bitmap>>(44-i))&1) {
      count++;
      if (!start_set) {
        start = i;
        start_set = 1;
      }
    }
  *rb_offset = 6*start;
  *n_rb = 6*count;
}

int get_nb_periods_per_frame(uint8_t tdd_period)
{

  int nb_periods_per_frame;
  switch(tdd_period) {
    case 0:
      nb_periods_per_frame = 20; // 10ms/0p5ms
      break;

    case 1:
      nb_periods_per_frame = 16; // 10ms/0p625ms
      break;

    case 2:
      nb_periods_per_frame = 10; // 10ms/1ms
      break;

    case 3:
      nb_periods_per_frame = 8; // 10ms/1p25ms
      break;

    case 4:
      nb_periods_per_frame = 5; // 10ms/2ms
      break;

    case 5:
      nb_periods_per_frame = 4; // 10ms/2p5ms
      break;

    case 6:
      nb_periods_per_frame = 2; // 10ms/5ms
      break;

    case 7:
      nb_periods_per_frame = 1; // 10ms/10ms
      break;

    default:
      AssertFatal(1==0,"Undefined tdd period %d\n", tdd_period);
  }
  return nb_periods_per_frame;
}


int get_first_ul_slot(int nrofDownlinkSlots, int nrofDownlinkSymbols, int nrofUplinkSymbols)
{
  return (nrofDownlinkSlots + (nrofDownlinkSymbols != 0 && nrofUplinkSymbols == 0));
}

int get_dmrs_port(int nl, uint16_t dmrs_ports)
{

  if (dmrs_ports == 0) return 0; // dci 1_0
  int p = -1;
  int found = -1;
  for (int i=0; i<12; i++) { // loop over dmrs ports
    if((dmrs_ports>>i)&0x01) { // check if current bit is 1
      found++;
      if (found == nl) { // found antenna port number corresponding to current layer
        p = i;
        break;
      }
    }
  }
  AssertFatal(p>-1,"No dmrs port corresponding to layer %d found\n",nl);
  return p;
}

frame_type_t get_frame_type(uint16_t current_band, uint8_t scs_index)
{
  frame_type_t current_type;
  int32_t delta_duplex = get_delta_duplex(current_band, scs_index);

  if (delta_duplex == 0)
    current_type = TDD;
  else
    current_type = FDD;

  LOG_I(NR_MAC, "NR band %d, duplex mode %s, duplex spacing = %d KHz\n", current_band, duplex_mode[current_type], delta_duplex);

  return current_type;
}

// Computes the duplex spacing (either positive or negative) in KHz
int32_t get_delta_duplex(int nr_bandP, uint8_t scs_index)
{
  int nr_table_idx = get_nr_table_idx(nr_bandP, scs_index);

  int32_t delta_duplex = (nr_bandtable[nr_table_idx].ul_min - nr_bandtable[nr_table_idx].dl_min);

  LOG_I(NR_MAC, "NR band duplex spacing is %d KHz (nr_bandtable[%d].band = %d)\n", delta_duplex, nr_table_idx, nr_bandtable[nr_table_idx].band);

  return delta_duplex;
}

// Returns the corresponding row index of the NR table
int get_nr_table_idx(int nr_bandP, uint8_t scs_index) {
  int scs_khz = 15 << scs_index;
  int supplementary_bands[] = {29,75,76,80,81,82,83,84,86,89,95};
  for(int j = 0; j < sizeofArray(supplementary_bands); j++){
    if (nr_bandP == supplementary_bands[j])
      AssertFatal(0 == 1, "Band %d is a supplementary band (%d). This is not supported yet.\n", nr_bandP, supplementary_bands[j]);
  }

  int i;
  for (i = 0; i < sizeofArray(nr_bandtable); i++) {
    if ( nr_bandtable[i].band == nr_bandP && nr_bandtable[i].deltaf_raster == scs_khz )
      break;
  }

  if (i == sizeofArray(nr_bandtable)) {
    LOG_I(PHY, "not found same deltaf_raster == scs_khz, use only band and last deltaf_raster \n");
    for(i=sizeofArray(nr_bandtable)-1; i >=0; i--)
       if ( nr_bandtable[i].band == nr_bandP )
         break;
  }

  AssertFatal(i > 0 && i < sizeofArray(nr_bandtable), "band is not existing: %d\n",nr_bandP);
  LOG_D(PHY, "NR band table index %d (Band %d, dl_min %lu, ul_min %lu)\n", i, nr_bandtable[i].band, nr_bandtable[i].dl_min,nr_bandtable[i].ul_min);

  return i;
}

int get_subband_size(int NPRB,int size) {
  // implements table  5.2.1.4-2 from 36.214
  //
  //Bandwidth part (PRBs)	Subband size (PRBs)
  // < 24	                   N/A
  //24 – 72	                   4, 8
  //73 – 144	                   8, 16
  //145 – 275	                  16, 32

  if (NPRB<24) return(1);
  if (NPRB<72) return (size==0 ? 4 : 8);
  if (NPRB<144) return (size==0 ? 8 : 16);
  if (NPRB<275) return (size==0 ? 16 : 32);
  AssertFatal(1==0,"Shouldn't get here, NPRB %d\n",NPRB);
 
}

void get_samplerate_and_bw(int mu,
                           int n_rb,
                           int8_t threequarter_fs,
                           double *sample_rate,
                           unsigned int *samples_per_frame,
                           double *tx_bw,
                           double *rx_bw) {

  if (mu == 0) {
    switch(n_rb) {
    case 270:
      if (threequarter_fs) {
        *sample_rate=92.16e6;
        *samples_per_frame = 921600;
        *tx_bw = 50e6;
        *rx_bw = 50e6;
      } else {
        *sample_rate=61.44e6;
        *samples_per_frame = 614400;
        *tx_bw = 50e6;
        *rx_bw = 50e6;
      }
      break;
    case 216:
      if (threequarter_fs) {
        *sample_rate=46.08e6;
        *samples_per_frame = 460800;
        *tx_bw = 40e6;
        *rx_bw = 40e6;
      }
      else {
        *sample_rate=61.44e6;
        *samples_per_frame = 614400;
        *tx_bw = 40e6;
        *rx_bw = 40e6;
      }
      break;
    case 160: //30 MHz
    case 133: //25 MHz
      if (threequarter_fs) {
        AssertFatal(1==0,"N_RB %d cannot use 3/4 sampling\n",n_rb);
      }
      else {
        *sample_rate=30.72e6;
        *samples_per_frame = 307200;
        *tx_bw = 20e6;
        *rx_bw = 20e6;
      }
      break;
    case 106:
      if (threequarter_fs) {
        *sample_rate=23.04e6;
        *samples_per_frame = 230400;
        *tx_bw = 20e6;
        *rx_bw = 20e6;
      }
      else {
        *sample_rate=30.72e6;
        *samples_per_frame = 307200;
        *tx_bw = 20e6;
        *rx_bw = 20e6;
      }
      break;
    case 52:
      if (threequarter_fs) {
        *sample_rate=11.52e6;
        *samples_per_frame = 115200;
        *tx_bw = 10e6;
        *rx_bw = 10e6;
      }
      else {
        *sample_rate=15.36e6;
        *samples_per_frame = 153600;
        *tx_bw = 10e6;
        *rx_bw = 10e6;
      }
      break;
    case 25:
      if (threequarter_fs) {
        *sample_rate=5.76e6;
        *samples_per_frame = 57600;
        *tx_bw = 5e6;
        *rx_bw = 5e6;
      }
      else {
        *sample_rate=7.68e6;
        *samples_per_frame = 76800;
        *tx_bw = 5e6;
        *rx_bw = 5e6;
      }
      break;
    default:
      AssertFatal(0==1,"N_RB %d not yet supported for numerology %d\n",n_rb,mu);
    }
  } else if (mu == 1) {
    switch(n_rb) {

    case 273:
      if (threequarter_fs) {
        *sample_rate=184.32e6;
        *samples_per_frame = 1843200;
        *tx_bw = 100e6;
        *rx_bw = 100e6;
      } else {
        *sample_rate=122.88e6;
        *samples_per_frame = 1228800;
        *tx_bw = 100e6;
        *rx_bw = 100e6;
      }
      break;
    case 217:
      if (threequarter_fs) {
        *sample_rate=92.16e6;
        *samples_per_frame = 921600;
        *tx_bw = 80e6;
        *rx_bw = 80e6;
      } else {
        *sample_rate=122.88e6;
        *samples_per_frame = 1228800;
        *tx_bw = 80e6;
        *rx_bw = 80e6;
      }
      break;
    case 162 :
      if (threequarter_fs) {
        AssertFatal(1==0,"N_RB %d cannot use 3/4 sampling\n",n_rb);
      }
      else {
        *sample_rate=61.44e6;
        *samples_per_frame = 614400;
        *tx_bw = 60e6;
        *rx_bw = 60e6;
      }

      break;

    case 133 :
      if (threequarter_fs) {
	AssertFatal(1==0,"N_RB %d cannot use 3/4 sampling\n",n_rb);
      }
      else {
        *sample_rate=61.44e6;
        *samples_per_frame = 614400;
        *tx_bw = 50e6;
        *rx_bw = 50e6;
      }

      break;
    case 106:
      if (threequarter_fs) {
        *sample_rate=46.08e6;
        *samples_per_frame = 460800;
        *tx_bw = 40e6;
        *rx_bw = 40e6;
      }
      else {
        *sample_rate=61.44e6;
        *samples_per_frame = 614400;
        *tx_bw = 40e6;
        *rx_bw = 40e6;
      }
     break;
    case 51:
      if (threequarter_fs) {
        *sample_rate=23.04e6;
        *samples_per_frame = 230400;
        *tx_bw = 20e6;
        *rx_bw = 20e6;
      }
      else {
        *sample_rate=30.72e6;
        *samples_per_frame = 307200;
        *tx_bw = 20e6;
        *rx_bw = 20e6;
      }
      break;
    case 24:
      if (threequarter_fs) {
        *sample_rate=11.52e6;
        *samples_per_frame = 115200;
        *tx_bw = 10e6;
        *rx_bw = 10e6;
      }
      else {
        *sample_rate=15.36e6;
        *samples_per_frame = 153600;
        *tx_bw = 10e6;
        *rx_bw = 10e6;
      }
      break;
    default:
      AssertFatal(0==1,"N_RB %d not yet supported for numerology %d\n",n_rb,mu);
    }
  } else if (mu == 3) {
    switch(n_rb) {
      case 132:
      case 128:
        if (threequarter_fs) {
          *sample_rate=184.32e6;
          *samples_per_frame = 1843200;
          *tx_bw = 200e6;
          *rx_bw = 200e6;
        } else {
          *sample_rate = 245.76e6;
          *samples_per_frame = 2457600;
          *tx_bw = 200e6;
          *rx_bw = 200e6;
        }
        break;

      case 66:
      case 64:
        if (threequarter_fs) {
          *sample_rate=92.16e6;
          *samples_per_frame = 921600;
          *tx_bw = 100e6;
          *rx_bw = 100e6;
        } else {
          *sample_rate = 122.88e6;
          *samples_per_frame = 1228800;
          *tx_bw = 100e6;
          *rx_bw = 100e6;
        }
        break;

      case 32:
        if (threequarter_fs) {
          *sample_rate=92.16e6;
          *samples_per_frame = 921600;
          *tx_bw = 50e6;
          *rx_bw = 50e6;
        } else {
          *sample_rate=61.44e6;
          *samples_per_frame = 614400;
          *tx_bw = 50e6;
          *rx_bw = 50e6;
        }
        break;

      default:
        AssertFatal(0==1,"N_RB %d not yet supported for numerology %d\n",n_rb,mu);
    }
  } else {
    AssertFatal(0 == 1,"Numerology %d not supported for the moment\n",mu);
  }
}

void get_K1_K2(int N1, int N2, int *K1, int *K2)
{
  // num of allowed k1 and k2 according to 5.2.2.2.1-3 and -4 in 38.214
  if(N2 == N1 || N1 == 2)
    *K1 = 2;
  else if (N2 == 1)
    *K1 = 5;
  else
    *K1 = 3;
  *K2 = N2 > 1 ? 2 : 1;
}

// from start symbol index and nb or symbols to symbol occupation bitmap in a slot
uint16_t SL_to_bitmap(int startSymbolIndex, int nrOfSymbols) {
 return ((1<<nrOfSymbols)-1)<<startSymbolIndex;
}

int get_SLIV(uint8_t S, uint8_t L) {
  return ( (uint16_t)(((L-1)<=7)? (14*(L-1)+S) : (14*(15-L)+(13-S))) );
}

void SLIV2SL(int SLIV,int *S,int *L) {

  int SLIVdiv14 = SLIV/14;
  int SLIVmod14 = SLIV%14;
  // Either SLIV = 14*(L-1) + S, or SLIV = 14*(14-L+1) + (14-1-S). Condition is 0 <= L <= 14-S
  if ((SLIVdiv14 + 1) >= 0 && (SLIVdiv14 <= 13-SLIVmod14)) {
    *L=SLIVdiv14+1;
    *S=SLIVmod14;
  } else  {
    *L=15-SLIVdiv14;
    *S=13-SLIVmod14;
  }
}

int get_ssb_subcarrier_offset(uint32_t absoluteFrequencySSB, uint32_t absoluteFrequencyPointA)
{
  uint32_t absolute_diff = (absoluteFrequencySSB - absoluteFrequencyPointA);
  const int scaling_5khz = absoluteFrequencyPointA < 600000 ? 3 : 1;
  return ((absolute_diff / scaling_5khz) % 24);
}

uint32_t get_ssb_offset_to_pointA(uint32_t absoluteFrequencySSB,
                                  uint32_t absoluteFrequencyPointA,
                                  int ssbSubcarrierSpacing,
                                  int frequency_range)
{
  uint32_t absolute_diff = (absoluteFrequencySSB - absoluteFrequencyPointA);
  const int scaling_5khz = absoluteFrequencyPointA < 600000 ? 3 : 1;
  int sco = get_ssb_subcarrier_offset(absoluteFrequencySSB, absoluteFrequencyPointA);
  const int scs_scaling = frequency_range == FR2 ? 1 << (ssbSubcarrierSpacing - 2) : 1 << ssbSubcarrierSpacing;
  const int scaled_abs_diff = absolute_diff / scaling_5khz;
  const int ssb_offset_point_a =
      (scaled_abs_diff - sco) / 12
      - 10 * scs_scaling; // absoluteFrequencySSB is the central frequency of SSB which is made by 20RBs in total
  AssertFatal(ssb_offset_point_a % scs_scaling == 0, "PRB offset %d can create frequency offset\n", ssb_offset_point_a);
  AssertFatal(sco % scs_scaling == 0, "ssb offset %d can create frequency offset\n", sco);
  return ssb_offset_point_a;
}
