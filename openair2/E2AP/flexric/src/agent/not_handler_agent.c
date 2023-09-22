#include "not_handler_agent.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "../lib/pending_events.h"

void notification_handle_ag(e2_agent_t* ag, sctp_msg_t const* msg)
{
  assert(ag != NULL);
  assert(msg != NULL && msg->type == SCTP_MSG_NOTIFICATION);

  // A pending event is created along with a timer of 3000 ms,
  // after which an event will be generated
  pending_event_t ev = SETUP_REQUEST_PENDING_EVENT;
  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_agent(&ag->io, wait_ms, wait_ms); 

  bi_map_insert(&ag->pending, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev)); 


  puts("E2 AGENT: Communication with the nearRT-RIC lost\n" );

}

