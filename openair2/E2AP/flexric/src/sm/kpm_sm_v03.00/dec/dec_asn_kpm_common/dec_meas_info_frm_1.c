
#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "dec_meas_info_frm_1.h"
#include "../../ie/asn/MeasurementInfoItem.h"
#include "dec_label_info.h"


meas_info_format_1_lst_t * kpm_dec_meas_info_asn(const MeasurementInfoList_t * meas_info_asn)
{
  assert(meas_info_asn != NULL); 


    meas_info_format_1_lst_t* meas_info = calloc(meas_info_asn->list.count, sizeof(meas_info_format_1_lst_t));
    assert(meas_info != NULL && "Memory exhausted");

    for (int i = 0; i < meas_info_asn->list.count; i++)
    {
        const MeasurementType_t * meas_type_asn = &meas_info_asn->list.array[i]->measType;
        assert(meas_type_asn->present == MeasurementType_PR_measName || meas_type_asn->present == MeasurementType_PR_measID);

        meas_type_t* meas_type = &meas_info[i].meas_type;

        switch (meas_type_asn->present)
        {
        case MeasurementType_PR_measName:
        {
            meas_type->type = NAME_MEAS_TYPE;
            meas_type->name.buf = calloc(meas_type_asn->choice.measName.size + 1, sizeof(char));
            assert(meas_type->name.buf != NULL && "Memory exhausted");
            memcpy(meas_type->name.buf, meas_type_asn->choice.measName.buf, meas_type_asn->choice.measName.size);
            meas_type->name.len = meas_type_asn->choice.measName.size;
            break;
        }

        case MeasurementType_PR_measID:
        {
            meas_type->type = ID_MEAS_TYPE;
            memcpy(&meas_type->id, &meas_type_asn->choice.measID, 2);
            break;
        }
        
        default:
            assert(0!=0 && "Unknown Measurement Type");
        }



        meas_info[i].label_info_lst_len = meas_info_asn->list.array[i]->labelInfoList.list.count;
        assert((meas_info[i].label_info_lst_len <= maxnoofLabelInfo && meas_info[i].label_info_lst_len >= 1) 
                && "Number of labels not allowed");

        meas_info[i].label_info_lst = calloc(meas_info[i].label_info_lst_len, sizeof(*meas_info[i].label_info_lst));

        for (size_t j = 0; j<meas_info[i].label_info_lst_len; j++)
        {
            meas_info[i].label_info_lst[j] = kpm_dec_label_info_asn(&meas_info_asn->list.array[i]->labelInfoList.list.array[j]->measLabel);

        }

    }

    return meas_info;
}
