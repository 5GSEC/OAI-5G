#file(GLOB KPM_RIC_INFO_COMMON_SRCS *.c)

set(KPM_RIC_INFO_COMMON_SRCS 
  label_info_lst.c
  test_info_lst.c
  meas_info_frm_1_lst.c
  meas_data_lst.c
  meas_type.c
  matching_cond_frm_3_lst.c
  meas_info_frm_3_lst.c
  meas_info_cond_ue_lst.c
  matching_cond_frm_4_lst.c
  ue_id_gran_period_lst.c
  bin_range_def.c
  meas_bin_range_info_lst.c
  )

add_library(kpm_ric_info_common_obj OBJECT ${KPM_RIC_INFO_COMMON_SRCS})

              
target_compile_options(kpm_ric_info_common_obj PUBLIC "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(kpm_ric_info_common_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
