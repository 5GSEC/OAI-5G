/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
a* You may obtain a copy of the License at
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



#include "../../xApp/e42_xapp_api.h"
#include "e42_iapp_api.h"
#include "msg_handler_iapp.h"
#include "lib/pending_events.h"
#include "util/alg_ds/alg/alg.h"
#include "util/compare.h"
#include "util/alg_ds/ds/lock_guard/lock_guard.h"
#include "util/time_now_us.h"

#include "iapp_if_generic.h"
#include "xapp_ric_id.h"

#include <stdio.h>




static
bool check_valid_msg_type(e2_msg_type_t msg_type )
{
  return 
         msg_type == RIC_SUBSCRIPTION_RESPONSE
      || msg_type == E42_SETUP_REQUEST
      || msg_type == E42_RIC_SUBSCRIPTION_REQUEST
      || msg_type == E42_RIC_SUBSCRIPTION_DELETE_REQUEST
      || msg_type == E42_RIC_CONTROL_REQUEST
      || msg_type == RIC_CONTROL_ACKNOWLEDGE
      || msg_type == RIC_INDICATION
      || msg_type == RIC_SUBSCRIPTION_DELETE_RESPONSE;
}

void init_handle_msg_iapp(handle_msg_fp_iapp (*handle_msg)[31])
{
  memset((*handle_msg), 0, sizeof(handle_msg_fp_iapp)*31);

  (*handle_msg)[RIC_SUBSCRIPTION_RESPONSE] = e2ap_handle_subscription_response_iapp;
  (*handle_msg)[E42_SETUP_REQUEST] = e2ap_handle_e42_setup_request_iapp;
  (*handle_msg)[E42_RIC_SUBSCRIPTION_REQUEST] = e2ap_handle_e42_ric_subscription_request_iapp;
  (*handle_msg)[E42_RIC_SUBSCRIPTION_DELETE_REQUEST] = e2ap_handle_e42_ric_subscription_delete_request_iapp;
  (*handle_msg)[E42_RIC_CONTROL_REQUEST] = e2ap_handle_e42_ric_control_request_iapp;
  (*handle_msg)[RIC_CONTROL_ACKNOWLEDGE] = e2ap_handle_e42_ric_control_ack_iapp;
  (*handle_msg)[RIC_INDICATION] = e2ap_handle_ric_indication_iapp;
  (*handle_msg)[RIC_SUBSCRIPTION_DELETE_RESPONSE] = e2ap_handle_subscription_delete_response_iapp;

//  (*handle_msg)[RIC_SUBSCRIPTION_REQUEST] = e2ap_handle_subscription_request_iapp;
//  (*handle_msg)[RIC_SUBSCRIPTION_DELETE_REQUEST] =  e2ap_handle_subscription_delete_request_iapp;
//  (*handle_msg)[RIC_CONTROL_REQUEST] = e2ap_handle_control_request_iapp;
//  (*handle_msg)[E2AP_ERROR_INDICATION] = e2ap_handle_error_indication_iapp;
//  (*handle_msg)[E2_SETUP_REQUEST] = e2ap_handle_setup_request_iapp;
//  (*handle_msg)[E2AP_RESET_REQUEST] =  e2ap_handle_reset_request_iapp;
//  (*handle_msg)[E2AP_RESET_RESPONSE] =  e2ap_handle_reset_response_iapp;
//  (*handle_msg)[RIC_SERVICE_UPDATE_ACKNOWLEDGE] =  e2ap_handle_service_update_ack_iapp;
//  (*handle_msg)[RIC_SERVICE_UPDATE_FAILURE] =  e2ap_handle_service_update_failure_iapp;
//  (*handle_msg)[RIC_SERVICE_QUERY] = e2ap_handle_service_query_iapp;
//  (*handle_msg)[E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE] =  e2ap_handle_node_configuration_update_ack_iapp;
//  (*handle_msg)[E2_NODE_CONFIGURATION_UPDATE_FAILURE] =  e2ap_handle_node_configuration_update_failure_iapp;
//  (*handle_msg)[E2_CONNECTION_UPDATE] =  e2ap_handle_connection_update_iapp;

}

