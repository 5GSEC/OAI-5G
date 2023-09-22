#include <assert.h>

#include "../../../util/conversions.h"

#include "dec_global_gnb_id.h"

global_gnb_id_t dec_global_gnb_id_asn(const GlobalGNB_ID_t * global_gnb_id_asn)
{
  assert(global_gnb_id_asn != NULL);

  global_gnb_id_t global_gnb_id = {0};

  // PLMN Identity
  PLMNID_TO_MCC_MNC(&global_gnb_id_asn->pLMNIdentity, global_gnb_id.plmn_id.mcc, global_gnb_id.plmn_id.mnc, global_gnb_id.plmn_id.mnc_digit_len);


  // gNB Type
  if (global_gnb_id_asn->gNB_ID.present == GNB_ID_PR_gNB_ID)
  {
    global_gnb_id.type = GNB_TYPE_ID;
    global_gnb_id.gnb_id = cp_macro_gnb_id_to_u32(global_gnb_id_asn->gNB_ID.choice.gNB_ID);
  }
  else
  {
    assert(false && "Unknown gNB Type");
  }

  return global_gnb_id;
}
