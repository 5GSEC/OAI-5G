/*! \file oaiori.c
 * \brief AW2S ORI API isolator 
 * \author  Raymond Knopp
 * \date 2019
 * \version 0.1
 * \company Eurecom
 * \maintainer:  raymond.knopp@eurecom.fr
 * \note
 * \warning
 */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <errno.h>
#include <linux/sysctl.h>
#include <pthread.h>

#include "common_lib.h"
#include "ethernet_lib.h"
#include "common/utils/system.h"
#include "ori.h"

#include "radio/COMMON/common_lib.h"

typedef struct eutra_bandentry_s {
  int16_t band;
  uint32_t ul_min;
  uint32_t ul_max;
  uint32_t dl_min;
  uint32_t dl_max;
  uint32_t N_OFFs_DL;
} eutra_bandentry_t;

typedef struct band_info_s {
  int nbands;
  eutra_bandentry_t band_info[100];
} band_info_t;


static const eutra_bandentry_t eutra_bandtable[] = {
  {1,  19200, 19800, 21100, 21700, 0     },
  {2,  18500, 19100, 19300, 19900, 6000  },
  {3,  17100, 17850, 18050, 18800, 12000 },
  {4,  17100, 17550, 21100, 21550, 19500 },
  {5,  8240,  8490,  8690,  8940,  24000 },
  {6,  8300,  8400,  8750,  8850,  26500 },
  {7,  25000, 25700, 26200, 26900, 27500 },
  {8,  8800,  9150,  9250,  9600,  34500 },
  {9,  17499, 17849, 18449, 18799, 38000 },
  {10, 17100, 17700, 21100, 21700, 41500 },
  {11, 14279, 14529, 14759, 15009, 47500 },
  {12, 6980,  7160,  7280,  7460,  50100 },
  {13, 7770,  7870,  7460,  7560,  51800 },
  {14, 7880,  7980,  7580,  7680,  52800 },
  {17, 7040,  7160,  7340,  7460,  57300 },
  {18, 8150,  9650,  8600,  10100, 58500 },
  {19, 8300,  8450,  8750,  8900,  60000 },
  {20, 8320,  8620,  7910,  8210,  61500 },
  {21, 14479, 14629, 14959, 15109, 64500 },
  {22, 34100, 34900, 35100, 35900, 66000 },
  {23, 20000, 20200, 21800, 22000, 75000 },
  {24, 16126, 16605, 15250, 15590, 77000 },
  {25, 18500, 19150, 19300, 19950, 80400 },
  {26, 8140,  8490,  8590,  8940,  86900 },
  {27, 8070,  8240,  8520,  8690,  90400 },
  {28, 7030,  7580,  7580,  8130,  92100 },
  {29, 0,     0,     7170,  7280,  96600 },
  {30, 23050, 23250, 23500, 23600, 97700 },
  {31, 45250, 34900, 46250, 35900, 98700 },
  {32, 0,     0,     14520, 14960, 99200 },
  {33, 19000, 19200, 19000, 19200, 360000},
  {34, 20100, 20250, 20100, 20250, 362000},
  {35, 18500, 19100, 18500, 19100, 363500},
  {36, 19300, 19900, 19300, 19900, 369500},
  {37, 19100, 19300, 19100, 19300, 375500},
  {38, 25700, 26200, 25700, 26200, 377500},
  {39, 18800, 19200, 18800, 19200, 382500},
  {40, 23000, 24000, 23000, 24000, 386500},
  {41, 24960, 26900, 24960, 26900, 396500},
  {42, 34000, 36000, 34000, 36000, 415900},
  {43, 36000, 38000, 36000, 38000, 435900},
  {44, 7030,  8030,  7030,  8030,  455900},
  {45, 14470, 14670, 14470, 14670, 465900},
  {46, 51500, 59250, 51500, 59250, 467900},
  {65, 19200, 20100, 21100, 22000, 655360},
  {66, 17100, 18000, 21100, 22000, 664360},
  {67, 0,     0,     7380,  7580,  67336 },
  {68, 6980,  7280,  7530,  7830,  67536 }
};


#define BANDTABLE_SIZE (sizeof(eutra_bandtable)/sizeof(eutra_bandentry_t))

