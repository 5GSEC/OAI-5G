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

/*
                                mme_app.c
                             -------------------
  AUTHOR  : Javier Morgade
  COMPANY : VICOMTECH Spain
  EMAIL   : javier.morgade@ieee.org
*/

#include <string.h>
#include <stdio.h>

#include "mme_app.h"
#include "mme_config.h"
#include "assertions.h"
#include "common/ran_context.h"
#include "executables/lte-softmodem.h"

#include "common/utils/LOG/log.h"

# include "intertask_interface.h"
#   include "s1ap_eNB.h"
#   include "sctp_eNB_task.h"
#   include "openair3/ocp-gtpu/gtp_itf.h"

#   include "x2ap_eNB.h"
#   include "x2ap_messages_types.h"
#   include "m2ap_eNB.h"
#   include "m3ap_MME.h"
#   include "m2ap_messages_types.h"
//#   include "m3ap_eNB.h"
#   include "m3ap_messages_types.h"
#   define X2AP_ENB_REGISTER_RETRY_DELAY   10

#include "openair1/PHY/INIT/phy_init.h"

extern RAN_CONTEXT_t RC;

#   define MCE_REGISTER_RETRY_DELAY 10

#include "executables/lte-softmodem.h"




/*------------------------------------------------------------------------------*/

//static uint32_t MCE_app_register(ngran_node_t node_type,uint32_t mce_id_start, uint32_t mce_id_end) {
//  uint32_t         mce_id;
//  MessageDef      *msg_p;
//  uint32_t         register_mce_pending = 0;
//
//  for (mce_id = mce_id_start; (mce_id < mce_id_end) ; mce_id++) {
//    {
//      // M3AP registration
//        /* note:  there is an implicit relationship between the data structure and the message name */
//        msg_p = itti_alloc_new_message (TASK_MME_APP, 0, M3AP_REGISTER_MCE_REQ);
//        //RCconfig_S1(msg_p, mce_id);
//
//        //if (mce_id == 0) 
//		//RCconfig_gtpu();
//
//        //LOG_I(MME_APP,"default drx %d\n",((M3AP_REGISTER_MCE_REQ(msg_p)).default_drx));
//
//        LOG_I(ENB_APP,"[MCE %d] MCE_app_register via M3AP for instance %d\n", mce_id, ENB_MODULE_ID_TO_INSTANCE(mce_id));
//        itti_send_msg_to_task (TASK_M3AP, ENB_MODULE_ID_TO_INSTANCE(mce_id), msg_p);
//
//      //{ // S1AP registration
//      //  /* note:  there is an implicit relationship between the data structure and the message name */
//      //  msg_p = itti_alloc_new_message (TASK_ENB_APP, 0, S1AP_REGISTER_ENB_REQ);
//      //  RCconfig_S1(msg_p, enb_id);
//
//      //  if (enb_id == 0) RCconfig_gtpu();
//
//      //  LOG_I(ENB_APP,"default drx %d\n",((S1AP_REGISTER_ENB_REQ(msg_p)).default_drx));
//
//      //  LOG_I(ENB_APP,"[eNB %d] eNB_app_register via S1AP for instance %d\n", enb_id, ENB_MODULE_ID_TO_INSTANCE(enb_id));
//      //  itti_send_msg_to_task (TASK_S1AP, ENB_MODULE_ID_TO_INSTANCE(enb_id), msg_p);
//      //}
//
//      register_mce_pending++;
//    }
//  }
//
//  return register_mce_pending;
//}


/*------------------------------------------------------------------------------*/
//static uint32_t MCE_app_register_x2(uint32_t mce_id_start, uint32_t mce_id_end) {
//  uint32_t         mce_id;
//  MessageDef      *msg_p;
//  uint32_t         register_mce_m2_pending = 0;
//
//  for (mce_id = mce_id_start; (mce_id < mce_id_end) ; mce_id++) {
//    {
//      msg_p = itti_alloc_new_message (TASK_ENB_APP, 0, X2AP_REGISTER_ENB_REQ);
//      RCconfig_X2(msg_p, mce_id);
//      itti_send_msg_to_task (TASK_X2AP, ENB_MODULE_ID_TO_INSTANCE(mce_id), msg_p);
//      register_mce_x2_pending++;
//    }
//  }
//
//  return register_mce_x2_pending;
//}

