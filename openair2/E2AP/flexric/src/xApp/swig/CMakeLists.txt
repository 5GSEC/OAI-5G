# This is a CMake example for Python
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
CMAKE_POLICY(VERSION 3.8)

PROJECT(XAPP_SDK)

CMAKE_POLICY(SET CMP0078 NEW) 
CMAKE_POLICY(SET CMP0086 NEW) 

FIND_PACKAGE(SWIG 4.0 REQUIRED)
FIND_PACKAGE(Python3 REQUIRED)
#find_package (Python3 COMPONENTS Interpreter Development)

INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

#SET(CMAKE_SWIG_FLAGS "")

#SET(CMAKE BUILD TYPE Debug)

SET_PROPERTY(SOURCE swig_wrapper.i PROPERTY CPLUSPLUS ON)


SET(XAPP_TARGET_LANGUAGE "PYTHON_LANG" CACHE STRING "xApp target language.")
SET_PROPERTY(CACHE XAPP_TARGET_LANGUAGE PROPERTY STRINGS "PYTHON_LANG" "GO_LANG" "JAVA_LANG" "JAVASCRIPT_LANG")
MESSAGE(STATUS "Selected xApp target language: ${XAPP_TARGET_LANGUAGE}")

#PYTHON compilation
if(XAPP_TARGET_LANGUAGE STREQUAL "PYTHON_LANG")
  SET(XAPP_LANG "python")

#GOLANG compilation
elseif(XAPP_TARGET_LANGUAGE STREQUAL "GO_LANG")
  SET(XAPP_LANG "go")

#JAVA compilation
elseif(XAPP_TARGET_LANGUAGE STREQUAL "JAVA_LANG")
  SET(XAPP_LANG "java")

#JAVASCRIPT compilation
elseif(XAPP_TARGET_LANGUAGE STREQUAL "JAVASCRIPT_LANG")
  SET(XAPP_LANG "javascript")

else()
  message(FATAL_ERROR "Unknown XAPP_TARGET_LANGUAGE")
endif()



SWIG_ADD_LIBRARY(xapp_sdk LANGUAGE ${XAPP_LANG} SOURCES 
                                                 swig_wrapper.i 
                                                 swig_wrapper.cpp
                                                 ../../lib/ap/e2ap_types/common/e2ap_plmn.c
                                                 ../../lib/ap/e2ap_types/common/e2ap_ran_function.c 
                                                 ../../lib/ap/e2ap_types/common/e2ap_global_node_id.c
                                                 #../../sm/kpm_sm_v03.00/ie/kpm_data_ie.c
                                                 ../../sm/mac_sm/ie/mac_data_ie.c 
                                                 ../../sm/rlc_sm/ie/rlc_data_ie.c
                                                 ../../sm/pdcp_sm/ie/pdcp_data_ie.c
                                                 ../../sm/slice_sm/ie/slice_data_ie.c
                                                 ../../sm/tc_sm/ie/tc_data_ie.c
                                                 ../../sm/gtp_sm/ie/gtp_data_ie.c
                                                 ../../util/byte_array.c
                                                 ../../util/alg_ds/alg/eq_float.c
                                                 )


#PYTHON compilation
if(XAPP_TARGET_LANGUAGE STREQUAL "PYTHON_LANG")
  ADD_COMPILE_DEFINITIONS(XAPP_LANG_PYTHON)
  SWIG_LINK_LIBRARIES(xapp_sdk ${PYTHON_LIBRARIES})

   add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/_xapp_sdk.so" "${CMAKE_BINARY_DIR}/examples/xApp/python3/_xapp_sdk.so" )

   add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/xapp_sdk.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/xapp_sdk.py" )

   add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
     "${CMAKE_SOURCE_DIR}/examples/xApp/python3/xapp_mac_rlc_pdcp_moni.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/xapp_mac_rlc_pdcp_moni.py" )

   add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
     "${CMAKE_SOURCE_DIR}/examples/xApp/python3/xapp_slice_moni_ctrl.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/xapp_slice_moni_ctrl.py" )

   add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/examples/xApp/python3/watch_slice_stats" "${CMAKE_BINARY_DIR}/examples/xApp/python3/watch_slice_stats" )

   add_custom_command(TARGET xapp_sdk POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/examples/xApp/python3/xapp_gtp_moni.py" "${CMAKE_BINARY_DIR}/examples/xApp/python3/xapp_gtp_moni.py" )
# I do not like this solution at all but it is not my code ...
target_compile_options(xapp_sdk PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter )

#GOLANG compilation
elseif(XAPP_TARGET_LANGUAGE STREQUAL "GO_LANG")
  ADD_COMPILE_DEFINITIONS(XAPP_LANG_GO)
  SET_PROPERTY(TARGET xapp_sdk PROPERTY SWIG_COMPILE_OPTIONS -intgosize 64)

#JAVA compilation
elseif(XAPP_TARGET_LANGUAGE STREQUAL "JAVA_LANG")
 ADD_COMPILE_DEFINITIONS(XAPP_LANG_JAVA)
 INCLUDE_DIRECTORIES(/usr/lib/jvm/default-java/include)
 INCLUDE_DIRECTORIES(/usr/lib/jvm/default-java/include/linux)

#JAVASCRIPT compilation
elseif(XAPP_TARGET_LANGUAGE STREQUAL "JAVASCRIPT_LANG")
  ADD_COMPILE_DEFINITIONS(XAPP_LANG_JAVASCRIPT)
  SET_PROPERTY(TARGET xapp_sdk PROPERTY SWIG_COMPILE_OPTIONS -v8)
  INCLUDE_DIRECTORIES(/usr/include/nodejs/src)
  INCLUDE_DIRECTORIES(/usr/include/nodejs/deps/v8/include/)

else()
  message(FATAL_ERROR "Unknown XAPP_TARGET_LANGUAGE")
endif()

#Comment this line to build it from the directory
ADD_DEPENDENCIES(xapp_sdk e42_xapp_shared)

SWIG_LINK_LIBRARIES(xapp_sdk
                    e42_xapp_shared
                    -ldl
                    -lsctp
                    -lpthread)



