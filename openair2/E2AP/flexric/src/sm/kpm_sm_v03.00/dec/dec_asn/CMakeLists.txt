set(KPM_DEC_IE_ASN_SRCS 
  dec_ric_action_def_frm_1.c
  dec_ric_action_def_frm_2.c
  dec_ric_action_def_frm_3.c
  dec_ric_action_def_frm_4.c
  dec_ric_action_def_frm_5.c
  dec_ric_event_trigger_frm_1.c
  dec_ric_ind_hdr_frm_1.c
  dec_ric_ind_msg_frm_1.c
  dec_ric_ind_msg_frm_2.c
  dec_ric_ind_msg_frm_3.c
  )

add_library(kpm_dec_ie_asn_obj OBJECT
                            ${KPM_DEC_IE_ASN_SRCS}
                            $<TARGET_OBJECTS:e2sm_kpm_asn1_obj>)

target_link_libraries(kpm_dec_ie_asn_obj PRIVATE
                            e2sm_kpm_asn1_obj)
              
target_compile_options(kpm_dec_ie_asn_obj PUBLIC "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(kpm_dec_ie_asn_obj PUBLIC "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(kpm_dec_ie_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
