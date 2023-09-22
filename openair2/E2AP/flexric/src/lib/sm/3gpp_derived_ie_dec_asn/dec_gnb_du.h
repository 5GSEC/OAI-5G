#ifndef DECRYPTION_GNB_DU_H
#define DECRYPTION_GNB_DU_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/UEID-GNB-DU.h"
#include "../3gpp_derived_ie/gnb_du.h"

gnb_du_e2sm_t dec_gNB_DU_UE_asn(const UEID_GNB_DU_t * gnb_du_asn);

#ifdef __cplusplus
}
#endif

#endif
