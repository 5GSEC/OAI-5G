
add_executable(xapp_tc_all 
                xapp_tc_all.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/util/time_now_us.c
                )

target_link_libraries(xapp_tc_all
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )


add_executable(xapp_tc_codel 
                xapp_tc_codel.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/util/time_now_us.c
                )

target_link_libraries(xapp_tc_codel
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_partition 
               xapp_tc_partition.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/util/time_now_us.c
                )

target_link_libraries(xapp_tc_partition
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )


add_executable(xapp_tc_ecn
                xapp_tc_ecn.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/util/time_now_us.c
                )

target_link_libraries(xapp_tc_ecn
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_osi_codel
               xapp_tc_osi_codel.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/util/time_now_us.c
                )

target_link_libraries(xapp_tc_osi_codel
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )


