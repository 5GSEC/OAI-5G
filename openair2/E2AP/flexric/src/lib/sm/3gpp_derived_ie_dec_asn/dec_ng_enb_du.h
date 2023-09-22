#ifndef DECRYPTION_NG_ENB_DU_H
#define DECRYPTION_NG_ENB_DU_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/UEID-NG-ENB-DU.h"
#include "../3gpp_derived_ie/ng_enb_du.h"

ng_enb_du_e2sm_t dec_ng_eNB_DU_UE_asn(const UEID_NG_ENB_DU_t * ng_enb_du_asn);

#ifdef __cplusplus
}
#endif

#endif
