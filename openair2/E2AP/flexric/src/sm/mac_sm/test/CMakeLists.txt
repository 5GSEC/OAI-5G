cmake_minimum_required(VERSION 3.15)

project (TEST_MAC_SM)

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

  message(STATUS "Code Profiler ON. Example usage: gprof test_mac_sm gmon.out > prof.txt && vim prof.txt")
endif()

option(CODE_COVERAGE "Code coverage" ON)
if(CODE_COVERAGE)
  add_compile_options("$<$<CONFIG:DEBUG>:-fprofile-arcs;-ftest-coverage>")
  add_link_options("$<$<CONFIG:DEBUG>:-lgcov;-coverage;>")

  message(STATUS "Code Coverage ON. Example usage: lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory out && cd out && firefox index.html")
endif()

set(SM_ENCODING_MAC "PLAIN" CACHE STRING "The MAC SM encoding to use")
set_property(CACHE SM_ENCODING_MAC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected MAC SM_ENCODING: ${SM_ENCODING_MAC}")


if(SM_ENCODING_MAC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_mac_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../mac_sm_agent.c 
                      ../mac_sm_ric.c 
                      ../enc/mac_enc_plain.c 
                      ../dec/mac_dec_plain.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../ie/mac_data_ie.c
                      ../../../../test/common/fill_ind_data.c
              )
elseif(SM_ENCODING_MAC STREQUAL "ASN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
  file(GLOB asn_sources "../ie/asn/*.c")
  #include_directories("/usr/local/share/asn1c/")
  target_compile_options(test_mac_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  add_executable(test_mac_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../mac_sm_agent.c 
                      ../mac_sm_server.c 
                      ../enc/mac_enc_asn.c 
                      ../dec/mac_dec_asn.c 
                      ../../util/alg_ds/alg/defer.c
                      ../../util/alg_ds/alg/eq_float.c
                      ../ie/mac_data_ie.c
                      ../../if_sm/read/sm_rd_if.c
                      ../../../../test/common/fill_ind_data.c
                      ${asn_sources} 
              )
elseif(SM_ENCODING_MAC STREQUAL "FLATBUFFERS")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/fb/ )
  add_executable(test_mac_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../mac_sm_agent.c 
                      ../mac_sm_server.c 
                      ../enc/mac_enc_fb.c 
                      ../dec/mac_dec_fb.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../ie/mac_data_ie.c
                      ../../if_sm/read/sm_rd_if.c
                      ../../../../test/common/fill_ind_data.c
              )
endif()

target_compile_definitions(test_mac_sm PUBLIC ${SM_ENCODING_MAC})
target_link_libraries(test_mac_sm PUBLIC -pthread)