uint32_t to_earfcn_DL_aw2s(int eutra_bandP, long long int dl_CarrierFreq, uint32_t bw) {
  uint32_t dl_CarrierFreq_by_100k = dl_CarrierFreq / 100000;
  int i=0;
  if (eutra_bandP > 68) { printf("eutra_band %d > 68\n", eutra_bandP); exit(-1);}



  if (eutra_bandP > 0) 
    for (i = 0; i < BANDTABLE_SIZE && eutra_bandtable[i].band != eutra_bandP; i++);
  printf("AW2S: band %d: index %d\n",eutra_bandP, i);

  if (i >= BANDTABLE_SIZE) { printf(" i = %d , it will trigger out-of-bounds read.\n",i); exit(-1);}

  while(i<BANDTABLE_SIZE) {
    if (dl_CarrierFreq_by_100k < eutra_bandtable[i].dl_min) {
                printf("Band %d, bw %u : DL carrier frequency %u .1 MHz < %u\n",
                eutra_bandtable[i].band, bw, dl_CarrierFreq_by_100k,
                eutra_bandtable[i].dl_min); i++; continue;}

    if(dl_CarrierFreq_by_100k >
                (eutra_bandtable[i].dl_max - bw)) {
                printf("Band %d, bw %u : DL carrier frequency %u .1MHz > %d\n",
                eutra_bandtable[i].band, bw, dl_CarrierFreq_by_100k,
                eutra_bandtable[i].dl_max - bw); i++;continue; }
    printf("AW2S: dl_CarrierFreq_by_100k %u, dl_min %d\n",dl_CarrierFreq_by_100k,eutra_bandtable[i].dl_min);

    return (dl_CarrierFreq_by_100k - eutra_bandtable[i].dl_min +
            (eutra_bandtable[i].N_OFFs_DL / 10));

  }
  printf("No DL band found\n");
  exit(-1);
}
uint32_t to_earfcn_UL_aw2s(int eutra_bandP, long long int ul_CarrierFreq, uint32_t bw) {
  uint32_t ul_CarrierFreq_by_100k = ul_CarrierFreq / 100000;
  int i;
  if(eutra_bandP >= 69) {
     printf("eutra_band %d > 68\n", eutra_bandP);
     exit(-1);
  }


  for (i = 0; i < BANDTABLE_SIZE && eutra_bandtable[i].band != eutra_bandP; i++);

  if(i >= BANDTABLE_SIZE) {printf("i = %d , it will trigger out-of-bounds read.\n",i); exit(-1);}

  if(ul_CarrierFreq_by_100k < eutra_bandtable[i].ul_min) {
              printf("Band %d, bw %u : UL carrier frequency %u Hz < %u\n",
              eutra_bandP, bw, ul_CarrierFreq_by_100k,
              eutra_bandtable[i].ul_min);
              exit(-1);
  }
  if(ul_CarrierFreq_by_100k > (eutra_bandtable[i].ul_max - bw)) {
              printf("Band %d, bw %u : UL carrier frequency %u Hz > %d\n",
              eutra_bandP, bw, ul_CarrierFreq_by_100k,
              eutra_bandtable[i].ul_max - bw);
              exit(-1);
  }
  return (ul_CarrierFreq_by_100k - eutra_bandtable[i].ul_min +
          ((eutra_bandtable[i].N_OFFs_DL + 180000) / 10));
}

int get_mac_addr(const char *iface,unsigned char *mac)

{
    int fd;
    struct ifreq ifr;
    int ret=0;

    memset(&ifr, 0, sizeof(ifr));
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
    if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) memcpy((void*)mac,(void *)ifr.ifr_hwaddr.sa_data,6);
    else {
      printf("No MAC address!\n");
      ret = -1;
    }

    printf("%s : %2x:%2x:%2x:%2x:%2x:%2x\n",iface,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    close(fd);
    return(ret);
}


void cb(void * userData, ORI_IndicationType_e type, ORI_IndicationValue_u value)
{
  printf("============> Got indication %s\n", ORI_IndicationType_Print(type));
  if(type == ORI_IndicationType_ObjectStateChange)
    {
      ORI_Object_s * obj = value.objectStateChange.object;
      printf("State change for %s:%d -> %s : %s\n", ORI_ObjectType_Print(obj->typeRef.type), obj->instanceNumber,
	     ORI_StateType_Print(value.objectStateChange.stateType),
	     value.objectStateChange.stateType == ORI_StateType_AST ? ORI_AST_Print(obj->ast) : ORI_FST_Print(obj->fst));
    }
  else if(type == ORI_IndicationType_FaultChange)
    {
      ORI_Object_s * obj = value.faultChange.object;
      ORI_Fault_s * fault = value.faultChange.fault;
      printf("Fault change for %s:%d -> %s : %s\n", ORI_ObjectType_Print(obj->typeRef.type), obj->instanceNumber,
	     ORI_FaultType_Print(value.faultChange.faultType),
	     ORI_FaultState_Print(fault->state));
      if(fault->state == ORI_FaultState_Active)
	{
	  printf("\t Fault severity: %s\n", ORI_FaultSeverity_Print(fault->severity));
	  printf("\t Timestamp: %s\n", fault->timestamp);
	  printf("\t Description: %s\n", fault->desc);
	}
    }
  printf("<============\n");
}

