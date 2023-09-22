add_executable(xapp_mac_rlc_pdcp_moni 
		xapp_mac_rlc_pdcp_moni.c
                ../../../../src/util/alg_ds/alg/defer.c
              )

 target_link_libraries(xapp_mac_rlc_pdcp_moni
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )


add_executable(xapp_gtp_moni 
		xapp_gtp_moni.c
                ../../../../src/util/alg_ds/alg/defer.c
              )

target_link_libraries(xapp_gtp_moni
                PUBLIC
                e42_xapp
                -pthread
                -lsctp
                -ldl
                )


