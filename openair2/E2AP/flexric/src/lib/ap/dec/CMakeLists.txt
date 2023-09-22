if(E2AP_ENCODING STREQUAL "ASN")
  add_library(e2ap_msg_dec_obj OBJECT 
                                e2ap_msg_dec_asn.c
                                $<TARGET_OBJECTS:e2ap_asn1_obj>
                                $<TARGET_OBJECTS:e2ap_types_obj>
                                $<TARGET_OBJECTS:e2ap_ep_obj>
                                )

  target_include_directories(e2ap_msg_dec_obj
                           PRIVATE
                           "../ie/asn")

  target_compile_options(e2ap_msg_dec_obj PRIVATE "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(e2ap_msg_dec_obj PRIVATE "-DASN_DISABLE_JER_SUPPORT")

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS")
  add_library(e2ap_msg_dec_obj OBJECT 
                                e2ap_msg_dec_fb.c
                                $<TARGET_OBJECTS:e2ap_types_obj>
                                $<TARGET_OBJECTS:e2ap_ep_obj>
                                )

  target_link_libraries(e2ap_msg_dec_obj PRIVATE
                                e2ap_fb_obj 
                                 e2ap_types_obj)
else()
  message(FATAL_ERROR "no E2AP encoding scheme selected")
endif()

add_compile_definitions(PRIVATE ${E2AP_ENCODING})

