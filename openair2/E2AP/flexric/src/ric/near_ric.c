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

#include "near_ric.h"
#include "e2_node.h"
#include "iApp/e42_iapp_api.h"

#include "msg_handler_ric.h"
#include "not_handler_ric.h"

#include "iApps/redis.h"
#include "iApps/stdout.h"
#include "iApps/influx.h"

#include "iApps/subscription_ric.h"
#include "lib/async_event.h" 
#include "lib/ap/free/e2ap_msg_free.h"
#include "lib/pending_event_ric.h"
#include "sm/sm_ric.h" 
#include "util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "util/alg_ds/alg/alg.h"
#include "util/alg_ds/ds/lock_guard/lock_guard.h"
#include "util/compare.h"

#include <assert.h>
#include <dlfcn.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

static inline
void free_sm_ric(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  sm_ric_t* sm = (sm_ric_t*)value;

  void* handle = sm->handle;
  sm->free_sm(sm);

  if(handle != NULL)
      dlclose(handle);
}

static inline
void free_fd(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);
  int* fd = (int*) key;
  assert(*fd > 0 );
  free(value);
}

static inline
void free_pending_ev_ric(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);
  pending_event_ric_t* ev = (pending_event_ric_t*)key;
  assert(valid_pending_event(ev->ev));
  free(value);
}

static inline
void free_subscribed(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

 seq_arr_t* arr = (seq_arr_t*)value; 
 seq_free(arr, NULL);
 free(arr);
}

static
void register_listeners_for_ran_func_id(near_ric_t* ric, uint16_t const* ran_func_id, subs_ric_t subs)
{
 void* start_it = assoc_front(&ric->pub_sub);
 void* end_it = assoc_end(&ric->pub_sub);
 void* it = find_if(&ric->pub_sub, start_it, end_it, ran_func_id, eq_ran_func_id);

  if(it == end_it){
    seq_arr_t* arr = malloc(sizeof(seq_arr_t));
    assert(arr != NULL && "Memory exhausted!!!");
    seq_init(arr, sizeof(subs_ric_t)); //  
    seq_push_back(arr, &subs, sizeof(subs_ric_t));
    assoc_insert(&ric->pub_sub, ran_func_id, sizeof(*ran_func_id), arr); 
  
    // For testing for only one SM
    //void* new_front = assoc_front(&ric->pub_sub);
    //uint16_t* new_key = assoc_key(&ric->pub_sub, new_front); 
    //seq_arr_t* new_val = assoc_value(&ric->pub_sub, new_front);
    //assert(new_val == arr);
    //assert(*new_key == *ran_func_id);
    // End testing
  } else {
    seq_arr_t* arr = assoc_value(&ric->pub_sub, it);
    seq_push_back(arr, &subs, sizeof(subs_ric_t));
  } 
}

static
void load_default_pub_sub_ric(near_ric_t* ric)
{
  assert(ric != NULL);

  void* it = assoc_front(&ric->plugin.sm_ds);
  void* end_it = assoc_end(&ric->plugin.sm_ds);
  while(it != end_it){
    const uint16_t *ran_func_id = assoc_key(&ric->plugin.sm_ds, it);

    subs_ric_t std_listener = {.name = "stdout listener", .fp = notify_stdout_listener };
    register_listeners_for_ran_func_id(ric, ran_func_id, std_listener);

    subs_ric_t redis_listener = {.name = "redis listener", .fp = notify_redis_listener };
    register_listeners_for_ran_func_id(ric, ran_func_id, redis_listener);

    subs_ric_t influx_listener = {.name = "influx listener", .fp = notify_influx_listener };
    register_listeners_for_ran_func_id(ric, ran_func_id, influx_listener);

//    subs_ric_t nng_listener = {.name = "nanomsg listener", .fp = notify_nng_listener };
//    register_listeners_for_ran_func_id(ric, ran_func_id, nng_listener);

    it = assoc_next(&ric->plugin.sm_ds, it);
  }
}

