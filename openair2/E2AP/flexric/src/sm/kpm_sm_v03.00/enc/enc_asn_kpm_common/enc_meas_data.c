#include "enc_meas_data.h"
#include "../../ie/asn/asn_constant.h"
#include "../../ie/asn/MeasurementDataItem.h"
#include "../../ie/asn/MeasurementRecordItem.h"
#include <assert.h>
#include <stdbool.h>


MeasurementData_t kpm_enc_meas_data_asn(const meas_data_lst_t * meas_data, const size_t meas_data_len)
{
    assert((meas_data_len <= maxnoofMeasurementRecord && meas_data_len >= 1) 
              && "Number of measures not allowed");
    
    MeasurementData_t meas_data_asn = {0};

    for (size_t i = 0; i<meas_data_len; i++)
    {
        MeasurementDataItem_t *mData = calloc(1, sizeof(MeasurementDataItem_t));
        assert(mData != NULL && "Memory exhausted");


        // Measurement Record : [1, 2147483647]
        assert((meas_data[i].meas_record_len <= maxnoofMeasurementValue && meas_data[i].meas_record_len >= 1) 
              && "Number of measures not allowed");

        for (size_t j = 0; j<meas_data[i].meas_record_len; j++)
        {
            MeasurementRecordItem_t *mRecord = calloc(1, sizeof(MeasurementRecordItem_t));
            assert (mRecord != NULL && "Memory exhausted");
            switch (meas_data[i].meas_record_lst[j].value){
              case INTEGER_MEAS_VALUE:
                memcpy(&mRecord->choice.integer, &meas_data[i].meas_record_lst[j].int_val, 4);
                mRecord->present = MeasurementRecordItem_PR_integer;
                break;
              case REAL_MEAS_VALUE:
                mRecord->present = MeasurementRecordItem_PR_real;
                memcpy(&mRecord->choice.real, &meas_data[i].meas_record_lst[j].real_val, 8);
                break;
              case NO_VALUE_MEAS_VALUE:
                mRecord->present = MeasurementRecordItem_PR_noValue;
                mRecord->choice.noValue = 0;
                break;
              default:
                assert(false && "Non valid Measurement Record type");
            }

            int rc1 = ASN_SEQUENCE_ADD(&mData->measRecord.list, mRecord);
            assert(rc1 == 0);

        }


        // Incomplete Flag - OPTIONAL
        if (meas_data[i].incomplete_flag != NULL)
        {
            assert(*meas_data[i].incomplete_flag == TRUE_ENUM_VALUE && "has only one value (true)");
            mData->incompleteFlag = calloc(1, sizeof(*mData->incompleteFlag));
            assert(mData->incompleteFlag != NULL && "Memory exhausted");
            *mData->incompleteFlag = 0;
        }


        int rc2 = ASN_SEQUENCE_ADD(&meas_data_asn.list, mData);
        assert(rc2 == 0);

    }


    return meas_data_asn;
}
