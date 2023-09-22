set(SM_ENCODING_RC "ASN" CACHE STRING "The RC SM encoding to use")
set_property(CACHE SM_ENCODING_RC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected RC SM_ENCODING: ${SM_ENCODING_RC}")


if(SM_ENCODING_RC  STREQUAL "PLAIN")
  message(FATAL_ERROR "Not implemented PLAIN encoding")

elseif(SM_ENCODING_RC STREQUAL "ASN")
  add_executable(test_rc_sm
                      main.c 
                      ../../rnd/fill_rnd_data_rc.c
                      ../../../src/util/time_now_us.c
                      ../../../src/util/alg_ds/alg/defer.c
                      )

elseif(SM_ENCODING_RC STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")

endif()

target_compile_definitions(test_rc_sm PUBLIC ${SM_ENCODING_RC})
target_link_libraries(test_rc_sm PUBLIC rc_sm_static -pthread -lm)


enable_testing()
add_test(Unit_test_RC test_rc_sm)
