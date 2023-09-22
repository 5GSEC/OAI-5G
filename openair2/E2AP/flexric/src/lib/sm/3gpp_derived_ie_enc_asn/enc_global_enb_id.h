#ifndef ENCRYPTION_GLOBAL_ENB_ID_H
#define ENCRYPTION_GLOBAL_ENB_ID_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/GlobalENB-ID.h"
#include "../3gpp_derived_ie/global_enb_id.h"

GlobalENB_ID_t enc_global_enb_id_asn(const global_enb_id_t * global_enb_id);

#ifdef __cplusplus
}
#endif

#endif
