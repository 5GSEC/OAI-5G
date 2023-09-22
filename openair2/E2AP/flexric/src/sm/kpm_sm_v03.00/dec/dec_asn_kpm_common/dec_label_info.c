#include <assert.h>

#include "dec_label_info.h"

#include "../../../../util/conversions.h"

label_info_lst_t kpm_dec_label_info_asn(const MeasurementLabel_t * meas_label_asn)
{
    assert(meas_label_asn != NULL);

    label_info_lst_t label_info = {0};
    if(meas_label_asn->noLabel != NULL){
      label_info.noLabel = calloc(1, sizeof(enum_value_e)); 
      assert(label_info.noLabel != NULL && "Memory exhausted");
      // Only ONE type supported
      *label_info.noLabel = TRUE_ENUM_VALUE;
    }

    // WARNING!!!!
    // This is broken!!!
    //     /* specification mentions that if 'noLabel' is included, other elements in the same datastructure 
    //     * 'LabelInfoItem_t' shall not be included.
    //     */

    //
    // if (meas_label_asn->noLabel == 0)
    // {
    //     label_info->noLabel = TRUE_ENUM_VALUE;
 
    //     return label_info;
    // }


    if (meas_label_asn->plmnID != NULL) {
        label_info.plmn_id = calloc(1, sizeof( e2sm_plmn_t ));  
        assert(label_info.plmn_id != NULL && "Memory exhausted" );
        PLMNID_TO_MCC_MNC(meas_label_asn->plmnID, label_info.plmn_id->mcc, label_info.plmn_id->mnc, label_info.plmn_id->mnc_digit_len);
    }

    if (meas_label_asn->sliceID != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->fiveQI != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->qFI != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->qCI != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->qCImax != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->qCImin != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->aRPmax != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->aRPmin != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->bitrateRange != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->layerMU_MIMO != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->sUM != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->distBinX != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->distBinY != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->distBinZ != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->preLabelOverride != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->startEndInd != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->min != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->max != NULL) {
        assert(false && "not implemented");
    }
    if (meas_label_asn->avg != NULL) {
      label_info.avg = calloc(1, sizeof(enum_value_e)); 
      assert(label_info.avg != NULL && "Memory exhausted");
      // Only ONE type supported
      *label_info.avg = TRUE_ENUM_VALUE;
    }

    return label_info;
}