static
void init_handle_msg_ric(e2ap_handle_msg_fp_ric (*handle_msg)[30])
{
  memset((*handle_msg), 0, sizeof(e2ap_handle_msg_fp_ric)*30);
  (*handle_msg)[RIC_SUBSCRIPTION_RESPONSE] =  e2ap_handle_subscription_response_ric;
  (*handle_msg)[RIC_SUBSCRIPTION_FAILURE] =  e2ap_handle_subscription_failure_ric;
  (*handle_msg)[RIC_SUBSCRIPTION_DELETE_RESPONSE] =  e2ap_handle_subscription_delete_response_ric;
  (*handle_msg)[RIC_SUBSCRIPTION_DELETE_FAILURE] =  e2ap_handle_subscription_delete_failure_ric;
  (*handle_msg)[RIC_INDICATION] =  e2ap_handle_indication_ric;
  (*handle_msg)[RIC_CONTROL_ACKNOWLEDGE] =  e2ap_handle_control_ack_ric;
  (*handle_msg)[RIC_CONTROL_FAILURE] =  e2ap_handle_control_failure_ric;
  (*handle_msg)[E2AP_ERROR_INDICATION] =  e2ap_handle_error_indication_ric;
  (*handle_msg)[E2_SETUP_REQUEST] =  e2ap_handle_setup_request_ric;
  (*handle_msg)[E2AP_RESET_REQUEST] =  e2ap_handle_reset_request_ric;
  (*handle_msg)[E2AP_RESET_RESPONSE] =  e2ap_handle_reset_response_ric;
  (*handle_msg)[RIC_SERVICE_UPDATE] =  e2ap_handle_service_update_ric;
  (*handle_msg)[E2_NODE_CONFIGURATION_UPDATE] =  e2ap_handle_node_configuration_update_ric;
  (*handle_msg)[E2_CONNECTION_UPDATE_ACKNOWLEDGE] =  e2ap_handle_connection_update_ack_ric;
  (*handle_msg)[E2_CONNECTION_UPDATE_FAILURE] =  e2ap_handle_connection_update_failure_ric;
}

static inline
void init_pub_sub_ds_ric(near_ric_t* ric)
{
  assert(ric != NULL);

  // Publish Subscriber observer pattern
  uint16_t const ran_func_id = 0; 
  assoc_init(&ric->pub_sub, sizeof(ran_func_id), cmp_ran_func_id,  free_subscribed);  
} 

static inline
void init_e2_nodes_ric(near_ric_t* ric)
{
  assert(ric != NULL);

  pthread_mutexattr_t attr = {0};
#ifdef DEBUG
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); 
#endif
  int rc = pthread_mutex_init(&ric->conn_e2_nodes_mtx, &attr);
  assert(rc == 0);

  seq_init(&ric->conn_e2_nodes, sizeof(e2_node_t));
}

static inline
void init_pending_events(near_ric_t* ric)
{
  assert(ric != NULL);
  size_t const fd_sz = sizeof(int);
  size_t const event_sz = sizeof( pending_event_ric_t );
  bi_map_init(&ric->pending, fd_sz, event_sz, cmp_fd, cmp_pending_event_ric, free_fd, free_pending_ev_ric );

  pthread_mutexattr_t attr = {0};
#ifdef DEBUG
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); 
#endif
 
  int rc = pthread_mutex_init(&ric->pend_mtx, &attr);
  assert(rc == 0);
}

near_ric_t* init_near_ric(fr_args_t const* args)
{
  assert(args != NULL);
  near_ric_t* ric = calloc(1, sizeof(near_ric_t));
  assert(ric != NULL);

  char* addr = get_near_ric_ip(args);
  defer({ free(addr); } );

  const int port = 36421;
  printf("[NEAR-RIC]: nearRT-RIC IP Address = %s, PORT = %d\n", addr, port);
  e2ap_init_ep_ric(&ric->ep, addr, port);

  init_asio_ric(&ric->io); 

  add_fd_asio_ric(&ric->io, ric->ep.base.fd);

  init_ap(&ric->ap.base.type);

  init_handle_msg_ric(&ric->handle_msg);

  init_plugin_ric(&ric->plugin, args->libs_dir);

  init_pub_sub_ds_ric(ric); 

  load_default_pub_sub_ric(ric); 

  init_e2_nodes_ric(ric);

  init_pending_events(ric);

  near_ric_if_t ric_if = {.type = ric};
  init_iapp_api(addr, ric_if);

  ric->req_id = 1021; // 0 could be a sign of a bug
  ric->stop_token = false;
  ric->server_stopped = false;

  return ric;
}

