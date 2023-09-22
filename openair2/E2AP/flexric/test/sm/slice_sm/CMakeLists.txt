set(SM_ENCODING_SLICE "PLAIN" CACHE STRING "The SLICE SM encoding to use")
set_property(CACHE SM_ENCODING_SLICE PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SLICE SM_ENCODING: ${SM_ENCODING_SLICE}")


if(SM_ENCODING_SLICE STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_executable(test_slice_sm
                      main.c 
                      ../../rnd/fill_rnd_data_slice.c
                      ../../../src/util/time_now_us.c
                      ../../../src/util/alg_ds/alg/defer.c
              )
elseif(SM_ENCODING_SLICE STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")

elseif(SM_ENCODING_SLICE STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented ASN encoding")

endif()

#target_compile_options(test_slice_sm PRIVATE "-fno-omit-frame-pointer;-fsanitize=address;-pg;-g") # works even if -fno-rtti is set to CXX_FLAGS
target_compile_definitions(test_slice_sm PUBLIC ${SM_ENCODING_SLICE})
target_link_libraries(test_slice_sm PUBLIC slice_sm_static -pthread)

enable_testing()
add_test(Unit_test_SLICE test_slice_sm)
