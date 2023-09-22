#ifndef RAN_FUNC_SM_PDCP_READ_WRITE_AGENT_H
#define RAN_FUNC_SM_PDCP_READ_WRITE_AGENT_H

#include "../../flexric/src/agent/e2_agent_api.h"

void read_pdcp_sm(void*);

void read_pdcp_setup_sm(void* data);

sm_ag_if_ans_t write_ctrl_pdcp_sm(void const* data);

#endif