/*------------------------------------------------------------------------------*/
//static uint32_t MCE_app_register_m2(uint32_t mce_id_start, uint32_t mce_id_end) {
//  uint32_t         mce_id;
//  MessageDef      *msg_p;
//  uint32_t         register_mce_m2_pending = 0;
//
//  LOG_W(MME_APP,"Register ...");
//  for (mce_id = mce_id_start; (mce_id < mce_id_end) ; mce_id++) {
//    {
//  //	LOG_W(MME_APP,"Register commes inside ...\n");
//      msg_p = itti_alloc_new_message (TASK_MME_APP, 0, M2AP_REGISTER_MCE_REQ);
//      //RCconfig_M2_MCE(msg_p, mce_id);
//      itti_send_msg_to_task (TASK_M2AP_MCE, ENB_MODULE_ID_TO_INSTANCE(mce_id), msg_p);
//  //	LOG_W(MME_APP,"Register sent ...\n");
//      register_mce_m2_pending++;
//    }
//  }
//
//  return register_mce_m2_pending;
//}
//
//
//
static uint32_t MME_app_handle_m3ap_setup_req(instance_t instance){
	
  	//uint32_t         mce_id=0;
  	MessageDef      *msg_p;
        msg_p = itti_alloc_new_message (TASK_MME_APP, 0, M3AP_SETUP_RESP);
        itti_send_msg_to_task (TASK_M3AP_MME, ENB_MODULE_ID_TO_INSTANCE(instance), msg_p);
	
	return 0;
}
static uint32_t MME_app_handle_m3ap_session_start_resp(instance_t instance){
	
  
	return 0;
}
//
//static uint32_t MME_app_handle_m3ap_session_stop_resp(instance_t instance){
//	
//  
//	return 0;
//}



