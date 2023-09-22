add_subdirectory(db)
add_subdirectory(swig)

if(BUILDING_LIBRARY STREQUAL "STATIC")
  set(XAPP_BLD_LIB  "STATIC")
elseif(BUILDING_LIBRARY STREQUAL "DYNAMIC")
  set(XAPP_BLD_LIB  "SHARED")
else()
  message(FATAL_ERROR "Unknown type")
endif()

set(XAPP_SRC 
  e2ap_xapp.c
  asio_xapp.c
  endpoint_xapp.c
  msg_handler_xapp.c
  msg_generator_xapp.c
  e42_xapp.c
  e42_xapp_api.c
  plugin_agent.c
  plugin_ric.c
  pending_event_xapp.c
  sync_ui.c
  act_proc.c
  msg_dispatcher_xapp.c
  ../sm/agent_if/read/sm_ag_if_rd.c
 
  $<TARGET_OBJECTS:e2ap_ep_obj> 
  $<TARGET_OBJECTS:e2ap_ap_obj>
  $<TARGET_OBJECTS:msg_hand_obj>
  $<TARGET_OBJECTS:e2ap_ds_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:e2_conf_obj>
  $<TARGET_OBJECTS:e2ap_msg_enc_obj>
  $<TARGET_OBJECTS:e2ap_msg_dec_obj>
  $<TARGET_OBJECTS:e2ap_msg_free_obj>
  $<TARGET_OBJECTS:e2ap_types_obj> 
  $<TARGET_OBJECTS:pending_events_obj>
  $<TARGET_OBJECTS:e42_xapp_db_obj>

  # KPM SM
  ../sm/kpm_sm_v03.00/ie/kpm_data_ie.c
  ../../test/rnd/fill_rnd_data_kpm.c
  #$<TARGET_OBJECTS:e2sm_kpm_asn1_obj>
  $<TARGET_OBJECTS:kpm_ric_info_common_obj>
  $<TARGET_OBJECTS:kpm_ric_info_obj>

  # RAN Control SM
  ../sm/rc_sm/ie/rc_data_ie.c
  ../../test/rnd/fill_rnd_data_rc.c
  $<TARGET_OBJECTS:e2sm_rc_ir_obj>
  $<TARGET_OBJECTS:e2sm_common_ie_obj>
  $<TARGET_OBJECTS:sm_common_ie_obj>
  $<TARGET_OBJECTS:3gpp_derived_ie_obj>
  $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
  $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
  $<TARGET_OBJECTS:sm_common_dec_asn_obj>
  $<TARGET_OBJECTS:sm_common_enc_asn_obj>
  $<TARGET_OBJECTS:sm_common_ie_obj>

  

  ../util/ngran_type.c
  ../sm/mac_sm/ie/mac_data_ie.c
  ../sm/rlc_sm/ie/rlc_data_ie.c
  ../sm/pdcp_sm/ie/pdcp_data_ie.c
  ../sm/slice_sm/ie/slice_data_ie.c
  ../sm/tc_sm/ie/tc_data_ie.c
  ../sm/gtp_sm/ie/gtp_data_ie.c

  )

if(E2AP_ENCODING STREQUAL "ASN")
  add_library(e42_xapp 
            ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )

          add_library(e42_xapp_shared SHARED # STATIC or SHARED
            ${XAPP_SRC}
	    ../util/conversions.c
            $<TARGET_OBJECTS:e2ap_asn1_obj>
            )

	 target_compile_options(e42_xapp_shared PRIVATE "-DASN_DISABLE_OER_SUPPORT" -fPIC)
	 target_include_directories(e42_xapp_shared PRIVATE "../lib/ap/ie/asn")

elseif(E2AP_ENCODING STREQUAL "FLATBUFFERS" )
  add_library(e42_xapp ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            )

  add_library(e42_xapp_shared ${XAPP_BLD_LIB} # STATIC or SHARED
            ${XAPP_SRC}
            )

endif()

target_compile_definitions(e42_xapp PRIVATE ${E2AP_ENCODING} ${XAPP_DB} )
target_compile_definitions(e42_xapp_shared PRIVATE ${E2AP_ENCODING} ${XAPP_DB} )

add_definitions(-DXAPP_DB_DIR="${XAPP_DB_DIR}")

#string(TIMESTAMP NOW "%Y-%m-%dT%H:%M:%SZ")
#string(APPEND XAPP_DB_DIR ${NOW} )
#string(APPEND XAPP_DB_DIR .sqlite3 )
#add_definitions(-DXAPP_DB_DIR="${TODAY}")

