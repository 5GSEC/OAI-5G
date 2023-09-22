add_subdirectory(iApp)

if(BUILDING_LIBRARY STREQUAL "STATIC")
  set(RIC_BLD_LIB  "STATIC")
elseif(BUILDING_LIBRARY STREQUAL "DYNAMIC")
  set(RIC_BLD_LIB  "SHARED")
else()
  message(FATAL_ERROR "Unknown type")
endif()


#if(IAPP_EMULATOR)

# include_directories(/usr/local/share/asn1c/)
    #${CMAKE_CURRENT_SOURCE_DIR}../lib/ap/ie/asn/ )   
    #  set(RIC_IAPP_SRC 
    #            ../../test/iapp-xapp/near_ric_emulator.c
    #            ../util/alg_ds/alg/defer.c
    #            ../sm/mac_sm/ie/mac_data_ie.c
    #            ../sm/mac_sm/enc/mac_enc_plain.c
    #            ../sm/rlc_sm/ie/rlc_data_ie.c
    #            ../sm/rlc_sm/enc/rlc_enc_plain.c
    #)
#endif()

set(RIC_SRC 
            e2ap_ric.c
            e2_node.c
            asio_ric.c
            endpoint_ric.c
            msg_handler_ric.c
            near_ric.c
            near_ric_api.c
            iApps/redis.c
            iApps/stdout.c
            iApps/influx.c
            iApps/string_parser.c
            plugin_ric.c
            map_e2_node_sockaddr.c
            not_handler_ric.c
            ${RIC_IAPP_SRC}
            $<TARGET_OBJECTS:e2ap_ep_obj> 
            $<TARGET_OBJECTS:e2ap_ap_obj>
            $<TARGET_OBJECTS:msg_hand_obj> 
            $<TARGET_OBJECTS:e2ap_ds_obj>
            $<TARGET_OBJECTS:e2ap_alg_obj>
            $<TARGET_OBJECTS:e2_conf_obj>
            $<TARGET_OBJECTS:e2_time_obj>
            $<TARGET_OBJECTS:e2ap_msg_enc_obj>
            $<TARGET_OBJECTS:e2ap_msg_dec_obj>
            $<TARGET_OBJECTS:e2ap_msg_free_obj>
            $<TARGET_OBJECTS:e2ap_types_obj> 
            $<TARGET_OBJECTS:pending_events_obj>
            $<TARGET_OBJECTS:e2_ngran_obj>
            )

if(E2AP_ENCODING STREQUAL "ASN")
  add_library(near_ric 
            ${RIC_BLD_LIB}# STATIC or SHARED
            ${RIC_SRC}
            $<TARGET_OBJECTS:e42_iapp>
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )

  add_library(near_ric_test 
            ${RIC_BLD_LIB}# STATIC or SHARED
            ${RIC_SRC}
            $<TARGET_OBJECTS:e42_iapp>
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS" )
  add_library(near_ric 
            ${RIC_BLD_LIB} # STATIC or SHARED
            ${RIC_SRC}
            )

  add_library(near_ric_test 
            ${RIC_BLD_LIB} # STATIC or SHARED
            ${RIC_SRC}
            )

endif()



if(INCL_WHAT_YOU_USE)
  find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
  set_property(TARGET near_ric PROPERTY C_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()


target_compile_definitions(near_ric PRIVATE ${E2AP_ENCODING})

target_compile_definitions(near_ric_test PRIVATE ${E2AP_ENCODING})
target_compile_definitions(near_ric_test PRIVATE TEST_AGENT_RIC)
