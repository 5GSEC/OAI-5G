cmake_minimum_required(VERSION 3.15) # setting this is required
project(nanomsg_iapp)            # this sets the project name

set(CMAKE_C_STANDARD 11)

set(default_build_type "Debug")

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(SANITIZER "NONE" CACHE STRING "Sanitizers")
set_property(CACHE SANITIZER PROPERTY STRINGS "NONE" "ADDRESS" "THREAD")
message(STATUS "Selected SANITIZER TYPE: ${SANITIZER}")

if(SANITIZER STREQUAL "ADDRESS")

add_compile_options("$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>")
add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")

elseif(SANITIZER STREQUAL  "THREAD" )

add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=thread;-g;>")
add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=thread;>")

endif()

option(CODE_COVERAGE "Code coverage" OFF)
if(CODE_COVERAGE)
add_compile_options("$<$<CONFIG:DEBUG>:-fprofile-arcs;-ftest-coverage>")
add_link_options("$<$<CONFIG:DEBUG>:-lgcov;-coverage;>")
endif()

option(CODE_PROFILER "Code Profiler" OFF)
if( CODE_PROFILER )
add_compile_options("$<$<CONFIG:DEBUG>:-pg>")
endif()

add_executable(nanomsg_iapp 
                          main.c 
                          keep_alive/keep_alive_timer.c
                          ../alg_ds/ds/seq_container/seq_arr.c 
                          ../alg_ds/registry.c 
                          ../alg_ds/alg/string/search_naive.c
                          ../alg_ds/alg/string/booyer_moore.c
                          req_reply.c
                          pub_sub.c
                          )

target_link_libraries(nanomsg_iapp 
                      PUBLIC 
                      -lnng
                      -pthread
                      )

