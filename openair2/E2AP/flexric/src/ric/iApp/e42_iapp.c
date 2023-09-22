
#include "../../util/alg_ds/alg/defer.h"
#include "../../util/alg_ds/alg/find.h"
#include "../../util/alg_ds/alg/alg.h"
#include "../../util/alg_ds/ds/lock_guard/lock_guard.h"

#include "e42_iapp.h"
#include "msg_handler_iapp.h"
#include "iapp_if_generic.h"
#include "xapp_ric_id.h"

#include "../../lib/ep/sctp_msg.h"

#include <stdio.h>
#include <pthread.h>

static
int64_t time_now_us(void)
{
  struct timespec tms;

  /* The C11 way */
  /* if (! timespec_get(&tms, TIME_UTC))  */

  /* POSIX.1-2008 way */
  if (clock_gettime(CLOCK_REALTIME,&tms)) {
    return -1;
  }
  /* seconds, multiplied with 1 million */
  int64_t micros = tms.tv_sec * 1000000;
  /* Add full microseconds */
  micros += tms.tv_nsec/1000;
  /* round up if necessary */
  if (tms.tv_nsec % 1000 >= 500) {
    ++micros;
  }
  return micros;
}


e42_iapp_t* init_e42_iapp(const char* addr, near_ric_if_t ric_if)
{
  assert(addr != NULL);
//  assert(ric != NULL);

  printf("[iApp]: Initializing ... \n");

  e42_iapp_t* iapp = calloc(1, sizeof(*iapp));
  assert(iapp != NULL && "Memory exhausted");

  iapp->ric_if = ric_if;

  // Emulator
  start_near_ric_iapp_gen(iapp->ric_if.type);

  uint32_t const port = 36422;
  printf("[iApp]: nearRT-RIC IP Address = %s, PORT = %d\n", addr, port);
  e2ap_init_ep_iapp(&iapp->ep, addr, port);

  init_asio_iapp(&iapp->io); 

  add_fd_asio_iapp(&iapp->io, iapp->ep.base.fd);

  assert(iapp->io.efd < 1024);

  init_ap(&iapp->ap.base.type);

  assert(iapp->io.efd < 1024);

  init_handle_msg_iapp(&iapp->handle_msg);

  assert(iapp->io.efd < 1024);

  init_reg_e2_node(&iapp->e2_nodes);

  assert(iapp->io.efd < 1024);

  init_map_ric_id(&iapp->map_ric_id);


  iapp->xapp_id = 7;

  iapp->stop_token = false;
  iapp->stopped = false;

  return iapp;
}


static inline
bool net_pkt(const e42_iapp_t* iapp, int fd)
{
  assert(iapp != NULL);
  assert(fd > 0);
  return fd == iapp->ep.base.fd;
}

static
void consume_fd(int fd)
{
  assert(fd > 0);
  uint64_t read_buf = 0;
  ssize_t const bytes = read(fd,&read_buf, sizeof(read_buf));
  assert(bytes == sizeof(read_buf));
}


/*
static
e2ap_msg_t compose_msg(e42_iapp_t* iapp)
{
  byte_array_t ba = e2ap_recv_msg_iapp(&iapp->ep);
  defer({free_byte_array(ba); } );

  return e2ap_msg_dec_iapp(&iapp->ap,ba); 
}

static
void handle_msg(e42_iapp_t* iapp, e2ap_msg_t const* msg)
{
  e2ap_msg_t ans = e2ap_msg_handle_iapp(iapp, msg);
  defer({ e2ap_msg_free_iapp(&iapp->ap, &ans);} );

  if(ans.type != NONE_E2_MSG_TYPE ){
      //printf("Encoding and sending message type = %d \n", ans.type );
      byte_array_t ba_ans = e2ap_msg_enc_iapp(&iapp->ap, &ans); 
      defer({ free_byte_array(ba_ans); } );

      // Get the endpoint for the appropiate xApp
      e2ap_send_bytes_iapp(&iapp->ep, ba_ans);
      if(ans.type == RIC_SUBSCRIPTION_DELETE_RESPONSE)
        printf("RIC_SUBSCRIPTION_DELETE_RESPONSE sent with size = %ld \n", ba_ans.len);

      if(ans.type ==  RIC_INDICATION ){
        int64_t now = time_now_us();
        //printf("Time diff at iapp after sending = %ld \n", now -msg->tstamp);
       // assert(0!=0);
      }
  }
}
*/

