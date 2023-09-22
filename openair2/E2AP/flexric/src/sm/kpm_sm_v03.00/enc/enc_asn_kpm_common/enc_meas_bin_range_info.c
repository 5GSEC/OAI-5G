#include <assert.h>

#include "enc_meas_bin_range_info.h"
#include "enc_bin_range_def.h"


DistMeasurementBinRangeItem_t * kpm_enc_meas_bin_range_item(const meas_bin_range_info_lst_t * meas_bin_range_item)
{
  DistMeasurementBinRangeItem_t * meas_bin_range_item_asn = calloc(1, sizeof(DistMeasurementBinRangeItem_t));
  assert(meas_bin_range_item_asn != NULL && "Memory exhausted");

  // Measurement Type
  const meas_type_t meas_type = meas_bin_range_item->meas_type;
  assert(meas_type.type == NAME_MEAS_TYPE || meas_type.type == ID_MEAS_TYPE);

  meas_bin_range_item_asn->measType.present = meas_type.type == NAME_MEAS_TYPE 
                                       ? MeasurementType_PR_measName : MeasurementType_PR_measID; 
      
  if (meas_bin_range_item_asn->measType.present == MeasurementType_PR_measName) {
    const size_t len = meas_bin_range_item->meas_type.name.len;
    int ret = OCTET_STRING_fromBuf(&(meas_bin_range_item_asn->measType.choice.measName), (char *)(meas_bin_range_item->meas_type.name.buf), len);
    assert(ret == 0);
  }
  else {
    memcpy(&meas_bin_range_item_asn->measType.choice.measID, &meas_bin_range_item->meas_type.id, 2);
  }


  // Bin Range
  meas_bin_range_item_asn->binRangeDef = kpm_enc_bin_range_def(&meas_bin_range_item->bin_range_def);



  return meas_bin_range_item_asn;
}
