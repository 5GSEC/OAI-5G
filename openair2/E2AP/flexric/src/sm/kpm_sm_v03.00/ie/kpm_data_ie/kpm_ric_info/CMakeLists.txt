set(KPM_RIC_INFO_SRCS 
  kpm_ran_function_def.c
  kpm_ric_action_def.c
  kpm_ric_action_def_frm_1.c
  kpm_ric_action_def_frm_2.c
  kpm_ric_action_def_frm_3.c
  kpm_ric_action_def_frm_4.c
  kpm_ric_action_def_frm_5.c
  kpm_ric_event_trigger_def.c
  kpm_ric_ind_hdr.c
  kpm_ric_ind_hdr_frm_1.c
  kpm_ric_ind_msg.c
  kpm_ric_ind_msg_frm_1.c
  kpm_ric_ind_msg_frm_2.c
  kpm_ric_ind_msg_frm_3.c
  )


add_library(kpm_ric_info_obj OBJECT
                            ${KPM_RIC_INFO_SRCS}
                            )

              
