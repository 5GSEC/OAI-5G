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
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */



#include "e2ap_msg_free.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ba_if_not_null(byte_array_t* ba)
{
  if(ba != NULL)
  {
    free_byte_array(*ba);
    free(ba);
  }
}

static
void free_node_config_update(e2_node_component_config_update_t* dst)
{
  switch(dst->update_present){
    case E2_NODE_COMPONENT_CONFIG_UPDATE_GNB_CONFIG_UPDATE:
      {
        free_ba_if_not_null(dst->gnb.ngap_gnb_cu_cp);
        free_ba_if_not_null(dst->gnb.xnap_gnb_cu_cp);
        free_ba_if_not_null(dst->gnb.e1ap_gnb_cu_cp);
        free_ba_if_not_null(dst->gnb.f1ap_gnb_cu_cp);
        break;
      }
    case E2_NODE_COMPONENT_CONFIG_UPDATE_EN_GNB_CONFIG_UPDATE:
      {
        free_ba_if_not_null(dst->en_gnb.x2ap_en_gnb);
        break;
      }
    case E2_NODE_COMPONENT_CONFIG_UPDATE_NG_ENB_CONFIG_UPDATE:
      {
        free_ba_if_not_null(dst->ng_enb.ngap_ng_enb);
        free_ba_if_not_null(dst->ng_enb.xnap_ng_enb );
        break;
      }
    case E2_NODE_COMPONENT_CONFIG_UPDATE_ENB_CONFIG_UPDATE:
      {
        free_ba_if_not_null(dst->enb.s1ap_enb);
        free_ba_if_not_null(dst->enb.x2ap_enb);
        break;
      } 
    case E2_NODE_COMPONENT_CONFIG_UPDATE_NOTHING:
    default:
      assert(0!=0 && "Invalid code path");
  }
  if(dst->id_present != NULL){
    free(dst->id_present);
  }
}


// RIC -> E2
void e2ap_free_subscription_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_REQUEST);
  return  e2ap_free_subscription_request(&msg->u_msgs.ric_sub_req);
}


void e2ap_free_subscription_request(ric_subscription_request_t* sr)
{
  assert(sr != NULL);
  free_byte_array(sr->event_trigger);
  for(size_t i = 0; i < sr->len_action; ++i){
    free_ba_if_not_null(sr->action[i].definition  );
    if(sr->action[i].subseq_action != NULL){
      if(sr->action[i].subseq_action->time_to_wait_ms != NULL)
        free(sr->action[i].subseq_action->time_to_wait_ms);
      free(sr->action[i].subseq_action);
    }
  }
  free(sr->action);
}

// E2 -> RIC 
void e2ap_free_subscription_response_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_RESPONSE);
  return  e2ap_free_subscription_response(&msg->u_msgs.ric_sub_resp);
}

void e2ap_free_subscription_response(ric_subscription_response_t* sr)
{
  free(sr->admitted);
  free(sr->not_admitted);
}


//E2 -> RIC 
void e2ap_free_subscription_failure_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_FAILURE);
  e2ap_free_subscription_failure(&msg->u_msgs.ric_sub_fail);
}


void e2ap_free_subscription_failure(ric_subscription_failure_t* sf)
{
  assert(sf != NULL);
  assert(sf->not_admitted != NULL);
  free(sf->not_admitted);

  if(sf->crit_diag != NULL){
    assert(0!=0 && "Not implemented");
  }
}



//RIC -> E2
void e2ap_free_subscription_delete_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_REQUEST);
  e2ap_free_subscription_delete_request(&msg->u_msgs.ric_sub_del_req);

}

void e2ap_free_subscription_delete_request(ric_subscription_delete_request_t* dr)
{
  assert(dr != NULL);
 // No heap allocated
}

// E2 -> RIC
void e2ap_free_subscription_delete_response_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_RESPONSE);
  e2ap_free_subscription_delete_response(&msg->u_msgs.ric_sub_del_resp);
}

void e2ap_free_subscription_delete_response(ric_subscription_delete_response_t* dr)
{
  assert(dr != NULL);
  // No heap allocated
}


//E2 -> RIC
void e2ap_free_subscription_delete_failure_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_FAILURE);
  e2ap_free_subscription_delete_failure(&msg->u_msgs.ric_sub_del_fail);
}

void e2ap_free_subscription_delete_failure(ric_subscription_delete_failure_t* df)
{
  assert(df != NULL);
  if(df->crit_diag != NULL){
    assert(0!=0 && "Not implemented");
  }
}

// E2 -> RIC
void e2ap_free_indication_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_INDICATION);

  ric_indication_t* ind = &msg->u_msgs.ric_ind;
  e2ap_free_indication(ind); 
}