static
void e2_event_loop_iapp(e42_iapp_t* iapp)
{
  assert(iapp != NULL);
  while(iapp->stop_token == false){ 
    int fd = event_asio_iapp(&iapp->io);
    if(fd == -1) continue; // no event happened. Just for checking the stop_token condition

    if(net_pkt(iapp, fd) == true){

      sctp_msg_t rcv = e2ap_recv_msg_iapp(&iapp->ep);
      defer({ free_sctp_msg(&rcv); } );

      e2ap_msg_t msg = e2ap_msg_dec_iapp(&iapp->ap, rcv.ba); 
      defer({e2ap_msg_free_iapp(&iapp->ap, &msg); } );

      // TODO: need to update connected e2 nodes
      e2ap_msg_t ans = e2ap_msg_handle_iapp(iapp, &msg);
      defer({e2ap_msg_free_iapp(&iapp->ap, &ans);});

      if(ans.type == E42_SETUP_RESPONSE ){
        const uint16_t xapp_id = ans.u_msgs.e42_stp_resp.xapp_id;
        e2ap_reg_sock_addr_iapp(&iapp->ep, xapp_id, &rcv.info);;
      }


      if(ans.type != NONE_E2_MSG_TYPE ){

        sctp_msg_t sctp_msg = { .info.addr = rcv.info.addr,
                .info.sri = rcv.info.sri,
        };

        sctp_msg.ba = e2ap_msg_enc_iapp(&iapp->ap, &ans);
        defer({ free_sctp_msg(&sctp_msg); } );

        // Get the endpoint for the appropiate xApp
        e2ap_send_sctp_msg_iapp(&iapp->ep, &sctp_msg);

        if(ans.type == RIC_SUBSCRIPTION_DELETE_RESPONSE)
          printf("RIC_SUBSCRIPTION_DELETE_RESPONSE sent with size = %ld \n", sctp_msg.ba.len);
        if(ans.type ==  RIC_INDICATION ){
          int64_t now = time_now_us();
          printf("Time diff at iapp after sending = %ld \n", now - msg.tstamp);
          // assert(0!=0);
        }
      }

      // handle_msg(iapp, &msg);

    } else {
      printf("Pending event timeout happened. Communication lost?\n");
      consume_fd(fd);
      //      assert(fd == ric->ep.base.fd && "Only pkt interrupt implemented done. Pending events need to be implemented");
    }
  }
  iapp->stopped = true; 
}


// Blocking call
void start_e42_iapp(e42_iapp_t* iapp)
{
  assert(iapp != NULL);
  assert(iapp->io. efd < 1024);

  e2_event_loop_iapp(iapp);
}

void free_e42_iapp(e42_iapp_t* iapp)
{
  assert( iapp != NULL);

  iapp->stop_token = true;
  while(iapp->stopped == false){
    sleep(1);
  }

  // Emulator
  //stop_near_ric_iapp_gen(iapp->ric_if.type);

  e2ap_free_ep_iapp(&iapp->ep);

  free_reg_e2_node(&iapp->e2_nodes);

  free_map_ric_id(&iapp->map_ric_id);

  free(iapp);
}


void add_e2_node_iapp(e42_iapp_t* i, global_e2_node_id_t* id, size_t len, ran_function_t const ran_func[len])
{
  assert(i != NULL);
  assert(id != NULL);
  assert(len > 0);
  assert(ran_func != NULL);

  add_reg_e2_node(&i->e2_nodes,id, len, ran_func);
}

void rm_e2_node_iapp(e42_iapp_t* i, global_e2_node_id_t* id)
{
  assert(i != NULL);
  assert(id != NULL);

  rm_reg_e2_node(&i->e2_nodes,id);
}

/*
static
xapp_ric_id_t get_ric_gen_id(e42_iapp_t* iapp, e2ap_msg_t const* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);

  assert(msg->type == RIC_INDICATION 
        || msg->type == RIC_CONTROL_ACKNOWLEDGE); 
  
  uint32_t ric_req_id = 0;
  if(msg->type == RIC_INDICATION){
    ric_req_id = msg->u_msgs.ric_ind.ric_id.ric_req_id;
  } else if(msg->type == RIC_CONTROL_ACKNOWLEDGE){
    ric_req_id = msg->u_msgs.ric_ctrl_ack.ric_id.ric_req_id;
  } else {
    assert(0!=0 && "Unknown message type" );
  }

  xapp_ric_id_t x = find_xapp_map_ric_id(&iapp->map_ric_id, ric_req_id);
  return x;
}

static
sctp_info_t get_sctp_info(e42_iapp_t* iapp, e2ap_msg_t const* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);

  xapp_ric_id_t ric_id = get_ric_gen_id(iapp, msg);
  sctp_info_t info = find_map_xapps_sad(&iapp->ep.xapps, ric_id.xapp_id);
  return info;
}
*/

void notify_msg_iapp(e42_iapp_t* iapp, e2ap_msg_t const* msg)
{
  assert(iapp != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_INDICATION 
      || msg->type == RIC_SUBSCRIPTION_RESPONSE 
      || msg->type == RIC_SUBSCRIPTION_DELETE_RESPONSE
      || msg->type == RIC_CONTROL_ACKNOWLEDGE);


  e2ap_msg_t ans = e2ap_msg_handle_iapp(iapp, msg);
  defer({ e2ap_msg_free_iapp(&iapp->ap, &ans); } );

  assert(ans.type ==  NONE_E2_MSG_TYPE );
/*
  if(ans.type != NONE_E2_MSG_TYPE){

    sctp_msg_t sctp_msg = { .info = *from};

    sctp_msg.ba = e2ap_msg_enc_iapp(&iapp->ap, &ans); 
    defer({ free_sctp_msg(&sctp_msg); } );

    // Get the endpoint for the appropiate xApp
    e2ap_send_sctp_msg_iapp(&iapp->ep, &sctp_msg);
  }
*/

//  sctp_info_t s = find_map_xapps_sad(map_xapps_sockaddr_t* m, uint16_t xapp_id)
//  sctp_info_t s = find_xapps
//  handle_msg(iapp, msg);
}

