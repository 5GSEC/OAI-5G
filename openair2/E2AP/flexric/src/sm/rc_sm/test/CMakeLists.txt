cmake_minimum_required(VERSION 3.15)

project (TEST_RC_SM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra") 

set(default_build_type "Debug")

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(COMPILER "GCC" CACHE STRING "Compiler")
set_property(CACHE COMPILER PROPERTY STRINGS "GCC" "CLANG")

if(COMPILER STREQUAL CLANG)
  add_compile_options("-fblocks;")
  add_link_options("-lBlocksRuntime;")
endif()

set(SANITIZER "ADDRESS" CACHE STRING "Sanitizers")
set_property(CACHE SANITIZER PROPERTY STRINGS "NONE" "ADDRESS" "THREAD")
message(STATUS "Selected SANITIZER TYPE: ${SANITIZER}")

if(SANITIZER STREQUAL "ADDRESS")
  add_compile_options("$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>")
  add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
elseif(SANITIZER STREQUAL "THREAD" )
  add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=thread;-g;>")
  add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=thread;>")
endif()

option(CODE_PROFILER "Code Profiler" ON)
if(CODE_PROFILER)
  add_compile_options("$<$<CONFIG:DEBUG>:-pg>")
  add_link_options("$<$<CONFIG:DEBUG>:-pg;>")

  message(STATUS "Code Profiler ON. Example usage: gprof test_rc_sm gmon.out > prof.txt && vim prof.txt")
endif()

option(CODE_COVERAGE "Code coverage" ON)
if(CODE_COVERAGE)
  add_compile_options("$<$<CONFIG:DEBUG>:-fprofile-arcs;-ftest-coverage>")
  add_link_options("$<$<CONFIG:DEBUG>:-lgcov;-coverage;>")

  message(STATUS "Code Coverage ON. Example usage: lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory out && cd out && firefox index.html")
endif()

set(SM_ENCODING_RC "ASN" CACHE STRING "The RC SM encoding to use")
set_property(CACHE SM_ENCODING_RC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected RC SM_ENCODING: ${SM_ENCODING_RC}")


if(SM_ENCODING_RC STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_rc_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../rc_sm_agent.c 
                      ../rc_sm_ric.c 
                      ../enc/rc_enc_plain.c 
                      ../dec/rc_dec_plain.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../ie/rc_data_ie.c
                      ../../../../test/common/fill_ind_data.c
              )
elseif(SM_ENCODING_RC STREQUAL "ASN")

  file(GLOB asn_sources "../ie/asn/*.c")

  add_executable(test_rc_sm
    main.c 
    ../../sm_proc_data.c 
    ../rc_sm_agent.c 
    ../rc_sm_ric.c 
    ../enc/rc_enc_asn.c 
    ../dec/rc_dec_asn.c 
    ../../../util/alg_ds/alg/defer.c
    ../../../util/byte_array.c

    ../../../util/alg_ds/alg/eq_float.c


    ../ie/rc_data_ie.c
    ../ie/ir/msg_ev_trg.c  
    ../ie/ir/network_interface_e2rc.c
    ../ie/ir/e2_node_info_chng.c

    ../ie/ir/e2sm_rc_ev_trg_frmt_1.c
    ../ie/ir/e2sm_rc_ev_trg_frmt_2.c
    ../ie/ir/e2sm_rc_ev_trg_frmt_3.c
    ../ie/ir/e2sm_rc_ev_trg_frmt_4.c
    ../ie/ir/e2sm_rc_ev_trg_frmt_5.c

    ../ie/ir/ue_info_chng.c 

    ../ie/ir/ran_param_req.c 
    ../ie/ir/ran_param_list.c 
    ../ie/ir/ran_param_struct.c
    ../ie/ir/ran_param_elm_key_true.c
    ../ie/ir/ran_param_elm_key_false.c
    ../ie/ir/ran_param_cond_crit_def.c
    ../ie/ir/ran_param_insertion.c

    ../ie/ir/policy_cond.c 
    ../ie/ir/policy_action.c 

    ../ie/ir/lst_ran_param.c

    ../ie/ir/rrc_state.c 
    ../ie/ir/rrc_state_lst.c 

    ../ie/ir/ran_func_def_insert.c 

    ../ie/ir/ran_param_list.c 
    ../ie/ir/ran_param_ins_ind.c 

    ../ie/ir/ran_param_struct.c 
    ../ie/ir/ran_param_test.c

    ../ie/ir/ran_param_test_struct.c

    ../ie/ir/ran_param_cond_crit_def.c
    ../ie/ir/ran_parameter_test_condition.c
    ../ie/ir/ran_parameter_value.c
    ../ie/ir/ran_parameter_value_type.c

    ../ie/ir/seq_ran_param.c 

    ../ie/ir/e2sm_rc_act_def_frmt_1.c
    ../ie/ir/e2sm_rc_act_def_frmt_2.c
    ../ie/ir/e2sm_rc_act_def_frmt_3.c
    ../ie/ir/e2sm_rc_act_def_frmt_4.c


    ../ie/ir/e2sm_rc_ind_hdr_frmt_1.c
    ../ie/ir/e2sm_rc_ind_hdr_frmt_2.c
    ../ie/ir/e2sm_rc_ind_hdr_frmt_3.c


    ../ie/ir/e2sm_rc_ind_msg_frmt_1.c
    ../ie/ir/e2sm_rc_ind_msg_frmt_2.c
    ../ie/ir/e2sm_rc_ind_msg_frmt_3.c
    ../ie/ir/e2sm_rc_ind_msg_frmt_4.c
    ../ie/ir/e2sm_rc_ind_msg_frmt_5.c
    ../ie/ir/e2sm_rc_ind_msg_frmt_6.c



    ../ie/ir/e2sm_rc_ctrl_hdr_frmt_1.c
    ../ie/ir/e2sm_rc_ctrl_hdr_frmt_2.c
    ../ie/ir/e2sm_rc_ctrl_msg_frmt_1.c
    ../ie/ir/e2sm_rc_ctrl_msg_frmt_2.c


    ../ie/ir/e2sm_rc_ctrl_out_frmt_1.c
    ../ie/ir/e2sm_rc_ctrl_out_frmt_2.c
    ../ie/ir/e2sm_rc_ctrl_out_frmt_3.c

    ../ie/ir/seq_ran_param_2.c
    ../ie/ir/seq_ctrl_act_out.c 
    ../ie/ir/seq_ctrl_sty_mul_out.c  

    ../ie/ir/seq_policy_sty.c  

    ../ie/ir/param_report_def.c
    ../ie/ir/ran_param_test_list.c


    ../ie/ir/seq_ctrl_sma.c
    ../ie/ir/seq_ctrl_act.c


    ../ie/ir/seq_cell_info.c
    ../ie/ir/seq_cell_info_2.c
    ../ie/ir/seq_ue_id.c
    ../ie/ir/seq_ue_info.c
    ../ie/ir/seq_ins_ind_act_def.c
    ../ie/ir/seq_ins_style_2.c
    ../ie/ir/seq_ins_ind_act_2.c
    
    ../ie/ir/ran_func_def_ctrl.c

    ../ie/ir/ran_func_def_ev_trig.c 
    ../ie/ir/seq_ran_param_3.c 
    ../ie/ir/seq_call_proc_type.c 
    ../ie/ir/seq_ev_trg_style.c 
    ../ie/ir/call_proc_break.c 

    ../ie/ir/seq_ins_ind.c 

    ../ie/ir/seq_pol_action.c 

    ../ie/ir/ran_func_def_report.c 
    ../ie/ir/seq_report_sty.c 
    ../ie/ir/seq_ins_sty.c 


    ../ie/ir/ran_func_def_policy.c 

    ../ie/ir/seq_ctrl_style.c 

    ../ie/ir/seq_ctrl_act_2.c 

    ../../kpm_sm_v03.00/ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_hdr.c

    ../../../lib/sm/sm_common_ie/ran_function_name.c


    ../ie/ir/seq_ins_style_multiple_action.c 

    ../../../lib/sm/3gpp_derived_ie/rrc_msg_id.c
    ../../../lib/sm/enc_asn_sm_common/enc_ue_id.c
    ../../../lib/sm/enc_asn_sm_common/enc_cell_global_id.c

    ../../../lib/sm/sm_common_ie/cell_global_id.c

    ../../../lib/sm/dec_asn_sm_common/dec_ue_id.c
    ../../../lib/sm/dec_asn_sm_common/dec_cell_global_id.c


    ../../../lib/sm/sm_common_ie/ue_id.c

    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_enb.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_en_gnb.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_global_enb_id.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_global_gnb_id.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_global_ng_enb.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_global_ng_ran.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_gnb.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_gnb_cu_up.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_gnb_du.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_ng_enb.c
    ../../../lib/sm/3gpp_derived_ie_dec_asn/dec_ng_enb_du.c



    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_enb.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_en_gnb.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_global_enb_id.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_global_gnb_id.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_global_ng_enb.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_global_ng_ran.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_gnb.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_gnb_cu_up.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_gnb_du.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_ng_enb.c
    ../../../lib/sm/3gpp_derived_ie_enc_asn/enc_ng_enb_du.c




    ../../../lib/sm/3gpp_derived_ie/enb.c
    ../../../lib/sm/3gpp_derived_ie/en_gnb.c
    ../../../lib/sm/3gpp_derived_ie/eutra_cgi.c
    ../../../lib/sm/3gpp_derived_ie/global_enb_id.c
    ../../../lib/sm/3gpp_derived_ie/global_enb_id.h
    ../../../lib/sm/3gpp_derived_ie/global_gnb_id.c
    ../../../lib/sm/3gpp_derived_ie/global_ng_enb_id.c
    ../../../lib/sm/3gpp_derived_ie/global_ng_ran_node_id.c
    ../../../lib/sm/3gpp_derived_ie/gnb.c
    ../../../lib/sm/3gpp_derived_ie/gnb_cu_up.c
    ../../../lib/sm/3gpp_derived_ie/gnb_du.c
    ../../../lib/sm/3gpp_derived_ie/guami.c
    ../../../lib/sm/3gpp_derived_ie/e2sm_gummei.c
    ../../../lib/sm/3gpp_derived_ie/ng_enb.c
    ../../../lib/sm/3gpp_derived_ie/ng_enb_du.c
    ../../../lib/sm/3gpp_derived_ie/nr_cgi.c
    ../../../lib/sm/3gpp_derived_ie/nr_cgi.h
    ../../../lib/sm/3gpp_derived_ie/plmn_identity.c
    ../../../lib/sm/3gpp_derived_ie/rrc_msg_id.c





    ../../../util/conversions.c


    ../../agent_if/read/sm_ag_if_rd.c
    fill_rnd_data_rc.c

    ../../mac_sm/ie/mac_data_ie.c
    ../../rlc_sm/ie/rlc_data_ie.c
    ../../pdcp_sm/ie/pdcp_data_ie.c
    ../../slice_sm/ie/slice_data_ie.c
    ../../gtp_sm/ie/gtp_data_ie.c
    ../../kpm_sm_v03.00/ie/kpm_data_ie.c
    ../../tc_sm/ie/tc_data_ie.c


    ${asn_sources} 
    )

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
  include_directories("/usr/local/share/asn1c/")


  target_compile_options(test_rc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")

elseif(SM_ENCODING_RC STREQUAL "FLATBUFFERS")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/fb/ )
  add_executable(test_rc_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../rc_sm_agent.c 
                      ../rc_sm_server.c 
                      ../enc/rc_enc_fb.c 
                      ../dec/rc_dec_fb.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../ie/rc_data_ie.c
                      ../../if_sm/read/sm_rd_if.c
                      ../../../../test/common/fill_ind_data.c
              )
endif()

target_compile_definitions(test_rc_sm PUBLIC ${SM_ENCODING_RC})
target_link_libraries(test_rc_sm PUBLIC -pthread -lm)