e2ap_msg_t e2ap_handle_subscription_response_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_RESPONSE);

  ric_subscription_response_t const* src = &msg->u_msgs.ric_sub_resp; 

  xapp_ric_id_t x = find_xapp_map_ric_id(&iapp->map_ric_id, src->ric_id.ric_req_id);
  assert(src->ric_id.ran_func_id == x.ric_id.ran_func_id);
  assert(src->ric_id.ric_inst_id == x.ric_id.ric_inst_id);

  e2ap_msg_t ans = {.type = RIC_SUBSCRIPTION_RESPONSE};
  defer({ e2ap_msg_free_iapp(&iapp->ap, &ans);} );
  ric_subscription_response_t* dst = &ans.u_msgs.ric_sub_resp;
  *dst = mv_ric_subscription_respponse(src);
  dst->ric_id.ric_req_id = x.ric_id.ric_req_id;

  sctp_msg_t sctp_msg = {0}; 
  defer({ free_sctp_msg(&sctp_msg); } );
  sctp_msg.info = find_map_xapps_sad(&iapp->ep.xapps, x.xapp_id);
  sctp_msg.ba = e2ap_msg_enc_iapp(&iapp->ap, &ans); 
       
  e2ap_send_sctp_msg_iapp(&iapp->ep, &sctp_msg);

  e2ap_msg_t none = {.type = NONE_E2_MSG_TYPE};
  return none;
}

e2ap_msg_t e2ap_handle_subscription_delete_response_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_RESPONSE );

  ric_subscription_delete_response_t const* src = &msg->u_msgs.ric_sub_del_resp; 

  xapp_ric_id_t x = find_xapp_map_ric_id(&iapp->map_ric_id, src->ric_id.ric_req_id);
  assert(src->ric_id.ran_func_id == x.ric_id.ran_func_id);
  assert(src->ric_id.ric_inst_id == x.ric_id.ric_inst_id);

  e2ap_msg_t ans = {.type = RIC_SUBSCRIPTION_DELETE_RESPONSE };
  defer( { e2ap_msg_free_iapp(&iapp->ap, &ans); } );
  ric_subscription_delete_response_t* dst = &ans.u_msgs.ric_sub_del_resp;
  dst->ric_id = x.ric_id;

  sctp_msg_t sctp_msg = {0};
  defer({ free_sctp_msg(&sctp_msg); } );
  sctp_msg.info = find_map_xapps_sad(&iapp->ep.xapps, x.xapp_id);
  sctp_msg.ba = e2ap_msg_enc_iapp(&iapp->ap, &ans); 
       
  e2ap_send_sctp_msg_iapp(&iapp->ep, &sctp_msg);

  printf("[iApp]: RIC_SUBSCRIPTION_DELETE_RESPONSE sent \n");

  e2ap_msg_t none = {.type = NONE_E2_MSG_TYPE};
  return none;
}

e2ap_msg_t e2ap_handle_e42_ric_control_ack_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_ACKNOWLEDGE);

  ric_control_acknowledge_t const* src = &msg->u_msgs.ric_ctrl_ack; 

  xapp_ric_id_t x = find_xapp_map_ric_id(&iapp->map_ric_id, src->ric_id.ric_req_id);
  assert(src->ric_id.ran_func_id == x.ric_id.ran_func_id);
  assert(src->ric_id.ric_inst_id == x.ric_id.ric_inst_id);

  e2ap_msg_t ans = {.type = RIC_CONTROL_ACKNOWLEDGE };
  defer( { e2ap_msg_free_iapp(&iapp->ap, &ans); } );
  ric_control_acknowledge_t* dst = &ans.u_msgs.ric_ctrl_ack;
  dst->ric_id = x.ric_id;
  dst->status = src->status; 

  printf("[iApp]: RIC_CONTROL_ACKNOWLEDGE tx\n");

  sctp_msg_t sctp_msg = {0};
  defer({ free_sctp_msg(&sctp_msg); } );
  sctp_msg.info = find_map_xapps_sad(&iapp->ep.xapps, x.xapp_id);
  sctp_msg.ba = e2ap_msg_enc_iapp(&iapp->ap, &ans); 
       
  e2ap_send_sctp_msg_iapp(&iapp->ep, &sctp_msg);

  e2ap_msg_t none = {.type = NONE_E2_MSG_TYPE};
  return none;
}

