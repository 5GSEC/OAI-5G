#include "ngran_types.h"

#include <assert.h>
#include <stddef.h>

char* get_ngran_name(ngran_node_t ran_type)
{
  assert(ran_type >= 0);
  switch (ran_type)
  {
    case ngran_eNB: return "ngran_eNB";
    case ngran_ng_eNB: return "ngran_ng_eNB";
    case ngran_gNB: return "ngran_gNB";
    case ngran_eNB_CU: return "ngran_eNB_CU";
    case ngran_ng_eNB_CU: return "ngran_ng_eNB_CU";
    case ngran_gNB_CU: return "ngran_gNB_CU";
    case ngran_eNB_DU: return "ngran_eNB_DU";
    case ngran_gNB_DU: return "ngran_gNB_DU";
    case ngran_eNB_MBMS_STA: return "ngran_eNB_MBMS_STA";
    default: return NULL;
  }
}