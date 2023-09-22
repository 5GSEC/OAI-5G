#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "dec_meas_info_frm_3.h"
#include "../../ie/asn/MeasurementCondItem.h"
#include "dec_matching_cond_frm_3.h"
#include "dec_bin_range_def.h"

meas_info_format_3_lst_t * kpm_dec_meas_info_cond_asn(const MeasurementCondList_t * meas_info_cond_asn, const size_t meas_info_lst_len)
{
    meas_info_format_3_lst_t * meas_info_cond = calloc(meas_info_lst_len, sizeof(meas_info_format_3_lst_t));
    assert(meas_info_cond != NULL && "Memory exhausted");

    for (size_t i = 0; i<meas_info_lst_len; i++)
    {
        const MeasurementType_t * meas_type_asn = &meas_info_cond_asn->list.array[i]->measType;
        assert(meas_type_asn->present == MeasurementType_PR_measName || meas_type_asn->present == MeasurementType_PR_measID);

        switch (meas_type_asn->present)
        {
        case MeasurementType_PR_measName:
        {
            meas_info_cond[i].meas_type.type = NAME_MEAS_TYPE;
            meas_info_cond[i].meas_type.name.buf = calloc(meas_type_asn->choice.measName.size + 1, sizeof(char));
            assert(meas_info_cond[i].meas_type.name.buf != NULL && "Memory exhausted");
            memcpy(meas_info_cond[i].meas_type.name.buf, meas_type_asn->choice.measName.buf, meas_type_asn->choice.measName.size);
            meas_info_cond[i].meas_type.name.len = meas_type_asn->choice.measName.size;
            break;
        }
        case MeasurementType_PR_measID:
        {
            meas_info_cond[i].meas_type.type = ID_MEAS_TYPE;
            meas_info_cond[i].meas_type.id = meas_type_asn->choice.measID;
            break;
        }
        default:
            assert(0!=0 && "Unknown Measurement Type" );

        }


        meas_info_cond[i].matching_cond_lst_len = meas_info_cond_asn->list.array[i]->matchingCond.list.count;
        assert(meas_info_cond[i].matching_cond_lst_len <= maxnoofConditionInfo && meas_info_cond[i].matching_cond_lst_len >= 1);

        meas_info_cond[i].matching_cond_lst = calloc(meas_info_cond[i].matching_cond_lst_len, sizeof(*meas_info_cond[i].matching_cond_lst));
        assert(meas_info_cond[i].matching_cond_lst != NULL && "Memory exhausted");

        for (size_t j = 0; j<meas_info_cond[i].matching_cond_lst_len; j++)
        {
            meas_info_cond[i].matching_cond_lst[j] = kpm_dec_matching_cond_asn(meas_info_cond_asn->list.array[i]->matchingCond.list.array[j]);
        }

        // Bin Range Definition
        // not yet implemented in ASN.1 - possible extension
        meas_info_cond[i].bin_range_def = calloc(1, sizeof(bin_range_def_t));
        assert(meas_info_cond[i].bin_range_def != NULL && "Memory exhausted");
        *meas_info_cond[i].bin_range_def = kpm_dec_bin_range_def(meas_info_cond_asn->list.array[i]->binRangeDef);
    }

    return meas_info_cond;
}
