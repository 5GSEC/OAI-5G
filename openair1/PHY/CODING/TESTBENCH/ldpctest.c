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

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "assertions.h"
#include "SIMULATION/TOOLS/sim.h"
#include "common/utils/load_module_shlib.h"
#include "PHY/CODING/nrLDPC_extern.h"
//#include "openair1/SIMULATION/NR_PHY/nr_unitary_defs.h"
#include "openair1/PHY/CODING/nrLDPC_decoder_LYC/nrLDPC_decoder_LYC.h"
#include "openair1/PHY/defs_nr_common.h"
#include "coding_unitary_defs.h"
#include "common/utils/LOG/log.h"

#define MAX_BLOCK_LENGTH 8448

#ifndef malloc16
#    define malloc16(x) memalign(32,x)
#endif

#define NR_LDPC_PROFILER_DETAIL
#define NR_LDPC_ENABLE_PARITY_CHECK

// 4-bit quantizer
char quantize4bit(double D,double x)
{
  double qxd;
  qxd = floor(x/D);
  //  printf("x=%f,qxd=%f\n",x,qxd);

  if (qxd <= -8)
    qxd = -8;
  else if (qxd > 7)
    qxd = 7;

  return((char)qxd);
}

char quantize8bit(double D,double x)
{
  double qxd;
  //char maxlev;
  qxd = floor(x/D);

  //maxlev = 1<<(B-1);

  //printf("x=%f,qxd=%f,maxlev=%d\n",x,qxd, maxlev);

  if (qxd <= -128)
    qxd = -128;
  else if (qxd >= 128)
    qxd = 127;

  return((char)qxd);
}

typedef struct {
  double n_iter_mean[400];
  double n_iter_std[400];
  int n_iter_max[400];
  double snr[400];
  double ber[400];
  double bler[400];
} n_iter_stats_t;

nrLDPC_encoderfunc_t encoder_orig;

short lift_size[51]= {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,20,22,24,26,28,30,32,36,40,44,48,52,56,60,64,72,80,88,96,104,112,120,128,144,160,176,192,208,224,240,256,288,320,352,384};

