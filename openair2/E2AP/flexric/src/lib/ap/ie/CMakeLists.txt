if(E2AP_ENCODING STREQUAL "ASN")
  add_subdirectory(asn)
  add_library(e2ap_ie_obj OBJECT 
                          $<TARGET_OBJECTS:e2ap_asn1_obj> )
elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS")
  add_subdirectory(fb)
  add_library(e2ap_ie_obj INTERFACE)
  target_link_libraries(e2ap_ie_obj INTERFACE  
                        e2ap_fb_obj)
else()
  message("Unforeseen E2AP_ENCODING type")
endif()



