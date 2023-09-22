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


#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "endpoint_ric.h"
#include "msg_handler_ric.h"
#include "util/alg_ds/alg/alg.h"
#include "util/compare.h"
#include "util/ngran_types.h"
//#include "act_req.h"
#include "e2ap_ric.h"
#include "near_ric.h"
#include "e2_node.h"
#include "lib/pending_events.h"
#include "lib/pending_event_ric.h"
#include "lib/ap/free/e2ap_msg_free.h"
#include "iApps/subscription_ric.h"
#include "iApp/e42_iapp_api.h"

#include "util/alg_ds/ds/lock_guard/lock_guard.h"

static inline
bool check_valid_msg_type(e2_msg_type_t msg_type)
{
  if(msg_type == RIC_SUBSCRIPTION_RESPONSE ||
      msg_type == RIC_SUBSCRIPTION_FAILURE ||
      msg_type == RIC_SUBSCRIPTION_DELETE_RESPONSE ||
      msg_type == RIC_SUBSCRIPTION_DELETE_FAILURE ||
      msg_type == RIC_INDICATION ||
      msg_type == RIC_CONTROL_ACKNOWLEDGE ||
      msg_type == RIC_CONTROL_FAILURE ||
      msg_type == E2AP_ERROR_INDICATION ||
      msg_type == E2_SETUP_REQUEST ||
      msg_type == E2AP_RESET_REQUEST ||
      msg_type == E2AP_RESET_RESPONSE ||
      msg_type == RIC_SERVICE_UPDATE ||
      msg_type == E2_NODE_CONFIGURATION_UPDATE ||
      msg_type == E2_CONNECTION_UPDATE_ACKNOWLEDGE ||
      msg_type == E2_CONNECTION_UPDATE_FAILURE )
    return true;

  return false;
}

static inline
e2_setup_response_t generate_setup_response(near_ric_t* ric, const e2_setup_request_t* req)
{
  assert(ric != NULL);
  assert(req != NULL);

  const size_t len_acc = req->len_rf;
  accepted_ran_function_t* accepted = calloc(len_acc, sizeof(accepted_ran_function_t));
  assert(accepted != NULL && "Memory exhausted");

  for (size_t i = 0; i < len_acc; ++i) {
    void* start_it = assoc_front(&ric->plugin.sm_ds);
    void* end_it = assoc_end(&ric->plugin.sm_ds);
    uint16_t const id = req->ran_func_item[i].id;

    void* it = find_if(&ric->plugin.sm_ds, start_it, end_it, &id, eq_ran_func_id); 

    if(it != end_it){
      assert(id == *(uint16_t*)assoc_key(&ric->plugin.sm_ds, it) );
      accepted[i] = id;
      char def[128] = {0};
      assert(req->ran_func_item[i].def.len < 127 );
      memcpy(def, req->ran_func_item[i].def.buf, req->ran_func_item[i].def.len);
      printf("[NEAR-RIC]: Accepting RAN function ID %d with def = %s \n", id, def);
    } else {
      printf("Unknown RAN function ID, thus rejecting %d \n", id);

      void* it = assoc_front(&ric->plugin.sm_ds);
      while(it != assoc_end(&ric->plugin.sm_ds)){
      //  printf("Registered keys = %d \n", *(uint16_t*)assoc_key(&ric->plugin.sm_ds, it));        
        it = assoc_next(&ric->plugin.sm_ds, it);
      }

      assert(0!=0 && "Unknown RAN function ID from the agent received");
    }
    fflush(stdout);
  }

  e2_setup_response_t sr = {
      .id.plmn = req->id.plmn, 
      .id.near_ric_id.double_word = 25,
      .accepted = accepted,
      .len_acc = len_acc,
      .rejected = NULL,
      .len_rej = 0,
      .comp_conf_update_ack_list = NULL,
      .len_ccual = 0
  };

  return sr;
}

