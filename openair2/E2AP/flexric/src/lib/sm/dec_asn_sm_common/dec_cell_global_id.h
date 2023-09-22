#ifndef DECRYPTION_CELL_GLOBAL_ID_H
#define DECRYPTION_CELL_GLOBAL_ID_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/CGI.h"
#include "../sm_common_ie/cell_global_id.h"

cell_global_id_t dec_cell_global_id_asn(const CGI_t * cell_global_id_asn);

#ifdef __cplusplus
}
#endif

#endif
