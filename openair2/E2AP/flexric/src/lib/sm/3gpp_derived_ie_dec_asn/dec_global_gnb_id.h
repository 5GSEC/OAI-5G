#ifndef DECRYPTION_GLOBAL_GNB_ID_H
#define DECRYPTION_GLOBAL_GNB_ID_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/GlobalGNB-ID.h"
#include "../3gpp_derived_ie/global_gnb_id.h"

global_gnb_id_t dec_global_gnb_id_asn(const GlobalGNB_ID_t * global_gnb_id_asn);

#ifdef __cplusplus
}
#endif

#endif