static
void stop_pending_event(near_ric_t* ric, pending_event_ric_t* ev )
{
  assert(ric != NULL);
  assert(ev != NULL);

  int rc = pthread_mutex_lock(&ric->pend_mtx);
  assert(rc == 0);
  int* fd = bi_map_extract_right(&ric->pending, ev, sizeof(*ev));
  rc = pthread_mutex_unlock(&ric->pend_mtx);
  assert(rc == 0);

//  assert(bi_map_size(&ric->pending) == 0 && "Just one SM supported");
  assert(*fd > 0);
  //printf("fd value in stopping pending event = %d \n", *fd);
  rm_fd_asio_ric(&ric->io, *fd);
  free(fd);
}

e2ap_msg_t e2ap_msg_handle_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  const e2_msg_type_t msg_type = msg->type;
  assert(check_valid_msg_type(msg_type) == true);

  assert(ric->handle_msg[ msg_type ] != NULL);
  return ric->handle_msg[msg_type](ric, msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// E2 -> RIC 
 e2ap_msg_t e2ap_handle_subscription_response_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_RESPONSE);

  ric_subscription_response_t const* resp = &msg->u_msgs.ric_sub_resp;

  pending_event_ric_t ev = {.ev = SUBSCRIPTION_REQUEST_PENDING_EVENT, .id = resp->ric_id }; 
  stop_pending_event(ric, &ev);

  assert(resp->len_na == 0 && "No other case implemented");
  assert(resp->len_admitted == 1 && "No other case implemented");
  assert(resp->admitted[0].ric_act_id == 0 && "No other case implemented");

  // Active Request
//  act_req_t req = {.id = resp->ric_id};

//  int rc_mtx = pthread_mutex_lock(&ric->act_req_mtx);
//  assert(rc_mtx == 0);
//  seq_push_back(&ric->act_req, &req, sizeof(req) );
//  rc_mtx = pthread_mutex_unlock(&ric->act_req_mtx);
//  assert(rc_mtx == 0);

#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

//E2 -> RIC 
 e2ap_msg_t e2ap_handle_subscription_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_FAILURE);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_subscription_delete_response_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_RESPONSE);
  
  ric_subscription_delete_response_t const* resp = &msg->u_msgs.ric_sub_del_resp;

  pending_event_ric_t ev = {.ev = SUBSCRIPTION_DELETE_REQUEST_PENDING_EVENT, .id = resp->ric_id }; 
  stop_pending_event(ric, &ev);

#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif
  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

//E2 -> RIC
 e2ap_msg_t e2ap_handle_subscription_delete_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_FAILURE);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE };
  return ans;
}

static
void publish_ind_msg(near_ric_t* ric,  uint16_t ran_func_id, sm_ag_if_rd_ind_t* d)
{

  // find RIC request ID, pass the data to the assoc. SM
  void* start_it = assoc_front(&ric->pub_sub);
  void* end_it = assoc_end(&ric->pub_sub);

  void* sm_it = find_if(&ric->pub_sub, start_it, end_it, &ran_func_id, eq_ran_func_id);
  assert(sm_it != end_it && "Could not find a RAN function that matches the SM");


  if(sm_it != end_it){
    seq_arr_t* arr = assoc_value(&ric->pub_sub, start_it);  
    void* it = seq_front(arr);
    void* it_end = seq_end(arr);
    while(it != it_end){
      subs_ric_t* sub = (subs_ric_t*)it;
      sub->fp(d);
      it = seq_next(arr, it);
    }
    start_it = assoc_next(&ric->pub_sub, start_it);
  }
 
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_indication_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_INDICATION);

  ric_indication_t const* ric_ind = &msg->u_msgs.ric_ind;

  const uint16_t ran_func_id = ric_ind->ric_id.ran_func_id;  
  sm_ric_t* sm = sm_plugin_ric(&ric->plugin, ran_func_id);

  sm_ind_data_t data = {.ind_hdr = ric_ind->hdr.buf,
                        .len_hdr = ric_ind->hdr.len,
                        .ind_msg = ric_ind->msg.buf,
                        .len_msg = ric_ind->msg.len,
  };
  if(ric_ind->call_process_id != NULL){
    data.call_process_id = ric_ind->call_process_id->buf;
    data.len_cpid = ric_ind->call_process_id->len;
  }

  sm_ag_if_rd_ind_t d = sm->proc.on_indication(sm, &data);
  defer({ sm->alloc.free_ind_data(&d); } );
  assert(d.type == MAC_STATS_V0 || d.type == RLC_STATS_V0 
        || d.type == PDCP_STATS_V0 || d.type == SLICE_STATS_V0 
        || d.type == KPM_STATS_V3_0 || d.type == RAN_CTRL_STATS_V1_03 
        || d.type == GTP_STATS_V0 || d.type == TC_STATS_V0 );

  publish_ind_msg(ric, ran_func_id, &d);

  // Notify the iApp
