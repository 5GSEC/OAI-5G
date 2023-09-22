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

#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#include "common/config/config_userapi.h"
#include "common/utils/load_module_shlib.h"
#include "common/utils/LOG/log.h"
#include "common/ran_context.h" 

#include "SIMULATION/TOOLS/sim.h"
#include "SIMULATION/RF/rf.h"
#include "PHY/types.h"
#include "PHY/defs_gNB.h"
#include "PHY/defs_nr_UE.h"
#include "SCHED_NR/sched_nr.h"
#include "SCHED_NR_UE/phy_frame_config_nr.h"
#include "PHY/phy_vars_nr_ue.h"
#include "PHY/NR_REFSIG/refsig_defs_ue.h"
#include "PHY/MODULATION/nr_modulation.h"
#include "PHY/MODULATION/modulation_eNB.h"
#include "PHY/MODULATION/modulation_UE.h"
#include "PHY/INIT/nr_phy_init.h"
#include "PHY/NR_TRANSPORT/nr_transport_proto.h"
#include "PHY/NR_TRANSPORT/nr_transport_common_proto.h"
#include "PHY/NR_UE_TRANSPORT/nr_transport_proto_ue.h"
#include "nr_unitary_defs.h"
#include <openair2/LAYER2/NR_MAC_COMMON/nr_mac_common.h>
#include <openair2/RRC/LTE/rrc_vars.h>
#include <executables/softmodem-common.h>
#include <openair2/RRC/NR_UE/rrc_defs.h>
#include <openair3/ocp-gtpu/gtp_itf.h>
#include "executables/nr-uesoftmodem.h"
#include "nfapi/oai_integration/vendor_ext.h"

#define NR_PRACH_DEBUG 1
#define PRACH_WRITE_OUTPUT_DEBUG 1

THREAD_STRUCT thread_struct;
char *parallel_config = NULL;
char *worker_config = NULL;

char *uecap_file;
PHY_VARS_gNB *gNB;
PHY_VARS_NR_UE *UE;
RAN_CONTEXT_t RC;
RU_t *ru;
double cpuf;
openair0_config_t openair0_cfg[MAX_CARDS];
//uint8_t nfapi_mode=0;
uint64_t downlink_frequency[MAX_NUM_CCs][4];
int32_t uplink_frequency_offset[MAX_NUM_CCs][4];
uint16_t sl_ahead = 0;
uint32_t N_RB_DL = 106;

NR_IF_Module_t *NR_IF_Module_init(int Mod_id) { return (NULL); }
nfapi_mode_t nfapi_getmode(void) { return NFAPI_MODE_UNKNOWN; }
void init_downlink_harq_status(NR_DL_UE_HARQ_t *dl_harq) { }

/* temporary dummy implem of get_softmodem_optmask, till basic simulators implemented as device */
uint64_t get_softmodem_optmask(void) {return 0;}
static softmodem_params_t softmodem_params;
softmodem_params_t *get_softmodem_params(void) {
  return &softmodem_params;
}
//Fixme: Uniq dirty DU instance, by global var, datamodel need better management
instance_t DUuniqInstance=0;
instance_t CUuniqInstance=0;

void inc_ref_sched_response(int _)
{
  LOG_E(PHY, "fatal\n");
  exit(1);
}
void deref_sched_response(int _)
{
  LOG_E(PHY, "fatal\n");
  exit(1);
}

void nr_derive_key_ng_ran_star(uint16_t pci, uint64_t nr_arfcn_dl, const uint8_t key[32], uint8_t *key_ng_ran_star)
{
}

void nr_rrc_ue_generate_RRCSetupRequest(module_id_t module_id, const uint8_t gNB_index)
{
  return;
}

int8_t nr_mac_rrc_data_req_ue(const module_id_t Mod_idP,
                              const int         CC_id,
                              const uint8_t     gNB_id,
                              const frame_t     frameP,
                              const rb_id_t     Srb_id,
                              uint8_t           *buffer_pP)
{
  return 0;
}

int8_t nr_rrc_RA_succeeded(const module_id_t mod_id, const uint8_t gNB_index) {
  return 0;
}

void nr_derive_key(int alg_type, uint8_t alg_id, const uint8_t key[32], uint8_t out[16])
{
  (void)alg_type;
}

