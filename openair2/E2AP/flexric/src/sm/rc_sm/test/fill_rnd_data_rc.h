#ifndef FILL_RND_RC_EVENT_TRIGGERR_H
#define FILL_RND_RC_EVENT_TRIGGERR_H


#include "../ie/rc_data_ie.h"

e2sm_rc_event_trigger_t fill_rnd_rc_event_trigger(void);

e2sm_rc_action_def_t fill_rnd_rc_action_def(void);

e2sm_rc_ind_hdr_t fill_rnd_rc_ind_hdr(void);

e2sm_rc_ind_msg_t fill_rnd_rc_ind_msg(void);

e2sm_rc_cpid_t fill_rnd_rc_cpid(void);

e2sm_rc_ctrl_hdr_t fill_rnd_rc_ctrl_hdr(void);

e2sm_rc_ctrl_msg_t fill_rnd_rc_ctrl_msg(void);

e2sm_rc_ctrl_out_t fill_rnd_rc_ctrl_out(void);

e2sm_rc_func_def_t fill_rnd_rc_ran_func_def(void);


ran_function_name_t fill_rc_ran_func_name(void);

ran_func_def_ev_trig_t fill_rc_ran_func_def_ev_trig(void);

ran_func_def_report_t fill_rc_ran_func_def_report(void);

ran_func_def_insert_t fill_rc_ran_func_def_insert(void);

ran_func_def_ctrl_t fill_rc_ran_func_def_ctrl(void);

ran_func_def_policy_t fill_rc_ran_func_def_policy(void);

#endif

