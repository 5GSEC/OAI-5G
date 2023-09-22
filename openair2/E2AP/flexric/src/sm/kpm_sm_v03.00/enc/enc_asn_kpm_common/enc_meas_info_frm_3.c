#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "enc_meas_info_frm_3.h"
#include "enc_matching_cond_frm_3.h"
#include "enc_bin_range_def.h"
#include "../../ie/asn/MeasurementCondList.h"
#include "../../ie/asn/MeasurementCondItem.h"
#include "../../ie/asn/BinRangeDefinition.h"

MeasurementCondList_t kpm_enc_meas_info_cond_asn(const meas_info_format_3_lst_t * meas_info_cond, const size_t meas_info_cond_len)
{
    assert((meas_info_cond_len <= maxnoofMeasurementInfo && meas_info_cond_len >= 1) 
              && "Number of measures not allowed");
    
    MeasurementCondList_t meas_cond_list_asn = {0};

    int ret;

    for (size_t i = 0; i<meas_info_cond_len; i++)
    {
        MeasurementCondItem_t *mInfo = calloc(1, sizeof(MeasurementCondItem_t));
        assert (mInfo != NULL && "Memory exhausted");


        const meas_type_t meas_type =  meas_info_cond[i].meas_type;
        assert(meas_type.type == NAME_MEAS_TYPE || meas_type.type == ID_MEAS_TYPE);

        mInfo->measType.present = meas_type.type == NAME_MEAS_TYPE 
                                             ? MeasurementType_PR_measName : MeasurementType_PR_measID; 
      
        if (mInfo->measType.present == MeasurementType_PR_measName) {
          const size_t len = meas_info_cond[i].meas_type.name.len;
          ret = OCTET_STRING_fromBuf(&(mInfo->measType.choice.measName), (char *)(meas_info_cond[i].meas_type.name.buf), len);
          assert(ret == 0);
        }
        else {
          mInfo->measType.choice.measID = meas_info_cond[i].meas_type.id;
        }


        assert(meas_info_cond[i].matching_cond_lst_len <= maxnoofConditionInfo && meas_info_cond[i].matching_cond_lst_len >= 1);

        for (size_t j = 0; j<meas_info_cond[i].matching_cond_lst_len; j++)
        {
            MatchingCondItem_t * matching_cond = kpm_enc_matching_cond_asn(&meas_info_cond[i].matching_cond_lst[j]);
            int rc1 = ASN_SEQUENCE_ADD(&mInfo->matchingCond.list, matching_cond);
            assert(rc1 == 0);
        }


        if (meas_info_cond[i].bin_range_def != NULL) {
          mInfo->binRangeDef = calloc(1, sizeof(BinRangeDefinition_t));
          assert(mInfo->binRangeDef != NULL && "Memory exhausted");
          *mInfo->binRangeDef = kpm_enc_bin_range_def(meas_info_cond[i].bin_range_def);
        }


        int rc2 = ASN_SEQUENCE_ADD(&meas_cond_list_asn.list, mInfo);
        assert(rc2 == 0);
    }

    return meas_cond_list_asn;

}
