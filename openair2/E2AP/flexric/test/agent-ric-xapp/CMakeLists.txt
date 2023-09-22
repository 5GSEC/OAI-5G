if(E2AP_ENCODING STREQUAL "ASN")
add_executable(test_ag_ric_xapp 
                test_ag_ric_xapp.c
                  ../../../test/rnd/fill_rnd_data_gtp.c                  
                  ../../../test/rnd/fill_rnd_data_tc.c                  
                  ../../../test/rnd/fill_rnd_data_mac.c                  
                  ../../../test/rnd/fill_rnd_data_rlc.c                  
                  ../../../test/rnd/fill_rnd_data_pdcp.c                  
                  ../../../test/rnd/fill_rnd_data_kpm.c                  
                  ../../../test/rnd/fill_rnd_data_rc.c                  
                  ../../../test/rnd/fill_rnd_data_slice.c                  
 




                ../../src/sm/mac_sm/ie/mac_data_ie.c
                ../../src/sm/rlc_sm/ie/rlc_data_ie.c
                ../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ../../src/sm/slice_sm/ie/slice_data_ie.c
                ../../src/sm/kpm_sm_v03.00/ie/kpm_data_ie.c
                ../../src/sm/tc_sm/ie/tc_data_ie.c
                ../../src/sm/gtp_sm/ie/gtp_data_ie.c
                ../../src/util/alg_ds/alg/defer.c
                ../../
                )

target_link_libraries(test_ag_ric_xapp
                      PUBLIC
                      e2_agent
                      near_ric
                      e42_iapp
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

else()
  message(FATAL_ERROR "Only E2AP_ENCODING allowed ")
endif()

