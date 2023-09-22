set(SM_ENCODING_RLC "PLAIN" CACHE STRING "The RLC SM encoding to use")
set_property(CACHE SM_ENCODING_RLC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected RLC SM_ENCODING: ${SM_ENCODING_RLC}")


if(SM_ENCODING_RLC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_rlc_sm
                      main.c 
                      ../../rnd/fill_rnd_data_rlc.c
                      ../../../src/util/time_now_us.c
              )
elseif(SM_ENCODING_RLC STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")

elseif(SM_ENCODING_RLC STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented ASN encoding")

endif()

target_compile_definitions(test_rlc_sm PUBLIC ${SM_ENCODING_RLC})
target_link_libraries(test_rlc_sm PUBLIC rlc_sm_static -pthread)

enable_testing()
add_test(Unit_test_RLC test_rlc_sm)
