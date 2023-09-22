#ifndef ENCRYPTION_ENB_H
#define ENCRYPTION_ENB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/UEID-ENB.h"
#include "../3gpp_derived_ie/enb.h"

UEID_ENB_t * enc_eNB_UE_asn(const enb_e2sm_t * enb);

#ifdef __cplusplus
}
#endif

#endif