int test_ldpc(short max_iterations,
              int nom_rate,
              int denom_rate,
              double SNR,
              unsigned char qbits,
              short block_length,
              unsigned int ntrials,
              int n_segments,
              unsigned int *errors,
              unsigned int *errors_bit,
              double *errors_bit_uncoded,
              unsigned int *crc_misses,
              time_stats_t *time_optim,
              time_stats_t *time_decoder,
              n_iter_stats_t *dec_iter,
              int nsnr)
{
  //clock initiate
  //time_stats_t time,time_optim,tinput,tprep,tparity,toutput, time_decoder;
  time_stats_t time, tinput,tprep,tparity,toutput;
  double n_iter_mean = 0;
  double n_iter_std = 0;
  int n_iter_max = 0;
  unsigned int segment_bler = 0;

  double sigma;
  sigma = 1.0/sqrt(2*SNR);
  opp_enabled=1;
  //short test_input[block_length];
  unsigned char *test_input[MAX_NUM_NR_DLSCH_SEGMENTS_PER_LAYER*NR_MAX_NB_LAYERS]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};;
  //short *c; //padded codeword
  unsigned char estimated_output[MAX_NUM_DLSCH_SEGMENTS][block_length];
  memset(estimated_output, 0, sizeof(estimated_output));
  unsigned char *channel_input[MAX_NUM_DLSCH_SEGMENTS];
  unsigned char *channel_input_optim[MAX_NUM_DLSCH_SEGMENTS];
  //double channel_output[68 * 384];
  double modulated_input[MAX_NUM_DLSCH_SEGMENTS][68 * 384] = { 0 };
  char channel_output_fixed[MAX_NUM_DLSCH_SEGMENTS][68  * 384] = { 0 };
  short BG=0,nrows=0;//,ncols;
  int no_punctured_columns,removed_bit;
  int i1,Zc,Kb=0;
  int R_ind = 0;
  //Table of possible lifting sizes
  //short lift_size[51]= {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,20,22,24,26,28,30,32,36,40,44,48,52,56,60,64,72,80,88,96,104,112,120,128,144,160,176,192,208,224,240,256,288,320,352,384};
  //int n_segments=1;
  int code_rate_vec[8] = {15, 13, 25, 12, 23, 34, 56, 89};
  //double code_rate_actual_vec[8] = {0.2, 0.33333, 0.4, 0.5, 0.66667, 0.73333, 0.81481, 0.88};

  t_nrLDPC_dec_params decParams[MAX_NUM_DLSCH_SEGMENTS]={0};

  t_nrLDPC_time_stats decoder_profiler = {0};

  int32_t n_iter = 0;

  *errors=0;
  *errors_bit=0;
  *errors_bit_uncoded=0;
  *crc_misses=0;

  // generate input block
  for(int j=0;j<MAX_NUM_DLSCH_SEGMENTS;j++) {
    test_input[j]=(unsigned char *)malloc16(sizeof(unsigned char) * block_length/8);
    memset(test_input[j], 0, sizeof(unsigned char) * block_length / 8);
    channel_input[j] = (unsigned char *)malloc16(sizeof(unsigned char) * 68*384);
    memset(channel_input[j], 0, sizeof(unsigned char) * 68 * 384);
    channel_input_optim[j] = (unsigned char *)malloc16(sizeof(unsigned char) * 68*384);
    memset(channel_input_optim[j], 0, sizeof(unsigned char) * 68 * 384);
  }

  reset_meas(&time);
  reset_meas(time_optim);
  reset_meas(time_decoder);
  reset_meas(&tinput);
  reset_meas(&tprep);
  reset_meas(&tparity);
  reset_meas(&toutput);
  //Reset Decoder profiles
  reset_meas(&decoder_profiler.llr2llrProcBuf);
  reset_meas(&decoder_profiler.llr2CnProcBuf);
  reset_meas(&decoder_profiler.cnProc);
  reset_meas(&decoder_profiler.cnProcPc);
  reset_meas(&decoder_profiler.bnProc);
  reset_meas(&decoder_profiler.bnProcPc);
  reset_meas(&decoder_profiler.cn2bnProcBuf);
  reset_meas(&decoder_profiler.bn2cnProcBuf);
  reset_meas(&decoder_profiler.llrRes2llrOut);
  reset_meas(&decoder_profiler.llr2bit);
  //reset_meas(&decoder_profiler.total);

  for (int j=0;j<MAX_NUM_DLSCH_SEGMENTS;j++) {
    for (int i=0; i<block_length/8; i++) {
      test_input[j][i]=(unsigned char) rand();
      //test_input[j][i]=j%256;
      //test_input[j][i]=252;
    }
  }

  //determine number of bits in codeword
  if (block_length>3840)
  {
    BG = 1;
    Kb = 22;
    nrows = 46; //parity check bits
    //ncols=22; //info bits
  }
  else if (block_length<=3840)
  {
    BG = 2;
    nrows = 42; //parity check bits
    //ncols=10; // info bits

    if (block_length>640)
      Kb = 10;
    else if (block_length>560)
      Kb = 9;
    else if (block_length>192)
      Kb = 8;
    else
      Kb = 6;
  }

  if (nom_rate == 1)
	  if (denom_rate == 5)
		  if (BG == 2)
			  R_ind = 0;
		  else
			  printf("Not supported");
	  else if (denom_rate == 3)
		  R_ind = 1;
	  else if (denom_rate == 2)
		  //R_ind = 3;
  	  	  printf("Not supported");
	  else
		  printf("Not supported");

  else if (nom_rate == 2)
	  if (denom_rate == 5)
		  //R_ind = 2;
  	  	  printf("Not supported");
	  else if (denom_rate == 3)
		  R_ind = 4;
	  else
		  printf("Not supported");

  else if ((nom_rate == 22) && (denom_rate == 30))
		  //R_ind = 5;
  	  	  printf("Not supported");
  else if ((nom_rate == 22) && (denom_rate == 27))
		  //R_ind = 6;
  	  	  printf("Not supported");
  else if ((nom_rate == 22) && (denom_rate == 25))
	  if (BG == 1)
		  R_ind = 7;
	  else
		  printf("Not supported");
  else
	  printf("Not supported");

  //find minimum value in all sets of lifting size
  Zc=0;

  for (i1=0; i1 < 51; i1++)
  {
    if (lift_size[i1] >= (double) block_length/Kb)
    {
      Zc = lift_size[i1];
      //printf("%d\n",Zc);
      break;
    }
  }

  printf("ldpc_test: codeword_length %d, n_segments %d, block_length %d, BG %d, Zc %d, Kb %d\n",n_segments *block_length, n_segments, block_length, BG, Zc, Kb);
  no_punctured_columns=(int)((nrows-2)*Zc+block_length-block_length*(1/((float)nom_rate/(float)denom_rate)))/Zc;
  //  printf("puncture:%d\n",no_punctured_columns);
  removed_bit=(nrows-no_punctured_columns-2) * Zc+block_length-(int)(block_length/((float)nom_rate/(float)denom_rate));
  encoder_implemparams_t impp=INIT0_LDPCIMPLEMPARAMS;

  impp.gen_code=1;
  if (ntrials==0)
    encoder_orig(test_input,channel_input, Zc, BG, block_length, BG, &impp);
  impp.gen_code=0;
  decode_abort_t dec_abort;
  init_abort(&dec_abort);
  for (int trial=0; trial < ntrials; trial++)
  {
	segment_bler = 0;
    //// encoder
    start_meas(&time);
    for(int j=0;j<n_segments;j++) {
      encoder_orig(&(test_input[j]), &(channel_input[j]),Zc,Kb,block_length,BG,&impp);
    }
    stop_meas(&time);

/*    start_meas(time_optim);
    ldpc_encoder_optim_8seg(test_input,channel_input_optim,Zc,Kb,block_length,BG,n_segments,&tinput,&tprep,&tparity,&toutput);
    for(j=0;j<n_segments;j++) {
      ldpc_encoder_optim(test_input[j],channel_input_optim[j],Zc,Kb,block_length,BG,&tinput,&tprep,&tparity,&toutput);
      }
    stop_meas(time_optim);*/
    impp.n_segments=n_segments;
    for(int j=0;j<(n_segments/8+1);j++) {
    	start_meas(time_optim);
    	impp.macro_num=j;
    	nrLDPC_encoder(test_input,channel_input_optim,Zc,Kb,block_length, BG, &impp);
    	stop_meas(time_optim);
    }
    
    if (ntrials==1)    
      for (int j=0;j<n_segments;j++)
        for (int i = 0; i < block_length+(nrows-no_punctured_columns) * Zc - removed_bit; i++)
          if (channel_input[j][i]!=channel_input_optim[j][i]) {
            printf("differ in seg %d pos %d (%u,%u)\n", j, i, channel_input[j][i], channel_input_optim[j][i]);
            return (-1);
          }
      //else{
           // printf("NOT differ in seg %d pos %d (%d,%d)\n",j,i,channel_input[j][i],channel_input_optim[j][i]);
     // }
    if (trial== 0) {
      printf("nrows: %d\n", nrows);
      printf("no_punctured_columns: %d\n", no_punctured_columns);
      printf("removed_bit: %d\n", removed_bit);
      printf("To: %d\n", (Kb+nrows-no_punctured_columns) * Zc-removed_bit);
      printf("number of undecoded bits: %d\n", (Kb+nrows-no_punctured_columns-2) * Zc-removed_bit);
    }

    if (1) { // Transmitting one segment 
      for(int j=0;j<n_segments;j++) {
        for (int i = 2*Zc; i < (Kb+nrows-no_punctured_columns) * Zc-removed_bit; i++) {
#ifdef DEBUG_CODER
          if ((i&0xf)==0)
            printf("\ne %d..%d:    ",i,i+15);
#endif

          if (channel_input_optim[j][i-2*Zc]==0)
            modulated_input[j][i]=1.0;///sqrt(2);  //QPSK
          else
            modulated_input[j][i]=-1.0;///sqrt(2);

          ///channel_output[i] = modulated_input[i] + gaussdouble(0.0,1.0) * 1/sqrt(2*SNR);
          //channel_output_fixed[i] = (char) ((channel_output[i]*128)<0?(channel_output[i]*128-0.5):(channel_output[i]*128+0.5)); //fixed point 9-7
          //printf("llr[%d]=%d\n",i,channel_output_fixed[i]);

          //channel_output_fixed[i] = (char)quantize(sigma/4.0,(2.0*modulated_input[i]) - 1.0 + sigma*gaussdouble(0.0,1.0),qbits);
          channel_output_fixed[j][i] = (char)quantize(sigma/4.0/4.0,modulated_input[j][i] + sigma*gaussdouble(0.0,1.0),qbits);
          //channel_output_fixed[i] = (char)quantize8bit(sigma/4.0,(2.0*modulated_input[i]) - 1.0 + sigma*gaussdouble(0.0,1.0));
          //printf("llr[%d]=%d\n",i,channel_output_fixed[i]);
          //printf("channel_output_fixed[%d]: %d\n",i,channel_output_fixed[i]);


          //Uncoded BER
          unsigned char channel_output_uncoded = channel_output_fixed[j][i]<0 ? 1 /* QPSK demod */ : 0;

          if (channel_output_uncoded != channel_input_optim[j][i-2*Zc])
      *errors_bit_uncoded = (*errors_bit_uncoded) + 1;

        }
     
#ifdef DEBUG_CODER
        printf("\n");
        exit(-1);
#endif

        decParams[j].BG=BG;
        decParams[j].Z=Zc;
        decParams[j].R=code_rate_vec[R_ind];//13;
        decParams[j].numMaxIter=max_iterations;
        decParams[j].outMode = nrLDPC_outMode_BIT;
        decParams[j].block_length=block_length;
        nrLDPC_initcall(&decParams[j], (int8_t*)channel_output_fixed[j], (int8_t*)estimated_output[j]);
      }
      for(int j=0;j<n_segments;j++) {
        start_meas(time_decoder);
        set_abort(&dec_abort, false);
        n_iter = nrLDPC_decoder(&decParams[j],
                                (int8_t *)channel_output_fixed[j],
                                (int8_t *)estimated_output[j],
                                &decoder_profiler,
                                &dec_abort);
        stop_meas(time_decoder);
        //count errors
        if ( memcmp(estimated_output[j], test_input[j], block_length/8 ) != 0 ) {
          segment_bler++;
        }
        for (int i=0; i<block_length; i++)
        {
          unsigned char estoutputbit = (estimated_output[j][i/8]&(1<<(i&7)))>>(i&7);
          unsigned char inputbit = (test_input[j][i/8]&(1<<(i&7)))>>(i&7); // Further correct for multiple segments
          if (estoutputbit != inputbit)
            *errors_bit = (*errors_bit) + 1;
        }

        n_iter_mean += n_iter;
        n_iter_std +=  pow(n_iter-1,2);

        if ( n_iter > n_iter_max )
          n_iter_max = n_iter;

      } // end segments

      if (segment_bler != 0)
        *errors = (*errors) + 1;

    }
  }


  dec_iter->n_iter_mean[nsnr] = n_iter_mean/(double)ntrials/(double)n_segments - 1;
  dec_iter->n_iter_std[nsnr] = sqrt(n_iter_std/(double)ntrials/(double)n_segments - pow(n_iter_mean/(double)ntrials/(double)n_segments - 1,2));
  dec_iter->n_iter_max[nsnr] = n_iter_max -1;

  *errors_bit_uncoded = *errors_bit_uncoded / (double)((Kb+nrows-no_punctured_columns-2) * Zc-removed_bit);

  for(int j=0;j<MAX_NUM_DLSCH_SEGMENTS;j++) {
    free(test_input[j]);
    free(channel_input[j]);
    free(channel_input_optim[j]);
  }

  print_meas(&time,"ldpc_encoder",NULL,NULL);
  print_meas(time_optim,"ldpc_encoder_optim",NULL,NULL);
  print_meas(&tinput,"ldpc_encoder_optim(input)",NULL,NULL);
  print_meas(&tprep,"ldpc_encoder_optim(prep)",NULL,NULL);
  print_meas(&tparity,"ldpc_encoder_optim(parity)",NULL,NULL);
  print_meas(&toutput,"ldpc_encoder_optim(output)",NULL,NULL);
  printf("\n");
  print_meas(time_decoder,"ldpc_decoder",NULL,NULL);
  print_meas(&decoder_profiler.llr2llrProcBuf,"llr2llrProcBuf",NULL,NULL);
  print_meas(&decoder_profiler.llr2CnProcBuf,"llr2CnProcBuf",NULL,NULL);
  print_meas(&decoder_profiler.cnProc,"cnProc (per iteration)",NULL,NULL);
  print_meas(&decoder_profiler.cnProcPc,"cnProcPc (per iteration)",NULL,NULL);
  print_meas(&decoder_profiler.bnProc,"bnProc (per iteration)",NULL,NULL);
  print_meas(&decoder_profiler.bnProcPc,"bnProcPc(per iteration)",NULL,NULL);
  print_meas(&decoder_profiler.cn2bnProcBuf,"cn2bnProcBuf (per iteration)",NULL,NULL);
  print_meas(&decoder_profiler.bn2cnProcBuf,"bn2cnProcBuf (per iteration)",NULL,NULL);
  print_meas(&decoder_profiler.llrRes2llrOut,"llrRes2llrOut",NULL,NULL);
  print_meas(&decoder_profiler.llr2bit,"llr2bit",NULL,NULL);
  printf("\n");

  return *errors;
}

