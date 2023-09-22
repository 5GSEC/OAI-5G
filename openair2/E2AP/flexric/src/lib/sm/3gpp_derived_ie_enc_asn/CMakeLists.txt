set(3GPP_DERIVED_IE_ENC_ASN_SRCS
  enc_enb.c
  enc_en_gnb.c
  enc_global_enb_id.c
  enc_global_gnb_id.c
  enc_global_ng_enb.c
  enc_global_ng_ran.c
  enc_gnb.c
  enc_gnb_cu_up.c
  enc_gnb_du.c
  enc_ng_enb.c
  enc_ng_enb_du.c
  )

include_directories("../../ap/ie/asn")
add_library(3gpp_derived_ie_enc_asn_obj OBJECT ${3GPP_DERIVED_IE_ENC_ASN_SRCS} )
              
target_compile_options(3gpp_derived_ie_enc_asn_obj PRIVATE "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(3gpp_derived_ie_enc_asn_obj PRIVATE "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(3gpp_derived_ie_enc_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
