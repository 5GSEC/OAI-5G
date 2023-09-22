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

#include "asio_agent.h"
#include "e2_agent.h"
#include "endpoint_agent.h"
#include "msg_handler_agent.h"
#include "not_handler_agent.h"
#include "lib/async_event.h"
#include "lib/ap/e2ap_ap.h"
#include "lib/ap/free/e2ap_msg_free.h"
#include "sm/mac_sm/mac_sm_agent.h"
#include "sm/rlc_sm/rlc_sm_agent.h"
#include "util/alg_ds/alg/alg.h"
#include "util/alg_ds/ds/lock_guard/lock_guard.h"
#include "util/compare.h"

#include <assert.h>
#include <stdio.h>


// Equality file descriptors
static inline
bool eq_fd_pair(const void* key1, const void* key2 )
{
  assert(key1 != NULL);
  assert(key2 != NULL);

  fd_pair_t* fd1 = (fd_pair_t*)key1;
  fd_pair_t* fd2 = (fd_pair_t*)key2;

  assert(fd1->r > 0 && "File descriptors must be larger than zero");
  assert(fd2->r > 0 && "File descriptors must be larger than zero");

  assert(fd1->w > 0 && "File descriptors must be larger than zero");
  assert(fd2->w > 0 && "File descriptors must be larger than zero");

  return eq_fd(&fd1->r, &fd2->r) && eq_fd(&fd1->w, &fd2->w);
}

// Comparation file descriptors
static inline 
int cmp_fd_pair(void const* fd_v1, void const* fd_v2)
{
  assert(fd_v1 != NULL);
  assert(fd_v2 != NULL);
 fd_pair_t* fd1 = (fd_pair_t*)fd_v1;
 fd_pair_t* fd2 = (fd_pair_t*)fd_v2;

  assert(fd1->r > 0&& "File descriptors must be larger than zero");
  assert(fd2->r > 0&& "File descriptors must be larger than zero");

  assert(fd1->w > 0&& "File descriptors must be larger than zero");
  assert(fd2->w > 0&& "File descriptors must be larger than zero");

  int cmp = cmp_fd(&fd1->r, &fd2->r);
  if(cmp != 0)
    return cmp;

  return cmp_fd(&fd1->w, &fd2->w);
}

static
e2_setup_request_t generate_setup_request(e2_agent_t* ag)
{
  assert(ag != NULL);

  const size_t len_rf = assoc_size(&ag->plugin.sm_ds);
  assert(len_rf > 0 && "No RAN function/service model registered. Check if the Service Models are located at shared library paths, default location is /usr/local/flexric/");

  ran_function_t* ran_func = calloc(len_rf, sizeof(*ran_func));
  assert(ran_func != NULL);

  e2_setup_request_t sr = {
    .id = ag->global_e2_node_id,
    .ran_func_item = ran_func,
    .len_rf = len_rf,
    .comp_conf_update = NULL,
    .len_ccu = 0
  };

  void* it = assoc_front(&ag->plugin.sm_ds);
  for(size_t i = 0; i < len_rf; ++i){
    sm_agent_t* sm = assoc_value(&ag->plugin.sm_ds, it);
    assert(sm->ran_func_id == *(uint16_t*)assoc_key(&ag->plugin.sm_ds, it) && "RAN function mismatch");

    sm_e2_setup_data_t def = sm->proc.on_e2_setup(sm);
    assert(sm->ran_func_id == def.rf.id);

    if(def.len_rfd > 0)
      free(def.ran_fun_def);

    ran_func[i] = def.rf;

    it = assoc_next(&ag->plugin.sm_ds ,it);
  }
  assert(it == assoc_end(&ag->plugin.sm_ds) && "Length mismatch");
  return sr;
}

