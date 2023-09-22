cmake_minimum_required(VERSION 3.15) # setting this is required
project(mac_sm)            # this sets the project name

set(SM_ENCODING_MAC_SRC
                      ../sm_proc_data.c 
                      mac_sm_ric.c 
                      mac_sm_agent.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                       # This dependency sucks!
                      ../../lib/ap/e2ap_types/common/e2ap_ran_function.c

                     ie/mac_data_ie.c
                      )

if(SM_ENCODING_MAC STREQUAL "PLAIN")
  set(SM_ENCODING_MAC_SRC_ASN 
    ${SM_ENCODING_MAC_SRC}
    enc/mac_enc_plain.c 
    dec/mac_dec_plain.c 
    )

  # Shared
  add_library(mac_sm SHARED ${SM_ENCODING_MAC_SRC_ASN})
  target_compile_options(mac_sm PRIVATE -fvisibility=hidden)

  # Static
  add_library(mac_sm_static STATIC ${SM_ENCODING_MAC_SRC_ASN})

elseif(SM_ENCODING_MAC STREQUAL "ASN")
  message(FATAL_ERROR "MAC SM ASN not implemented")

  add_subdirectory(ie/asn)

  add_library(mac_sm SHARED
                      ${SM_ENCODING_MAC_SRC}
                      enc/mac_enc_asn.c 
                      dec/mac_dec_asn.c 
                      )
                    #target_include_directories(mac_sm PRIVATE
                    #        "ie/asn")
                    #  target_compile_options(mac_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
                    #  target_compile_options(mac_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)



elseif(SM_ENCODING_MAC STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "MAC SM FB not implemented")
  add_library(mac_sm SHARED 
                      ${SM_ENCODING_MAC_SRC}
                      enc/mac_enc_fb.c 
                      dec/mac_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknown SM encoding type")
endif()


target_compile_definitions(mac_sm PUBLIC ${SM_ENCODING_MAC})
target_compile_definitions(mac_sm_static PUBLIC ${SM_ENCODING_MAC})


