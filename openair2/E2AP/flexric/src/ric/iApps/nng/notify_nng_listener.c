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



#include "notify_nng_listener.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../string_parser.h"

#include "../../../util/alg_ds/ds/assoc_container/assoc_reg.h"
#include "../../../util/alg_ds/ds/seq_container/seq_generic.h"
#include "../../../util/alg_ds/alg/find.h"
#include "../../../util/alg_ds/alg/for_each.h"
#include "../../../util/alg_ds/alg/defer.h"

#include "keep_alive/keep_alive_timer.h"
#include "msgs/xapp_msgs.h"

#include "req_reply.h"

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

static
pthread_once_t init_nng_once = PTHREAD_ONCE_INIT;

static
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static
assoc_reg_t reg = {0};

static
nng_socket sock;

static
req_reply_server_arg_t arg_ping;

static
req_reply_server_arg_t arg_msg;

typedef struct{
  bool active;
} xapp_act_t;

static inline
void fatal(const char *func, int rv)
{
  fprintf(stderr, "%s: %s\n", func, nng_strerror(rv));
  exit(1);
}

static
void mark_as_inactive(const void* it, const void* data)
{
  assert(data == NULL);
  assert(it != NULL);

  registry_bucket_t* b = (registry_bucket_t*)it;

  xapp_act_t* xapp = (xapp_act_t*)b->val;
  xapp->active = false;
}

static inline
bool xapp_not_active(void* data)
{
  assert(data != NULL);
  xapp_act_t* xapp = (xapp_act_t*)data;
  return !xapp->active;
}

static
void keep_alive_timer_expired(void* data)
{
  assert(data != NULL);
  //printf("Keep alive timer function called!\n");
  //fflush(stdout);

  assoc_reg_t* reg = (assoc_reg_t*)data;

  pthread_mutex_lock(&mtx);
  defer({pthread_mutex_unlock(&mtx);});

  seq_arr_t arr = remove_if_range_reg(reg, xapp_not_active);
  size_t sz = seq_size(&arr);
  if(sz > 0)
    printf("[iApp]: Disconnected xApps = %lu \n", sz);

  void* start_it = assoc_reg_front(reg);
  void* end = assoc_reg_end(reg);

  void* d = NULL;
  for_each_reg(reg, start_it, end, mark_as_inactive, d);

  //for_all_registry(reg, mark_as_inactive);
  //size_t sz = seq_size(&arr); 
  
  while(sz > 0){
    printf("[iApp]: Deleted xApp id = %u \n", *(uint32_t*)seq_at(&arr,sz-1)); 
    sz -= 1;
  }

  seq_arr_free(&arr, NULL);
}

static
uint64_t generate_id(xapp_init_msg_t const* msg, void* data)
{
  (void)msg;
  printf("generate id called\n");
  assert(data != NULL);
  assoc_reg_t* reg = (assoc_reg_t*)data;

  xapp_act_t xapp = {.active = true};

  pthread_mutex_lock(&mtx);
  defer({pthread_mutex_unlock(&mtx);});
  uint32_t const id = assoc_reg_push_back(reg, &xapp, sizeof(xapp));
 // printf("Id registeered = %u \n", id);
  //uint32_t const id = push_back_registry(reg, &xapp, sizeof(xapp));

  return id;
}


static
bool keep_alive_ping(xapp_keep_alive_msg_t const* msg , void* data)
{
  assert(msg != NULL);
  assert(data != NULL);

  assoc_reg_t* reg = (assoc_reg_t*)data;

  //printf("Keep alive ping function called \n");

  pthread_mutex_lock(&mtx);
  defer({pthread_mutex_unlock(&mtx);});

 if(reg->sz == 0){
    printf("Register size is zero \n");
    return false; 
  }

  void* start_it = assoc_reg_front(reg);
  void* end = assoc_reg_end(reg);
 
  assert(start_it != end);

  uint32_t const xapp_id = msg->xapp_id;
  void* it = find_reg(reg, start_it, end, xapp_id);

  if(it == end){
    printf("ID = %u not registered!!", xapp_id);
    fflush(stdout);
    //assert(0!=0);
    return false; 
  }
  
  xapp_act_t* v = assoc_reg_value(reg, it);
  v->active = true;


  return true; 
}

static
xapp_request_answer_e request_fun(xapp_req_msg_t const* msg, void* data)
{
  assert(msg != NULL);
  assert(data != NULL);

  assoc_reg_t* reg = (assoc_reg_t*)data;

  pthread_mutex_lock(&mtx);
  defer({pthread_mutex_unlock(&mtx);});

  void* it = assoc_reg_front(reg);
  void* end = assoc_reg_end(reg);

  uint32_t const xapp_id = msg->xapp_id;
  it = find_reg(reg, it, end, xapp_id);

  if(it == end){
    //assert(0!=0);
    printf("Unknown xapp ID in request fun = %u \n", xapp_id);
    fflush(stdout);
    return XAPP_ANS_UNKNOWN_ID;
  }

  xapp_act_t* v = assoc_reg_value(reg, it);


  assert(v != NULL && "Registered memory corrupted!");

// ToDO: Insert the code for managing the number of requests!

  return XAPP_ANS_OK;
}

