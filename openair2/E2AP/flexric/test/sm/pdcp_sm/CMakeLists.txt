set(SM_ENCODING_PDCP "PLAIN" CACHE STRING "The PDCP SM encoding to use")
set_property(CACHE SM_ENCODING_PDCP PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected PDCP SM_ENCODING: ${SM_ENCODING_PDCP}")


if(SM_ENCODING_PDCP  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_pdcp_sm
                      main.c 
                    ../../rnd/fill_rnd_data_pdcp.c
                    ../../../src/util/time_now_us.c
              )
elseif(SM_ENCODING_PDCP STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")
elseif(SM_ENCODING_PDCP STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented ASN encoding")
endif()

target_compile_definitions(test_pdcp_sm PUBLIC ${SM_ENCODING_PDCP})
target_link_libraries(test_pdcp_sm PUBLIC pdcp_sm_static -pthread)

enable_testing()
add_test(Unit_test_PDCP test_pdcp_sm)
