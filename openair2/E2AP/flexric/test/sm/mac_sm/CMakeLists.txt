set(SM_ENCODING_MAC "PLAIN" CACHE STRING "The MAC SM encoding to use")
set_property(CACHE SM_ENCODING_MAC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected MAC SM_ENCODING: ${SM_ENCODING_MAC}")

if(SM_ENCODING_MAC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_mac_sm
                      main.c 
                      ../../rnd/fill_rnd_data_mac.c
                      ../../../src/util/time_now_us.c
                      )              
elseif(SM_ENCODING_MAC STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")
elseif(SM_ENCODING_MAC STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")
endif()

target_compile_definitions(test_mac_sm PUBLIC ${SM_ENCODING_MAC})
target_link_libraries(test_mac_sm PUBLIC mac_sm_static -pthread)

enable_testing()
add_test(Unit_test_MAC test_mac_sm)
