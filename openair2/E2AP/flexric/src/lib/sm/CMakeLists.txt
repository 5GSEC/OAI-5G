add_subdirectory(3gpp_derived_ie)
add_subdirectory(3gpp_derived_ie_dec_asn)
add_subdirectory(3gpp_derived_ie_enc_asn)
add_subdirectory(sm_common_ie)
add_subdirectory(dec_asn_sm_common)
add_subdirectory(enc_asn_sm_common)


add_library(e2sm_common_ie_obj OBJECT
                                $<TARGET_OBJECTS:3gpp_derived_ie_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
                                $<TARGET_OBJECTS:sm_common_ie_obj>
                                $<TARGET_OBJECTS:sm_common_dec_asn_obj>
                                $<TARGET_OBJECTS:sm_common_enc_asn_obj>
                                )

                              target_link_libraries(e2sm_common_ie_obj PUBLIC
                                        3gpp_derived_ie_obj
                                        3gpp_derived_ie_dec_asn_obj
                                        3gpp_derived_ie_enc_asn_obj
                                        sm_common_ie_obj
                                        sm_common_dec_asn_obj
                                        sm_common_enc_asn_obj)
