#ifndef ENCRYPTION_CELL_GLOBAL_ID_H
#define ENCRYPTION_CELL_GLOBAL_ID_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../../sm/rc_sm/ie/asn/CGI.h" 
#include "../sm_common_ie/cell_global_id.h"

CGI_t enc_cell_global_id_asn(const cell_global_id_t * cell_global_id);

#ifdef __cplusplus
}
#endif

#endif
