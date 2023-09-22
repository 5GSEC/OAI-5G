set(SM_ENCODING_GTP "PLAIN" CACHE STRING "The GTP SM encoding to use")
set_property(CACHE SM_ENCODING_GTP PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected GTP SM_ENCODING: ${SM_ENCODING_GTP}")

if(SM_ENCODING_GTP STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_gtp_sm
                    main.c 
                    ../../rnd/fill_rnd_data_gtp.c
                    ../../../src/util/time_now_us.c
              )
elseif(SM_ENCODING_GTP STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")

elseif(SM_ENCODING_GTP STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")

endif()

target_compile_definitions(test_gtp_sm PUBLIC ${SM_ENCODING_GTP})
target_link_libraries(test_gtp_sm PUBLIC gtp_sm_static -pthread)

enable_testing()
add_test(Unit_test_GTP test_gtp_sm)  