static
ric_indication_t generate_aindication(e2_agent_t* ag, sm_ind_data_t* data, aind_event_t* ai_ev)
{
  assert(ag != NULL);
  assert(data != NULL);
  assert(ai_ev != NULL);

  ric_indication_t ind = {
    .ric_id = ai_ev->ric_id, 
    .action_id = ai_ev->action_id, 
    .sn = NULL, 
    .type = RIC_IND_REPORT };

  ind.hdr.len = data->len_hdr;
  ind.hdr.buf = data->ind_hdr;
  ind.msg.len = data->len_msg;
  ind.msg.buf = data->ind_msg;
  if(data->call_process_id != NULL){
    ind.call_process_id = malloc(sizeof(data->len_cpid) );
    assert(ind.call_process_id != NULL && "Memory exhausted" );
    ind.call_process_id->buf = data->call_process_id;
    ind.call_process_id->len = data->len_cpid;
  }
  return ind;
}



static
ric_indication_t generate_indication(e2_agent_t* ag, sm_ind_data_t* data, ind_event_t* i_ev)
{
  assert(ag != NULL);
  assert(data != NULL);
  assert(i_ev != NULL);

  ric_indication_t ind = {
    .ric_id = i_ev->ric_id, 
    .action_id = i_ev->action_id, 
    .sn = NULL, 
    .type = RIC_IND_REPORT };

  ind.hdr.len = data->len_hdr;
  ind.hdr.buf = data->ind_hdr;
  ind.msg.len = data->len_msg;
  ind.msg.buf = data->ind_msg;
  if(data->call_process_id != NULL){
    ind.call_process_id = malloc(sizeof(data->len_cpid) );
    assert(ind.call_process_id != NULL && "Memory exhausted" );
    ind.call_process_id->buf = data->call_process_id;
    ind.call_process_id->len = data->len_cpid;
  }
  return ind;
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
void free_pending_ev(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);
  pending_event_t* ev = (pending_event_t*)key;
  assert(valid_pending_event(*ev));
  free(value);
}

static inline
void free_pending_agent(e2_agent_t* ag)
{
  assert(ag != NULL);
  bi_map_free(&ag->pending);
}

static inline
void free_indication_event(e2_agent_t* ag)
{
  assert(ag != NULL);
  bi_map_free(&ag->ind_event);
  pthread_mutex_destroy(&ag->mtx_ind_event);
}

static inline
void init_pending_events(e2_agent_t* ag)
{
  assert(ag != NULL);
  size_t fd_sz = sizeof(int);
  size_t event_sz = sizeof( pending_event_t );
  bi_map_init(&ag->pending, fd_sz, event_sz, cmp_fd, cmp_pending_event, free_fd, free_pending_ev );
}

static inline
void free_ind_event_map(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  ind_event_t* ev = (ind_event_t* )value;
  if(ev->sm->free_act_def != NULL)
    ev->sm->free_act_def(ev->sm, ev->act_def); 

  free(ev);
}

static inline
void free_key(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  (void)key;

  int* fd = (int*)value;
  free(fd);
}


static inline
void init_indication_event(e2_agent_t* ag)
{
  assert(ag != NULL);
  size_t key_sz_fd = sizeof(int);
  size_t key_sz_ind = sizeof(ind_event_t);

  bi_map_init(&ag->ind_event, key_sz_fd, key_sz_ind, cmp_fd, cmp_ind_event, free_ind_event_map, free_key);

  pthread_mutexattr_t attr = {0};
#ifdef DEBUG
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); 
#endif
  int rc = pthread_mutex_init(&ag->mtx_ind_event, &attr);
  assert(rc == 0);

}

static inline
void* ind_fd(e2_agent_t* ag, int fd)
{
  assert(ag != NULL);
  assert(fd > 0);

  void* start_it = assoc_front(&ag->ind_event.left);
  void* end_it = assoc_end(&ag->ind_event.left);

  void* it = find_if(&ag->ind_event.left, start_it, end_it, &fd, eq_fd );
  return it;
}