static
e42_setup_response_t generate_setup_response(e42_iapp_t* iapp, e42_setup_request_t const* req)
{
  assert(iapp != NULL);
  assert(req != NULL);

  ran_function_t* rf = req->ran_func_item; 
  assert(rf != NULL);
  size_t const len_rf = req->len_rf; 
  assert(len_rf > 0);

  // ToDo, permissions and whether the SM exists in the iApp, should be checked
  e2_node_arr_t ans = generate_e2_node_arr( &iapp->e2_nodes); 


  e42_setup_response_t sr = {.xapp_id = iapp->xapp_id++,
                             .len_e2_nodes_conn = ans.len,
                             .nodes = ans.n};

  return sr;
}

// xApp -> iApp
e2ap_msg_t e2ap_handle_e42_setup_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == E42_SETUP_REQUEST); 
  const e42_setup_request_t* req = &msg->u_msgs.e42_stp_req;

  printf("[iApp]: E42 SETUP-REQUEST received\n");

  e2ap_msg_t ans = {.type = E42_SETUP_RESPONSE };
  ans.u_msgs.e42_stp_resp = generate_setup_response(iapp, req); 

  printf("[iApp]: E42 SETUP-RESPONSE sent\n");
  fflush(stdout);

  return ans;
}

e2ap_msg_t e2ap_handle_ric_indication_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_INDICATION); 

  ric_indication_t const* src = &msg->u_msgs.ric_ind;

  xapp_ric_id_t x = find_xapp_map_ric_id(&iapp->map_ric_id, src->ric_id.ric_req_id);
  assert(src->ric_id.ran_func_id == x.ric_id.ran_func_id);
  assert(src->ric_id.ric_inst_id == x.ric_id.ric_inst_id);

  e2ap_msg_t ans = {.type = RIC_INDICATION};
  defer( { e2ap_msg_free_iapp(&iapp->ap, &ans); } );
  ric_indication_t* dst = &ans.u_msgs.ric_ind;
  // Moving transfers ownership
  *dst = mv_ric_indication((ric_indication_t*)src);
  dst->ric_id.ric_req_id = x.ric_id.ric_req_id;

  sctp_msg_t sctp_msg = {0}; 
  defer({ free_sctp_msg(&sctp_msg); } );
  sctp_msg.info = find_map_xapps_sad(&iapp->ep.xapps, x.xapp_id);
  sctp_msg.ba = e2ap_msg_enc_iapp(&iapp->ap, &ans); 

  e2ap_send_sctp_msg_iapp(&iapp->ep, &sctp_msg);

  e2ap_msg_t none = {.type = NONE_E2_MSG_TYPE};
  return none;
}

static
bool valid_xapp_id(e42_iapp_t* iapp, uint32_t xapp_id)
{
  assert(iapp != NULL);
  assert(xapp_id < 1 << 16);
  return xapp_id <= iapp->xapp_id;
}

static
bool valid_global_e2_node(e42_iapp_t* iapp, global_e2_node_id_t const* id )
{
  assert(iapp != NULL);
  assert(id != NULL);

  e2_node_arr_t nodes = generate_e2_node_arr(&iapp->e2_nodes); 
  defer( {  free_e2_node_arr(&nodes);  }  );

  for(size_t i = 0; i < nodes.len; ++i){
   if( eq_global_e2_node_id(&nodes.n[i].id, id) == true)
     return true; 
  }

  return false;
}

e2ap_msg_t e2ap_handle_e42_ric_subscription_delete_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == E42_RIC_SUBSCRIPTION_DELETE_REQUEST); 

  const e42_ric_subscription_delete_request_t* src = &msg->u_msgs.e42_ric_sub_del_req;

  xapp_ric_id_t x = { .ric_id = src->sdr.ric_id,
                      .xapp_id = src->xapp_id 
                    };

  e2_node_ric_req_t n = find_ric_req_map_ric_id(&iapp->map_ric_id, &x);

  ric_subscription_delete_request_t dst = cp_ric_subscription_delete_request(&src->sdr);
  dst.ric_id.ric_req_id = n.ric_req_id;

  fwd_ric_subscription_request_delete_gen(iapp->ric_if.type, &n.e2_node_id, &dst, notify_msg_iapp_api);

  printf("[iApp]: RIC_SUBSCRIPTION_DELETE_REQUEST sent \n");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// xApp -> iApp
