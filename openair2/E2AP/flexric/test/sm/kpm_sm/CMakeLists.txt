set(SM_ENCODING_KPM "ASN" CACHE STRING "The KPM SM encoding to use")
set_property(CACHE SM_ENCODING_KPM PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected KPM SM_ENCODING: ${SM_ENCODING_KPM}")

if(SM_ENCODING_KPM  STREQUAL "PLAIN")
  message(FATAL_ERROR "Not implemented PLAIN encoding")

elseif(SM_ENCODING_KPM STREQUAL "ASN")
  add_executable(test_kpm_sm
                      main.c 
                      ../../rnd/fill_rnd_data_kpm.c
                      ../../../src/util/time_now_us.c
                      ../../../src/util/alg_ds/alg/defer.c
                      )

elseif(SM_ENCODING_KPM STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")
endif()

target_compile_definitions(test_kpm_sm PUBLIC ${SM_ENCODING_KPM})
target_link_libraries(test_kpm_sm PUBLIC kpm_sm_static -pthread -lm)

enable_testing()
add_test(Unit_test_KPM test_kpm_sm)