static inline
bool net_pkt(e2ap_ep_t const *ep, int fd)
{
  assert(ep != NULL);
  assert(fd > 0);
  return fd == ep->fd;
}

static inline
void consume_fd(int fd)
{
  assert(fd > 0);
  uint64_t read_buf = 0;
  ssize_t const bytes = read(fd,&read_buf, sizeof(read_buf));
  assert(bytes == sizeof(read_buf));
}

static inline
bool eq_sock_addr(void const* m0_v, void const* m1_v)
{
  assert(m0_v != NULL);
  assert(m1_v != NULL);

  struct sockaddr_in* m0 = (struct sockaddr_in*)m0_v;
  struct sockaddr_in* m1 = (struct sockaddr_in*)m1_v;

  if(m0->sin_family != m1->sin_family)
    return false;

  if(m0->sin_addr.s_addr != m1->sin_addr.s_addr)
    return false;

  if(m0->sin_port != m1->sin_port)
    return false;
  
  return true;
}

static inline
bool addr_registered_or_first_msg(e2ap_msg_t* msg, seq_arr_t* arr, sctp_msg_t const* rcv)
{
  assert(msg != NULL);
  assert(arr != NULL);
  assert(rcv != NULL);

  void* it = seq_front(arr);
  void* end = seq_end(arr);
  it = find_if(arr, it, end, (struct sockaddr_in*) &rcv->info.addr, eq_sock_addr );

  if(msg->type == E2_SETUP_REQUEST){
    assert(it == end && "Socket address already registered!");
    seq_push_back(arr, (struct sockaddr_in*)&rcv->info.addr, sizeof(rcv->info.addr));
    return true;
  }
  assert(it != end && "Socket address not registered and msg is not E2_SETUP_REQUEST");
  return it != end;
}

/*
static
async_event_t find_event_type(near_ric_t* ric, int fd)
{
  assert(ric != NULL);
  assert(fd > 0);
  async_event_t e = {.type = UNKNOWN_EVENT };
  if (net_pkt(ric, fd) == true){
    e.type = NETWORK_EVENT;
  } else if (pend_event(ric, fd, &e.p_ev) == true){
    e.type = PENDING_EVENT;
  } else{
    assert("Unknown event happened!");
  }
  return e;
}
*/

static inline
bool pend_event(near_ric_t* ric, int fd, pending_event_t** p_ev)
{
  assert(ric != NULL);
  assert(fd > 0);
  assert(*p_ev == NULL);
 
  {
  lock_guard(&ric->pend_mtx);
  assert(bi_map_size(&ric->pending) == 1 );

  void* start_it = assoc_front(&ric->pending.left);
  void* end_it = assoc_end(&ric->pending.left);

  void* it = find_if(&ric->pending.left,start_it, end_it, &fd, eq_fd);

  assert(it != end_it);
  *p_ev = assoc_value(&ric->pending.left ,it);
  }
  return *p_ev != NULL;
}



static
async_event_t next_asio_event_ric(near_ric_t* ric)
{
  assert(ric != NULL);

  int const fd = event_asio_ric(&ric->io);

  async_event_t e = {.type = UNKNOWN_EVENT,
                     .fd = fd};

  if(fd == -1){ // no event happened. Just for checking the stop_token condition
    e.type = CHECK_STOP_TOKEN_EVENT;
  } else if (net_pkt(&ric->ep.base, fd) == true){

    e.msg = e2ap_recv_msg_ric(&ric->ep);
    if(e.msg.type == SCTP_MSG_NOTIFICATION ){
      e.type = SCTP_CONNECTION_SHUTDOWN_EVENT;
    } else if (e.msg.type == SCTP_MSG_PAYLOAD){
       e.type = SCTP_MSG_ARRIVED_EVENT;
    } else { 
      assert(0!=0 && "Unknown type");
    }

  } else if (pend_event(ric, fd, &e.p_ev) == true){
    e.type = PENDING_EVENT;
  } else{
    assert( 0!=0 && "Unknown event happened!");
  }
  return e;
}

