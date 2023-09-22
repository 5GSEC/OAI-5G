#ifndef DECRYPTION_EN_GNB_H
#define DECRYPTION_EN_GNB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/UEID-EN-GNB.h"
#include "../3gpp_derived_ie/en_gnb.h"

en_gnb_e2sm_t dec_en_gNB_UE_asn(const UEID_EN_GNB_t * en_gnb_asn);

#ifdef __cplusplus
}
#endif

#endif
