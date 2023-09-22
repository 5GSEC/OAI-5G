cmake_minimum_required(VERSION 3.15) # setting this is required
project(kpm_sm)            # this sets the project name

add_subdirectory(dec)
add_subdirectory(enc)
add_subdirectory(ie)

# KPM Service Model
set(SM_ENCODING_KPM_SRC
                      ../sm_proc_data.c 
                      kpm_sm_ric.c 
                      kpm_sm_agent.c 
                      ../../util/byte_array.c 
                      ../../util/conversions.c
                      ../../util/alg_ds/alg/defer.c 
                      ../../util/alg_ds/alg/eq_float.c 
                      ../../util/alg_ds/ds/seq_container/seq_arr.c 
                      ../../util/alg_ds/ds/seq_container/seq_ring.c 
                      ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                      ../../util/alg_ds/ds/assoc_container/bimap.c
                      )

if(SM_ENCODING_KPM STREQUAL "PLAIN")
  message(FATAL_ERROR "KPM SM PLAIN not implemented")
elseif(SM_ENCODING_KPM STREQUAL "ASN")
  add_subdirectory(ie/asn)

  set(SM_ENCODING_KPM_SRC_ASN
    ${SM_ENCODING_KPM_SRC}
    ../../lib/ap/e2ap_types/common/e2ap_ran_function.c
    enc/kpm_enc_asn.c 
    dec/kpm_dec_asn.c 
    ie/kpm_data_ie.c
    $<TARGET_OBJECTS:e2sm_kpm_asn1_obj>
    $<TARGET_OBJECTS:kpm_ric_info_common_obj>
    $<TARGET_OBJECTS:kpm_ric_info_obj>
    $<TARGET_OBJECTS:kpm_dec_common_asn_obj>
    $<TARGET_OBJECTS:kpm_dec_ie_asn_obj>
    $<TARGET_OBJECTS:kpm_enc_common_asn_obj>
    $<TARGET_OBJECTS:kpm_enc_ie_asn_obj>
    $<TARGET_OBJECTS:3gpp_derived_ie_obj>
    $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
    $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
    $<TARGET_OBJECTS:sm_common_ie_obj>
    $<TARGET_OBJECTS:sm_common_dec_asn_obj>
    $<TARGET_OBJECTS:sm_common_enc_asn_obj>
    )

  # Shared library
  add_library(kpm_sm SHARED ${SM_ENCODING_KPM_SRC_ASN})

  target_link_libraries(kpm_sm PRIVATE -lm)
  target_include_directories(kpm_sm PRIVATE "ie/asn")
  target_compile_options(kpm_sm PRIVATE "-DASN_DISABLE_OER_SUPPORT" -fPIC)
  target_compile_options(kpm_sm PRIVATE "-DASN_DISABLE_JER_SUPPORT" -fPIC)
  target_compile_options(kpm_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC -fvisibility=hidden)

  # Static Library
  add_library(kpm_sm_static STATIC ${SM_ENCODING_KPM_SRC_ASN})

  target_link_libraries(kpm_sm_static PRIVATE -lm)
  target_include_directories(kpm_sm_static PRIVATE "ie/asn")
  target_compile_options(kpm_sm_static PRIVATE "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(kpm_sm_static PUBLIC "-DASN_DISABLE_JER_SUPPORT")
  target_compile_options(kpm_sm_static PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

elseif(SM_ENCODING_KPM STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "KPM SM FB not implemented")
else()
  message(FATAL_ERROR "Unknown KPM SM encoding type")
endif()

target_compile_definitions(kpm_sm PUBLIC ${SM_ENCODING_KPM})
target_compile_definitions(kpm_sm_static PUBLIC ${SM_ENCODING_KPM})

