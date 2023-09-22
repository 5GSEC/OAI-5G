add_executable(xapp_hw
  hw.c
  ../../../../src/util/alg_ds/alg/defer.c
  )

 target_link_libraries(xapp_hw
 						PUBLIC
 						e42_xapp
 						-pthread
 						-lsctp
 						-ldl
 						)

