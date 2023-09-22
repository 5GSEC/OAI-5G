#include "enc_meas_info_cond_ue.h"
#include "../../../../lib/sm/enc_asn_sm_common/enc_ue_id.h"
#include "../../ie/kpm_data_ie/data/meas_type.h"
#include "../../ie/asn/asn_constant.h"
#include "../../ie/asn/MeasurementCondUEidItem.h"
#include "../../ie/asn/MatchingUEidList.h"
#include "../../ie/asn/MatchingUEidItem.h"
#include "../../ie/asn/MatchingUEidPerGP.h"
#include "../../ie/asn/MatchingUEidPerGP-Item.h"
#include "enc_matching_cond_frm_3.h"
#include "enc_ue_id_gran_period_lst.h"

#include <assert.h>

MeasurementCondUEidList_t kpm_enc_meas_info_cond_ue_asn(const meas_info_cond_ue_lst_t * meas_cond_ue, const size_t meas_cond_ue_len)
{
  assert((meas_cond_ue_len <= maxnoofMeasurementInfo && meas_cond_ue_len >= 1) 
      && "Number of measures not allowed");

  MeasurementCondUEidList_t meas_cond_ue_asn={0};

  for (size_t i = 0; i<meas_cond_ue_len; i++)
  {
    MeasurementCondUEidItem_t * cond_ue_item = calloc(1, sizeof(MeasurementCondUEidItem_t));
    assert(cond_ue_item != NULL && "Memory exhausted");

    // Measurement Type
    const meas_type_t meas_type =  meas_cond_ue[i].meas_type;
    assert(meas_type.type == NAME_MEAS_TYPE || meas_type.type == ID_MEAS_TYPE);

    cond_ue_item->measType.present = meas_type.type == NAME_MEAS_TYPE 
      ? MeasurementType_PR_measName : MeasurementType_PR_measID; 

    if (cond_ue_item->measType.present == MeasurementType_PR_measName) {
      const size_t len = meas_cond_ue[i].meas_type.name.len;
      int ret = OCTET_STRING_fromBuf(&(cond_ue_item->measType.choice.measName), (char *)(meas_cond_ue[i].meas_type.name.buf), len);
      assert(ret == 0);
    }
    else {
      cond_ue_item->measType.choice.measID = meas_cond_ue[i].meas_type.id;
    }

    // Matching Condition
    assert((meas_cond_ue[i].matching_cond_lst_len <= maxnoofConditionInfo && meas_cond_ue[i].matching_cond_lst_len >= 1) 
        && "Number of labels not allowed");

    for (size_t j = 0; j<meas_cond_ue[i].matching_cond_lst_len; j++)
    {
      MatchingCondItem_t * matching_cond = kpm_enc_matching_cond_asn(&meas_cond_ue[i].matching_cond_lst[j]);
      int rc1 = ASN_SEQUENCE_ADD(&cond_ue_item->matchingCond.list, matching_cond);
      assert(rc1 == 0);
    }


    // UE_id Matched - OPTIONAL
    if (meas_cond_ue[i].ue_id_matched_lst_len != 0)
    {
      assert(meas_cond_ue[i].ue_id_matched_lst_len >= 1 && meas_cond_ue[i].ue_id_matched_lst_len <= maxnoofUEID);
      cond_ue_item->matchingUEidList = calloc(meas_cond_ue[i].ue_id_matched_lst_len, sizeof(MatchingUEidList_t));
      assert(cond_ue_item->matchingUEidList != NULL && "Memory exhausted");

      for (size_t j = 0; j<meas_cond_ue[i].ue_id_matched_lst_len; j++)
      {
        MatchingUEidItem_t * matching_ue_id_item = calloc(1, sizeof(MatchingUEidItem_t));
        assert(matching_ue_id_item != NULL && "Memory exhausted");
        matching_ue_id_item->ueID = enc_ue_id_asn(&meas_cond_ue[i].ue_id_matched_lst[j]);
        int rc1 = ASN_SEQUENCE_ADD(&cond_ue_item->matchingUEidList->list, matching_ue_id_item);
        assert(rc1 == 0);
      }
    }
    else
    {
      cond_ue_item->matchingUEidList = NULL;
    }

    // UE_id Granularity Period - OPTIONAL
    // not implemented in ASN.1 - possible extension
    if (meas_cond_ue[i].ue_id_gran_period_lst != NULL || meas_cond_ue[i].ue_id_gran_period_lst_len != 0)
    {
      assert(meas_cond_ue[i].ue_id_gran_period_lst_len >=1 && meas_cond_ue[i].ue_id_gran_period_lst_len <= maxnoofUEID);
      cond_ue_item->matchingUEidPerGP = calloc(meas_cond_ue[i].ue_id_gran_period_lst_len, sizeof(MatchingUEidPerGP_t));
      assert(cond_ue_item->matchingUEidPerGP != NULL && "Memory exhausted");

      for (size_t j = 0; j<meas_cond_ue[i].ue_id_gran_period_lst_len; j++)
      {
        MatchingUEidPerGP_Item_t * matching_gp_ue_item = kpm_enc_ue_id_gran_period_asn(&meas_cond_ue[i].ue_id_gran_period_lst[j]);
        int rc1 = ASN_SEQUENCE_ADD(&cond_ue_item->matchingUEidPerGP->list, matching_gp_ue_item);
        assert(rc1 == 0);
      }
    }

    int rc1 = ASN_SEQUENCE_ADD(&meas_cond_ue_asn.list, cond_ue_item);
    assert(rc1 == 0);
  }

  return meas_cond_ue_asn;
}