static
void e2_event_loop_ric(near_ric_t* ric)
{
  assert(ric != NULL);
  while(ric->stop_token == false){ 

    async_event_t e = next_asio_event_ric(ric);
    assert(e.type != UNKNOWN_EVENT && "Unknown event triggered ");

    switch(e.type)
    {
      case SCTP_MSG_ARRIVED_EVENT:
        {
          defer({free_sctp_msg(&e.msg);});

          e2ap_msg_t msg = e2ap_msg_dec_ric(&ric->ap, e.msg.ba); 
          defer({e2ap_msg_free_ric(&ric->ap, &msg); } );

          if(msg.type == E2_SETUP_REQUEST){
            global_e2_node_id_t* id = &msg.u_msgs.e2_stp_req.id;
            printf("Received message with id = %d, port = %d \n", id->nb_id, e.msg.info.addr.sin_port);
            e2ap_reg_sock_addr_ric(&ric->ep, id, &e.msg.info);
          }

          e2ap_msg_t ans = e2ap_msg_handle_ric(ric, &msg);
          defer({ e2ap_msg_free_ric(&ric->ap, &ans); } );

          if(ans.type != NONE_E2_MSG_TYPE){

            sctp_msg_t sctp_msg = { .info = e.msg.info }; 

            sctp_msg.ba = e2ap_msg_enc_ric(&ric->ap, &ans); 
            defer({free_sctp_msg(&sctp_msg); } );

            e2ap_send_sctp_msg_ric(&ric->ep, &sctp_msg);
          }

          break;
        }
      case PENDING_EVENT:
        {
          printf("Pending event timeout happened. Communication with E2 Node lost?\n");
          consume_fd(e.fd);

          break;
        }
      case SCTP_CONNECTION_SHUTDOWN_EVENT: 
        {
          defer({free_sctp_msg(&e.msg);});
          notification_handle_ric(ric, &e.msg);
          break;
        }
      case CHECK_STOP_TOKEN_EVENT:
        {
          break;
        }

      default:
        assert(0!=0 && "Unknown event happened");
    }
  }
  ric->server_stopped = true; 
}

// Blocking call
void start_near_ric(near_ric_t* ric)
{
  assert(ric != NULL);
  e2_event_loop_ric(ric);
}

static
void static_free_e2_node(void* it )
{
  assert(it != NULL);
  e2_node_t* n = (e2_node_t*)it;
  free_e2_node(n);
}

void free_near_ric(near_ric_t* ric)
{
  assert(ric != NULL);

  ric->stop_token = true;
  while(ric->server_stopped == false){
    sleep(1);
  }

  e2ap_free_ep_ric(&ric->ep);

  free_plugin_ric(&ric->plugin); 

  assoc_free(&ric->pub_sub); 

  seq_free(&ric->conn_e2_nodes, static_free_e2_node );

  int rc = pthread_mutex_destroy(&ric->conn_e2_nodes_mtx);
  assert(rc == 0);

  rc = pthread_mutex_destroy(&ric->pend_mtx);
  assert(rc == 0);

  bi_map_free(&ric->pending);

  stop_iapp_api();

  free(ric);
}

static
ric_subscription_request_t generate_subscription_request(near_ric_t* ric, sm_ric_t const* sm, uint16_t ran_func_id, void* cmd)
{
  assert(ric != NULL);
  ric_subscription_request_t sr = {0}; 
  const ric_gen_id_t ric_id = {.ric_req_id = ric->req_id++ ,.ric_inst_id = 0, .ran_func_id = ran_func_id};

  sm_subs_data_t data = sm->proc.on_subscription(sm, cmd);

  sr.ric_id = ric_id;
  sr.event_trigger.len = data.len_et;
  sr.event_trigger.buf = data.event_trigger;

  // We just support one action per subscription msg
  sr.len_action = 1; 
  sr.action = calloc(1,sizeof(ric_action_t ) );
  assert(sr.action != NULL && "Memory exhausted ");

  sr.action[0].id = 0;
  sr.action[0].type = RIC_ACT_REPORT;
  if(data.action_def != NULL){
    sr.action[0].definition = malloc(sizeof(byte_array_t));
    assert(sr.action[0].definition != NULL && "Memory exhausted");
    sr.action[0].definition->buf = data.action_def;
    sr.action[0].definition->len = data.len_ad;
  }

  // Only fulfilled when the type is RIC_ACT_INSERT  
  sr.action[0].subseq_action = NULL;

  return sr;
}

