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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "nfapi_nr_interface_scf.h"
#include "PHY/TOOLS/tools_defs.h"
#include "SIMULATION/RF/rf.h"
#include "sim.h"

//#define DEBUG_CH
//#define DOPPLER_DEBUG

uint8_t multipath_channel_nosigconv(channel_desc_t *desc)
{
  random_channel(desc,0);
  return(1);
}

//#define CHANNEL_SSE
#ifdef CHANNEL_SSE
void __attribute__ ((no_sanitize_address)) multipath_channel(channel_desc_t *desc,
                       double tx_sig_re[NB_ANTENNAS_TX][30720*2],
                       double tx_sig_im[NB_ANTENANS_TX][30720*2],
                       double rx_sig_re[NB_ANTENNAS_RX][30720*2],
                       double rx_sig_im[NB_ANTENNAS_RX][30720*2],
                       uint32_t length,
                       uint8_t keep_channel,
             		       int log_channel)
{

  int i,ii,j,l;
  int length1, length2, tail;
  __m128d rx_tmp128_re_f,rx_tmp128_im_f,rx_tmp128_re,rx_tmp128_im, rx_tmp128_1,rx_tmp128_2,rx_tmp128_3,rx_tmp128_4,tx128_re,tx128_im,ch128_x,ch128_y,pathloss128;

  double path_loss = pow(10,desc->path_loss_dB/20);
  int dd = abs(desc->channel_offset);

  pathloss128 = _mm_set1_pd(path_loss);

#ifdef DEBUG_CH
  printf("[CHANNEL] keep = %d : path_loss = %g (%f), nb_rx %d, nb_tx %d, dd %d, len %d \n",keep_channel,path_loss,desc->path_loss_dB,desc->nb_rx,desc->nb_tx,dd,desc->channel_length);
#endif

  if (keep_channel) {
    // do nothing - keep channel
  } else {
    random_channel(desc,0);
  }

  start_meas(&desc->convolution);

#ifdef DEBUG_CH

  for (l = 0; l<(int)desc->channel_length; l++) {
    printf("%p (%f,%f) ",desc->ch[0],desc->ch[0][l].x,desc->ch[0][l].y);
  }

  printf("\n");
#endif

  tail = ((int)length-dd)%2;

  if(tail)
    length1 = ((int)length-dd)-1;
  else
    length1 = ((int)length-dd);

  length2 = length1/2;

  for (i=0; i<length2; i++) { //
    for (ii=0; ii<desc->nb_rx; ii++) {
      // rx_tmp.x = 0;
      // rx_tmp.y = 0;
      rx_tmp128_re_f = _mm_setzero_pd();
      rx_tmp128_im_f = _mm_setzero_pd();

      for (j=0; j<desc->nb_tx; j++) {
        for (l = 0; l<(int)desc->channel_length; l++) {
          if ((i>=0) && (i-l)>=0) { //SIMD correct only if length1 > 2*channel_length...which is almost always satisfied
            // tx.x = tx_sig_re[j][i-l];
            // tx.y = tx_sig_im[j][i-l];
            tx128_re = _mm_loadu_pd(&tx_sig_re[j][2*i-l]); // tx_sig_re[j][i-l+1], tx_sig_re[j][i-l]
            tx128_im = _mm_loadu_pd(&tx_sig_im[j][2*i-l]);
          } else {
            //tx.x =0;
            //tx.y =0;
            tx128_re = _mm_setzero_pd();
            tx128_im = _mm_setzero_pd();
          }

          ch128_x = _mm_set1_pd(desc->ch[ii+(j*desc->nb_rx)][l].x);
          ch128_y = _mm_set1_pd(desc->ch[ii+(j*desc->nb_rx)][l].y);
          //  rx_tmp.x += (tx.x * desc->ch[ii+(j*desc->nb_rx)][l].x) - (tx.y * desc->ch[ii+(j*desc->nb_rx)][l].y);
          //  rx_tmp.y += (tx.y * desc->ch[ii+(j*desc->nb_rx)][l].x) + (tx.x * desc->ch[ii+(j*desc->nb_rx)][l].y);
          rx_tmp128_1 = _mm_mul_pd(tx128_re,ch128_x);
          rx_tmp128_2 = _mm_mul_pd(tx128_re,ch128_y);
          rx_tmp128_3 = _mm_mul_pd(tx128_im,ch128_x);
          rx_tmp128_4 = _mm_mul_pd(tx128_im,ch128_y);
          rx_tmp128_re = _mm_sub_pd(rx_tmp128_1,rx_tmp128_4);
          rx_tmp128_im = _mm_add_pd(rx_tmp128_2,rx_tmp128_3);
          rx_tmp128_re_f = _mm_add_pd(rx_tmp128_re_f,rx_tmp128_re);
          rx_tmp128_im_f = _mm_add_pd(rx_tmp128_im_f,rx_tmp128_im);
        } //l
      }  // j

      //rx_sig_re[ii][i+dd] = rx_tmp.x*path_loss;
      //rx_sig_im[ii][i+dd] = rx_tmp.y*path_loss;
      rx_tmp128_re_f = _mm_mul_pd(rx_tmp128_re_f,pathloss128);
      rx_tmp128_im_f = _mm_mul_pd(rx_tmp128_im_f,pathloss128);
      _mm_storeu_pd(&rx_sig_re[ii][2*i+dd],rx_tmp128_re_f); // max index: length-dd -1 + dd = length -1
      _mm_storeu_pd(&rx_sig_im[ii][2*i+dd],rx_tmp128_im_f);
      /*
      if ((ii==0)&&((i%32)==0)) {
      printf("%p %p %f,%f => %e,%e\n",rx_sig_re[ii],rx_sig_im[ii],rx_tmp.x,rx_tmp.y,rx_sig_re[ii][i-dd],rx_sig_im[ii][i-dd]);
      }
      */
      //rx_sig_re[ii][i] = sqrt(.5)*(tx_sig_re[0][i] + tx_sig_re[1][i]);
      //rx_sig_im[ii][i] = sqrt(.5)*(tx_sig_im[0][i] + tx_sig_im[1][i]);

    } // ii
  } // i

  stop_meas(&desc->convolution);

}

