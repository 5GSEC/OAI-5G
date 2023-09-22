#ifndef RAN_FUNC_SM_SLICE_READ_WRITE_AGENT_H
#define RAN_FUNC_SM_SLICE_READ_WRITE_AGENT_H

#include "../../flexric/src/agent/e2_agent_api.h"

void read_slice_sm(void*);

void read_slice_setup_sm(void* data);

sm_ag_if_ans_t write_ctrl_slice_sm(void const* data);

#endif