/*
size_t num_conn_e2_nodes(near_ric_t* ric)
{
  assert(ric != NULL);
  return seq_size(&ric->conn_e2_nodes); // e2_node_t 
}
*/

seq_arr_t conn_e2_nodes(near_ric_t* ric)
{
  assert(ric != NULL);

  seq_arr_t arr = {0};
  seq_init(&arr, sizeof(e2_node_t));

  lock_guard(&ric->conn_e2_nodes_mtx);

  void* it = seq_front(&ric->conn_e2_nodes);
  void* end = seq_end(&ric->conn_e2_nodes);
  while(it != end){
    e2_node_t* n = (e2_node_t*)it;   
    seq_push_back(&arr, n, sizeof(e2_node_t));
    it = seq_next(&ric->conn_e2_nodes, it);
  }

  return arr;
}

uint16_t report_service_near_ric(near_ric_t* ric, global_e2_node_id_t const* id, uint16_t ran_func_id, void* cmd)
{
  assert(ric != NULL);
  assert(ran_func_id != 0 && "Reserved SM ID value");
  assert(ran_func_id < 150 && "Not still reached upper limit");
  assert(cmd != NULL);

  sm_ric_t* sm = sm_plugin_ric(&ric->plugin ,ran_func_id); 
  
  ric_subscription_request_t sr = generate_subscription_request(ric, sm, ran_func_id, cmd);  

  // A pending event is created along with a timer of 3000 ms,
  // after which an event will be generated
  pending_event_ric_t ev = {.ev = SUBSCRIPTION_REQUEST_PENDING_EVENT, .id = sr.ric_id };

  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_ric(&ric->io, wait_ms, wait_ms); 

  {
    lock_guard(&ric->pend_mtx);
    bi_map_insert(&ric->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 
  }

  byte_array_t ba_msg = e2ap_enc_subscription_request_ric(&ric->ap, &sr); 

  printf("[NEAR-RIC]: Report Service Asked from nb_id = %d \n", id->nb_id);

  e2ap_send_bytes_ric(&ric->ep, id, ba_msg);
   
  e2ap_free_subscription_request_ric(&ric->ap, &sr);
  free_byte_array(ba_msg);
  return sr.ric_id.ric_req_id;
}

/*
static inline
bool ran_func_id_active(const void* value, const void* it)
{
  assert(value != NULL);
  assert(it != NULL);
  uint16_t* ran_func_id = (uint16_t*)value;
  act_req_t* act = (act_req_t*)it;
  return act->id.ran_func_id == *ran_func_id;
}

static inline
ric_subscription_delete_request_t generate_subscription_delete_request(near_ric_t* ric, act_req_t* act)
{
  assert(ric != NULL);
  assert(act != NULL);
  ric_subscription_delete_request_t sd = {.ric_id = act->id };
  return sd;
}

*/

void rm_report_service_near_ric(near_ric_t* ric, global_e2_node_id_t const* id, uint16_t ran_func_id, uint16_t act_id)
{
  assert(ric != NULL);
  assert(id != NULL);
  assert(act_id > 0);

  // The active requests values are stored in the xApp SDK. Therefore, 
  // there is no data here and we cannot check whether it was before 
  // subscribed.
  ric_subscription_delete_request_t sd = {.ric_id.ric_req_id = act_id, 
                                          .ric_id.ran_func_id = ran_func_id};

  /*
  ric_subscription_delete_request_t sd = {0}; 
  {
//    lock_guard(&ric->act_req_mtx);
//    void* start_it = seq_front(&ric->act_req);
//    void* end_it = seq_end(&ric->act_req);
//    void* it = find_if(&ric->act_req, start_it, end_it, &ran_func_id, ran_func_id_active);
//    assert(it != end_it && "Requested RAN function not actived");
    sd = generate_subscription_delete_request(ric, it);  
  }
*/

 // A pending event is created along with a timer of 3000 ms,
  // after which an event will be generated
  pending_event_ric_t ev = {.ev = SUBSCRIPTION_DELETE_REQUEST_PENDING_EVENT, .id = sd.ric_id };

  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_ric(&ric->io, wait_ms, wait_ms); 

  {
    lock_guard(&ric->pend_mtx);
    bi_map_insert(&ric->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 
  }

  byte_array_t ba_msg = e2ap_enc_subscription_delete_request_ric(&ric->ap, &sd); 

//  struct sockaddr_in const to = find_map_e2_node_sad(&ric->e2_node_sock, id);
  e2ap_send_bytes_ric(&ric->ep, id, ba_msg);

  free_byte_array(ba_msg);
}

static
ric_control_request_t generate_control_request(near_ric_t* ric, sm_ric_t* sm, void* ctrl)
{
  assert(ric != NULL);
  assert(sm != NULL);
  assert(ctrl != NULL);

  const ric_gen_id_t ric_id = {.ric_req_id = ric->req_id++ ,.ric_inst_id = 0, .ran_func_id = sm->ran_func_id};

  ric_control_request_t ctrl_req = {.ric_id = ric_id };
  ctrl_req.ack_req = malloc(sizeof(ric_control_ack_req_t ));
  assert(ctrl_req.ack_req != NULL && "Memory exhausted" );
  *ctrl_req.ack_req = RIC_CONTROL_REQUEST_ACK; 

  sm_ctrl_req_data_t data = sm->proc.on_control_req(sm, ctrl);

  ctrl_req.hdr.len = data.len_hdr;
  ctrl_req.hdr.buf = data.ctrl_hdr;
  ctrl_req.msg.len = data.len_msg;
  ctrl_req.msg.buf = data.ctrl_msg;

  return ctrl_req;
}

void control_service_near_ric(near_ric_t* ric, global_e2_node_id_t const* id, uint16_t ran_func_id, void* ctrl)
{
  assert(ric != NULL);
  assert(ran_func_id > 0);
  assert(ctrl != NULL);

//  assert(ran_func_id == SM_RC_ID || ran_func_id == SM_SLICE_ID || ran_func_id == SM_TC_ID );
  assert(ran_func_id == 3 || ran_func_id == 145 || ran_func_id == 146);

  sm_ric_t* sm = sm_plugin_ric(&ric->plugin ,ran_func_id); 

  ric_control_request_t ctrl_req = generate_control_request(ric, sm, ctrl);

  // A pending event is created along with a timer of 1000 ms,
  // after which an event will be generated
  pending_event_ric_t ev = {.ev = CONTROL_REQUEST_PENDING_EVENT, .id = ctrl_req.ric_id };

  long const wait_ms = 2000;
  int fd_timer = create_timer_ms_asio_ric(&ric->io, wait_ms, wait_ms); 
  //printf("RIC: Control fd_timer for control with value created == %d\n", fd_timer);

  {
    lock_guard(&ric->pend_mtx);
    bi_map_insert(&ric->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 
  }

  byte_array_t ba_msg = e2ap_enc_control_request_ric(&ric->ap, &ctrl_req); 

//  struct sockaddr_in const to = find_map_e2_node_sad(&ric->e2_node_sock, id);
  e2ap_send_bytes_ric(&ric->ep, id, ba_msg);

  printf("[NEAR-RIC]: CONTROL SERVICE sent\n");

  e2ap_free_control_request_ric(&ric->ap, &ctrl_req);
  free_byte_array(ba_msg);
}

void load_sm_near_ric(near_ric_t* ric, const char* file_name)
{
  assert(ric != NULL);
  assert(file_name != NULL);
  assert(strlen(file_name) < 128);
  // PATH_MAX defined in limits.h
  char full_path[PATH_MAX+1 + 128] = {0};

  const char* dir_path = getcwd(full_path, sizeof(full_path));
  assert(dir_path != NULL);
  if(full_path[strlen(full_path)] != '/')
    full_path[strlen(full_path)] = '/'; 
  
  char* ptr = full_path + strlen(full_path);
  strncat(ptr, file_name, strlen(file_name));

  load_plugin_ric(&ric->plugin, full_path);

  tx_plugin_ric(&ric->plugin, strlen(full_path), full_path); 
}

///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////

void start_near_ric_iapp(near_ric_t* ric)
{
  assert(ric != NULL);
//  assert(0!=0 && "not implemented");
}

void stop_near_ric_iapp()
{
  //assert(ric != NULL);
//  assert(0!=0 && "not implemented");
}

uint16_t fwd_ric_subscription_request(near_ric_t* ric, global_e2_node_id_t const* id, ric_subscription_request_t const* sr, void (*f)(e2ap_msg_t const* msg))
{
  assert(ric != NULL);
  assert(sr != NULL);
  assert(f != NULL);
  uint16_t const ric_req_id = ric->req_id++;

  *(uint16_t*)&sr->ric_id.ric_req_id = ric_req_id;


  // A pending event is created along with a timer of 3000 ms,
  // after which an event will be generated
  pending_event_ric_t ev = {.ev = SUBSCRIPTION_REQUEST_PENDING_EVENT, .id = sr->ric_id };

  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_ric(&ric->io, wait_ms, wait_ms); 

  {
    lock_guard(&ric->pend_mtx);
    bi_map_insert(&ric->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 
  }

  byte_array_t ba_msg = e2ap_enc_subscription_request_ric(&ric->ap, sr); 
  defer({ free_byte_array(ba_msg); });

  e2ap_send_bytes_ric(&ric->ep, id, ba_msg);
   
  return ric_req_id;
}

void fwd_ric_subscription_request_delete(near_ric_t* ric, global_e2_node_id_t const* id, ric_subscription_delete_request_t const* sdr, void (*f)(e2ap_msg_t const* msg))
{
  assert(ric != NULL);
  assert(sdr != NULL);
  assert(f != NULL);

  // A pending event is created along with a timer of 3000 ms,
  // after which an event will be generated
  pending_event_ric_t ev = {.ev = SUBSCRIPTION_DELETE_REQUEST_PENDING_EVENT, .id = sdr->ric_id };

  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_ric(&ric->io, wait_ms, wait_ms); 

  {
    lock_guard(&ric->pend_mtx);
    bi_map_insert(&ric->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 
  }

  byte_array_t ba_msg = e2ap_enc_subscription_delete_request_ric(&ric->ap, sdr); 
  defer({ free_byte_array(ba_msg); }  );

  e2ap_send_bytes_ric(&ric->ep, id, ba_msg);

  puts("[NEAR-RIC]: SUBSCRIPTION DELETE REQUEST tx\n" );
}

uint16_t fwd_ric_control_request(near_ric_t* ric, global_e2_node_id_t const* id, ric_control_request_t const* cr,  void (*f)(e2ap_msg_t const* msg))
{
  assert(ric != NULL);
  assert(cr != NULL);
  assert(f != NULL);

  uint16_t const ric_req_id = ric->req_id++;
  *(uint16_t*)&cr->ric_id.ric_req_id = ric_req_id;

  // A pending event is created along with a timer of 3000 ms,
  // after which an event will be generated
  pending_event_ric_t ev = {.ev = CONTROL_REQUEST_PENDING_EVENT, .id = cr->ric_id };

  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_ric(&ric->io, wait_ms, wait_ms); 
  {
    lock_guard(&ric->pend_mtx);
    bi_map_insert(&ric->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 
  }

  byte_array_t ba_msg = e2ap_enc_control_request_ric(&ric->ap, cr); 
  defer({ free_byte_array(ba_msg); } );

  e2ap_send_bytes_ric(&ric->ep, id, ba_msg);

  puts("[NEAR-RIC]: CONTROL SERVICE sent\n");

  return ric_req_id; 
}

