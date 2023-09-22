if(BUILDING_LIBRARY STREQUAL "STATIC")
  set(E2_AGENT_BLD_LIB  "STATIC")
elseif(BUILDING_LIBRARY STREQUAL "DYNAMIC")
  set(E2_AGENT_BLD_LIB  "SHARED")
else()
  message(FATAL_ERROR "Unknown type")
endif()


set(E2_AGENT_SRC 
            asio_agent.c
            e2ap_agent.c
            e2_agent.c
            e2_agent_api.c
            endpoint_agent.c
            msg_handler_agent.c
            e2_agent_api.c
            plugin_agent.c
            not_handler_agent.c
            ../sm/sm_proc_data.c
            $<TARGET_OBJECTS:e2ap_ap_obj>
            $<TARGET_OBJECTS:e2ap_ep_obj>
            $<TARGET_OBJECTS:e2ap_ds_obj>
            $<TARGET_OBJECTS:e2ap_alg_obj>
            $<TARGET_OBJECTS:e2_conf_obj>
            $<TARGET_OBJECTS:pending_events_obj>
            $<TARGET_OBJECTS:e2ap_types_obj>
            $<TARGET_OBJECTS:e2ap_msg_enc_obj>
            $<TARGET_OBJECTS:e2ap_msg_dec_obj>
            $<TARGET_OBJECTS:e2ap_msg_free_obj>
            $<TARGET_OBJECTS:e2_ngran_obj>
  )

if(E2AP_ENCODING STREQUAL "ASN")

  #    add_library(e2_agent ${E2_AGENT_BLD_LIB} #STATIC or SHARED
  add_library(e2_agent STATIC  #STATIC or SHARED
            ${E2_AGENT_SRC}
            $<TARGET_OBJECTS:e2ap_ie_obj>
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS")
          add_library(e2_agent ${E2_AGENT_BLD_LIB} #STATIC or SHARED
            ${E2_AGENT_SRC}
           )

          #bundle_static_library(e2_agent libflatccrt_d.a)



else()
  message(FATAL_ERROR "Error, while compiling the agent" )
endif()


if(INCL_WHAT_YOU_USE)
  find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
  set_property(TARGET e2_agent  PROPERTY C_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()

if(${BUILDING_LIBRARY} STREQUAL "DYNAMIC" )
  target_link_libraries(e2_agent  
                       PUBLIC  
                       -pthread 
                       -lsctp 
                       -ldl 
                       ) 

endif()

target_compile_definitions(e2_agent PRIVATE ${E2AP_ENCODING})