e2ap_msg_t e2ap_handle_e42_ric_subscription_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == E42_RIC_SUBSCRIPTION_REQUEST); 

  e42_ric_subscription_request_t const * e42_sr = &msg->u_msgs.e42_ric_sub_req;
 
  assert(valid_xapp_id(iapp, e42_sr->xapp_id) == true);
  assert(valid_global_e2_node(iapp, &e42_sr->id ) );

  xapp_ric_id_t xapp_ric_id = {.ric_id = e42_sr->sr.ric_id,
                                .xapp_id = e42_sr->xapp_id };

  printf("[iApp]: SUBSCRIPTION-REQUEST xapp_ric_id->ric_id.ran_func_id %d  \n", xapp_ric_id.ric_id.ran_func_id );

  // I do not like the mtx here but there is a data race if not
  lock_guard(&iapp->map_ric_id.mtx); 

  uint16_t const new_ric_id = fwd_ric_subscription_request_gen(iapp->ric_if.type, &e42_sr->id, &e42_sr->sr, notify_msg_iapp_api);

  e2_node_ric_req_t n = { .ric_req_id =  new_ric_id,
                          .e2_node_id = cp_global_e2_node_id(&e42_sr->id) }; 


  add_map_ric_id(&iapp->map_ric_id, &n, &xapp_ric_id);

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

// xApp -> iApp
e2ap_msg_t e2ap_handle_e42_ric_control_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == E42_RIC_CONTROL_REQUEST );

  e42_ric_control_request_t const* e42_cr = &msg->u_msgs.e42_ric_ctrl_req;

  assert(valid_xapp_id(iapp, e42_cr->xapp_id) == true);
  assert(valid_global_e2_node(iapp, &e42_cr->id));

  xapp_ric_id_t xapp_ric_id = {.ric_id = e42_cr->ctrl_req.ric_id,
                                .xapp_id = e42_cr->xapp_id};

  // I do not like the mtx here but there is a data race if not
  lock_guard(&iapp->map_ric_id.mtx); 

  uint16_t new_ric_id = fwd_ric_control_request_gen(iapp->ric_if.type, &e42_cr->id, &e42_cr->ctrl_req, notify_msg_iapp_api);

  e2_node_ric_req_t n = { .ric_req_id =  new_ric_id,
                          .e2_node_id = cp_global_e2_node_id(&e42_cr->id) }; 


  add_map_ric_id(&iapp->map_ric_id, &n, &xapp_ric_id);

  printf("[iApp]: E42_RIC_CONTROL_REQUEST rx\n");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_msg_handle_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  const e2_msg_type_t msg_type = msg->type;
  assert(check_valid_msg_type(msg_type) == true);
  assert(iapp->handle_msg[ msg_type ] != NULL);
  return iapp->handle_msg[msg_type](iapp, msg); 
}

e2ap_msg_t e2ap_handle_subscription_delete_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_REQUEST);

  assert(0 != 0 && "Should not come here");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}
  
// The purpose of the RIC Control procedure is to initiate or resume a specific functionality in the E2 Node.
e2ap_msg_t e2ap_handle_control_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_REQUEST);

  assert(0!=0 && "Should not come here...");

  printf("[E2-AGENT]: CONTROL ACKNOWLEDGE sent\n");
  e2ap_msg_t ans = {.type = RIC_CONTROL_ACKNOWLEDGE};

  return ans; 
}

e2ap_msg_t e2ap_handle_error_indication_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_reset_request_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_reset_response_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}
  
e2ap_msg_t e2ap_handle_service_update_ack_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_service_update_failure_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_service_query_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_node_configuration_update_ack_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_node_configuration_update_failure_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_connection_update_iapp(e42_iapp_t* iapp, const e2ap_msg_t* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}


