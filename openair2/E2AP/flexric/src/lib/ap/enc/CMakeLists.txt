if(E2AP_ENCODING STREQUAL "ASN")

  add_library(e2ap_msg_enc_obj OBJECT 
                                e2ap_msg_enc_asn.c
                                $<TARGET_OBJECTS:e2ap_asn1_obj>
                                $<TARGET_OBJECTS:e2ap_types_obj>
                                )

  target_link_libraries(e2ap_msg_enc_obj PRIVATE 
                                      e2ap_asn1_obj 
                                      e2ap_types_obj
                                      )

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS")
  add_library(e2ap_msg_enc_obj OBJECT 
                              e2ap_msg_enc_fb.c
                              $<TARGET_OBJECTS:e2ap_types_obj>
                              )

  target_link_libraries(e2ap_msg_enc_obj PRIVATE
                                 e2ap_fb_obj 
                                 e2ap_types_obj)
else()
  message(FATAL_ERROR "Unknonw E2AP encoding type")
endif()

