#############################
# E2 Agent test  
#############################

add_library(test_agent_obj OBJECT 
  ../../../test/rnd/fill_rnd_data_gtp.c                  
  ../../../test/rnd/fill_rnd_data_tc.c                  
  ../../../test/rnd/fill_rnd_data_mac.c                  
  ../../../test/rnd/fill_rnd_data_rlc.c                  
  ../../../test/rnd/fill_rnd_data_pdcp.c                  
  ../../../test/rnd/fill_rnd_data_kpm.c                  
  ../../../test/rnd/fill_rnd_data_rc.c                  
  ../../../test/rnd/fill_rnd_data_slice.c                  
  ../../../src/util/time_now_us.c
  sm_gtp.c
  sm_kpm.c
  sm_mac.c
  sm_pdcp.c
  sm_rc.c
  sm_rlc.c
  sm_slice.c
  sm_tc.c
      )

#############################
# E2 Agent gNB
#############################

add_executable(emu_agent_gnb
  test_agent.c
  $<TARGET_OBJECTS:test_agent_obj>
  )

target_link_libraries(emu_agent_gnb
                      PUBLIC
                      e2_agent
                      ${FlatCC} 
                      -pthread
                      -lsctp
                      -ldl
                      -lm)

target_compile_definitions(emu_agent_gnb PRIVATE TEST_AGENT_GNB 
                                                 TEST_AGENT_RAN_TYPE=ngran_gNB
                                                 TEST_AGENT_MCC=505  
                                                 TEST_AGENT_MNC=1
                                                 TEST_AGENT_MNC_DIG_LEN=2
                                                 TEST_AGENT_NB_ID=1
                                                 TEST_AGENT_CU_DU_ID=0
                                                 ) #



#############################
# E2 Agent gNB-CU
#############################

add_executable(emu_agent_gnb_cu
  test_agent.c
  $<TARGET_OBJECTS:test_agent_obj>
  )

target_link_libraries(emu_agent_gnb_cu
                        PUBLIC
                        e2_agent
                        ${FlatCC}
                        -pthread
                        -lsctp
                        -ldl)

target_compile_definitions(emu_agent_gnb_cu PRIVATE TEST_AGENT_GNB_CU
                                                    TEST_AGENT_RAN_TYPE=ngran_gNB_CU
                                                    TEST_AGENT_MCC=505
                                                    TEST_AGENT_MNC=1
                                                    TEST_AGENT_MNC_DIG_LEN=2
                                                    TEST_AGENT_NB_ID=2
                                                    TEST_AGENT_CU_DU_ID=21
                                                    ) #

#############################
# E2 Agent gNB-DU
#############################

add_executable(emu_agent_gnb_du
  test_agent.c
  $<TARGET_OBJECTS:test_agent_obj>
  )

target_link_libraries(emu_agent_gnb_du
                        PUBLIC
                        e2_agent
                        ${FlatCC}
                        -pthread
                        -lsctp
                        -ldl)

target_compile_definitions(emu_agent_gnb_du PRIVATE TEST_AGENT_GNB_DU
                                                    TEST_AGENT_RAN_TYPE=ngran_gNB_DU
                                                    TEST_AGENT_MCC=505
                                                    TEST_AGENT_MNC=1
                                                    TEST_AGENT_MNC_DIG_LEN=2
                                                    TEST_AGENT_NB_ID=2
                                                    TEST_AGENT_CU_DU_ID=22
                                                    ) #

#############################
# E2 Agent eNB
#############################

add_executable(emu_agent_enb
  test_agent.c
  $<TARGET_OBJECTS:test_agent_obj>
  )

target_link_libraries(emu_agent_enb
                        PUBLIC
                        e2_agent
                        ${FlatCC}
                        -pthread
                        -lsctp
                        -ldl)

target_compile_definitions(emu_agent_enb PRIVATE TEST_AGENT_ENB
                                                 TEST_AGENT_RAN_TYPE=ngran_eNB
                                                 TEST_AGENT_MCC=208
                                                 TEST_AGENT_MNC=94
                                                 TEST_AGENT_MNC_DIG_LEN=2
                                                 TEST_AGENT_NB_ID=4
                                                 TEST_AGENT_CU_DU_ID=0
                                                 ) #

