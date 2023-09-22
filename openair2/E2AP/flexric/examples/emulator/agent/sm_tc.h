#ifndef SM_TC_READ_WRITE_AGENT_H
#define SM_TC_READ_WRITE_AGENT_H

#include "../../../src/agent/e2_agent_api.h"

void read_tc_sm(void*);

void read_tc_setup_sm(void* data);

sm_ag_if_ans_t write_ctrl_tc_sm(void const* data);

#endif