static inline
bool net_pkt(const e2_agent_t* ag, int fd)
{
  assert(ag != NULL);
  assert(fd > 0);
  return fd == ag->ep.base.fd;
}

static inline
bool ind_event(e2_agent_t* ag, int fd, ind_event_t** i_ev)
{
  assert(*i_ev == NULL);
  void* it = ind_fd(ag, fd);   
  void* end_it = assoc_end(&ag->ind_event.left); // bi_map_end_left(&ag->ind_event);
  if(it != end_it){
    *i_ev = assoc_value(&ag->ind_event.left, it);
    return true;
  } 
  return false;
}

static inline
bool aind_event(e2_agent_t* ag, int fd, aind_event_t* ai_ev)
{
  if(fd != ag->io.pipe.r)
    return false;

  pop_tsq(&ag->aind, ai_ev, sizeof(aind_event_t));

  return true;
}

static inline
bool pend_event(e2_agent_t* ag, int fd, pending_event_t** p_ev)
{
  assert(ag != NULL);
  assert(fd > 0);
  assert(*p_ev == NULL);
  
  assert(bi_map_size(&ag->pending) == 1 );

  void* start_it = assoc_front(&ag->pending.left);
  void* end_it = assoc_end(&ag->pending.left);

  void* it = find_if(&ag->pending.left,start_it, end_it, &fd, eq_fd);

  assert(it != end_it);
  *p_ev = assoc_value(&ag->pending.left ,it);
  return *p_ev != NULL;
}

/*
static
async_event_t find_event_type(e2_agent_t* ag, int fd)
{
  assert(ag != NULL);
  assert(fd > 0);
  async_event_t e = {.type = UNKNOWN_EVENT };
  if (net_pkt(ag, fd) == true){
    e.type = NETWORK_EVENT;
  } else if (ind_event(ag, fd, &e.i_ev) == true) {
    e.type = INDICATION_EVENT;
  } else if (pend_event(ag, fd, &e.p_ev) == true){
    e.type = PENDING_EVENT;
  } else{
    assert("Unknown event happened!");
  }
  return e;
}
*/

static
void consume_fd(int fd)
{
  assert(fd > 0);
  uint64_t read_buf = 0;
  ssize_t const bytes = read(fd,&read_buf, sizeof(read_buf));
  assert(bytes <= (ssize_t)sizeof(read_buf));
}

static
async_event_t next_async_event_agent(e2_agent_t* ag)
{
  assert(ag != NULL);

  int const fd = event_asio_agent(&ag->io);

  async_event_t e = {.type = UNKNOWN_EVENT,
                     .fd = fd};

  if(fd == -1){ // no event happened. Just for checking the stop_token condition
    e.type = CHECK_STOP_TOKEN_EVENT;

  } else if (net_pkt(ag, fd) == true){
    e.msg = e2ap_recv_msg_agent(&ag->ep);
    if(e.msg.type == SCTP_MSG_NOTIFICATION){
      e.type = SCTP_CONNECTION_SHUTDOWN_EVENT;

    } else if (e.msg.type == SCTP_MSG_PAYLOAD){
       e.type = SCTP_MSG_ARRIVED_EVENT;

    } else { 
      assert(0!=0 && "Unknown type");
    }

  } else if(aind_event(ag, fd, &e.ai_ev) == true) {
    e.type = APERIODIC_INDICATION_EVENT;

  } else if (ind_event(ag, fd, &e.i_ev) == true) {
    e.type = INDICATION_EVENT;

  } else if (pend_event(ag, fd, &e.p_ev) == true){
    e.type = PENDING_EVENT;

  } else {
    assert(0!=0 && "Unknown event happened!");
  }
  return e;
}

