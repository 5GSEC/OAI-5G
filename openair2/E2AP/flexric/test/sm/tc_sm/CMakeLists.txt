set(SM_ENCODING_TC "PLAIN" CACHE STRING "The TC SM encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected TC SM_ENCODING: ${SM_ENCODING_TC}")


if(SM_ENCODING_TC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_tc_sm
                      main.c 
                      ../../rnd/fill_rnd_data_tc.c
                      ../../../src/util/time_now_us.c
              )
elseif(SM_ENCODING_TC STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")

elseif(SM_ENCODING_TC STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")

endif()

target_compile_definitions(test_tc_sm PUBLIC ${SM_ENCODING_TC})
target_link_libraries(test_tc_sm PUBLIC tc_sm_static -pthread)
enable_testing()
add_test(Unit_test_TC test_tc_sm)