int main(int argc, char *argv[])
{

  unsigned int errors, errors_bit, crc_misses;
  double errors_bit_uncoded;
  short block_length=8448; // decoder supports length: 1201 -> 1280, 2401 -> 2560
  // default to check output inside ldpc, the NR version checks the outer CRC defined by 3GPP
  char *ldpc_version = "_parityCheck";
  /* version of the ldpc decoder library to use (XXX suffix to use when loading libldpc_XXX.so */
  short max_iterations=5;
  int n_segments=1;
  //double rate=0.333;
  
  int nom_rate=1;
  int denom_rate=3;
  double SNR0=-2.0,SNR,SNR_lin;
  unsigned char qbits=8;
  unsigned int decoded_errors[10000]; // initiate the size of matrix equivalent to size of SNR
  int c,i=0, i1 = 0;

  int n_trials = 1;
  double SNR_step = 0.1;

  randominit(0);
  int test_uncoded= 0;

  time_stats_t time_optim[10], time_decoder[10];
  n_iter_stats_t dec_iter;

  short BG=0,Zc,Kb=0;

  while ((c = getopt (argc, argv, "q:r:s:S:l:G:n:d:i:t:u:hv:")) != -1)
    switch (c)
    {
      case 'q':
        qbits = atoi(optarg);
        break;

      case 'r':
        nom_rate = atoi(optarg);
        break;

      case 'd':
        denom_rate = atoi(optarg);
        break;

      case 'l':
        block_length = atoi(optarg);
        break;
		
      case 'G':
        ldpc_version="_cuda";
        break;

      case 'n':
        n_trials = atoi(optarg);
        break;

      case 's':
        SNR0 = atof(optarg);
        break;

      case 'S':
        n_segments = atof(optarg);
        break;

      case 't':
        SNR_step = atof(optarg);
        break;

      case 'i':
        max_iterations = atoi(optarg);
        break;

      case 'u':
        test_uncoded = atoi(optarg);
        break;
      case 'v':
          ldpc_version=strdup(optarg);
        break;
      case 'h':
      default:
              printf("CURRENTLY SUPPORTED CODE RATES: \n");
              printf("BG1 (blocklength > 3840): 1/3, 2/3, 22/25 (8/9) \n");
              printf("BG2 (blocklength <= 3840): 1/5, 1/3, 2/3 \n\n");
              printf("-h This message\n");
              printf("-q Quantization bits, Default: 8\n");
              printf("-r Nominator rate, (1, 2, 22), Default: 1\n");
              printf("-d Denominator rate, (3, 5, 25), Default: 1\n");
              printf("-l Block length (l > 3840 -> BG1, rest BG2 ), Default: 8448\n");
			  printf("-G give 1 to run cuda for LDPC, Default: 0\n");
              printf("-n Number of simulation trials, Default: 1\n");
              //printf("-M MCS2 for TB 2\n");
              printf("-s SNR per information bit (EbNo) in dB, Default: -2\n");
              printf("-S Number of segments (Maximum: 8), Default: 1\n");
              printf("-t SNR simulation step, Default: 0.1\n");
              printf("-i Max decoder iterations, Default: 5\n");
              printf("-u Set SNR per coded bit, Default: 0\n");
              printf("-v XXX Set ldpc shared library version. libldpc_XXX.so will be used \n");
              exit(1);
              break;
    }
  //printf("the decoder supports BG2, Kb=10, Z=128 & 256\n");
  //printf(" range of blocklength: 1201 -> 1280, 2401 -> 2560\n");
  printf("block length %d: \n", block_length);
  printf("n_trials %d: \n", n_trials);
  printf("SNR0 %f: \n", SNR0);

  load_nrLDPClib(ldpc_version);
  load_nrLDPClib_ref("_orig", &encoder_orig);
  //for (block_length=8;block_length<=MAX_BLOCK_LENGTH;block_length+=8)


  //determine number of bits in codeword
  if (block_length>3840)
  {
    BG = 1;
    Kb = 22;
    //nrows=46; //parity check bits
    //ncols=22; //info bits
  }
  else if (block_length<=3840)
  {
    BG = 2;
    //nrows=42; //parity check bits
    //ncols=10; // info bits

    if (block_length>640)
      Kb = 10;
    else if (block_length>560)
      Kb = 9;
    else if (block_length>192)
      Kb = 8;
    else
      Kb = 6;
      }

  //find minimum value in all sets of lifting size
  Zc=0;

  for (i1=0; i1 < 51; i1++)
  {
    if (lift_size[i1] >= (double) block_length/Kb)
    {
      Zc = lift_size[i1];
      //printf("%d\n",Zc);
      break;
    }
  }

  char fname[200];
  sprintf(fname,"ldpctest_BG_%d_Zc_%d_rate_%d-%d_block_length_%d_maxit_%d.txt",BG,Zc,nom_rate,denom_rate,block_length, max_iterations);
  FILE *fd=fopen(fname,"w");
  AssertFatal(fd!=NULL,"cannot open %s\n",fname);

  fprintf(fd,"SNR BLER BER UNCODED_BER ENCODER_MEAN ENCODER_STD ENCODER_MAX DECODER_TIME_MEAN DECODER_TIME_STD DECODER_TIME_MAX DECODER_ITER_MEAN DECODER_ITER_STD DECODER_ITER_MAX\n");

  for (SNR=SNR0;SNR<SNR0+20.0;SNR+=SNR_step) {
	  //reset_meas(&time_optim);
	  //reset_meas(&time_decoder);
	  //n_iter_stats_t dec_iter = {0, 0, 0};
    if (test_uncoded == 1)
    	SNR_lin = pow(10,SNR/10.0);
    else
    	SNR_lin = pow(10,SNR/10.0)*nom_rate/denom_rate;
    printf("Linear SNR: %f\n", SNR_lin);
    decoded_errors[i]=test_ldpc(max_iterations,
                                nom_rate,
                                denom_rate,
                                SNR_lin,   // noise standard deviation
                                qbits,
                                block_length,   // block length bytes
                                n_trials,
                                n_segments,
                                &errors,
                                &errors_bit,
                                &errors_bit_uncoded,
                                &crc_misses,
                                time_optim,
                                time_decoder,
                                &dec_iter,
                                i);

    dec_iter.snr[i] = SNR;
    dec_iter.ber[i] = (float)errors_bit/(float)n_trials/(float)block_length/(double)n_segments;
    dec_iter.bler[i] = (float)decoded_errors[i]/(float)n_trials;
    printf("SNR %f, BLER %f (%u/%d)\n", SNR, dec_iter.bler[i], decoded_errors[i], n_trials);
    printf("SNR %f, BER %f (%u/%d)\n", SNR, dec_iter.ber[i], decoded_errors[i], n_trials);
    printf("SNR %f, Uncoded BER %f (%u/%d)\n",SNR, errors_bit_uncoded/(float)n_trials/(double)n_segments, decoded_errors[i], n_trials);
    printf("SNR %f, Mean iterations: %f\n",SNR, dec_iter.n_iter_mean[i]);
    printf("SNR %f, Std iterations: %f\n",SNR, dec_iter.n_iter_std[i]);
    printf("SNR %f, Max iterations: %d\n",SNR, dec_iter.n_iter_max[i]);
    printf("\n");
    printf("Encoding time mean: %15.3f us\n",(double)time_optim->diff/time_optim->trials/1000.0/get_cpu_freq_GHz());
    printf("Encoding time std: %15.3f us\n",sqrt((double)time_optim->diff_square/time_optim->trials/pow(1000,2)/pow(get_cpu_freq_GHz(),2)-pow((double)time_optim->diff/time_optim->trials/1000.0/get_cpu_freq_GHz(),2)));
    printf("Encoding time max: %15.3f us\n",(double)time_optim->max/1000.0/get_cpu_freq_GHz());
    printf("\n");
    printf("Decoding time mean: %15.3f us\n",(double)time_decoder->diff/time_decoder->trials/1000.0/get_cpu_freq_GHz());
    printf("Decoding time std: %15.3f us\n",sqrt((double)time_decoder->diff_square/time_decoder->trials/pow(1000,2)/pow(get_cpu_freq_GHz(),2)-pow((double)time_decoder->diff/time_decoder->trials/1000.0/get_cpu_freq_GHz(),2)));
    printf("Decoding time max: %15.3f us\n",(double)time_decoder->max/1000.0/get_cpu_freq_GHz());

    fprintf(fd,"%f %f %f %f %f %f %f %f %f %f %f %f %d \n",
    		SNR,
    		(double)decoded_errors[i]/(double)n_trials ,
    		(double)errors_bit/(double)n_trials/(double)block_length/(double)n_segments ,
    		errors_bit_uncoded/(double)n_trials/(double)n_segments ,
    		(double)time_optim->diff/time_optim->trials/1000.0/get_cpu_freq_GHz(),
    		sqrt((double)time_optim->diff_square/time_optim->trials/pow(1000,2)/pow(get_cpu_freq_GHz(),2)-pow((double)time_optim->diff/time_optim->trials/1000.0/get_cpu_freq_GHz(),2)),
    		(double)time_optim->max/1000.0/get_cpu_freq_GHz(),
    		(double)time_decoder->diff/time_decoder->trials/1000.0/get_cpu_freq_GHz(),
    		sqrt((double)time_decoder->diff_square/time_decoder->trials/pow(1000,2)/pow(get_cpu_freq_GHz(),2)-pow((double)time_decoder->diff/time_decoder->trials/1000.0/get_cpu_freq_GHz(),2)),
    		(double)time_decoder->max/1000.0/get_cpu_freq_GHz(),
    		dec_iter.n_iter_mean[i],
    		dec_iter.n_iter_std[i],
    		dec_iter.n_iter_max[i]
    		);

    i=i+1;
    if (decoded_errors[i-1] == 0) break;

  }
  fclose(fd);
  LOG_M("ldpctestStats.m","SNR",&dec_iter.snr[0],i,1,7);
  LOG_MM("ldpctestStats.m","BLER",&dec_iter.bler[0],i,1,7);
  LOG_MM("ldpctestStats.m","BER",&dec_iter.ber[0],i,1,7);
  LOG_MM("ldpctestStats.m","meanIter",&dec_iter.n_iter_mean[0],i,1,7);

  loader_reset();
  logTerm();

  return(0);
}
