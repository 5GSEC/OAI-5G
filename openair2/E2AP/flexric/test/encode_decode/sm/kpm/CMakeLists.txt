project (test_enc_dec_kpm_sm)

add_executable(test_enc_dec_kpm_sm 
  ../../../rnd/fill_rnd_data_kpm.c
  ../../../../src/util/time_now_us.c
  ../../../../src/util/alg_ds/alg/defer.c 
  main.c)

target_include_directories(test_enc_dec_kpm_sm PUBLIC "../../../../src/sm/kpm_sm_v03.00/ie/asn")

target_link_libraries(test_enc_dec_kpm_sm 
                            PUBLIC 
                            kpm_sm_static
                            -lm
                            )

target_compile_options(test_enc_dec_kpm_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