#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE };
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_control_ack_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_ACKNOWLEDGE);

  ric_control_acknowledge_t const* ack = &msg->u_msgs.ric_ctrl_ack;
  assert( ack->status == RIC_CONTROL_STATUS_SUCCESS && "Only success supported ") ;

  pending_event_ric_t ev = {.ev = CONTROL_REQUEST_PENDING_EVENT, .id = ack->ric_id }; 
  stop_pending_event(ric, &ev);

  printf("[NEAR-RIC]: CONTROL ACKNOWLEDGE received\n");

#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_control_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_FAILURE);
  assert(0!=0 && "not implemented" );

  e2ap_msg_t ans = {0};
  return ans;
}
  
///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// RIC <-> E2 
 e2ap_msg_t e2ap_handle_error_indication_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2AP_ERROR_INDICATION);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_setup_request_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_REQUEST); 

//  printf("[E2AP] Received SETUP-REQUEST\n");
  fflush(stdout);
  const e2_setup_request_t* req = &msg->u_msgs.e2_stp_req;

  const e2ap_plmn_t* plmn = &req->id.plmn;
  const char* ran_type = get_ngran_name(req->id.type);
  if (NODE_IS_MONOLITHIC(req->id.type))
    printf("[E2AP] Received SETUP-REQUEST from PLMN %3d.%*d Node ID %d RAN type %s\n", plmn->mcc, plmn->mnc_digit_len, plmn->mnc, req->id.nb_id, ran_type);
  else
    printf("[E2AP] Received SETUP-REQUEST from PLMN %3d.%*d Node ID %d RAN type %s CU/DU ID %ld\n", plmn->mcc, plmn->mnc_digit_len, plmn->mnc, req->id.nb_id, ran_type, *req->id.cu_du_id);
  // Add the E2 Node into the iApp
  add_e2_node_iapp_api((global_e2_node_id_t*)&req->id, req->len_rf, req->ran_func_item);


  e2ap_msg_t ans = {.type = E2_SETUP_RESPONSE };
  ans.u_msgs.e2_stp_resp = generate_setup_response(ric, req); 

  e2_node_t n = {0};
  init_e2_node(&n, &req->id, ans.u_msgs.e2_stp_resp.len_acc, ans.u_msgs.e2_stp_resp.accepted); 

  lock_guard(&ric->conn_e2_nodes_mtx);
  seq_push_back(&ric->conn_e2_nodes, &n, sizeof(n));

  return ans;
}

// RIC <-> E2
 e2ap_msg_t e2ap_handle_reset_request_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2AP_RESET_REQUEST);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// RIC <-> E2
 e2ap_msg_t e2ap_handle_reset_response_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2AP_RESET_RESPONSE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}
  
// E2 -> RIC
 e2ap_msg_t e2ap_handle_service_update_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SERVICE_UPDATE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_node_configuration_update_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_NODE_CONFIGURATION_UPDATE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {0};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_connection_update_ack_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_CONNECTION_UPDATE_ACKNOWLEDGE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_connection_update_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_CONNECTION_UPDATE_FAILURE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

