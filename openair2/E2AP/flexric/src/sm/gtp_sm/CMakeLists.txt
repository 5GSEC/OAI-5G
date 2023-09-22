cmake_minimum_required(VERSION 3.15) # setting this is required
project(gtp_sm)            # this sets the project name

set(SM_ENCODING_GTP_SRC
                      ../sm_proc_data.c 
                      gtp_sm_agent.c 
                      gtp_sm_ric.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                      ie/gtp_data_ie.c 
                      )


if(SM_ENCODING_GTP STREQUAL "PLAIN")
  set(SM_ENCODING_GTP_SRC_PLAIN
    ${SM_ENCODING_GTP_SRC}
    # This dependency sucks!
    ../../lib/ap/e2ap_types/common/e2ap_ran_function.c
    enc/gtp_enc_plain.c 
    dec/gtp_dec_plain.c 
    )

  # Shared
  add_library(gtp_sm SHARED
              ${SM_ENCODING_GTP_SRC_PLAIN}
              )
  target_compile_options(gtp_sm PRIVATE -fvisibility=hidden)

  # Static
  add_library(gtp_sm_static STATIC 
              ${SM_ENCODING_GTP_SRC_PLAIN}
              )

else()
  message(FATAL_ERROR "Unknown SM encoding type ")
endif()

target_compile_definitions(gtp_sm PUBLIC ${SM_ENCODING_GTP})
target_compile_definitions(gtp_sm_static PUBLIC ${SM_ENCODING_GTP})

