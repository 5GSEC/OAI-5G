set(SM_COMMON_ENC_ASN_SRCS
    enc_cell_global_id.c
    enc_ue_id.c
    enc_ran_function_name.c
    )

include_directories("../../ap/ie/asn")
add_library(sm_common_enc_asn_obj OBJECT 
                                ${SM_COMMON_ENC_ASN_SRCS}
                                )
target_compile_options(sm_common_enc_asn_obj PRIVATE "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(sm_common_enc_asn_obj PRIVATE "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(sm_common_enc_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
