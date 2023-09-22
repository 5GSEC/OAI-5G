#ifndef DECRYPTION_UE_ID_H
#define DECRYPTION_UE_ID_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../sm_common_ie/ue_id.h"
#include "../../../sm/rc_sm/ie/asn/UEID.h"

ue_id_e2sm_t dec_ue_id_asn(const UEID_t * ue_id_asn);

#ifdef __cplusplus
}
#endif

#endif
