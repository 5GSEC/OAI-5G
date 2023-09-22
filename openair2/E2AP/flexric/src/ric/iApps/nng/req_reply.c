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



#include "req_reply.h"
#include <assert.h>                                        // for assert
#include <limits.h>                                        // for LLONG_MAX
#include <nng/nng.h>                                       // for nng_send
#include <nng/protocol/reqrep0/rep.h>                      // for nng_rep0_open
#include <pthread.h>                                       // for pthread_t
#include <stdio.h>                                         // for printf, NULL
#include <stdlib.h>                                        // for strtoull
#include <string.h>                                        // for strlen
#include "../../../util/alg_ds/alg/string/search_naive.h"  // for search_naive
#include "ric/iApps/nng/msgs/xapp_msgs.h"                  // for xapp_msg_t


#define INIT "INIT"
#define KEEP_ALIVE "PING"

#define REQUEST "REQUEST"
#define REPORT "REPORT"
#define CONTROL "CONTROL"
#define INSERT "INSERT"
#define POLICY "POLICY"

static
pthread_t nn_run_thread_ping;
bool first_time_server_ping = true; 

static
pthread_t nn_run_thread_msg;
bool first_time_server_msg = true; 

static
nng_socket sock_ping;

static
nng_socket sock_msg;



static inline
void fatal(const char *func, int rv)
{
  fprintf(stderr, "%s: %s\n", func, nng_strerror(rv));
  exit(1);
}

static
int64_t parse_xapp_msg_id(char* buf, size_t sz)
{
  assert(buf != NULL);
  assert(sz > 0);

  const char* needle = "ID=";

  char* it = strstr( buf, needle);
  if(it == NULL){
    printf("ID not found in the string passed\n");
    return -1;
  }
  assert(it != NULL && "ID not found in the string passed");

  const char* delimiter = ";";
  char* end = strstr(it, delimiter);
  if(end == NULL){
    printf("Could not find the delimiter in the buf \n");
    return -1;
  }

  assert(end != NULL && "Could not find the delimiter in the buf");
  int const distance = end - it;

  int id_sz = distance - strlen(needle); 
  assert(id_sz > -1);
  char dst[64] = {0};
  assert(id_sz < 64);
  strncpy(dst, it + strlen(needle) , id_sz);
  const int base = 10;

  char** endptr = NULL;
  const uint64_t id = strtoull(dst, endptr, base);
  assert(id != 0 && id < ULLONG_MAX);
  return id;
}

static
int64_t parse_int64(char* needle, char* buf, size_t sz)
{
  assert(needle != NULL);
  assert(buf != NULL);
  assert(sz != 0);
  char* it = search_naive(strlen(needle), needle, sz, buf);
  if(it == NULL){
    printf(" Could not find the string ID=!\n");
    return -1;
  }

  assert(it != NULL && "Could not find the string ID=!");
  char* delimiter = ";";
  char* end = search_naive(strlen(delimiter), delimiter, sz - (buf-it) , it);
  if(end == NULL){
    printf("Could not find the delimiter in the buf\n");
    return -1;
  }
  assert(end != NULL && "Could not find the delimiter in the buf");

  int const distance = end - it;

  int id_sz = distance - strlen(needle); 
  assert(id_sz > -1);
  char dst[64] = {0};
  assert(id_sz < 64);
  strncpy(dst, it + strlen(needle) , id_sz);
  const int base = 10;

  char** endptr = NULL;
  const uint64_t id = strtoull(dst, endptr, base);
  if(id == 0 || id > LLONG_MAX){
    puts("ID too large or zero \n");
    return -1;
  }
  assert(id != 0 && id < LLONG_MAX);

  return id;
}