//
//static uint32_t MCE_app_send_m2ap_mbms_scheduling_information(instance_t instance){
//	
//  	uint32_t         mce_id=0;
//  	MessageDef      *msg_p;
//        msg_p = itti_alloc_new_message (TASK_MME_APP, 0, M2AP_MBMS_SCHEDULING_INFORMATION);
//        itti_send_msg_to_task (TASK_M2AP_MCE, ENB_MODULE_ID_TO_INSTANCE(instance), msg_p);
//	
//	return 0;
//}
//
static uint32_t MME_app_send_m3ap_session_start_req(instance_t instance){
	
  	//uint32_t         mce_id=0;
  	MessageDef      *msg_p;
        msg_p = itti_alloc_new_message (TASK_MME_APP, 0, M3AP_MBMS_SESSION_START_REQ);
        itti_send_msg_to_task (TASK_M3AP_MME, ENB_MODULE_ID_TO_INSTANCE(instance), msg_p);
	
	return 0;
}
//static uint32_t MME_app_send_m3ap_session_stop_req(instance_t instance){
//	
//  	//uint32_t         mce_id=0;
//  	MessageDef      *msg_p;
//        msg_p = itti_alloc_new_message (TASK_MME_APP, 0, M3AP_MBMS_SESSION_STOP_REQ);
//        itti_send_msg_to_task (TASK_M3AP_MME, ENB_MODULE_ID_TO_INSTANCE(instance), msg_p);
//	
//	return 0;
//}
static uint32_t MME_app_send_m3ap_session_update_req(instance_t instance){
	
  	//uint32_t         mce_id=0;
  	MessageDef      *msg_p;
        msg_p = itti_alloc_new_message (TASK_MME_APP, 0, M3AP_MBMS_SESSION_UPDATE_REQ);
        itti_send_msg_to_task (TASK_M3AP_MME, ENB_MODULE_ID_TO_INSTANCE(instance), msg_p);
	
	return 0;
}
/*------------------------------------------------------------------------------*/
void *MME_app_task(void *args_p) {
  //uint32_t                        mce_nb = RC.nb_inst; 
  //uint32_t                        mce_id_start = 0;
  //uint32_t                        mce_id_end = mce_id_start + mce_nb;
  //uint32_t                        register_mce_pending=0;
  //uint32_t                        registered_mce=0;
  //long                            mce_register_retry_timer_id;
  //uint32_t                        m2_register_mce_pending = 0;
 // uint32_t                        x2_registered_mce = 0;
 // long                            x2_mce_register_retry_timer_id;
 // uint32_t                        m2_register_mce_pending = 0;
 // uint32_t                        m2_registered_mce = 0;
 // long                            m2_mce_register_retry_timer_id;
  long                            m3_mme_register_session_start_timer_id;
  MessageDef                     *msg_p           = NULL;
  instance_t                      instance;
  int                             result;
  /* for no gcc warnings */
  (void)instance;
  itti_mark_task_ready (TASK_MME_APP);

  /* Try to register each MCE */
  // This assumes that node_type of all RRC instances is the same
  if (EPC_MODE_ENABLED) {
    //register_mce_pending = MCE_app_register(RC.rrc[0]->node_type, mce_id_start, mce_id_end);
  }

    /* Try to register each MCE with each other */
 // if (is_x2ap_enabled() && !NODE_IS_DU(RC.rrc[0]->node_type)) {
 //   x2_register_enb_pending = MCE_app_register_x2 (enb_id_start, enb_id_end);
 // }
  if ( is_m3ap_MME_enabled() ){
 	RCconfig_MME();
  }
 // /* Try to register each MCE with MCE each other */
 //if (is_m3ap_MME_enabled() /*&& !NODE_IS_DU(RC.rrc[0]->node_type)*/) {
   //m2_register_mce_pending = MCE_app_register_m2 (mce_id_start, mce_id_end);
 //}

  do {
    // Wait for a message
    itti_receive_msg (TASK_MME_APP, &msg_p);
    instance = ITTI_MSG_DESTINATION_INSTANCE (msg_p);

    switch (ITTI_MSG_ID(msg_p)) {
    case TERMINATE_MESSAGE:
      LOG_W(MME_APP, " *** Exiting MME_APP thread\n");
      itti_exit_task ();
      break;

    case MESSAGE_TEST:
      LOG_I(MME_APP, "MME_APP Received %s\n", ITTI_MSG_NAME(msg_p));
      break;

    case M3AP_REGISTER_MCE_CNF: //M3AP_REGISTER_MCE_CNF debería
      //AssertFatal(!NODE_IS_DU(RC.rrc[0]->node_type), "Should not have received S1AP_REGISTER_ENB_CNF\n");
       // if (EPC_MODE_ENABLED) {
       //   LOG_I(MME_APP, "[MCE %d] Received %s: associated MME %d\n", instance, ITTI_MSG_NAME (msg_p),
       //         M3AP_REGISTER_MCE_CNF(msg_p).nb_mme);
       //   DevAssert(register_mce_pending > 0);
       //   register_mce_pending--;

       //   /* Check if at least MCE is registered with one MME */
       //   if (M3AP_REGISTER_MCE_CNF(msg_p).nb_mme > 0) {
       //     registered_mce++;
       //   }

       //   /* Check if all register MCE requests have been processed */
       //   if (register_mce_pending == 0) {
       //     if (registered_mce == mce_nb) {
       //       /* If all MCE are registered, start L2L1 task */
       //      // MessageDef *msg_init_p;
       //      // msg_init_p = itti_alloc_new_message (TASK_ENB_APP, 0, INITIALIZE_MESSAGE);
       //      // itti_send_msg_to_task (TASK_L2L1, INSTANCE_DEFAULT, msg_init_p);
       //     } else {
       //       LOG_W(MME_APP, " %d MCE not associated with a MME, retrying registration in %d seconds ...\n",
       //             mce_nb - registered_mce,  MCE_REGISTER_RETRY_DELAY);

       //       /* Restart the MCE registration process in MCE_REGISTER_RETRY_DELAY seconds */
       //       if (timer_setup (MCE_REGISTER_RETRY_DELAY, 0, TASK_MME_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT,
       //                        NULL, &mce_register_retry_timer_id) < 0) {
       //         LOG_E(MME_APP, " Can not start MCE register retry timer, use \"sleep\" instead!\n");
       //         sleep(MCE_REGISTER_RETRY_DELAY);
       //         /* Restart the registration process */
       //         registered_mce = 0;
       //         register_mce_pending = MCE_app_register (RC.rrc[0]->node_type,mce_id_start, mce_id_end);
       //       }
       //     }
       //   }
       // } /* if (EPC_MODE_ENABLED) */

      break;

   // case M3AP_SETUP_RESP:

   //   //LOG_I(MME_APP, "Received %s: associated ngran_MCE_CU %s with %d cells to activate\n", ITTI_MSG_NAME (msg_p),
   //   
   //   handle_m3ap_setup_resp(&M3AP_SETUP_RESP(msg_p));

   //   DevAssert(register_mce_pending > 0);
   //   register_mce_pending--;

   //   /* Check if at least MCE is registered with one MME */
   //   //if (M3AP_SETUP_RESP(msg_p).num_cells_to_activate > 0) {
   //   //  registered_enb++;
   //   //}

   //   /* Check if all register MCE requests have been processed */
   //   if (register_mce_pending == 0) {
   //     if (registered_mce == mce_nb) {
   //       /* If all MCE cells are registered, start L2L1 task */
   //       MessageDef *msg_init_p;

   //       //msg_init_p = itti_alloc_new_message (TASK_MME_APP, 0, INITIALIZE_MESSAGE);
   //       //itti_send_msg_to_task (TASK_L2L1, INSTANCE_DEFAULT, msg_init_p);

   //     } else {
   //       LOG_W(MME_APP, " %d MCE not associated with a MME, retrying registration in %d seconds ...\n",
   //             mce_nb - registered_mce,  MCE_REGISTER_RETRY_DELAY);

   //       /* Restart the MCE registration process in MCE_REGISTER_RETRY_DELAY seconds */
   //       if (timer_setup (MCE_REGISTER_RETRY_DELAY, 0, TASK_MME_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT,
   //                        NULL, &mce_register_retry_timer_id) < 0) {
   //         LOG_E(ENB_APP, " Can not start MCE register retry timer, use \"sleep\" instead!\n");

   //         sleep(MCE_REGISTER_RETRY_DELAY);
   //         /* Restart the registration process */
   //         registered_mce = 0;
   //         register_mce_pending = MCE_app_register (RC.rrc[0]->node_type,mce_id_start, mce_id_end);//, enb_properties_p);
   //       }
   //     }
   //   }

   //   break;

    case M3AP_DEREGISTERED_MCE_IND: //M3AP_DEREGISTERED_MCE_IND debería
      if (EPC_MODE_ENABLED) {
  	LOG_W(MME_APP, "[MCE %ld] Received %s: associated MME %d\n", instance, ITTI_MSG_NAME (msg_p),
  	      M3AP_DEREGISTERED_MCE_IND(msg_p).nb_mme);
  	/* TODO handle recovering of registration */
      }

      break;

    case TIMER_HAS_EXPIRED:
        //if (EPC_MODE_ENABLED) {
	//      LOG_I(MME_APP, " Received %s: timer_id %ld\n", ITTI_MSG_NAME (msg_p), TIMER_HAS_EXPIRED(msg_p).timer_id);

	//      if (TIMER_HAS_EXPIRED (msg_p).timer_id == mce_register_retry_timer_id) {
	//	/* Restart the registration process */
	//	registered_mce = 0;
	//	//register_mce_pending = MCE_app_register (RC.rrc[0]->node_type, mce_id_start, mce_id_end);
	//      }

	//      //if (TIMER_HAS_EXPIRED (msg_p).timer_id == x2_mce_register_retry_timer_id) {
	//      //  /* Restart the registration process */
	//      //  x2_registered_mce = 0;
	//      //  x2_register_mce_pending = MCE_app_register_x2 (mce_id_start, mce_id_end);
	//      //}
        //} /* if (EPC_MODE_ENABLED) */
	if(TIMER_HAS_EXPIRED(msg_p).timer_id == m3_mme_register_session_start_timer_id){
		MME_app_send_m3ap_session_start_req(0);
	}

      break;

 //   case X2AP_DEREGISTERED_ENB_IND:
 //     LOG_W(ENB_APP, "[MCE %d] Received %s: associated MCE %d\n", instance, ITTI_MSG_NAME (msg_p),
 //           X2AP_DEREGISTERED_ENB_IND(msg_p).nb_x2);
 //     /* TODO handle recovering of registration */
 //     break;

 //   case X2AP_REGISTER_ENB_CNF:
 //     LOG_I(ENB_APP, "[MCE %d] Received %s: associated MCE %d\n", instance, ITTI_MSG_NAME (msg_p),
 //           X2AP_REGISTER_ENB_CNF(msg_p).nb_x2);
 //     DevAssert(x2_register_enb_pending > 0);
 //     x2_register_enb_pending--;

 //     /* Check if at least MCE is registered with one target MCE */
 //     if (X2AP_REGISTER_ENB_CNF(msg_p).nb_x2 > 0) {
 //       x2_registered_enb++;
 //     }

 //     /* Check if all register MCE requests have been processed */
 //     if (x2_register_enb_pending == 0) {
 //       if (x2_registered_enb == enb_nb) {
 //         /* If all MCE are registered, start RRC HO task */
 //         } else {
 //         uint32_t x2_not_associated = enb_nb - x2_registered_enb;
 //         LOG_W(ENB_APP, " %d MCE %s not associated with the target\n",
 //               x2_not_associated, x2_not_associated > 1 ? "are" : "is");

 //         // timer to retry
 //         /* Restart the MCE registration process in ENB_REGISTER_RETRY_DELAY seconds */
 //         if (timer_setup (X2AP_ENB_REGISTER_RETRY_DELAY, 0, TASK_ENB_APP,
 //       		   INSTANCE_DEFAULT, TIMER_ONE_SHOT, NULL,
 //       		   &x2_enb_register_retry_timer_id) < 0) {
 //           LOG_E(ENB_APP, " Can not start MCE X2AP register: retry timer, use \"sleep\" instead!\n");
 //           sleep(X2AP_ENB_REGISTER_RETRY_DELAY);
 //           /* Restart the registration process */
 //           x2_registered_enb = 0;
 //           x2_register_enb_pending = MCE_app_register_x2 (enb_id_start, enb_id_end);
 //         }
 //       }
 //     }

 //     break;

 //   case M2AP_DEREGISTERED_ENB_IND:
 //     LOG_W(ENB_APP, "[MCE %d] Received %s: associated MCE %d\n", instance, ITTI_MSG_NAME (msg_p),
 //           M2AP_DEREGISTERED_ENB_IND(msg_p).nb_m2);
 //     /* TODO handle recovering of registration */
 //     break;

 //   case M2AP_REGISTER_ENB_CNF:
 //     LOG_I(ENB_APP, "[MCE %d] Received %s: associated MCE %d\n", instance, ITTI_MSG_NAME (msg_p),
 //           M2AP_REGISTER_ENB_CNF(msg_p).nb_m2);
 //     DevAssert(m2_register_enb_pending > 0);
 //     m2_register_enb_pending--;

 //     /* Check if at least MCE is registered with one target MCE */
 //     if (M2AP_REGISTER_ENB_CNF(msg_p).nb_m2 > 0) {
 //       m2_registered_enb++;
 //     }

 //     /* Check if all register MCE requests have been processed */
 //     if (m2_register_enb_pending == 0) {
 //       if (m2_registered_enb == enb_nb) {
 //         /* If all MCE are registered, start RRC HO task */
 //         } else {
 //         uint32_t m2_not_associated = enb_nb - m2_registered_enb;
 //         LOG_W(ENB_APP, " %d MCE %s not associated with the target\n",
 //               m2_not_associated, m2_not_associated > 1 ? "are" : "is");

 //         // timer to retry
 //         /* Restart the MCE registration process in ENB_REGISTER_RETRY_DELAY seconds */
 //         //if (timer_setup (X2AP_ENB_REGISTER_RETRY_DELAY, 0, TASK_ENB_APP,
 //         //      	   INSTANCE_DEFAULT, TIMER_ONE_SHOT, NULL,
 //         //      	   &x2_enb_register_retry_timer_id) < 0) {
 //         //  LOG_E(ENB_APP, " Can not start MCE X2AP register: retry timer, use \"sleep\" instead!\n");
 //         //  sleep(X2AP_ENB_REGISTER_RETRY_DELAY);
 //         //  /* Restart the registration process */
 //         //  x2_registered_enb = 0;
 //         //  x2_register_enb_pending = MCE_app_register_x2 (enb_id_start, enb_id_end);
 //         //}
 //       }
 //     }

 //     break;
	case M3AP_RESET:
	LOG_I(MME_APP,"Received M3AP_RESET message %s\n",ITTI_MSG_NAME(msg_p));	
	break;

	case M3AP_SETUP_REQ:
	LOG_I(MME_APP,"Received M3AP_REQ message %s\n",ITTI_MSG_NAME(msg_p));	
	MME_app_handle_m3ap_setup_req(0);
	if(timer_setup(1,0,TASK_MME_APP,INSTANCE_DEFAULT,TIMER_ONE_SHOT,NULL,&m3_mme_register_session_start_timer_id)<0){
	}
	//MME_app_send_m3ap_session_start_req(0);
	break;

	case M3AP_MBMS_SESSION_START_RESP:
	LOG_I(MME_APP,"Received M3AP_MBMS_SESSION_START_RESP message %s\n",ITTI_MSG_NAME(msg_p));
	MME_app_handle_m3ap_session_start_resp(0);
	//MME_app_send_m3ap_session_stop_req(0);
	break;

	case M3AP_MBMS_SESSION_STOP_RESP:
	LOG_I(MME_APP,"Received M3AP_MBMS_SESSION_STOP_RESP message %s\n",ITTI_MSG_NAME(msg_p));
	MME_app_send_m3ap_session_update_req(0);
	break;

	case M3AP_MBMS_SESSION_UPDATE_RESP:
	LOG_I(MME_APP,"Received M3AP_MBMS_SESSION_UPDATE_RESP message %s\n",ITTI_MSG_NAME(msg_p));
	// trigger something new here !!!!!!
	break;
 
	case M3AP_MBMS_SESSION_UPDATE_FAILURE:
	LOG_I(MME_APP,"Received M3AP_MBMS_SESSION_UPDATE_FAILURE message %s\n",ITTI_MSG_NAME(msg_p));	
	break;
 
	case M3AP_MCE_CONFIGURATION_UPDATE:
	LOG_I(MME_APP,"Received M3AP_MCE_CONFIGURATION_UPDATE message %s\n",ITTI_MSG_NAME(msg_p)); 	   
	break;
 
    default:
      LOG_E(MME_APP, "Received unexpected message %s\n", ITTI_MSG_NAME (msg_p));
      break;
    }

    result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
  } while (1);

  return NULL;
}