static
void stop_nng_listener(void)
{
  stop_keep_alive_timer();
  stop_req_reply_server_ping();
  stop_req_reply_server_msg();

  assoc_reg_free(&reg);

  int rv = 0;
  if ((rv = nng_close(sock)) != 0) {
    fatal("nng_rep0_open", rv);
  }
}


static
void init_pub_server(const char* url)
{
  assert(url != NULL);

  int rv = -1;

  if ((rv = nng_pub0_open(&sock)) != 0) {
    fatal("nng_pub0_open", rv);
  }
  if ((rv = nng_listen(sock, url, NULL, 0)) < 0) {
    fatal("nng_listen", rv);
  }

}

static
void init_nng_iapp()
{
//  const char* url_ping = "ipc:///tmp/reqrep.ipc";
//  const char* url_msg = "ipc:///tmp/reqrep2.ipc";
//  const char* url_pub = "ipc:///tmp/pubsub.ipc"; 

  const char* url_ping = "ws://0.0.0.0:9990";
  const char* url_msg = "ws://0.0.0.0:9991";
  const char* url_pub = "ws://0.0.0.0:9992";

//  const char* url_ping = "ws://192.168.52.10:9990";
//  const char* url_msg = "ws://192.168.52.10:9991";
//  const char* url_pub = "ws://192.168.52.10:9992";


  const uint32_t exp_time_ms = 5000;

  assoc_reg_init(&reg, sizeof(xapp_act_t));

  init_keep_alive_timer(exp_time_ms, keep_alive_timer_expired, &reg);

  size_t const url_ping_sz = strlen(url_ping);

  arg_ping.init_fp = generate_id;
  arg_ping.keep_alive_fp = keep_alive_ping;
  arg_ping.request_fp = NULL;
  arg_ping.data = &reg;

  assert(url_ping_sz < sizeof(arg_ping.url)/sizeof(arg_ping.url[0]) && "Too large url for the struct req_rreply_server_arg_t");

  strncpy(arg_ping.url, url_ping, url_ping_sz); 

  init_req_reply_server_ping(&arg_ping);


  size_t const url_msg_sz = strlen(url_msg);

  arg_msg.init_fp = NULL;
  arg_msg.keep_alive_fp = NULL;
  arg_msg.request_fp = request_fun;
  arg_msg.data = &reg;

  assert(url_msg_sz < sizeof(arg_msg.url)/sizeof(arg_msg.url[0]) && "Too large url for the struct req_rreply_server_arg_t");

  strncpy(arg_msg.url, url_msg, url_msg_sz); 

  init_req_reply_server_msg(&arg_msg);

  init_pub_server(url_pub);

  // at exit clean the resources taken
  atexit(stop_nng_listener);
}


void notify_nng_listener(sm_ag_if_rd_t const* data)
{
  assert(data != NULL);
  assert(data->type == MAC_STATS_V0 || data->type == RLC_STATS_V0 || data->type == PDCP_STATS_V0 || data->type == SLICE_STATS_V0);

  pthread_once(&init_nng_once, init_nng_iapp);

  if(data->type == MAC_STATS_V0){
    mac_ind_msg_t const* ind =  &data->mac_stats.msg;
    for(uint32_t i = 0; i < ind->len_ue_stats; ++i){
      char stats[1024] = {0};
      to_string_mac_ue_stats(&ind->ue_stats[i], ind->tstamp, stats, 1024);
      int rv = -1;
      if ((rv = nng_send(sock, stats, strlen(stats) + 1, 0)) != 0) {
        fatal("nng_send", rv);
      }
    }
  } else if (data->type == RLC_STATS_V0){
    rlc_ind_msg_t const* ind = &data->rlc_stats.msg;

    for(uint32_t i = 0; i < ind->len; ++i){
      char stats[1024] = {0};
      to_string_rlc_rb(&ind->rb[i], ind->tstamp, stats, 1024);
      int rv;
      if ((rv = nng_send(sock, stats, strlen(stats) + 1, 0)) != 0) {
        fatal("nng_send", rv);
      }
    }
  } else if (data->type == PDCP_STATS_V0){
    pdcp_ind_msg_t const* ind = &data->pdcp_stats.msg;

    for(uint32_t i = 0; i < ind->len; ++i){
      char stats[512] = {0};
      pdcp_radio_bearer_stats_t* rb = &ind->rb[i];
      to_string_pdcp_rb(rb, ind->tstamp, stats, 512);
      int rv;
      if ((rv = nng_send(sock, stats, strlen(stats) + 1, 0)) != 0) {
        fatal("nng_send", rv);
      }
    } 
  } else if(data->type == SLICE_STATS_V0){
    slice_ind_msg_t const* ind = &data->slice_stats.msg;

    char stats[2048] = {0};
    to_string_slice(ind, ind->tstamp, stats, 2048);
      int rv;
      if ((rv = nng_send(sock, stats, strlen(stats) + 1, 0)) != 0) {
        fatal("nng_send", rv);
      }
  } else {
    assert(0 != 0 && "invalid data type ");
  }

}

