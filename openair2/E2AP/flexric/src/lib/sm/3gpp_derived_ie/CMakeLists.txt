set(3GPP_DERIVED_IE_SRCS
  e2sm_gummei.c
  enb.c
  en_gnb.c
  eutra_cgi.c
  global_enb_id.c
  global_gnb_id.c
  global_ng_enb_id.c
  global_ng_ran_node_id.c
  gnb.c
  gnb_cu_up.c
  gnb_du.c
  guami.c
  ng_enb.c
  ng_enb_du.c
  nr_cgi.c
  plmn_identity.c
  rrc_msg_id.c
  )

include_directories("../../ap/ie/asn")
add_library(3gpp_derived_ie_obj OBJECT ${3GPP_DERIVED_IE_SRCS})
              
