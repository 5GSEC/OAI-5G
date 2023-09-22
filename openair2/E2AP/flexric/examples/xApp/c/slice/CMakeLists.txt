add_executable(xapp_slice_moni_ctrl 
             xapp_slice_moni_ctrl.c
                ../../../../src/util/alg_ds/alg/defer.c
                ../../../../src/util/time_now_us.c
                )

 target_link_libraries(xapp_slice_moni_ctrl
 						PUBLIC
 						e42_xapp
 						-pthread
 						-lsctp
 						-ldl
 						)