void e2ap_free_indication(ric_indication_t* ind)
{
  if(ind->sn != NULL){  
    free(ind->sn);
  }

  free_byte_array(ind->hdr); 
  free_byte_array(ind->msg);
  free_ba_if_not_null(ind->call_process_id);
}

// RIC -> E2
void e2ap_free_control_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_REQUEST);
  ric_control_request_t* ric_req = &msg->u_msgs.ric_ctrl_req;
  e2ap_free_control_request(ric_req);
}

void e2ap_free_control_request(ric_control_request_t* ric_req)
{
  assert(ric_req);
  free_ba_if_not_null(ric_req->call_process_id);
  free_byte_array(ric_req->hdr);
  free_byte_array(ric_req->msg);
  if(ric_req->ack_req != NULL)
    free(ric_req->ack_req); 
}


// E2 -> RIC
void e2ap_free_control_ack_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_ACKNOWLEDGE);
  e2ap_free_control_ack(&msg->u_msgs.ric_ctrl_ack);
}

void e2ap_free_control_ack(ric_control_acknowledge_t* c_ack)
{
  assert(c_ack);
  free_ba_if_not_null(c_ack->call_process_id);
  free_ba_if_not_null(c_ack->control_outcome);
}


// E2 -> RIC
void e2ap_free_control_failure_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_FAILURE);
 e2ap_free_control_failure(&msg->u_msgs.ric_ctrl_fail);
}

void e2ap_free_control_failure(ric_control_failure_t* cf)
{
  assert(cf != NULL);
  free_ba_if_not_null(cf->call_process_id);
  free_ba_if_not_null(cf->control_outcome);
}
 

///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// RIC <-> E2 
void e2ap_free_error_indication_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2AP_ERROR_INDICATION);
  e2ap_free_error_indication(&msg->u_msgs.err_ind);
}

void e2ap_free_error_indication(e2ap_error_indication_t* ei)
{
  assert(ei != NULL);

  if(ei->ric_id != NULL)
    free(ei->ric_id);
  if(ei->cause != NULL)
    free(ei->cause);
  if(ei->crit_diag != NULL){
    assert(0!= 0 && "Not implemented" );
  }
}

// E2 -> RIC
void e2ap_free_setup_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_REQUEST );
  return e2ap_free_setup_request(&msg->u_msgs.e2_stp_req);
}

// E2 -> RIC
void e2ap_free_setup_request(e2_setup_request_t* sr)
{
  assert(sr != NULL);

  for(size_t i = 0; i < sr->len_rf; ++i){
    ran_function_t* dst = &sr->ran_func_item[i];
    free_byte_array(dst->def);
    free_ba_if_not_null(dst->oid);
  }
  free(sr->ran_func_item);

  for(size_t i = 0; i < sr->len_ccu; ++i){
    free_node_config_update(&sr->comp_conf_update[i]);  
  } 
  free(sr->comp_conf_update); 
}


// RIC -> E2
void e2ap_free_setup_response_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_RESPONSE);
  return e2ap_free_setup_response(&msg->u_msgs.e2_stp_resp);
} 

// RIC -> E2
void e2ap_free_setup_response(e2_setup_response_t* sr)
{
  assert( sr != NULL);

  if(sr->len_acc > 0){
    free(sr->accepted);
  }

  if(sr->len_rej > 0){
    assert(0!= 0 && "Not Implemented");
  }

  if(sr->len_ccual > 0){
    assert(0!= 0 && "Not Implemented");
  }

}

// RIC -> E2
void e2ap_free_setup_failure_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_FAILURE);
  e2ap_free_setup_failure(&msg->u_msgs.e2_stp_fail);
}

void e2ap_free_setup_failure(e2_setup_failure_t* sf)
{
  assert(sf != NULL);

  if(sf->time_to_wait_ms != NULL)
    free(sf->time_to_wait_ms);

  if(sf->crit_diag != NULL){
    assert(0!=0 && "Not implemented");
  }
  if(sf->tl_info != NULL){
    free_byte_array(sf->tl_info->address);
    free_ba_if_not_null(sf->tl_info->port);
    free(sf->tl_info);
  }
}

// RIC <-> E2
void e2ap_free_reset_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2AP_RESET_REQUEST);
  e2ap_free_reset_request(&msg->u_msgs.rst_req);
}

void e2ap_free_reset_request(e2ap_reset_request_t* rr)
{
  assert(rr != NULL);
  // No heap allocated
}


// RIC <-> E2
void e2ap_free_reset_response_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2AP_RESET_RESPONSE);
  e2ap_free_reset_response(&msg->u_msgs.rst_resp);
}
 
void e2ap_free_reset_response(e2ap_reset_response_t* rr)
{
  assert(rr != NULL);
  if(rr->crit_diag != NULL){
    assert(0!=0 && "Not implemented");
  }
}


// E2 -> RIC
void e2ap_free_service_update_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SERVICE_UPDATE);
  e2ap_free_service_update(&msg->u_msgs.ric_serv_updt);
}

void e2ap_free_service_update(ric_service_update_t* su)
{
  assert(su != NULL);
  for(size_t i = 0; i < su->len_added; ++i){
    ran_function_t* dst = &su->added[i]; 
    free_byte_array(dst->def);
    free_ba_if_not_null(dst->oid);
  }
  free(su->added);

  for(size_t i = 0; i < su->len_modified; ++i){
    ran_function_t* dst = &su->modified[i]; 
    free_byte_array(dst->def);
    free_ba_if_not_null(dst->oid);
  }
  free(su->modified);

  free(su->deleted);
}

// RIC -> E2
void e2ap_free_service_update_ack_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SERVICE_UPDATE_ACKNOWLEDGE);
  e2ap_free_service_update_ack(&msg->u_msgs.ric_serv_updt_ack);
}

void e2ap_free_service_update_ack(ric_service_update_ack_t* su)
{
  assert(su != NULL);

  if(su->accepted != NULL)
    free(su->accepted);

  if(su->rejected != NULL)
    free(su->rejected);
}





// RIC -> E2
void e2ap_free_service_update_failure_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SERVICE_UPDATE_FAILURE);
 e2ap_free_service_update_failure(&msg->u_msgs.ric_serv_updt_fail);
}

void e2ap_free_service_update_failure(ric_service_update_failure_t* uf)
{
  assert(uf != NULL);
  free(uf->rejected); 
  if(uf->crit_diag != NULL){
    assert(0!=0 && "not implemented");
  }

  if(uf->time_to_wait != NULL)
    free(uf->time_to_wait);
}

// RIC -> E2
void e2ap_free_service_query_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == RIC_SERVICE_QUERY);
  e2ap_free_service_query(&msg->u_msgs.ric_serv_query);
}

void e2ap_free_service_query(ric_service_query_t* sq)
{
  assert(sq != NULL);

  if(sq->accepted != NULL)
    free(sq->accepted);
}

// E2 -> RIC
void e2ap_free_node_configuration_update_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_NODE_CONFIGURATION_UPDATE);
  e2ap_free_node_configuration_update(&msg->u_msgs.e2_node_conf_updt);
}

void e2ap_free_node_configuration_update(e2_node_configuration_update_t* cu)
{
  assert(cu != NULL);
  for(size_t i = 0; i < cu->len_ccul; ++i){
    free_node_config_update(&cu->comp_conf_update_list[i]);  
  }
  free(cu->comp_conf_update_list);
}

// RIC -> E2
void e2ap_free_node_configuration_update_ack_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE);
  e2ap_free_node_configuration_update_ack(&msg->u_msgs.e2_node_conf_updt_ack);
}

void e2ap_free_node_configuration_update_ack(e2_node_configuration_update_ack_t* cua){

  for(size_t i = 0; i < cua->len_ccual; ++i){
    e2_node_component_config_update_ack_item_t* dst = &cua->comp_conf_update_ack_list[i];
    if(dst->failure_cause != NULL)
      free(dst->failure_cause);
    if(dst->id_present != NULL)
      free(dst->id_present);
  }
  if(cua->comp_conf_update_ack_list != NULL)
    free(cua->comp_conf_update_ack_list);
}

// RIC -> E2
void e2ap_free_node_configuration_update_failure_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_NODE_CONFIGURATION_UPDATE_FAILURE);
  e2ap_free_node_configuration_update_failure(&msg->u_msgs.e2_node_conf_updt_fail);
}

void e2ap_free_node_configuration_update_failure(e2_node_configuration_update_failure_t* cuf)
{
  assert(cuf != NULL);
  if(cuf->time_wait != NULL)
    free(cuf->time_wait);

  if(cuf->crit_diag != NULL)
    assert(0!=0 && "Not implemented");
}


// RIC -> E2
void e2ap_free_node_connection_update_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_CONNECTION_UPDATE);
  e2ap_free_node_connection_update(&msg->u_msgs.e2_conn_updt);
}