#else

void add_noise(c16_t **rxdata,
               const double **r_re,
               const double **r_im,
               const double sigma,
               const int length,
               const int slot_offset,
               const double ts,
               const int delay,
               const uint16_t pdu_bit_map,
               const uint8_t nb_antennas_rx)
{
  for (int i = 0; i < length; i++) {
    for (int ap = 0; ap < nb_antennas_rx; ap++) {
      c16_t *rxd = &rxdata[ap][slot_offset + i + delay];
      rxd->r = r_re[ap][i] + sqrt(sigma / 2) * gaussZiggurat(0.0, 1.0); // convert to fixed point
      rxd->i = r_im[ap][i] + sqrt(sigma / 2) * gaussZiggurat(0.0, 1.0);
      /* Add phase noise if enabled */
      if (pdu_bit_map & PUSCH_PDU_BITMAP_PUSCH_PTRS) {
        phase_noise(ts, &rxdata[ap][slot_offset + i + delay].r, &rxdata[ap][slot_offset + i + delay].i);
      }
    }
  }
}

void __attribute__ ((no_sanitize_address)) multipath_channel(channel_desc_t *desc,
                       double *tx_sig_re[NB_ANTENNAS_TX],
                       double *tx_sig_im[NB_ANTENNAS_TX],
                       double *rx_sig_re[NB_ANTENNAS_RX],
                       double *rx_sig_im[NB_ANTENNAS_RX],
                       uint32_t length,
                       uint8_t keep_channel,
		                   int log_channel)
{

  double path_loss = pow(10,desc->path_loss_dB/20);
  int dd;
  dd = abs(desc->channel_offset);

#ifdef DEBUG_CH
  printf("[CHANNEL] keep = %d : path_loss = %g (%f), nb_rx %d, nb_tx %d, dd %d, len %d \n",
         keep_channel, path_loss, desc->path_loss_dB, desc->nb_rx, desc->nb_tx, dd, desc->channel_length);
#endif

  if (keep_channel) {
    // do nothing - keep channel
  } else {
    random_channel(desc,0);
  }

#ifdef DEBUG_CH
  for (l = 0; l<(int)desc->channel_length; l++) {
    printf("ch[%i] = (%f, %f)\n", l, desc->ch[0][l].r, desc->ch[0][l].i);
  }
#endif

  struct complexd cexp_doppler[length];
  if (desc->max_Doppler != 0.0) {
    get_cexp_doppler(cexp_doppler, desc, length);
  }

  for (int i=0; i<((int)length-dd); i++) {
    for (int ii=0; ii<desc->nb_rx; ii++) {
      struct complexd rx_tmp={0};
      for (int j=0; j<desc->nb_tx; j++) {
        struct complexd *chan=desc->ch[ii+(j*desc->nb_rx)];
        for (int l = 0; l<(int)desc->channel_length; l++) {
          if ((i>=0) && (i-l)>=0) {
            struct complexd tx;
            tx.r = tx_sig_re[j][i-l];
            tx.i = tx_sig_im[j][i-l];
            rx_tmp.r += (tx.r * chan[l].r) - (tx.i * chan[l].i);
            rx_tmp.i += (tx.i * chan[l].r) + (tx.r * chan[l].i);
          }
          #if 0
          if (i==0 && log_channel == 1) {
            printf("channel[%d][%d][%d] = %f dB \t(%e, %e)\n",
                   ii, j, l, 10 * log10(pow(chan[l].r, 2.0) + pow(chan[l].i, 2.0)), chan[l].r, chan[l].i);
	        }
          #endif
        } //l
      }  // j
#if 0
      if (desc->max_Doppler != 0.0)
        rx_tmp = cdMul(rx_tmp, cexp_doppler[i]);
#endif

#ifdef DOPPLER_DEBUG
      printf("[k %2i] cexp_doppler = (%7.4f, %7.4f), abs(cexp_doppler) = %.4f\n",
                   i,
                   cexp_doppler[i].r,
                   cexp_doppler[i].i,
                   sqrt(cexp_doppler[i].r * cexp_doppler[i].r + cexp_doppler[i].i * cexp_doppler[i].i));
#endif

      rx_sig_re[ii][i+dd] = rx_tmp.r*path_loss;
      rx_sig_im[ii][i+dd] = rx_tmp.i*path_loss;
#ifdef DEBUG_CH
      if ((i%32)==0) {
	       printf("rx aa %d: %f, %f  =>  %e, %e\n",
                ii,  rx_tmp.r, rx_tmp.i, rx_sig_re[ii][i-dd], rx_sig_im[ii][i-dd]);
      }	
#endif      
      //rx_sig_re[ii][i] = sqrt(.5)*(tx_sig_re[0][i] + tx_sig_re[1][i]);
      //rx_sig_im[ii][i] = sqrt(.5)*(tx_sig_im[0][i] + tx_sig_im[1][i]);

    } // ii
  } // i
}
#endif