static
void e2_event_loop_agent(e2_agent_t* ag)
{
  assert(ag != NULL);
  while(ag->stop_token == false){

    async_event_t e = next_async_event_agent(ag); 
    assert(e.type != UNKNOWN_EVENT && "Unknown event triggered ");

    switch(e.type){
      case SCTP_MSG_ARRIVED_EVENT:
        {
          defer({free_sctp_msg(&e.msg);});

          e2ap_msg_t msg = e2ap_msg_dec_ag(&ag->ap, e.msg.ba);
          defer( { e2ap_msg_free_ag(&ag->ap, &msg);} );

          e2ap_msg_t ans = e2ap_msg_handle_agent(ag, &msg);
          defer( { e2ap_msg_free_ag(&ag->ap, &ans);} );

          if(ans.type != NONE_E2_MSG_TYPE){
            byte_array_t ba_ans = e2ap_msg_enc_ag(&ag->ap, &ans); 
            defer ({free_byte_array(ba_ans); } );

            e2ap_send_bytes_agent(&ag->ep, ba_ans);
          }

          break;
        }
      case APERIODIC_INDICATION_EVENT:
        {
          sm_agent_t const* sm = e.ai_ev.sm;
          void* ind_data = e.ai_ev.ind_data;
          sm_ind_data_t data = sm->proc.on_indication(sm, ind_data); // , &e.i_ev->ric_id);

          ric_indication_t ind = generate_aindication(ag, &data, &e.ai_ev);
          defer({ e2ap_free_indication(&ind); } );

          byte_array_t ba = e2ap_enc_indication_ag(&ag->ap, &ind); 
          defer({ free_byte_array(ba); } );
          
          e2ap_send_bytes_agent(&ag->ep, ba);

          consume_fd(ag->io.pipe.r);

          break;
        }
      case INDICATION_EVENT:
        {
          sm_agent_t const* sm = e.i_ev->sm;
          void* act_def = e.i_ev->act_def; 
          sm_ind_data_t data = sm->proc.on_indication(sm, act_def); // , &e.i_ev->ric_id);

          ric_indication_t ind = generate_indication(ag, &data, e.i_ev);
          defer({ e2ap_free_indication(&ind); } );

          byte_array_t ba = e2ap_enc_indication_ag(&ag->ap, &ind); 
          defer({ free_byte_array(ba); } );

          e2ap_send_bytes_agent(&ag->ep, ba);

          consume_fd(e.fd);

          break;
        }
      case PENDING_EVENT:
        {
          assert(*e.p_ev == SETUP_REQUEST_PENDING_EVENT && "Unforeseen pending event happened!" );

          // Resend the setup request message
          e2_setup_request_t sr = generate_setup_request(ag); 
          defer({ e2ap_free_setup_request(&sr); } );

          printf("[E2AP] Resending Setup Request after timeout\n");
          byte_array_t ba = e2ap_enc_setup_request_ag(&ag->ap, &sr); 
          defer({ free_byte_array(ba); } ); 

          e2ap_send_bytes_agent(&ag->ep, ba);

          consume_fd(e.fd);

          break;
        }
      case SCTP_CONNECTION_SHUTDOWN_EVENT: 
        {
          notification_handle_ag(ag, &e.msg);
          break;
        }
      case CHECK_STOP_TOKEN_EVENT:
        {
          break;
        }
      default:
        {
          assert(0!=0 && "Unknown event happened");
          break;
        }
    }
  }

  printf("ag->agent_stopped = true \n");
  ag->agent_stopped = true;
}

e2_agent_t* e2_init_agent(const char* addr, int port, global_e2_node_id_t ge2nid, sm_io_ag_ran_t io, char const* libs_dir)
{
  assert(addr != NULL);
  assert(port > 0 && port < 65535);

  printf("[E2 AGENT]: Initializing ... \n");

  e2_agent_t* ag = calloc(1, sizeof(*ag));
  assert(ag != NULL && "Memory exhausted");

  e2ap_init_ep_agent(&ag->ep, addr, port);

  init_asio_agent(&ag->io); 

  add_fd_asio_agent(&ag->io, ag->ep.base.fd);

  init_ap(&ag->ap.base.type);

  init_handle_msg_agent(&ag->handle_msg);

  init_plugin_ag(&ag->plugin, libs_dir, io);

  init_pending_events(ag);

  init_indication_event(ag);

  init_tsq(&ag->aind, sizeof(aind_event_t));

  ag->global_e2_node_id = ge2nid;
  ag->stop_token = false;
  ag->agent_stopped = false;
  
  return ag;
}