int aw2s_oricleanup(openair0_device *device) {

  ORI_s *               ori = (ORI_s*)device->thirdparty_priv;

  ORI_Disconnect(ori);
  ORI_Free(ori);

  return(0);
}

int aw2s_startstreaming(openair0_device *device) {

  ORI_s *               ori = (ORI_s*)device->thirdparty_priv;
  ORI_Result_e  result;
  ORI_Result_e  RE_result;

  openair0_config_t *openair0_cfg = device->openair0_cfg;
  ORI_Object_s *tx0,*tx1,*tx2,*tx3,*rx0,*rx1,*rx2,*rx3;
  if (openair0_cfg->nr_flag == 0 ) {
    tx0= ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ? ORI_ObjectType_TxEUtraFDD :ORI_ObjectType_TxEUtraTDD, 0, NULL);
    tx1= (openair0_cfg->tx_num_channels > 1) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_TxEUtraFDD : ORI_ObjectType_TxEUtraTDD, 1, NULL) : NULL;
    tx2= (openair0_cfg->tx_num_channels > 2) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_TxEUtraFDD : ORI_ObjectType_TxEUtraTDD, 2, NULL) : NULL;
    tx3= (openair0_cfg->tx_num_channels > 3) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_TxEUtraFDD : ORI_ObjectType_TxEUtraTDD, 3, NULL) : NULL;
    rx0= ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxEUtraFDD : ORI_ObjectType_RxEUtraTDD, 0, NULL);
    rx1= (openair0_cfg->rx_num_channels > 1) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxEUtraFDD : ORI_ObjectType_RxEUtraTDD, 1, NULL) : NULL;
    rx2= (openair0_cfg->rx_num_channels > 2) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxEUtraFDD : ORI_ObjectType_RxEUtraTDD, 2, NULL) : NULL;
    rx3= (openair0_cfg->rx_num_channels > 3) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxEUtraFDD : ORI_ObjectType_RxEUtraTDD, 3, NULL) : NULL;
  } else {
    tx0= ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ? ORI_ObjectType_TxNRFDD :ORI_ObjectType_TxNRTDD, 0, NULL);
    tx1= (openair0_cfg->tx_num_channels > 1) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_TxNRFDD : ORI_ObjectType_TxNRTDD, 1, NULL) : NULL;
    tx2= (openair0_cfg->tx_num_channels > 2) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_TxNRFDD : ORI_ObjectType_TxNRTDD, 2, NULL) : NULL;
    tx3= (openair0_cfg->tx_num_channels > 3) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_TxNRFDD : ORI_ObjectType_TxNRTDD, 3, NULL) : NULL;
    rx0= ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxNRFDD : ORI_ObjectType_RxNRTDD, 0, NULL);
    rx1= (openair0_cfg->rx_num_channels > 1) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxNRFDD : ORI_ObjectType_RxNRTDD, 1, NULL) : NULL;
    rx2= (openair0_cfg->rx_num_channels > 2) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxNRFDD : ORI_ObjectType_RxNRTDD, 2, NULL) : NULL;
    rx3= (openair0_cfg->rx_num_channels > 3) ? ORI_FindObject(ori, openair0_cfg->duplex_mode == duplex_mode_FDD ?ORI_ObjectType_RxNRFDD : ORI_ObjectType_RxNRTDD, 3, NULL) : NULL;

  }
  ORI_Object_s *link= ORI_FindObject(ori, ORI_ObjectType_ORILink, 0, NULL);
  if (tx0 == NULL || 
      (tx1 == NULL && openair0_cfg->tx_num_channels > 1) || 
      (tx2 == NULL && openair0_cfg->tx_num_channels > 2) ||
      (tx3 == NULL && openair0_cfg->tx_num_channels > 3) ||
      rx0 == NULL || 
      (rx1 == NULL && openair0_cfg->rx_num_channels > 1) || 
      (rx2 == NULL && openair0_cfg->rx_num_channels > 2) || 
      (rx3 == NULL && openair0_cfg->rx_num_channels > 3) || 
      link == NULL) {
         printf("tx0 %p, tx1 %p, tx2 %p, tx3 %p, rx0 %p, rx1 %p, rx2 %p, rx3 %p\n",tx0,tx1,tx2,tx3,rx0,rx1,rx2,rx3); 
         return (-1);
  }
 /*******************************************************************
   * UNLOCK Link 
   *******************************************************************/


  result = ORI_ObjectStateModification(ori, link, ORI_AST_Unlocked, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }

 /*******************************************************************
   * UNLOCK TX0
   *******************************************************************/


  /* Put Tx0 into service */
  result = ORI_ObjectStateModification(ori, tx0, ORI_AST_Unlocked, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));



  /*******************************************************************
   * UNLOCK TX1
   *******************************************************************/

  if (tx1) { 
    printf("\n\n\n========================================================\n");

    /* Put Tx1 into service */
   result = ORI_ObjectStateModification(ori, tx1, ORI_AST_Unlocked, &RE_result);
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));
  }
  /*******************************************************************
   * UNLOCK TX2
   *******************************************************************/

  if (tx2) { 
    printf("\n\n\n========================================================\n");

    /* Put Tx1 into service */
   result = ORI_ObjectStateModification(ori, tx2, ORI_AST_Unlocked, &RE_result);
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));
  }
  /*******************************************************************
   * UNLOCK TX3
   *******************************************************************/

  if (tx3) { 
    printf("\n\n\n========================================================\n");

    /* Put Tx3 into service */
   result = ORI_ObjectStateModification(ori, tx3, ORI_AST_Unlocked, &RE_result);
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));
  }
  /*******************************************************************
   * UNLOCK RX0
   *******************************************************************/

  printf("\n\n\n========================================================\n");

  /* Put Rx0 into service */
  result = ORI_ObjectStateModification(ori, rx0, ORI_AST_Unlocked, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));




  /*******************************************************************
   * UNLOCK RX1
   *******************************************************************/
  if (rx1) {
    printf("\n\n\n========================================================\n");

    /* Put Rx1 into service */
    result = ORI_ObjectStateModification(ori, rx1, ORI_AST_Unlocked, &RE_result);
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));
  }

  /*******************************************************************
   * UNLOCK RX2
   *******************************************************************/
  if (rx2) {
    printf("\n\n\n========================================================\n");

    /* Put Rx1 into service */
    result = ORI_ObjectStateModification(ori, rx2, ORI_AST_Unlocked, &RE_result);
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));
  }
  /*******************************************************************
   * UNLOCK RX3
   *******************************************************************/
  if (rx3) {
    printf("\n\n\n========================================================\n");

    /* Put Rx3 into service */
    result = ORI_ObjectStateModification(ori, rx3, ORI_AST_Unlocked, &RE_result);
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectStateModify: %s\n", ORI_Result_Print(RE_result));
  }

  device->fhstate.active=1; 
  return(0);

}

