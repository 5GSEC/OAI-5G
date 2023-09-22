add_subdirectory(e2ap_types)
add_subdirectory(ie)
add_subdirectory(enc)
add_subdirectory(dec)
add_subdirectory(free)


if(E2AP_ENCODING STREQUAL "ASN")
    add_library(e2ap_ap_obj OBJECT 
                          e2ap_ap_asn.c
                          $<TARGET_OBJECTS:e2ap_ie_obj>
                          $<TARGET_OBJECTS:e2ap_types_obj>
                          $<TARGET_OBJECTS:e2ap_msg_enc_obj>
                          $<TARGET_OBJECTS:e2ap_msg_dec_obj>
                          $<TARGET_OBJECTS:e2ap_msg_free_obj>
                          )

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS")
  add_library(e2ap_ap_obj OBJECT 
                          e2ap_ap_fb.c
                          $<TARGET_OBJECTS:e2ap_types_obj>
                          $<TARGET_OBJECTS:e2ap_msg_enc_obj>
                          $<TARGET_OBJECTS:e2ap_msg_dec_obj>
                          $<TARGET_OBJECTS:e2ap_msg_free_obj>
                          )

else()
  message(FATAL_ERROR "Unknonw E2AP encoding type")
endif()

# Set the MACRO variable that will be used by the compiler to select the encoding/decoding
target_compile_definitions(e2ap_ap_obj PRIVATE ${E2AP_ENCODING})