static
xapp_req_msg_t parse_xapp_msg_req(char* buf, size_t sz)
{
  assert(buf != NULL);
  assert(sz > 0);

  xapp_req_msg_t req_msg = {.type = XAPP_REQUEST_TYPE_UNKNOWN };

  char* needle = "ID=";

  int64_t const xapp_id = parse_int64(needle, buf, sz);
  if(xapp_id == -1) 
    return req_msg;
  req_msg.xapp_id = xapp_id; 
  char* it = search_naive(strlen(REPORT), REPORT, sz , buf);  

  if(it != NULL) {
    req_msg.type = XAPP_REQUEST_TYPE_REPORT; 
    xapp_request_report_t* rep = &req_msg.report;
    if(strncmp("REPORT=MAC", it, strlen("REPORT=MAC")) == 0 ) {
      rep->type = XAPP_MAC_REPORT;
    }else if (strncmp("REPORT=RLC", it, strlen("REPORT=RLC")) == 0 ){
      rep->type = XAPP_RLC_REPORT;
    }else if (strncmp("REPORT=RRC", it, strlen("REPORT=RRC")) == 0 ){
      rep->type = XAPP_RRC_REPORT;
    }else if (strncmp("REPORT=PDCP", it, strlen("REPORT=PDCP")) == 0 ){
      rep->type = XAPP_PDCP_REPORT;
    }else { 
      printf(" Unknown report asked! = %s \n", buf );
//      assert(0 != 0 && "Unknown report asked!");
    }

    return req_msg;
/*
    char* delimiter = ";";
    char* end = search_naive(strlen(delimiter), delimiter, sz - (buf-it) , it);
    assert(end != NULL && "Could not find the delimiter in the buf");

    sz -= end + 1 - buf;
    buf = end + 1;
*/
//    rep->mac.interval_ms = parse_int64("INTERVAL=", buf, sz); 


  }

  return req_msg;


  it = search_naive(strlen(CONTROL), CONTROL, sz, buf);  

  if(it != NULL) {
    req_msg.type = XAPP_REQUEST_TYPE_CONTROL; 
    
    return req_msg;
  }

  it = search_naive(strlen(INSERT), INSERT, sz, buf);  

  if(it != NULL) {
    req_msg.type = XAPP_REQUEST_TYPE_CONTROL; 
    
    return req_msg;
  }


  it = search_naive(strlen(POLICY), POLICY, sz, buf);  

  if(it != NULL) {
    req_msg.type = XAPP_REQUEST_TYPE_POLICY; 
    
    return req_msg;
  }

  assert("Unknown string received!!");

  return req_msg; 
}


static
xapp_msg_t parse_xapp_msg(size_t sz, char buf[sz])
{
  assert(buf != NULL);
  assert(sz > 0);
  printf("Size received = %lu\n", sz);
  fflush(stdout);

  char* cpy = malloc(sz+1);
  assert(cpy != NULL);
  cpy[sz] = '\0';

  strncpy(cpy, buf, sz);

  printf("Value of the buffer in parse_xapp = %s \n", cpy);
  fflush(stdout);
  free(cpy);

  xapp_msg_t msg = {.type = XAPP_MSG_UNKNOWN};

  if ((sz == (strlen(INIT) + 1)) && (strncmp(INIT, buf, strlen(INIT)) == 0)) {
    printf("NODE0: RECEIVED INIT\n");
    msg.type = XAPP_MSG_INIT;
  }else if (search_naive(strlen(KEEP_ALIVE), KEEP_ALIVE, sz, buf) != NULL) {
    printf("NODE0: RECEIVED PING\n");
    msg.type = XAPP_MSG_KEEP_ALIVE;
   int64_t const msg_id = parse_xapp_msg_id(buf,sz);
   if(msg_id != -1 ){
     msg.ping.xapp_id = msg_id;
   }  
  }else if (search_naive(strlen(REQUEST), REQUEST, sz, buf) != NULL) {
    printf("NODE0: RECEIVED REQUEST\n");
    msg.type = XAPP_MSG_REQUEST;
    msg.req = parse_xapp_msg_req(buf, sz);
  } else {
    printf("Unknown string received\n");
    msg.type = XAPP_MSG_UNKNOWN;
//    assert(0!=0 && "Unknown string received");
  }
  return msg;
}
   