uint32_t to_nrarfcn(int nr_bandP,
                    uint64_t dl_CarrierFreq,
                    uint8_t scs_index,
                    uint32_t bw);

int aw2s_oriinit(openair0_device *device) {


  ORI_s * 		ori;
  ORI_Result_e	result;
  ORI_Result_e	RE_result;
  ORI_Object_s * 	objects[100];
  uint32_t 		numObjects;
  uint32_t		i;

  eth_params_t *eth_params = device->eth_params;

  openair0_config_t *openair0_cfg = device->openair0_cfg;

  
  /*******************************************************************
   * CREATION AND CONNECTION
   *******************************************************************/

  printf("Initializing the ORI control interface\n");

  /* Create context */
  ori = ORI_Create();
  if(!ori)
    {
      printf("Failed to create ORI context\n");
      return -1;
    }
  device->thirdparty_priv = (void *)ori;
  ori->userData = (void *)ori;
  ori->indicationCallback = cb;
	
  /* Connect... */
  printf("Trying to connect to AW2S device on %s : %d\n",eth_params->remote_addr, eth_params->remote_portc);
  result = ORI_Connect(ori, eth_params->remote_addr, eth_params->remote_portc, 3000, 0);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_Connect failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }
	
  /* First health check */
  result = ORI_HealthCheck(ori, 6000, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_HealthCheck failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_HealthCheck: %s\n", ORI_Result_Print(RE_result));
	
  /* Set RE time */
  result = ORI_SetTime(ori, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_SetTime failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_SetTime: %s\n", ORI_Result_Print(RE_result));




  /*******************************************************************
   * SOFTWARE PARAMETERS ALIGNMENT
   *******************************************************************/

  /* Report all current objects parameters */
  result = ORI_ObjectParamReport(ori, NULL, 0, ORI_ObjectParam_All, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectParamReport failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }
  
   printf("ORI_ObjectParamReport: %s\n", ORI_Result_Print(RE_result));




  /*******************************************************************
   * TX PATHS CREATION
   *******************************************************************/

  /* Prepare parameters */
  ORI_ObjectTypeRef_s txTypeRef = { NULL, openair0_cfg->duplex_mode == duplex_mode_FDD ? 
                                          (openair0_cfg->nr_flag == 0 ? ORI_ObjectType_TxEUtraFDD : ORI_ObjectType_TxNRFDD):
                                          (openair0_cfg->nr_flag == 0 ? ORI_ObjectType_TxEUtraTDD : ORI_ObjectType_TxNRTDD)};
  ORI_ObjectParams_u txParams;
  ORI_ObjectParam_e txParamList[9];
  ORI_Result_e txParamResult[9];
  int num_txparams;

  txParamList[0] = ORI_ObjectParam_SigPath_antPort; 
  txParamList[1] = ORI_ObjectParam_SigPath_axcW; 
  txParamList[2] = ORI_ObjectParam_SigPath_axcB;
  txParamList[3] = ORI_ObjectParam_SigPath_chanBW; 
  txParamList[4] = (openair0_cfg->nr_flag ==0) ? ORI_ObjectParam_SigPath_earfcn : ORI_ObjectParam_SigPath_AWS_arfcn; 
  txParamList[5] = ORI_ObjectParam_TxSigPath_maxTxPwr;
  num_txparams = 6;


  /* Create tx0 */
  ORI_Object_s * tx0;
  printf("AW2S: duplex_mode %d, tx_bw %f, rx_bw %f\n",openair0_cfg->duplex_mode,openair0_cfg->tx_bw,openair0_cfg->rx_bw);
  if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0) {
    txParams.TxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    txParams.TxEUtraFDD.axcW = 1;
    txParams.TxEUtraFDD.axcB = 0;
    txParams.TxEUtraFDD.chanBW = openair0_cfg->tx_bw/100e3;
    txParams.TxEUtraTDD.earfcn = to_earfcn_DL_aw2s(-1,(long long int)openair0_cfg->tx_freq[0],txParams.TxEUtraTDD.chanBW);
    txParams.TxEUtraFDD.maxTxPwr = 430-((int)openair0_cfg->tx_gain[0]*10);
  }
  else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1) {
    txParams.TxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    txParams.TxNRFDD.axcW = 1;
    txParams.TxNRFDD.axcB = 0;
    txParams.TxNRFDD.chanBW = openair0_cfg->tx_bw/100e3;

    txParams.TxNRFDD.AWS_arfcn = to_nrarfcn(openair0_cfg->nr_band,(long long int)openair0_cfg->tx_freq[0],openair0_cfg->nr_scs_for_raster,(uint32_t)openair0_cfg->tx_bw);
    txParams.TxNRFDD.maxTxPwr = 430-((int)openair0_cfg->tx_gain[0]*10);
  }
  else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0) {
    txParams.TxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    txParams.TxEUtraTDD.axcW = 1;
    txParams.TxEUtraTDD.axcB = 0;
    txParams.TxEUtraTDD.chanBW = openair0_cfg->tx_bw/100e3;
    txParams.TxEUtraTDD.earfcn = to_earfcn_DL_aw2s(-1,(long long int)openair0_cfg->tx_freq[0],txParams.TxEUtraTDD.chanBW);
    txParams.TxEUtraTDD.maxTxPwr = 430-((int)openair0_cfg->tx_gain[0]*10);

    printf("AW2S: Configuring for LTE TDD, EARFCN %u, Power %d, BW %d\n",
	txParams.TxEUtraTDD.earfcn,txParams.TxEUtraTDD.maxTxPwr,txParams.TxEUtraTDD.chanBW);
  }
  else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1) {
    txParams.TxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    txParams.TxNRTDD.axcW = 1;
    txParams.TxNRTDD.axcB = 0;
    txParams.TxNRTDD.chanBW = openair0_cfg->tx_bw/100e3;
    txParams.TxNRTDD.AWS_arfcn = to_nrarfcn(openair0_cfg->nr_band,(long long int)openair0_cfg->tx_freq[0],openair0_cfg->nr_scs_for_raster,(uint32_t)openair0_cfg->tx_bw);
    txParams.TxNRTDD.maxTxPwr = 430-((int)openair0_cfg->tx_gain[0]*10);

    printf("AW2S: Configuring for NR TDD, NRARFCN %u, Power %d, BW %d\n",
	txParams.TxNRTDD.AWS_arfcn,txParams.TxNRTDD.maxTxPwr,txParams.TxNRTDD.chanBW);
  }
  else {
    aw2s_oricleanup(device);
    return -1;
  }
  result = ORI_ObjectCreation(ori, txTypeRef, txParams, txParamList, num_txparams, txParamResult, &tx0, &RE_result);
  if(RE_result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectCreation (txParams0.TxEUtra/NR/FDD/TDD) failed with error: %s (%s,%s,%s,%s,%s,%s\n", ORI_Result_Print(RE_result),
	ORI_Result_Print(txParamResult[0]),
        ORI_Result_Print(txParamResult[1]),
        ORI_Result_Print(txParamResult[2]),
        ORI_Result_Print(txParamResult[3]),
        ORI_Result_Print(txParamResult[4]),
        ORI_Result_Print(txParamResult[5]));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_ObjectCreation (txParams0.TxEUtra/NR/FDD/TDD): %s\n", ORI_Result_Print(RE_result));


  /* Create tx1 */
  if (openair0_cfg->tx_num_channels > 1) {
    ORI_Object_s * tx1;
    if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0) 
       txParams.TxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1)  
       txParams.TxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0)  
       txParams.TxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1)  
       txParams.TxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);
    result = ORI_ObjectCreation(ori, txTypeRef, txParams, txParamList, num_txparams, txParamResult, &tx1, &RE_result);
    if(RE_result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectCreation (txParams1.TxEUtra/NR/FDD/TDD) failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectCreation (txParams1.TxEUtra/NR/FDD/TDD): %s\n", ORI_Result_Print(RE_result));
  }

  if (openair0_cfg->tx_num_channels > 2) {
    ORI_Object_s * tx2;
    if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0) 
       txParams.TxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1)  
       txParams.TxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0)  
       txParams.TxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1)  
       txParams.TxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);
    result = ORI_ObjectCreation(ori, txTypeRef, txParams, txParamList, num_txparams, txParamResult, &tx2, &RE_result);
    if(RE_result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectCreation (txParams2.TxEUtra/NR/FDD/TDD) failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectCreation (txParams2.TxEUtra/NR/FDD/TDD): %s\n", ORI_Result_Print(RE_result));
  }
  if (openair0_cfg->tx_num_channels == 4) {
    ORI_Object_s * tx3;
    if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0) 
       txParams.TxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1)  
       txParams.TxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0)  
       txParams.TxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1)  
       txParams.TxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);
    result = ORI_ObjectCreation(ori, txTypeRef, txParams, txParamList, num_txparams, txParamResult, &tx3, &RE_result);
    if(RE_result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectCreation (txParams3.TxEUtra/NRFDD/TDD) failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectCreation (txParams3.TxEUtra/NRFDD/TDD): %s\n", ORI_Result_Print(RE_result));
  }


  /*******************************************************************
   * RX PATHS CREATION
   *******************************************************************/

  /* Prepare parameters */
  ORI_ObjectTypeRef_s rxTypeRef = { NULL,  openair0_cfg->duplex_mode == duplex_mode_FDD ? 
                                           (openair0_cfg->nr_flag == 0 ? ORI_ObjectType_RxEUtraFDD : ORI_ObjectType_RxNRFDD ) :
                                           (openair0_cfg->nr_flag == 0 ? ORI_ObjectType_RxEUtraTDD : ORI_ObjectType_RxNRTDD)};
  ORI_ObjectParams_u rxParams;
  ORI_ObjectParam_e rxParamList[5] = { ORI_ObjectParam_SigPath_antPort, ORI_ObjectParam_SigPath_axcW, ORI_ObjectParam_SigPath_axcB,
				       ORI_ObjectParam_SigPath_chanBW, 
                                       openair0_cfg->nr_flag == 0 ? ORI_ObjectParam_SigPath_earfcn : ORI_ObjectParam_SigPath_AWS_arfcn};
  ORI_Result_e rxParamResult[5];
  int num_rxparams = 5;
  /* Create rx0 */
  ORI_Object_s * rx0;
  if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0) {
    rxParams.RxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    rxParams.RxEUtraFDD.axcW = 1;
    rxParams.RxEUtraFDD.axcB = 0;
    rxParams.RxEUtraFDD.chanBW = txParams.TxEUtraFDD.chanBW;
    rxParams.RxEUtraFDD.earfcn = to_earfcn_UL_aw2s(-1,(long long int)openair0_cfg->rx_freq[0],txParams.TxEUtraFDD.chanBW);
    result = ORI_ObjectCreation(ori, rxTypeRef, rxParams, rxParamList, num_rxparams, rxParamResult, &rx0, &RE_result);
  }    
  else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1) {
    rxParams.RxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    rxParams.RxNRFDD.axcW = 1;
    rxParams.RxNRFDD.axcB = 0;
    rxParams.RxNRFDD.chanBW = txParams.TxNRFDD.chanBW;
    rxParams.RxNRFDD.AWS_arfcn = to_nrarfcn(openair0_cfg->nr_band,(long long int)openair0_cfg->rx_freq[0],openair0_cfg->nr_scs_for_raster,openair0_cfg->rx_bw);
    result = ORI_ObjectCreation(ori, rxTypeRef, rxParams, rxParamList, num_rxparams, rxParamResult, &rx0, &RE_result);
  }    
  else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0) {
    rxParams.RxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    rxParams.RxEUtraTDD.axcW = 1;
    rxParams.RxEUtraTDD.axcB = 0;
    rxParams.RxEUtraTDD.chanBW = txParams.TxEUtraTDD.chanBW;
    rxParams.RxEUtraTDD.earfcn = txParams.TxEUtraTDD.earfcn;

    printf("AW2S: Creating RX0 for EARFCN %u\n",rxParams.RxEUtraTDD.earfcn);
 
    result = ORI_ObjectCreation(ori, rxTypeRef, rxParams, rxParamList, num_rxparams, rxParamResult, &rx0, &RE_result);

  }
  else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1) {
    rxParams.RxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 0, NULL);
    rxParams.RxNRTDD.axcW = 1;
    rxParams.RxNRTDD.axcB = 0;
    rxParams.RxNRTDD.chanBW = txParams.TxNRTDD.chanBW;
    rxParams.RxNRTDD.AWS_arfcn = txParams.TxNRTDD.AWS_arfcn;

    printf("AW2S: Creating RX0 for NRARFCN %u\n",rxParams.RxNRTDD.AWS_arfcn);
 
    result = ORI_ObjectCreation(ori, rxTypeRef, rxParams, rxParamList, num_rxparams, rxParamResult, &rx0, &RE_result);

  }

  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectCreation (rxParams0.RxEUtraFDD/TDD) failed with error: %s\n", ORI_Result_Print(result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_ObjectCreation (rxParams0.RxEUtra/NR/FDD/TDD): %s\n", ORI_Result_Print(RE_result));
  
  if (openair0_cfg->rx_num_channels > 1) {  
    /* Create rx1 */
    ORI_Object_s * rx1;
    if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0)  
       rxParams.RxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1)  
       rxParams.RxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0)  
       rxParams.RxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1)  
       rxParams.RxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 1, NULL);

    result = ORI_ObjectCreation(ori, rxTypeRef, rxParams, rxParamList, num_rxparams, rxParamResult, &rx1, &RE_result);
  
  
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectCreation (rxParams1.RxEUtra/NR/FDD/TDD) failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectCreation (rxParams1.RxEUtra/NR/FDD/TDD): %s\n", ORI_Result_Print(RE_result));
  }

  if (openair0_cfg->rx_num_channels > 2) {  
    /* Create rx2 */
    ORI_Object_s * rx2;
    if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0)  
       rxParams.RxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1)  
       rxParams.RxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0)  
       rxParams.RxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1)  
       rxParams.RxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 2, NULL);

    result = ORI_ObjectCreation(ori, rxTypeRef, rxParams, rxParamList, num_rxparams, rxParamResult, &rx2, &RE_result);
  
  
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectCreation (rxParams2.RxEUtra/NR/FDD/TDD) failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectCreation (rxParams2.RxEUtra/NR/FDD/TDD): %s\n", ORI_Result_Print(RE_result));
  }
  if (openair0_cfg->rx_num_channels > 3) {  
    /* Create rx3 */
    ORI_Object_s * rx3;
    if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 0)  
       rxParams.RxEUtraFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_FDD && openair0_cfg->nr_flag == 1)  
       rxParams.RxNRFDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 0)  
       rxParams.RxEUtraTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);
    else if (openair0_cfg->duplex_mode == duplex_mode_TDD && openair0_cfg->nr_flag == 1)  
       rxParams.RxNRTDD.antPort = ORI_FindObject(ori, ORI_ObjectType_AntennaPort, 3, NULL);

    result = ORI_ObjectCreation(ori, rxTypeRef, rxParams, rxParamList, num_rxparams, rxParamResult, &rx3, &RE_result);
  
  
    if(result != ORI_Result_SUCCESS)
      {
        printf("ORI_ObjectCreation (rxParams3.RxEUtra/NR/FDD/TDD) failed with error: %s\n", ORI_Result_Print(result));
        aw2s_oricleanup(device);
        return -1;
      }
    printf("ORI_ObjectCreation (rxParams3.RxEUtra/NR/FDD/TDD): %s\n", ORI_Result_Print(RE_result));
  }
  /* Create link */

  ORI_ObjectParams_u linkParams;
  ORI_ObjectParam_e linkParamList[3] = {ORI_ObjectParam_ORILink_AWS_remoteMAC, ORI_ObjectParam_ORILink_AWS_remoteIP,ORI_ObjectParam_ORILink_AWS_remoteUdpPort};
  ORI_Result_e linkParamResult[3];

  ORI_Object_s *link= ORI_FindObject(ori, ORI_ObjectType_ORILink, 0, NULL);
  linkParams.ORILink.AWS_remoteUdpPort = eth_params->my_portd;


  if (get_mac_addr(eth_params->local_if_name,linkParams.ORILink.AWS_remoteMAC) < 0) return(-1);
  inet_pton(AF_INET,eth_params->my_addr,(struct in_addr*)linkParams.ORILink.AWS_remoteIP);
  result = ORI_ObjectParamModify(ori,link,linkParams,linkParamList,3,linkParamResult,&RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectParamModify (linkParams) failed with error: %s\n", ORI_Result_Print(RE_result));
      aw2s_oricleanup(device);
      return -1;
    }
  result = ORI_ObjectStateModification(ori, link, ORI_AST_Locked, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectStateModify failed with error: %s\n", ORI_Result_Print(RE_result));
      aw2s_oricleanup(device);
      return -1;
    }

  printf("ORI_ObjectParamModify (linkParams): %s (%d,%d,%d)\n", ORI_Result_Print(RE_result),linkParamResult[0],linkParamResult[1],linkParamResult[2]);

  /*******************************************************************
   * FULL OBJECT PARAMETERS UPDATE
   *******************************************************************/

  printf("\n\n\n========================================================\n");

  result = ORI_ObjectParamReport(ori, NULL, 0, ORI_ObjectParam_All, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectParamReport failed with error: %s\n", ORI_Result_Print(RE_result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_ObjectParamReport: %s\n", ORI_Result_Print(RE_result));





  /*******************************************************************
   * FULL OBJECT STATES UPDATE
   *******************************************************************/

  printf("\n\n\n========================================================\n");

  result = ORI_ObjectStateReport(ori, NULL, 0, ORI_StateType_All, ORI_EventDrivenReport_True, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectStateReport failed with error: %s\n", ORI_Result_Print(RE_result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_ObjectStateReport: %s\n", ORI_Result_Print(RE_result));

  /* Print reported states for all objects (some objects have non-applicable states though) */
  numObjects = ORI_GetAllObjects(ori, objects, 100, NULL);
  for(i=0; i<numObjects; i++)
    printf("%s:%d -> %s / %s\n", ORI_ObjectType_Print(objects[i]->typeRef.type), objects[i]->instanceNumber, ORI_AST_Print(objects[i]->ast), ORI_FST_Print(objects[i]->fst));





  /*******************************************************************
   * FULL OBJECT FAULTS UPDATE
   *******************************************************************/

  printf("\n\n\n========================================================\n");

  result = ORI_ObjectFaultReport(ori, NULL, 0, ORI_EventDrivenReport_True, &RE_result);
  if(result != ORI_Result_SUCCESS)
    {
      printf("ORI_ObjectFaultReport failed with error: %s\n", ORI_Result_Print(RE_result));
      aw2s_oricleanup(device);
      return -1;
    }
  printf("ORI_ObjectFaultReport: %s\n", ORI_Result_Print(RE_result));


  /*******************************************************************
   * PRINT STATES
   *******************************************************************/

  printf("\n\n\n========================================================\n");

  /* Due to event driven reporting set to "true", the states modifications of the objects should be reflected back into the model. Print states for all objects (some objects have non-applicable states though) */
  numObjects = ORI_GetAllObjects(ori, objects, 100, NULL);
  for(i=0; i<numObjects; i++)
    printf("%s:%d -> %s / %s\n", ORI_ObjectType_Print(objects[i]->typeRef.type), objects[i]->instanceNumber, ORI_AST_Print(objects[i]->ast), ORI_FST_Print(objects[i]->fst));





  return 0;
}


int transport_init(openair0_device *device, openair0_config_t *openair0_cfg, eth_params_t * eth_params ) {

  printf("Initializing AW2S (%p,%p,%p)\n",aw2s_oriinit,aw2s_oricleanup,aw2s_startstreaming); 
  device->thirdparty_init           = aw2s_oriinit;
  device->thirdparty_cleanup        = aw2s_oricleanup;
  device->thirdparty_startstreaming = aw2s_startstreaming;

  return(0);
}
