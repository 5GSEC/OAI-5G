#ifndef S_NSSAI_E2AP_H
#define S_NSSAI_E2AP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "../../../util/byte_array.h"

// 6.2.3.12   S-NSSAI

typedef struct S_NSSAI {
    uint8_t	  sST;
	uint8_t	  *sD[3];	/* OPTIONAL */
} S_NSSAI_t;


#ifdef __cplusplus
}
#endif


#endif

// done
