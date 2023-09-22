#ifndef DECRYPTION_GLOBAL_NG_RAN_H
#define DECRYPTION_GLOBAL_NG_RAN_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/GlobalNGRANNodeID.h"
#include "../3gpp_derived_ie/global_ng_ran_node_id.h"

global_ng_ran_node_id_t * dec_global_ng_ran_asn(const GlobalNGRANNodeID_t * global_ng_ran_asn);

#ifdef __cplusplus
}
#endif

#endif
