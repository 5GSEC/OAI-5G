set(3GPP_DERIVED_IE_DEC_ASN_SRCS
  dec_enb.c
  dec_en_gnb.c
  dec_global_enb_id.c
  dec_global_gnb_id.c
  dec_global_ng_enb.c
  dec_global_ng_ran.c
  dec_gnb.c
  dec_gnb_cu_up.c
  dec_gnb_du.c
  dec_ng_enb.c
  dec_ng_enb_du.c
  )


include_directories("../../ap/ie/asn")
add_library(3gpp_derived_ie_dec_asn_obj OBJECT
                                    ${3GPP_DERIVED_IE_DEC_ASN_SRCS}
                                    )
   
              
target_compile_options(3gpp_derived_ie_dec_asn_obj PRIVATE "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(3gpp_derived_ie_dec_asn_obj PRIVATE "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(3gpp_derived_ie_dec_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