void e2_start_agent(e2_agent_t* ag)
{
  assert(ag != NULL);

  // Resend the subscription request message
  e2_setup_request_t sr = generate_setup_request(ag); 
  defer({ e2ap_free_setup_request(&sr);  } );

  printf("[E2-AGENT]: Sending setup request\n");
  byte_array_t ba = e2ap_enc_setup_request_ag(&ag->ap, &sr); 
  defer({free_byte_array(ba); } ); 

  e2ap_send_bytes_agent(&ag->ep, ba);

  // A pending event is created along with a timer of 3000 ms,
  // after which an event will be generated
  pending_event_t ev = SETUP_REQUEST_PENDING_EVENT;
  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_agent(&ag->io, wait_ms, wait_ms); 
  //printf("fd_timer with value created == %d\n", fd_timer);

  bi_map_insert(&ag->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 

  e2_event_loop_agent(ag);
}

void e2_free_agent(e2_agent_t* ag)
{
  ag->stop_token = true;
  while(ag->agent_stopped == false){
    usleep(1000);
  }

  free_plugin_ag(&ag->plugin);

  free_pending_agent(ag);

  free_indication_event(ag);

  free_tsq(&ag->aind, NULL);

  free(ag);
}

void e2_async_event_agent(e2_agent_t* ag, uint32_t ric_req_id, void* ind_data)
{
  assert(ag != NULL);

  void* f = NULL; 
  void* l = NULL;
  void* it = NULL;

  ind_event_t tmp = {.ric_id.ric_req_id = ric_req_id, 
    .sm = NULL, 
    .action_id = 0 };

  assoc_rb_tree_t* tree = &ag->ind_event.right;
 
  for(size_t i =0; i < 10; ++i){
    int rc = pthread_mutex_lock(&ag->mtx_ind_event);  
    assert(rc == 0);

    f = assoc_rb_tree_front(tree);
    l = assoc_rb_tree_end(tree);
    it = find_if_rb_tree(tree, f, l, &tmp, eq_ind_event_ric_req_id); 
    if(it != l) break; 

    rc = pthread_mutex_unlock(&ag->mtx_ind_event);
    assert(rc == 0);

    // Give some time to propagate the subscription request and be sure
    // it has been writen in the ind_event ds.
    usleep(10);
  }

  assert(it != l && "Not found RIC Request ID");

  ind_event_t* ind_ev = assoc_rb_tree_key(tree, it);

  aind_event_t aind = {.ric_id = ind_ev->ric_id,
    .sm = ind_ev->sm,
    .action_id = ind_ev->action_id,
    .ind_data = ind_data};

  int rc = pthread_mutex_unlock(&ag->mtx_ind_event);
  assert(rc == 0);

  // Push the data into the queue
  push_tsq(&ag->aind, &aind, sizeof(aind_event_t));

  // Inform epoll that an aperiodic event happened
  int const num_char = 32;
  char str[num_char];
  memset(str, '\0', num_char);
  rc = snprintf(str, num_char ,"%u\n", ric_req_id );
  assert(rc > 0 && rc < num_char -1);

  rc = write(ag->io.pipe.w, str, rc);
  assert(rc != 0);
}

/*
void e2_async_agent(e2_agent_t* ag, uint32_t ric_req_id)
{
  assert(ag != NULL);
  
  ind_event_t tmp = {.ric_id.ric_req_id = ric_req_id, 
                     .sm = NULL, 
                     .action_id = 0 };

  void* start_r = NULL;
  void* end_r = NULL;
  void* it_r = NULL; 
  int num = 5;
  while(num > 0){
    
    int rc = pthread_mutex_lock(&ag->mtx_ind_event);  
    assert(rc == 0);

    start_r = assoc_rb_tree_front(&ag->ind_event.right);
    end_r = assoc_rb_tree_end(&ag->ind_event.right);
    it_r = find_if_rb_tree(&ag->ind_event.right, start_r, end_r, &tmp, eq_ind_event_ric_req_id); 
    if(it_r != end_r) break;

    rc = pthread_mutex_unlock(&ag->mtx_ind_event);
    assert(rc == 0);

    num -= 1;
    // Give some time to propagate the subscription request and be sure
    // it has been writen in the ind_event ds.
    usleep(10);
  }

  assert(it_r != end_r && "ric_rec_id not found");
  ind_event_t* ind_ev = assoc_rb_tree_key(&ag->ind_event.right, it_r);
  int fd_write = ind_ev->fd_write; 
  int rc = pthread_mutex_unlock(&ag->mtx_ind_event);
  assert(rc == 0);

  assert(fd_write > 0);
  int num_char = 32;
  char str[num_char];
  memset(str, '\0', num_char);
  rc = snprintf(str, num_char ,"%u\n", ric_req_id );
  assert(rc > 0 && rc < num_char -1);

  rc = write(fd_write, str, rc);
  assert(rc != 0);
}
*/

//////////////////////////////////
/////////////////////////////////

void e2_send_subscription_response(e2_agent_t* ag, const ric_subscription_response_t* sr)
{
  assert(ag != NULL);
  assert(sr != NULL);

  byte_array_t ba = e2ap_enc_subscription_response_ag(&ag->ap, sr);
  e2ap_send_bytes_agent(&ag->ep, ba);
  free_byte_array(ba);
}

void e2_send_subscription_failure(e2_agent_t* ag, const ric_subscription_failure_t* sf)
{
  assert(ag != NULL);
  assert(sf != NULL);

  byte_array_t ba = e2ap_enc_subscription_failure_ag(&ag->ap, sf);
  e2ap_send_bytes_agent(&ag->ep, ba);
  free_byte_array(ba);
}

void e2_send_indication_agent(e2_agent_t* ag, const ric_indication_t* indication)
{
  assert(ag != NULL);
  assert(indication != NULL);

  byte_array_t ba = e2ap_enc_indication_ag(&ag->ap, indication);
  e2ap_send_bytes_agent(&ag->ep, ba);
  free_byte_array(ba);
}

void e2_send_subscription_delete_response(e2_agent_t* ag, const ric_subscription_delete_response_t* sdr)
{
  assert(ag != NULL);
  assert(sdr != NULL);
  byte_array_t ba = e2ap_enc_subscription_delete_response_ag(&ag->ap, sdr);
  e2ap_send_bytes_agent(&ag->ep, ba);
  free_byte_array(ba);
}

void e2_send_subscription_delete_failure(e2_agent_t* ag, const ric_subscription_delete_failure_t* sdf)
{
  byte_array_t ba = e2ap_enc_subscription_delete_failure_ag(&ag->ap, sdf );
  e2ap_send_bytes_agent(&ag->ep, ba);
  free_byte_array(ba);
}

void e2_send_control_acknowledge(e2_agent_t* ag, const ric_control_acknowledge_t* ca)
{
  byte_array_t ba = e2ap_enc_control_acknowledge_ag(&ag->ap, ca);
  e2ap_send_bytes_agent(&ag->ep, ba);
  free_byte_array(ba);
}

void e2_send_control_failure(e2_agent_t* ag, const ric_control_failure_t* cf)
{
  byte_array_t ba = e2ap_enc_control_failure_ag(&ag->ap, cf);
  e2ap_send_bytes_agent(&ag->ep, ba);
  free_byte_array(ba);
}




