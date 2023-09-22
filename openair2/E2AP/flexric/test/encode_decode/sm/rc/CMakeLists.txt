project (test_enc_dec_rc_sm)

add_executable(test_enc_dec_rc_sm 
  ../../../rnd/fill_rnd_data_rc.c
  ../../../../src/util/time_now_us.c
  ../../../../src/util/alg_ds/alg/defer.c 
  main.c)

target_link_libraries(test_enc_dec_rc_sm PUBLIC 
  rc_sm_static
  -lm
  )

target_compile_options(test_enc_dec_rc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

target_compile_options(test_enc_dec_rc_sm  PRIVATE "-DASN_DISABLE_OER_SUPPORT -DPDU")

