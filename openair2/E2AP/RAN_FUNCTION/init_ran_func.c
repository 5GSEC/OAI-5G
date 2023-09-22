#include "init_ran_func.h"
#include "../flexric/src/agent/e2_agent_api.h"
#include "CUSTOMIZED/ran_func_mac.h"
#include "CUSTOMIZED/ran_func_rlc.h"
#include "CUSTOMIZED/ran_func_pdcp.h"
#include "CUSTOMIZED/ran_func_slice.h"
#include "CUSTOMIZED/ran_func_tc.h"
#include "CUSTOMIZED/ran_func_gtp.h"
#include "O-RAN/ran_func_kpm.h"
#include "O-RAN/ran_func_rc.h"

static
void init_read_ind_tbl(read_ind_fp (*read_ind_tbl)[SM_AGENT_IF_READ_V0_END])
{
  (*read_ind_tbl)[MAC_STATS_V0] =  read_mac_sm;
  (*read_ind_tbl)[RLC_STATS_V0] =  read_rlc_sm ;
  (*read_ind_tbl)[PDCP_STATS_V0] = read_pdcp_sm ;
  (*read_ind_tbl)[SLICE_STATS_V0] = read_slice_sm ;
  (*read_ind_tbl)[TC_STATS_V0] = read_tc_sm ;
  (*read_ind_tbl)[GTP_STATS_V0] = read_gtp_sm ; 
  (*read_ind_tbl)[KPM_STATS_V3_0] = read_kpm_sm ; 
  (*read_ind_tbl)[RAN_CTRL_STATS_V1_03] = read_rc_sm;
}

static
void init_read_setup_tbl(read_e2_setup_fp (*read_setup_tbl)[SM_AGENT_IF_E2_SETUP_ANS_V0_END])
{
  (*read_setup_tbl)[MAC_AGENT_IF_E2_SETUP_ANS_V0] =  read_mac_setup_sm;
  (*read_setup_tbl)[RLC_AGENT_IF_E2_SETUP_ANS_V0] =  read_rlc_setup_sm ;
  (*read_setup_tbl)[PDCP_AGENT_IF_E2_SETUP_ANS_V0] = read_pdcp_setup_sm ;
  (*read_setup_tbl)[SLICE_AGENT_IF_E2_SETUP_ANS_V0] =  read_slice_setup_sm ;
  (*read_setup_tbl)[TC_AGENT_IF_E2_SETUP_ANS_V0] =  read_tc_setup_sm ;
  (*read_setup_tbl)[GTP_AGENT_IF_E2_SETUP_ANS_V0] = read_gtp_setup_sm ; 
  (*read_setup_tbl)[KPM_V3_0_AGENT_IF_E2_SETUP_ANS_V0] = read_kpm_setup_sm ; 
  (*read_setup_tbl)[RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0] = read_rc_setup_sm;
}

static
void init_write_ctrl( write_ctrl_fp (*write_ctrl_tbl)[SM_AGENT_IF_WRITE_CTRL_V0_END])
{
  (*write_ctrl_tbl)[MAC_CTRL_REQ_V0] = write_ctrl_mac_sm;
  (*write_ctrl_tbl)[RLC_CTRL_REQ_V0] = write_ctrl_rlc_sm;
  (*write_ctrl_tbl)[PDCP_CTRL_REQ_V0] = write_ctrl_pdcp_sm;
  (*write_ctrl_tbl)[SLICE_CTRL_REQ_V0] = write_ctrl_slice_sm;
  (*write_ctrl_tbl)[TC_CTRL_REQ_V0] = write_ctrl_tc_sm;
  (*write_ctrl_tbl)[GTP_CTRL_REQ_V0] = write_ctrl_gtp_sm;
  (*write_ctrl_tbl)[RAN_CONTROL_CTRL_V1_03] = write_ctrl_rc_sm;
}

static
void init_write_subs(write_subs_fp (*write_subs_tbl)[SM_AGENT_IF_WRITE_SUBS_V0_END])
{
  (*write_subs_tbl)[MAC_SUBS_V0] = NULL;
  (*write_subs_tbl)[RLC_SUBS_V0] = NULL;
  (*write_subs_tbl)[PDCP_SUBS_V0] = NULL;
  (*write_subs_tbl)[SLICE_SUBS_V0] = NULL;
  (*write_subs_tbl)[TC_SUBS_V0] = NULL;
  (*write_subs_tbl)[GTP_SUBS_V0] = NULL;
  (*write_subs_tbl)[KPM_SUBS_V3_0] = NULL;
  (*write_subs_tbl)[RAN_CTRL_SUBS_V1_03] = write_subs_rc_sm;
}

sm_io_ag_ran_t init_ran_func_ag(void)
{
  sm_io_ag_ran_t io = {0};
  init_read_ind_tbl(&io.read_ind_tbl);
  init_read_setup_tbl(&io.read_setup_tbl);
  init_write_ctrl(&io.write_ctrl_tbl);
  init_write_subs(&io.write_subs_tbl);

  return io;
}