nrUE_params_t nrUE_params;

nrUE_params_t *get_nrUE_params(void) {
  return &nrUE_params;
}

nr_bler_struct nr_bler_data[NR_NUM_MCS];

void processSlotTX(void *arg) {}
int NB_UE_INST = 1;

int main(int argc, char **argv){

  char c;
  get_softmodem_params()->sl_mode = 0;
  double sigma2, sigma2_dB = 0, SNR, snr0 = -2.0, snr1 = 0.0, ue_speed0 = 0.0, ue_speed1 = 0.0;
  double **s_re, **s_im, **r_re, **r_im, iqim = 0.0, delay_avg = 0, ue_speed = 0, fs=-1, bw;
  int i, l, aa, aarx, trial, n_frames = 1, prach_start, rx_prach_start; //, ntrials=1;
  c16_t **txdata;
  int N_RB_UL = 106, delay = 0, NCS_config = 13, rootSequenceIndex = 1, threequarter_fs = 0, mu = 1, fd_occasion = 0, loglvl = OAILOG_INFO, numRA = 0, prachStartSymbol = 0;
  uint8_t snr1set = 0, ue_speed1set = 0, transmission_mode = 1, n_tx = 1, n_rx = 1, awgn_flag = 0, msg1_frequencystart = 0, num_prach_fd_occasions = 1, prach_format=0;
  uint8_t config_index = 98, prach_sequence_length = 1, restrictedSetConfig = 0, N_dur, N_t_slot, start_symbol;
  uint16_t Nid_cell = 0, preamble_tx = 0, preamble_delay, format, format0, format1;
  uint32_t tx_lev = 10000, prach_errors = 0; //,tx_lev_dB;
  uint64_t SSB_positions = 0x01;
  uint16_t RA_sfn_index;
  uint8_t N_RA_slot;
  uint8_t config_period;
  int prachOccasion = 0;
  double DS_TDL = .03;

  //  int8_t interf1=-19,interf2=-19;
  //  uint8_t abstraction_flag=0,calibration_flag=0;
  //  double prach_sinr;
  //  uint32_t nsymb;
  //  uint16_t preamble_max, preamble_energy_max;
  FILE *input_fd=NULL;
  char* input_file=NULL;
  int n_bytes=0;

  NR_DL_FRAME_PARMS *frame_parms;
  nfapi_nr_prach_config_t *prach_config;
  nfapi_nr_prach_pdu_t *prach_pdu;
  fapi_nr_prach_config_t *ue_prach_config;
  fapi_nr_ul_config_prach_pdu *ue_prach_pdu;

  channel_desc_t *UE2gNB;
  SCM_t channel_model = Rayleigh1;
  cpuf = get_cpu_freq_GHz();

  if ( load_configmodule(argc,argv,CONFIG_ENABLECMDLINEONLY) == 0) {
    exit_fun("[SOFTMODEM] Error, configuration module init failed\n");
  }

  randominit(0);

  while ((c = getopt (argc, argv, "hHaA:Cc:l:r:p:g:m:n:s:S:t:x:y:v:V:z:N:F:d:Z:L:R:E")) != -1) {
    switch (c) {
    case 'a':
      printf("Running AWGN simulation\n");
      awgn_flag = 1;
      /* ntrials not used later, no need to set */
      //ntrials=1;
      break;

    case 'c':
      config_index = atoi(optarg);
      break;

    case 'l':
      loglvl = atoi(optarg);
      break;

    case 'r':
      msg1_frequencystart = atoi(optarg);
      break;

    case 'd':
      delay = atoi(optarg);
      break;

    case 'g':
      switch((char)*optarg) {
      case 'A':
        channel_model=SCM_A;
        break;

      case 'B':
        channel_model=SCM_B;
        break;

      case 'C':
        channel_model=SCM_C;
        break;

      case 'D':
        channel_model=SCM_D;
        break;

      case 'E':
        channel_model=EPA;
        break;

      case 'F':
        channel_model=EVA;
        break;

      case 'G':
        channel_model=ETU;
        break;

      case 'H':
        channel_model=Rayleigh8;
        break;

      case 'I':
        channel_model=Rayleigh1;
        break;

      case 'J':
        channel_model=Rayleigh1_corr;
        break;

      case 'K':
        channel_model=Rayleigh1_anticorr;
        break;

      case 'L':
        channel_model=Rice8;
        break;

      case 'M':
        channel_model=Rice1;
        break;

      case 'N':
        channel_model=Rayleigh1_800;
        break;

      default:
        printf("Unsupported channel model!\n");
        exit(-1);
      }

      break;

    case 'E':
      threequarter_fs=1;
      break;

    case 'm':
      mu = atoi(optarg);
      break;

    case 'n':
      n_frames = atoi(optarg);
      break;

    case 's':
      snr0 = atof(optarg);
      printf("Setting SNR0 to %f\n",snr0);
      break;

    case 'S':
      snr1 = atof(optarg);
      snr1set=1;
      printf("Setting SNR1 to %f\n",snr1);
      break;

    case 'p':
      preamble_tx=atoi(optarg);
      break;

    case 'v':
      ue_speed0 = atoi(optarg);
      break;

    case 'V':
      ue_speed1 = atoi(optarg);
      ue_speed1set = 1;
      break;

    case 'Z':
      NCS_config = atoi(optarg);

      if ((NCS_config > 15) || (NCS_config < 0))
        printf("Illegal NCS_config %d, (should be 0-15)\n",NCS_config);

      break;

    case 'H':
      printf("High-Speed Flag enabled\n");
      restrictedSetConfig = 1;
      break;

    case 'L':
      rootSequenceIndex = atoi(optarg);

      if ((rootSequenceIndex < 0) || (rootSequenceIndex > 837))
        printf("Illegal rootSequenceNumber %d, (should be 0-837)\n",rootSequenceIndex);

      break;

    case 'x':
      transmission_mode=atoi(optarg);

      if ((transmission_mode!=1) &&
          (transmission_mode!=2) &&
          (transmission_mode!=6)) {
        printf("Unsupported transmission mode %d\n",transmission_mode);
        exit(-1);
      }

      break;

    case 'y':
      n_tx=atoi(optarg);

      if ((n_tx==0) || (n_tx>2)) {
        printf("Unsupported number of tx antennas %d\n",n_tx);
        exit(-1);
      }

      break;

    case 'z':
      n_rx=atoi(optarg);

      if ((n_rx==0) || (n_rx>2)) {
        printf("Unsupported number of rx antennas %d\n",n_rx);
        exit(-1);
      }

      break;

    case 'N':
      Nid_cell = atoi(optarg);
      break;

    case 'R':
      N_RB_UL = atoi(optarg);
      break;

    case 'F':
      input_fd = fopen(optarg,"r");
      input_file = optarg;
      
      if (input_fd==NULL) {
	printf("Problem with filename %s\n",optarg);
	exit(-1);
      }
      
      break;

    default:
    case 'h':
      printf("%s -h(elp) -a(wgn on) -p(extended_prefix) -N cell_id -f output_filename -F input_filename -g channel_model -n n_frames -s snr0 -S snr1 -x transmission_mode -y TXant -z RXant -i Intefrence0 -j Interference1 -A interpolation_file -C(alibration offset dB) -N CellId\n",
             argv[0]);
      printf("-h This message\n");
      printf("-a Use AWGN channel and not multipath\n");
      printf("-n Number of frames to simulate\n");
      printf("-s Starting SNR, runs from SNR0 to SNR0 + 5 dB.  If n_frames is 1 then just SNR is simulated\n");
      printf("-S Ending SNR, runs from SNR0 to SNR1\n");
      printf("-g [A,B,C,D,E,F,G,I,N] Use 3GPP SCM (A,B,C,D) or 36-101 (E-EPA,F-EVA,G-ETU) or Rayleigh1 (I) or Rayleigh1_800 (N) models (ignores delay spread and Ricean factor)\n");
      printf("-z Number of RX antennas used in gNB\n");
      printf("-N Nid_cell\n");
      printf("-O oversampling factor (1,2,4,8,16)\n");
      //    printf("-f PRACH format (0=1,1=2,2=3,3=4)\n");
      printf("-d Channel delay \n");
      printf("-v Starting UE velocity in km/h, runs from 'v' to 'v+50km/h'. If n_frames is 1 just 'v' is simulated \n");
      printf("-V Ending UE velocity in km/h, runs from 'v' to 'V'");
      printf("-L rootSequenceIndex (0-837)\n");
      printf("-Z NCS_config (ZeroCorrelationZone) (0-15)\n");
      printf("-H Run with High-Speed Flag enabled \n");
      printf("-R Number of PRB (6,15,25,50,75,100)\n");
      printf("-F Input filename (.txt format) for RX conformance testing\n");
      exit (-1);
      break;
    }
  }

  // Configure log
  logInit();
  set_glog(loglvl);
  SET_LOG_DEBUG(PRACH); 

  // Configure gNB and RU
  RC.gNB = (PHY_VARS_gNB**) malloc(2*sizeof(PHY_VARS_gNB *));
  RC.gNB[0] = malloc(sizeof(PHY_VARS_gNB));
  memset(RC.gNB[0],0,sizeof(PHY_VARS_gNB));

  RC.ru = (RU_t**) malloc(2*sizeof(RU_t *));
  RC.ru[0] = (RU_t*) malloc(sizeof(RU_t ));
  memset(RC.ru[0],0,sizeof(RU_t));
  RC.nb_RU = 1;

  gNB          = RC.gNB[0];
  ru           = RC.ru[0];
  frame_parms  = &gNB->frame_parms;
  prach_config = &gNB->gNB_config.prach_config;
  prach_pdu    = &gNB->prach_vars.list[0].pdu;
  frame_parms  = &gNB->frame_parms; //to be initialized I suppose (maybe not necessary for PBCH)

  s_re = malloc(2*sizeof(double*));
  s_im = malloc(2*sizeof(double*));
  r_re = malloc(2*sizeof(double*));
  r_im = malloc(2*sizeof(double*));

  frame_parms->nb_antennas_tx   = n_tx;
  frame_parms->nb_antennas_rx   = n_rx;
  frame_parms->N_RB_DL          = N_RB_UL;
  frame_parms->N_RB_UL          = N_RB_UL;
  frame_parms->threequarter_fs  = threequarter_fs;
  frame_parms->frame_type       = TDD;
  frame_parms->freq_range       = (mu != 3 ? nr_FR1 : nr_FR2);
  frame_parms->numerology_index = mu;

  nr_phy_config_request_sim(gNB, N_RB_UL, N_RB_UL, mu, Nid_cell, SSB_positions);

  uint64_t absoluteFrequencyPointA = to_nrarfcn(frame_parms->nr_band,
				       frame_parms->dl_CarrierFreq,
				       frame_parms->numerology_index,
				       frame_parms->N_RB_UL*(180e3)*(1 << frame_parms->numerology_index));

  uint8_t frame = 1;
  uint8_t subframe = 9;
  uint8_t slot = 10 * frame_parms->slots_per_subframe - 1;
  
  if (config_index<67 && mu != 3)  { prach_sequence_length=0; slot = subframe * frame_parms->slots_per_subframe; }
  uint16_t N_ZC = prach_sequence_length == 0 ? 839 : 139;

  printf("Config_index %d, prach_sequence_length %d\n",config_index,prach_sequence_length);


  printf("FFT Size %d, Extended Prefix %d, Samples per subframe %d, Frame type %s, Frequency Range %s\n",
         NUMBER_OF_OFDM_CARRIERS,
         frame_parms->Ncp,
         frame_parms->samples_per_subframe,
         frame_parms->frame_type == FDD ? "FDD" : "TDD",
         frame_parms->freq_range == nr_FR1 ? "FR1" : "FR2");

  ru->nr_frame_parms = frame_parms;
  ru->if_south       = LOCAL_RF;
  ru->nb_tx          = n_tx;
  ru->nb_rx          = n_rx;
  ru->num_gNB        = 1;
  ru->gNB_list[0]    = gNB;
  gNB->gNB_config.carrier_config.num_tx_ant.value = 1;
  gNB->gNB_config.carrier_config.num_rx_ant.value = 1;
  if (mu == 0)
    gNB->gNB_config.tdd_table.tdd_period.value = 7;
  else if (mu == 1)
    gNB->gNB_config.tdd_table.tdd_period.value = 6;
  else if (mu == 3)
    gNB->gNB_config.tdd_table.tdd_period.value = 3;
  else {
    printf("unsupported numerology %d\n",mu);
    exit(-1);
  }

  gNB->gNB_config.prach_config.num_prach_fd_occasions.value = num_prach_fd_occasions;
  gNB->gNB_config.prach_config.num_prach_fd_occasions_list = (nfapi_nr_num_prach_fd_occasions_t *) malloc(num_prach_fd_occasions*sizeof(nfapi_nr_num_prach_fd_occasions_t));

  gNB->proc.slot_rx       = slot;

  int ret = get_nr_prach_info_from_index(config_index,
					 (int)frame,
					 (int)slot,
					 absoluteFrequencyPointA,
					 mu,
					 frame_parms->frame_type,
					 &format,
					 &start_symbol,
					 &N_t_slot,
					 &N_dur,
					 &RA_sfn_index,
					 &N_RA_slot,
					 &config_period);

  if (ret == 0) {printf("No prach in %d.%d, mu %d, config_index %d\n",frame,slot,mu,config_index); exit(-1);}
  format0 = format&0xff;      // first column of format from table
  format1 = (format>>8)&0xff; // second column of format from table

  if (format1 != 0xff) {
    switch(format0) {
    case 0xa1:
      prach_format = 11;
      break;
    case 0xa2:
      prach_format = 12;
      break;
    case 0xa3:
      prach_format = 13;
      break;
    default:
      AssertFatal(1==0, "Only formats A1/B1 A2/B2 A3/B3 are valid for dual format");
    }
  } else {
    switch(format0) { // single PRACH format
    case 0:
      prach_format = 0;
      break;
    case 1:
      prach_format = 1;
      break;
    case 2:
      prach_format = 2;
      break;
    case 3:
      prach_format = 3;
      break;
    case 0xa1:
      prach_format = 4;
      break;
    case 0xa2:
      prach_format = 5;
      break;
    case 0xa3:
      prach_format = 6;
      break;
    case 0xb1:
      prach_format = 7;
      break;
    case 0xb4:
      prach_format = 8;
      break;
    case 0xc0:
      prach_format = 9;
      break;
    case 0xc2:
      prach_format = 10;
      break;
    default:
      AssertFatal(1 == 0, "Invalid PRACH format");
    }
  }

  printf("PRACH format %d\n",prach_format);
      
  prach_config->num_prach_fd_occasions_list[fd_occasion].prach_root_sequence_index.value = rootSequenceIndex;
  prach_config->num_prach_fd_occasions_list[fd_occasion].k1.value                        = msg1_frequencystart;
  prach_config->restricted_set_config.value                                              = restrictedSetConfig;
  prach_config->prach_sequence_length.value                                              = prach_sequence_length;
  prach_pdu->num_cs                                                                      = get_NCS(NCS_config, format0, restrictedSetConfig);
  prach_config->num_prach_fd_occasions_list[fd_occasion].num_root_sequences.value        = 1+(64/(N_ZC/prach_pdu->num_cs));
  prach_pdu->prach_format                                                                = prach_format;

  memcpy((void*)&ru->config,(void*)&RC.gNB[0]->gNB_config,sizeof(ru->config));
  RC.nb_nr_L1_inst=1;
  set_tdd_config_nr(&gNB->gNB_config, mu, 7, 6, 2, 4);
  phy_init_nr_gNB(gNB);
  nr_phy_init_RU(ru);

  // Configure UE
  UE = malloc(sizeof(PHY_VARS_NR_UE));
  memset((void*)UE,0,sizeof(PHY_VARS_NR_UE));
  PHY_vars_UE_g = malloc(2*sizeof(PHY_VARS_NR_UE**));
  PHY_vars_UE_g[0] = malloc(2*sizeof(PHY_VARS_NR_UE*));
  PHY_vars_UE_g[0][0] = UE;
  memcpy(&UE->frame_parms,frame_parms,sizeof(NR_DL_FRAME_PARMS));
  UE->nrUE_config.prach_config.num_prach_fd_occasions_list = (fapi_nr_num_prach_fd_occasions_t *) malloc(num_prach_fd_occasions*sizeof(fapi_nr_num_prach_fd_occasions_t));

  if (init_nr_ue_signal(UE, 1) != 0){
    printf("Error at UE NR initialisation\n");
    exit(-1);
  }

  ue_prach_pdu           = &UE->prach_vars[0]->prach_pdu;
  ue_prach_config        = &UE->nrUE_config.prach_config;
  txdata = UE->common_vars.txData;

  UE->prach_vars[0]->amp        = AMP;
  ue_prach_pdu->root_seq_id     = rootSequenceIndex;
  ue_prach_pdu->num_cs          = get_NCS(NCS_config, format0, restrictedSetConfig);
  ue_prach_pdu->restricted_set  = restrictedSetConfig;
  ue_prach_pdu->freq_msg1       = msg1_frequencystart;
  ue_prach_pdu->prach_format    = prach_format;

  ue_prach_config->prach_sub_c_spacing                                                = mu;
  ue_prach_config->prach_sequence_length                                              = prach_sequence_length;
  ue_prach_config->restricted_set_config                                              = restrictedSetConfig;
  ue_prach_config->num_prach_fd_occasions_list[fd_occasion].num_root_sequences        = prach_config->num_prach_fd_occasions_list[fd_occasion].num_root_sequences.value ;
  ue_prach_config->num_prach_fd_occasions_list[fd_occasion].prach_root_sequence_index = rootSequenceIndex;
  ue_prach_config->num_prach_fd_occasions_list[fd_occasion].k1                        = msg1_frequencystart;

  if (preamble_tx == 99)
    preamble_tx = (uint16_t)(taus()&0x3f);

  if (n_frames == 1)
    printf("raPreamble %d\n",preamble_tx);

  ue_prach_pdu->ra_PreambleIndex = preamble_tx;

  // Configure channel
  bw = N_RB_UL*(180e3)*(1 << frame_parms->numerology_index);
  AssertFatal(bw<=122.88e6,"Illegal channel bandwidth %f (mu %d,N_RB_UL %d)\n", bw, frame_parms->numerology_index, N_RB_UL);

  fs = frame_parms->samples_per_subframe * 1e3;

  LOG_I(PHY,"Running with bandwidth %f Hz, fs %f samp/s, FRAME_LENGTH_COMPLEX_SAMPLES %d\n",bw,fs,FRAME_LENGTH_COMPLEX_SAMPLES);

  UE2gNB = new_channel_desc_scm(UE->frame_parms.nb_antennas_tx,
                                gNB->frame_parms.nb_antennas_rx,
                                channel_model,
                                fs,
                                0,
                                bw,
                                DS_TDL,
                                0.0,
                                CORR_LEVEL_LOW,
                                0.0,
                                delay,
                                0,
                                0);

  if (UE2gNB==NULL) {
    printf("Problem generating channel model. Exiting.\n");
    exit(-1);
  }

  for (i=0; i<2; i++) {

    s_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(s_re[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    s_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(s_im[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));

    r_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(r_re[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    r_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(r_im[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
  }

  // compute PRACH sequence
  compute_nr_prach_seq(prach_config->prach_sequence_length.value,
                       prach_config->num_prach_fd_occasions_list[fd_occasion].num_root_sequences.value,
                       prach_config->num_prach_fd_occasions_list[fd_occasion].prach_root_sequence_index.value,
                       gNB->X_u);

  compute_nr_prach_seq(ue_prach_config->prach_sequence_length,
                       ue_prach_config->num_prach_fd_occasions_list[fd_occasion].num_root_sequences,
                       ue_prach_config->num_prach_fd_occasions_list[fd_occasion].prach_root_sequence_index,
                       UE->X_u);

  generate_nr_prach(UE, 0, frame, slot);

  /* tx_lev_dB not used later, no need to set */
  //tx_lev_dB = (unsigned int) dB_fixed(tx_lev);

  prach_start = subframe*frame_parms->samples_per_subframe;

  #ifdef NR_PRACH_DEBUG
  LOG_M("txsig0.m", "txs0", &txdata[0][subframe*frame_parms->samples_per_subframe], frame_parms->samples_per_subframe, 1, 1);
    LOG_M("txsig0_frame.m","txs0", txdata[0],frame_parms->samples_per_frame,1,1);
  #endif

  // multipath channel
  // dump_nr_prach_config(&gNB->frame_parms,subframe);

  for (i = 0; i < frame_parms->samples_per_subframe; i++) {
    for (aa=0; aa<1; aa++) {
      if (awgn_flag == 0) {
        s_re[aa][i] = ((double)(((short *)&txdata[aa][prach_start]))[(i<<1)]);
        s_im[aa][i] = ((double)(((short *)&txdata[aa][prach_start]))[(i<<1)+1]);
      } else {
        for (aarx=0; aarx<gNB->frame_parms.nb_antennas_rx; aarx++) {
          if (aa==0) {
            r_re[aarx][i] = ((double)(((short *)&txdata[aa][prach_start]))[(i<<1)]);
            r_im[aarx][i] = ((double)(((short *)&txdata[aa][prach_start]))[(i<<1)+1]);
          } else {
            r_re[aarx][i] += ((double)(((short *)&txdata[aa][prach_start]))[(i<<1)]);
            r_im[aarx][i] += ((double)(((short *)&txdata[aa][prach_start]))[(i<<1)+1]);
          }
        }
      }
    }
  }

  if (snr1set == 0) {
    if (n_frames == 1)
      snr1 = snr0 + .1;
    else
      snr1 = snr0 + 5.0;
  }

  printf("SNR0 %f, SNR1 %f\n", snr0, snr1);

  if (ue_speed1set == 0) {
    if (n_frames == 1)
      ue_speed1 = ue_speed0 + 10;
    else
      ue_speed1 = ue_speed0 + 50;
  }

  rx_prach_start = subframe*frame_parms->samples_per_subframe;
  if (n_frames==1) printf("slot %d, rx_prach_start %d\n",slot,rx_prach_start);
  uint16_t preamble_rx, preamble_energy;


  for (SNR=snr0; SNR<snr1; SNR+=.1) {
    for (ue_speed=ue_speed0; ue_speed<ue_speed1; ue_speed+=10) {
      delay_avg = 0.0;
      // max Doppler shift
      UE2gNB->max_Doppler = 1.9076e9*(ue_speed/3.6)/3e8;
      printf("n_frames %d SNR %f\n",n_frames,SNR);
      prach_errors=0;

      for (trial=0; trial<n_frames; trial++) {

	if (input_fd==NULL) {
          sigma2_dB = 10*log10((double)tx_lev) - SNR - 10*log10(N_RB_UL*12/N_ZC);

          if (n_frames==1)
            printf("sigma2_dB %f (SNR %f dB) tx_lev_dB %f\n",sigma2_dB,SNR,10*log10((double)tx_lev));

          //AWGN
          sigma2 = pow(10,sigma2_dB/10);
          //  printf("Sigma2 %f (sigma2_dB %f)\n",sigma2,sigma2_dB);

          if (awgn_flag == 0) {
            multipath_tv_channel(UE2gNB, s_re, s_im, r_re, r_im, frame_parms->samples_per_frame, 0);
          }

          if (n_frames==1) {
            printf("rx_level data symbol %f, tx_lev %f\n",
                   10*log10(signal_energy_fp(r_re,r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,0)),
                   10*log10(tx_lev));
          }

          for (i = 0; i< frame_parms->samples_per_subframe; i++) {
            for (aa = 0; aa < frame_parms->nb_antennas_rx; aa++) {
              ((short*) &ru->common.rxdata[aa][rx_prach_start])[2*i] = (short) (.167*(r_re[aa][i] +sqrt(sigma2/2)*gaussdouble(0.0,1.0)));
              ((short*) &ru->common.rxdata[aa][rx_prach_start])[2*i+1] = (short) (.167*(r_im[aa][i] + (iqim*r_re[aa][i]) + sqrt(sigma2/2)*gaussdouble(0.0,1.0)));
            }
          }
	} else {
	  n_bytes = fread(&ru->common.rxdata[0][rx_prach_start],sizeof(int32_t),frame_parms->samples_per_subframe,input_fd);
	  printf("fread %d bytes from file %s\n",n_bytes,input_file);
	  if (n_bytes!=frame_parms->samples_per_subframe) {
	    printf("expected %d bytes\n",frame_parms->samples_per_subframe);
	    exit(-1);
	  }
	}

	for (l = 0; l < frame_parms->symbols_per_slot; l++) {
	  for (aa = 0; aa < frame_parms->nb_antennas_rx; aa++) {
	    nr_slot_fep_ul(frame_parms,
			   (int32_t *)ru->common.rxdata[aa],
			   (int32_t *)ru->common.rxdataF[aa],
			   l,
			   slot,
			   ru->N_TA_offset);
	  }
	}
	
        rx_nr_prach_ru(ru, prach_format, numRA, prachStartSymbol, prachOccasion, frame, slot);

        for (int i = 0; i < ru->nb_rx; ++i)
          gNB->prach_vars.rxsigF[i] = ru->prach_rxsigF[prachOccasion][i];
	if (n_frames == 1) printf("ncs %d,num_seq %d\n",prach_pdu->num_cs,  prach_config->num_prach_fd_occasions_list[fd_occasion].num_root_sequences.value);
        rx_nr_prach(gNB, prach_pdu, prachOccasion, frame, subframe, &preamble_rx, &preamble_energy, &preamble_delay);

	//        printf(" preamble_energy %d preamble_rx %d preamble_tx %d \n", preamble_energy, preamble_rx, preamble_tx);

        if (preamble_rx != preamble_tx)
          prach_errors++;
        else
          delay_avg += (double)preamble_delay;

        N_ZC = (prach_sequence_length) ? 139 : 839;

        if (n_frames==1) {
          printf("preamble %d (tx %d) : energy %d, delay %d\n",preamble_rx,preamble_tx,preamble_energy,preamble_delay);
          #ifdef NR_PRACH_DEBUG
	  LOG_M("prach0.m","prach0", &txdata[0][prach_start], frame_parms->samples_per_subframe, 1, 1);
            LOG_M("prachF0.m","prachF0", &gNB->prach_vars.prachF[0], N_ZC, 1, 1);
            LOG_M("rxsig0.m","rxs0", &ru->common.rxdata[0][subframe*frame_parms->samples_per_subframe], frame_parms->samples_per_subframe, 1, 1);
            LOG_M("ru_rxsig0.m","rxs0", &ru->common.rxdata[0][subframe*frame_parms->samples_per_subframe], frame_parms->samples_per_subframe, 1, 1);
            LOG_M("ru_rxsigF0.m","rxsF0", ru->common.rxdataF[0], frame_parms->ofdm_symbol_size*frame_parms->symbols_per_slot, 1, 1);
            LOG_M("ru_prach_rxsigF0.m","rxsF0", ru->prach_rxsigF[0][0], N_ZC, 1, 1);
            LOG_M("prach_preamble.m","prachp", &gNB->X_u[0], N_ZC, 1, 1);
            LOG_M("ue_prach_preamble.m","prachp", &UE->X_u[0], N_ZC, 1, 1);
          #endif
        }
      }

      printf("SNR %f dB, UE Speed %f km/h: errors %u/%d (delay %f)\n", SNR, ue_speed, prach_errors, n_frames, delay_avg/(double)(n_frames-prach_errors));
      if (input_fd)
	break;
      if (prach_errors)
        break;

    } // UE Speed loop
    if (!prach_errors) {
      printf("PRACH test OK\n");
      break;
    }
    if (input_fd)
      break;
  } //SNR loop

  free_channel_desc_scm(UE2gNB);

  nr_phy_free_RU(ru);
  free(RC.ru[0]);
  free(RC.ru);

  phy_free_nr_gNB(gNB);
  // allocated in set_tdd_config_nr()
  int nb_slots_to_set = TDD_CONFIG_NB_FRAMES*(1<<mu)*NR_NUMBER_OF_SUBFRAMES_PER_FRAME;
  free(gNB->gNB_config.prach_config.num_prach_fd_occasions_list);
  for (int i = 0; i < nb_slots_to_set; ++i)
    free(gNB->gNB_config.tdd_table.max_tdd_periodicity_list[i].max_num_of_symbol_per_slot_list);
  free(gNB->gNB_config.tdd_table.max_tdd_periodicity_list);
  free(RC.gNB[0]);
  free(RC.gNB);

  term_nr_ue_signal(UE, 1);
  free(UE->nrUE_config.prach_config.num_prach_fd_occasions_list);
  free(UE);
  free(PHY_vars_UE_g[0]);
  free(PHY_vars_UE_g);

  for (i=0; i<2; i++) {
    free(s_re[i]);
    free(s_im[i]);
    free(r_re[i]);
    free(r_im[i]);
  }

  free(s_re);
  free(s_im);
  free(r_re);
  free(r_im);

  if (input_fd) fclose(input_fd);

  loader_reset();
  logTerm();

  return(0);
}
