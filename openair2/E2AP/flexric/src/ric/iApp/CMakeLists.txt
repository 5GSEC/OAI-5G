if(BUILDING_LIBRARY STREQUAL "STATIC")
  set(E2_IAPP_BLD_LIB  "STATIC")
elseif(BUILDING_LIBRARY STREQUAL "DYNAMIC")
  set(E2_IAPP_BLD_LIB  "SHARED")
else()
  message(FATAL_ERROR "Unknown type")
endif()


set(E2_IAPP_SRC 
            asio_iapp.c
            e2ap_iapp.c
            e42_iapp.c
            e42_iapp_api.c
            endpoint_iapp.c
            msg_handler_iapp.c
            map_ric_id.c
            map_xapps_sockaddr.c
            xapp_ric_id.c
            $<TARGET_OBJECTS:e2ap_ap_obj>
            $<TARGET_OBJECTS:e2ap_ep_obj>
            $<TARGET_OBJECTS:msg_hand_obj> 
            $<TARGET_OBJECTS:e2ap_ds_obj>
            $<TARGET_OBJECTS:e2ap_alg_obj>
            $<TARGET_OBJECTS:e2_conf_obj>
            $<TARGET_OBJECTS:pending_events_obj>
            $<TARGET_OBJECTS:e2ap_types_obj>
            $<TARGET_OBJECTS:e2ap_msg_enc_obj>
            $<TARGET_OBJECTS:e2ap_msg_dec_obj>
            $<TARGET_OBJECTS:e2ap_msg_free_obj>
            ${IAPP_EMULATOR_SRC}
    )

if(E2AP_ENCODING STREQUAL "ASN")

  add_library(e42_iapp 
              ${E2_IAPP_BLD_LIB} #STATIC or SHARED
              ${E2_IAPP_SRC}
              $<TARGET_OBJECTS:e2ap_ie_obj>
              $<TARGET_OBJECTS:e2ap_asn1_obj>
              )   

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS")
          add_library(e42_iapp 
            ${E2_AGENT_BLD_LIB} #STATIC or SHARED
            ${E2_AGENT_SRC}
           )

          #bundle_static_library(e42_iapp libflatccrt_d.a)
else()
  message(FATAL_ERROR "Error, while compiling the agent" )
endif()

#if(IAPP_EMULATOR)
#  target_compile_definitions(e42_iapp PRIVATE IAPP_EMULATOR)
#endif()



target_compile_definitions(e42_iapp PRIVATE ${E2AP_ENCODING})

target_link_libraries(e42_iapp
                      PUBLIC 
                      -pthread
                      -lsctp
                      -ldl
                      )

