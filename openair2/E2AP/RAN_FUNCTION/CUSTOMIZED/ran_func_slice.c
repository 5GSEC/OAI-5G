#include "ran_func_slice.h"
#include "../../flexric/test/rnd/fill_rnd_data_slice.h"
#include <assert.h>
#include <stdio.h>

void read_slice_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == SLICE_STATS_V0);

  slice_ind_data_t* slice = (slice_ind_data_t*)data;
  fill_slice_ind_data(slice);
}

void read_slice_setup_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == SLICE_AGENT_IF_E2_SETUP_ANS_V0 );

  assert(0 !=0 && "Not supported");
}

sm_ag_if_ans_t write_ctrl_slice_sm(void const* data)
{
  assert(data != NULL);
//  assert(data->type == SLICE_CTRL_REQ_V0);

  slice_ctrl_req_data_t const* slice_req_ctrl = (slice_ctrl_req_data_t const* )data; // &data->slice_req_ctrl;
  slice_ctrl_msg_t const* msg = &slice_req_ctrl->msg;

  if(msg->type == SLICE_CTRL_SM_V0_ADD){
    printf("[E2 Agent]: SLICE CONTROL ADD rx\n");
  } else if (msg->type == SLICE_CTRL_SM_V0_DEL){
    printf("[E2 Agent]: SLICE CONTROL DEL rx\n");
  } else if (msg->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
    printf("[E2 Agent]: SLICE CONTROL ASSOC rx\n");
  } else {
    assert(0!=0 && "Unknown msg_type!");
  }

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = SLICE_AGENT_IF_CTRL_ANS_V0;
  return ans;

}


