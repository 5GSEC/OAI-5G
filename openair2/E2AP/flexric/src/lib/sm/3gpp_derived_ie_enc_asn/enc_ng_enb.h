#ifndef ENCRYPTION_NG_ENB_H
#define ENCRYPTION_NG_ENB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/UEID-NG-ENB.h"
#include "../3gpp_derived_ie/ng_enb.h"

UEID_NG_ENB_t * enc_ng_eNB_UE_asn(const ng_enb_e2sm_t * ng_enb);

#ifdef __cplusplus
}
#endif

#endif
