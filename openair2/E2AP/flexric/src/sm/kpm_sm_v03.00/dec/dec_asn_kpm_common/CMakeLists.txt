set(KPM_DEC_COMMON_ASN_SRCS 
  dec_label_info.c
  dec_matching_cond_frm_3.c
  dec_matching_cond_frm_4.c
  dec_meas_data.c
  dec_meas_info_cond_ue.c
  dec_meas_info_frm_1.c
  dec_meas_info_frm_3.c
  dec_test_info.c
  dec_meas_bin_range_info.c
  dec_bin_range_def.c
  )


add_library(kpm_dec_common_asn_obj OBJECT
                                ${KPM_DEC_COMMON_ASN_SRCS}
                                $<TARGET_OBJECTS:e2sm_kpm_asn1_obj>)

target_link_libraries(kpm_dec_common_asn_obj PRIVATE
                                e2sm_kpm_asn1_obj)
              
target_compile_options(kpm_dec_common_asn_obj PUBLIC "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(kpm_dec_common_asn_obj PUBLIC "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(kpm_dec_common_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
