#include <assert.h>
#include "../../ie/asn/asn_constant.h"

#include "enc_meas_info_frm_1.h"
#include "enc_label_info.h"
#include "../../ie/asn/MeasurementInfoItem.h"
#include "../../ie/asn/LabelInfoItem.h"

MeasurementInfoList_t kpm_enc_meas_info_asn(const meas_info_format_1_lst_t *meas_info, const size_t meas_info_len) 
{
    assert((meas_info_len <= maxnoofMeasurementInfo && meas_info_len >= 1) 
              && "Number of measures not allowed");
    
    MeasurementInfoList_t meas_list_asn = {0};

    int ret;

    for (size_t i = 0; i<meas_info_len; i++)
    {
        MeasurementInfoItem_t *mInfo = calloc(1, sizeof(MeasurementInfoItem_t));
        assert (mInfo != NULL && "Memory exhausted");
        
        const meas_type_t meas_type =  meas_info[i].meas_type;
        assert(meas_type.type == NAME_MEAS_TYPE || meas_type.type == ID_MEAS_TYPE);

        mInfo->measType.present = meas_type.type == NAME_MEAS_TYPE 
                                             ? MeasurementType_PR_measName : MeasurementType_PR_measID; 
      
        if (mInfo->measType.present == MeasurementType_PR_measName) {
          const size_t len = meas_info[i].meas_type.name.len;
          ret = OCTET_STRING_fromBuf(&(mInfo->measType.choice.measName), (char *)(meas_info[i].meas_type.name.buf), len);
          assert(ret == 0);
        }
        else {
          memcpy(&mInfo->measType.choice.measID, &meas_info[i].meas_type.id, 2);
        }


        assert((meas_info[i].label_info_lst_len <= maxnoofLabelInfo && meas_info[i].label_info_lst_len >= 1) 
                && "Number of labels not allowed");

        for (size_t j = 0; j<meas_info[i].label_info_lst_len; j++)
        {
          LabelInfoItem_t * labels = kpm_enc_label_info_asn(&meas_info[i].label_info_lst[j]);
          int rc1 = ASN_SEQUENCE_ADD(&mInfo->labelInfoList.list, labels);
          assert(rc1 == 0);
        }
        
        int rc2 = ASN_SEQUENCE_ADD(&meas_list_asn.list, mInfo);
        assert(rc2 == 0);
    }

    return meas_list_asn;

}
