#ifndef DECODE_RAN_FUNCTION_NAME_E2SM_H
#define DECODE_RAN_FUNCTION_NAME_E2SM_H

#include "../../../sm/rc_sm/ie/asn/RANfunction-Name.h"
#include "../sm_common_ie/ran_function_name.h"

ran_function_name_t dec_ran_func_name(RANfunction_Name_t const* rf);

#endif

