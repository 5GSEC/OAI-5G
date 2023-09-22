cmake_minimum_required(VERSION 3.15) # setting this is required
project(tc_sm)            # this sets the project name


set(SM_ENCODING_TC "PLAIN" CACHE STRING "The E2AP encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SM_ENCODING: ${SM_ENCODING_TC}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../util)

set(SM_ENCODING_TC_SRC 
    # This dependency sucks!
    ../../lib/ap/e2ap_types/common/e2ap_ran_function.c
    ../sm_proc_data.c 
    tc_sm_agent.c 
    tc_sm_ric.c 
    ../../util/byte_array.c 
    ../../util/alg_ds/alg/defer.c 
    ../../util/alg_ds/alg/eq_float.c 
    ../../util/alg_ds/ds/seq_container/seq_arr.c 
    ../../util/alg_ds/ds/seq_container/seq_ring.c 
    ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
    ../../util/alg_ds/ds/assoc_container/bimap.c
    ie/tc_data_ie.c 
    ) 

if(SM_ENCODING_TC STREQUAL "PLAIN")
  set(SM_ENCODING_TC_SRC_PLAIN 
   ${SM_ENCODING_TC_SRC}  
    enc/tc_enc_plain.c 
    dec/tc_dec_plain.c  
    )
  # Shared
  add_library(tc_sm SHARED ${SM_ENCODING_TC_SRC_PLAIN}) 
  target_compile_options(tc_sm PRIVATE -fvisibility=hidden)

  # Static
  add_library(tc_sm_static STATIC ${SM_ENCODING_TC_SRC_PLAIN}) 
elseif(SM_ENCODING_TC STREQUAL "ASN" )
  message(FATAL_ERROR "TC SM ASN not implemented")
elseif(SM_ENCODING_TC STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "TC SM FB not implemented")
endif()

target_compile_definitions(tc_sm PUBLIC ${SM_ENCODING_TC})
target_compile_definitions(tc_sm_static PUBLIC ${SM_ENCODING_TC})