static
void* run_nn_req_reply_server_ping(void* arg_v)
{
  assert(arg_v != NULL);
  req_reply_server_arg_t* arg = (req_reply_server_arg_t*)arg_v;

  int rv;

  if ((rv = nng_rep0_open(&sock_ping)) != 0) {
    fatal("nng_rep0_open", rv);
  }
  if ((rv = nng_listen(sock_ping, arg->url, NULL, 0)) != 0) {
    fatal("nng_listen", rv);
  }

  for(;;) {

    char *buf = NULL;
    size_t sz = 0;
    if ((rv = nng_recv(sock_ping, &buf, &sz, NNG_FLAG_ALLOC)) != 0) {
      fatal("nng_recv", rv);
    }

    xapp_msg_t msg = parse_xapp_msg(sz, buf);

    if(msg.type == XAPP_MSG_INIT ){

      printf("Before generate id \n");
      fflush(stdout);
      const uint64_t id = arg->init_fp(&msg.init, arg->data);
      printf("After generate id \n");
      fflush(stdout);

      char str[64] = {0};
      sprintf(str, "%lu", id);
      printf("NODE0: SENDING REQUEST_ID %s\n", str);
      if ((rv = nng_send(sock_ping, str, strlen(str) + 1, 0)) != 0) {
        fatal("nng_send", rv);
      }
    } else if (msg.type == XAPP_MSG_KEEP_ALIVE){
      if(arg->keep_alive_fp(&msg.ping, arg->data) == true){
        char* pong = "PONG";
        if ((rv = nng_send(sock_ping, pong, strlen(pong) + 1, 0)) != 0) {
          fatal("nng_send", rv);
        }
      } else {
        printf("Unknow ping id value, so ignoring \n"); 
      }
    } else {
      //assert(0!=0 && "Unknown message type detected");
      printf("Unknown message type in ping server %s\n", buf);
    }
    nng_free(buf, sz);
  }

  printf("Ending req_reply ping server \n" );
  return NULL;
}

static
void* run_nn_req_reply_server_msg(void* arg_v)
{
  assert(arg_v != NULL);
  req_reply_server_arg_t* arg = (req_reply_server_arg_t*)arg_v;

  int rv;

  if ((rv = nng_rep0_open(&sock_msg)) != 0) {
    fatal("nng_rep0_open", rv);
  }
  if ((rv = nng_listen(sock_msg, arg->url, NULL, 0)) != 0) {
    fatal("nng_listen", rv);
  }

 for(;;) {

    char *buf = NULL;
    size_t sz = 0;
    if ((rv = nng_recv(sock_msg, &buf, &sz, NNG_FLAG_ALLOC)) != 0) {
      fatal("nng_recv", rv);
    }

    xapp_msg_t msg = parse_xapp_msg(sz, buf);

    if (msg.type == XAPP_MSG_REQUEST){

      xapp_request_answer_e const ans = arg->request_fp(&msg.req, arg->data);

      if(ans == XAPP_ANS_UNKNOWN_ID){
        char* str = "UNKNOWN_ID";
        if ((rv = nng_send(sock_msg, str, strlen(str) + 1, 0)) != 0) {
          fatal("nng_send", rv);
        }
      } else if (ans == XAPP_ANS_OK ) {
        char* str = "ANSWER_OK";
        if ((rv = nng_send(sock_msg, str, strlen(str) + 1, 0)) != 0) {
          fatal("nng_send", rv);
        }
      }
    } else {
     // assert(0!=0 && "Unknown message type detected");
     printf("Unknown message type detected in msg server = %s\n", buf);
    }
    nng_free(buf, sz);
  }

   printf("Returning from req_reply msg server \n" );
  return NULL;
}


static
void init_req_reply_server(pthread_t* t, void* (*f)(void*) , req_reply_server_arg_t* arg)
{
  assert(t != NULL);
  assert(f != NULL);
  assert(arg != NULL);

  const pthread_attr_t *restrict attr = NULL;
  int rc = pthread_create(t, attr, f, arg);
  assert(rc == 0 );
}



void init_req_reply_server_ping(req_reply_server_arg_t* arg)
{
  assert(arg != NULL);
  assert(first_time_server_ping == true);
  first_time_server_ping = false;
  init_req_reply_server( &nn_run_thread_ping, run_nn_req_reply_server_ping, arg);
  printf("[iApp]: Server ping started\n");

}

void init_req_reply_server_msg(req_reply_server_arg_t* arg)
{
  assert(arg != NULL);
  assert(first_time_server_msg == true);
  first_time_server_msg = false;
  init_req_reply_server( &nn_run_thread_msg, run_nn_req_reply_server_msg, arg);

  printf("[iApp]: Server Request/Reply started\n");
}

static
void stop_req_reply_server(nng_socket* sock, pthread_t* t)
{
  assert(sock != NULL);
  assert(t != NULL);

  int rv = 0;
  if ((rv = nng_close(*sock)) != 0) {
    fatal("nng_rep0_open", rv);
  }

  void **retval = NULL;
  int rc = pthread_join(*t, retval);
  assert(rc == 0);
}

void stop_req_reply_server_ping(void)
{
  stop_req_reply_server(&sock_ping, &nn_run_thread_ping);
}

void stop_req_reply_server_msg(void)
{
  stop_req_reply_server(&sock_msg, &nn_run_thread_msg);
}