void e2ap_free_node_connection_update(e2_node_connection_update_t* cu)
{
  assert(cu != NULL);

  for(size_t i = 0; i < cu->len_add; ++i){
   free_byte_array(cu->add[i].info.tnl_addr);
   free_ba_if_not_null(cu->add[i].info.tnl_port);
  }
  if(cu->add != NULL)
    free(cu->add);

  for(size_t i = 0; i < cu->len_rem; ++i){
   free_byte_array(cu->rem[i].info.tnl_addr);
   free_ba_if_not_null(cu->rem[i].info.tnl_port);
  }
  if(cu->rem != NULL)
    free(cu->rem);

  for(size_t i = 0; i < cu->len_mod; ++i){
   free_byte_array(cu->mod[i].info.tnl_addr);
   free_ba_if_not_null(cu->mod[i].info.tnl_port);
  }
  if(cu->mod != NULL)
    free(cu->mod);
}

void e2ap_free_node_connection_update_ack_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_CONNECTION_UPDATE_ACKNOWLEDGE);
  e2ap_free_node_connection_update_ack(&msg->u_msgs.e2_conn_updt_ack);
}

void e2ap_free_node_connection_update_ack(e2_node_connection_update_ack_t* ca)
{
  assert(ca != NULL);

  for(size_t i = 0; i < ca->len_setup; ++i){
   free_byte_array(ca->setup[i].info.tnl_addr);
   free_ba_if_not_null(ca->setup[i].info.tnl_port);
  }
  if(ca->setup != NULL)
    free(ca->setup);

  for(size_t i = 0; i < ca->len_failed; ++i){
   free_byte_array(ca->failed[i].info.tnl_addr);
   free_ba_if_not_null(ca->failed[i].info.tnl_port);
  }
  if(ca->failed != NULL)
    free(ca->failed);
}

// E2 -> RIC
void e2ap_free_node_connection_update_failure_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E2_CONNECTION_UPDATE_FAILURE );
 e2ap_free_node_connection_update_failure(&msg->u_msgs.e2_conn_updt_fail);
}

void e2ap_free_node_connection_update_failure(e2_node_connection_update_failure_t* cf)
{
  assert(cf != NULL);
  if(cf->time_wait != NULL)
    free(cf->time_wait);
  if(cf->crit_diag != NULL){
    assert(0!=0 && "Not implemented!");
  }
}

// xApp -> iApp
void e2ap_free_e42_setup_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E42_SETUP_REQUEST);
  e2ap_free_e42_setup_request(&msg->u_msgs.e42_stp_req);
}

void e2ap_free_e42_setup_request(e42_setup_request_t* sr) 
{
  assert(sr != NULL);
  for(size_t i = 0; i < sr->len_rf; ++i){
    ran_function_t* dst = &sr->ran_func_item[i];
    free_byte_array(dst->def);
    free_ba_if_not_null(dst->oid);
  }
  free(sr->ran_func_item);
}

// iApp -> xApp
void e2ap_free_e42_setup_response_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E42_SETUP_RESPONSE);
  e2ap_free_e42_setup_response(&msg->u_msgs.e42_stp_resp);
}
 
void e2ap_free_e42_setup_response(e42_setup_response_t* sr) 
{
  assert(sr != NULL);

  for(size_t i = 0; i < sr->len_e2_nodes_conn; ++i){
    e2_node_connected_t* n = &sr->nodes[i];
    free_e2_node_connected(n);
  }
  free(sr->nodes);
}

void e2ap_free_e42_ric_subscription_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E42_RIC_SUBSCRIPTION_REQUEST);
  e2ap_free_e42_ric_subscription_request(&msg->u_msgs.e42_ric_sub_req);
}

void e2ap_free_e42_ric_subscription_request(e42_ric_subscription_request_t* e42_sr)
{
  assert(e42_sr != NULL);
  e2ap_free_subscription_request(&e42_sr->sr);
  free_global_e2_node_id(&e42_sr->id);
}

void e2ap_free_e42_ric_subscription_delete_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E42_RIC_SUBSCRIPTION_DELETE_REQUEST);
  e2ap_free_e42_ric_subscription_delete_request(&msg->u_msgs.e42_ric_sub_del_req);
}

void e2ap_free_e42_ric_subscription_delete_request(e42_ric_subscription_delete_request_t* e42_sr)
{
  assert(e42_sr != NULL);
  e2ap_free_subscription_delete_request(&e42_sr->sdr);
}

void e2ap_free_e42_ric_control_request_msg(e2ap_msg_t* msg)
{
  assert(msg != NULL);
  assert(msg->type == E42_RIC_CONTROL_REQUEST);
  e2ap_free_e42_ric_control_request(&msg->u_msgs.e42_ric_ctrl_req);
}

void e2ap_free_e42_ric_control_request(e42_ric_control_request_t* e42_ctrl)
{
  assert(e42_ctrl != NULL);
  e2ap_free_control_request(&e42_ctrl->ctrl_req);
  free_global_e2_node_id(&e42_ctrl->id);
}

