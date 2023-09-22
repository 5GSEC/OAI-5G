###########
# nearRT-RIC 
###########

add_executable(nearRT-RIC near_ric.c )
target_link_libraries(nearRT-RIC 
                     PUBLIC 
                      near_ric
                      ${FlatCC} 
                      -pthread
                      -lsctp
                      -ldl)

