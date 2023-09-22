cmake_minimum_required(VERSION 3.15) # setting this is required
project(rc_sm)            # this sets the project name

set(SM_ENCODING_RC_SRC
  ../sm_proc_data.c 
  rc_sm_agent.c 
  rc_sm_ric.c 
  ../../util/byte_array.c 
  ../../util/conversions.c
  ../../util/alg_ds/alg/defer.c 
  ../../util/alg_ds/alg/eq_float.c 
  ../../util/alg_ds/ds/seq_container/seq_arr.c 
  ../../util/alg_ds/ds/seq_container/seq_ring.c 
  ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
  ../../util/alg_ds/ds/assoc_container/bimap.c
  ie/rc_data_ie.c 
  )


if(SM_ENCODING_RC STREQUAL "PLAIN")
  message(FATAL_ERROR "RC SM FB not implemented")
elseif(SM_ENCODING_RC STREQUAL "ASN" )
  add_subdirectory(ie/asn)
  add_subdirectory(ie/ir)

  set(SM_ENCODING_RC_SRC_ASN
    # This dependency sucks!
    ../../lib/ap/e2ap_types/common/e2ap_ran_function.c
    ${SM_ENCODING_RC_SRC}
    enc/rc_enc_asn.c 
    dec/rc_dec_asn.c 
    $<TARGET_OBJECTS:e2sm_rc_asn1_obj>
    $<TARGET_OBJECTS:e2sm_rc_ir_obj>
    $<TARGET_OBJECTS:3gpp_derived_ie_obj>
    $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
    $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
    $<TARGET_OBJECTS:sm_common_ie_obj>
    $<TARGET_OBJECTS:sm_common_dec_asn_obj>
    $<TARGET_OBJECTS:sm_common_enc_asn_obj>
    )


  # Shared 
  add_library(rc_sm SHARED ${SM_ENCODING_RC_SRC_ASN})

  target_include_directories(rc_sm PRIVATE "ie/asn")
  target_compile_options(rc_sm  PRIVATE "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(rc_sm  PRIVATE "-DASN_DISABLE_JER_SUPPORT")
  target_compile_options(rc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fvisibility=hidden )
  target_link_libraries(rc_sm PRIVATE -lm)  


  # Static
  add_library(rc_sm_static STATIC ${SM_ENCODING_RC_SRC_ASN})
  target_include_directories(rc_sm_static PRIVATE "ie/asn")
  target_compile_options(rc_sm_static PRIVATE "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(rc_sm_static PRIVATE "-DASN_DISABLE_JER_SUPPORT")
  target_compile_options(rc_sm_static PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fvisibility=hidden )
  target_link_libraries(rc_sm_static PRIVATE -lm)  

elseif(SM_ENCODING_RC STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "RC SM FB not implemented")
  add_library(rc_sm SHARED
                     ${SM_ENCODING_RC_SRC}
                      enc/rc_enc_fb.c 
                      dec/rc_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknown SM encoding type ")
endif()

target_compile_definitions(rc_sm PUBLIC ${SM_ENCODING_RC})
target_compile_definitions(rc_sm_static PUBLIC ${SM_ENCODING_RC})

