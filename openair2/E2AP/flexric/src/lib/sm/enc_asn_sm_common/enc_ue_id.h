#ifndef ENCRYPTION_UE_ID_H
#define ENCRYPTION_UE_ID_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/UEID.h"

#include "../sm_common_ie/ue_id.h"

UEID_t enc_ue_id_asn(const ue_id_e2sm_t * ue_id);

#ifdef __cplusplus
}
#endif

#endif
