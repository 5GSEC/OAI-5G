set(KPM_ENC_COMMON_ASN_SRCS 
  enc_label_info.c
  enc_matching_cond_frm_3.c
  enc_matching_cond_frm_4.c
  enc_meas_data.c
  enc_meas_info_cond_ue.c
  enc_meas_info_frm_1.c
  enc_meas_info_frm_3.c
  enc_test_info.c
  enc_meas_bin_range_info.c
  enc_bin_range_def.c
  enc_ue_id_gran_period_lst.c
  )

add_library(kpm_enc_common_asn_obj OBJECT
                                ${KPM_ENC_COMMON_ASN_SRCS}
                                $<TARGET_OBJECTS:e2sm_kpm_asn1_obj>)

target_link_libraries(kpm_enc_common_asn_obj PRIVATE
                                            e2sm_kpm_asn1_obj)

target_compile_options(kpm_enc_common_asn_obj PUBLIC "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(kpm_enc_common_asn_obj PUBLIC "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(kpm_enc_common_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
