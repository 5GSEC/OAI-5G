

if(FALSE) # fake a block comment


cmake_minimum_required(VERSION 3.15)

project (TC_SM)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_C_FLAGS "-Wall -Wextra -std=c11 -g -fno-omit-frame-pointer") 

set(SM_ENCODING_TC "PLAIN" CACHE STRING "The E2AP encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SM_ENCODING_TC: ${SM_ENCODING_TC}")


include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/fb/ )
include_directories("/usr/local/share/asn1c/")


file(GLOB asn_sources "../ie/asn/*.c")

add_executable(test_tc_sm main.c 
                      ../../sm_proc_data.c 
                      ../tc_sm_agent.c 
                      ../tc_sm_ric.c 
                      ../enc/tc_enc_plain.c 
                      ../enc/tc_enc_fb.c 
                      ../enc/tc_enc_asn.c 
                      ../dec/tc_dec_plain.c 
                      ../dec/tc_dec_fb.c 
                      ../dec/tc_dec_asn.c 
                      ../ie/tc_data_ie.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../../agent_if/read/sm_ag_if_rd.c
                      ${asn_sources} 
                      # This sucks...
                      ../../mac_sm/ie/mac_data_ie.c
                      ../../rlc_sm/ie/rlc_data_ie.c
                      ../../pdcp_sm/ie/pdcp_data_ie.c
                      ../../slice_sm/ie/slice_data_ie.c
                      ../../tc_sm/ie/tc_data_ie.c
                      )

target_compile_definitions(test_tc_sm PUBLIC ${SM_ENCODING_TC})
target_compile_options(test_tc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
target_link_libraries(test_tc_sm PUBLIC -pthread)

endif()




cmake_minimum_required(VERSION 3.15)

project (TEST_TC_SM)

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

  message(STATUS "Code Profiler ON. Example usage: gprof test_tc_sm gmon.out > prof.txt && vim prof.txt")
endif()

option(CODE_COVERAGE "Code coverage" ON)
if(CODE_COVERAGE)
  add_compile_options("$<$<CONFIG:DEBUG>:-fprofile-arcs;-ftest-coverage>")
  add_link_options("$<$<CONFIG:DEBUG>:-lgcov;-coverage;>")

  message(STATUS "Code Coverage ON. Example usage: lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory out && cd out && firefox index.html")
endif()

set(SM_ENCODING_TC "PLAIN" CACHE STRING "The TC SM encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected TC SM_ENCODING: ${SM_ENCODING_TC}")


if(SM_ENCODING_TC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_tc_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../tc_sm_agent.c 
                      ../tc_sm_ric.c 
                      ../enc/tc_enc_plain.c 
                      ../dec/tc_dec_plain.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../ie/tc_data_ie.c
                      ../../../../test/common/fill_ind_data.c
              )
elseif(SM_ENCODING_TC STREQUAL "ASN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
  file(GLOB asn_sources "../ie/asn/*.c")
  #include_directories("/usr/local/share/asn1c/")
  target_compile_options(test_tc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  add_executable(test_tc_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../tc_sm_agent.c 
                      ../tc_sm_server.c 
                      ../enc/tc_enc_asn.c 
                      ../dec/tc_dec_asn.c 
                      ../../util/alg_ds/alg/defer.c
                      ../../util/alg_ds/alg/eq_float.c
                      ../ie/tc_data_ie.c
                      ../../if_sm/read/sm_rd_if.c
                      ${asn_sources} 
                      ../../../../test/common/fill_ind_data.c
              )
elseif(SM_ENCODING_TC STREQUAL "FLATBUFFERS")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/fb/ )
  add_executable(test_tc_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../tc_sm_agent.c 
                      ../tc_sm_server.c 
                      ../enc/tc_enc_fb.c 
                      ../dec/tc_dec_fb.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../ie/tc_data_ie.c
                      ../../if_sm/read/sm_rd_if.c
                      ../../../../test/common/fill_ind_data.c
              )
endif()

target_compile_definitions(test_tc_sm PUBLIC ${SM_ENCODING_TC})
target_link_libraries(test_tc_sm PUBLIC -pthread)


if(FALSE)

cmake_minimum_required(VERSION 3.15)

project (TC_SM)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_C_FLAGS "-Wall -Wextra --std=gnu11 -g -fsanitize=address -fno-omit-frame-pointer") 


set(SM_ENCODING_TC "PLAIN" CACHE STRING "The E2AP encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SM_ENCODING_TC: ${SM_ENCODING_TC}")


include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/fb/ )
include_directories("/usr/local/share/asn1c/")


file(GLOB asn_sources "../ie/asn/*.c")

add_executable(test_tc_sm main.c 
                      ../../sm_proc_data.c 
                      ../tc_sm_agent.c 
                      ../tc_sm_ric.c 
                      ../enc/tc_enc_plain.c 
                      ../enc/tc_enc_fb.c 
                      ../enc/tc_enc_asn.c 
                      ../dec/tc_dec_plain.c 
                      ../dec/tc_dec_fb.c 
                      ../dec/tc_dec_asn.c 
                      ../ie/tc_data_ie.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../agent_if/read/sm_ag_if_rd.c
                      # Horrible design. Fix It.
                      ../../mac_sm/ie/mac_data_ie.c
                      ../../rlc_sm/ie/rlc_data_ie.c
                      ../../pdcp_sm/ie/pdcp_data_ie.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ${asn_sources} 
                      )

target_compile_definitions(test_tc_sm PUBLIC ${SM_ENCODING_TC})
target_compile_options(test_tc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
target_link_libraries(test_tc_sm PUBLIC -pthread)

endif()



