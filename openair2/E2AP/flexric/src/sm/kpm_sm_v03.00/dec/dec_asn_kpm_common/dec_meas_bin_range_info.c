#include <assert.h>
#include "../../ie/asn/asn_constant.h"
#include "../../ie/asn/BinRangeItem.h"

#include "dec_meas_bin_range_info.h"
#include "dec_bin_range_def.h"

meas_bin_range_info_lst_t kpm_dec_meas_bin_range_item(const DistMeasurementBinRangeItem_t * meas_bin_range_item_asn)
{
  meas_bin_range_info_lst_t meas_bin_range_item = {0};

  // Measurement Type
  assert(meas_bin_range_item_asn->measType.present == MeasurementType_PR_measName || meas_bin_range_item_asn->measType.present == MeasurementType_PR_measID);

  switch (meas_bin_range_item_asn->measType.present)
  {
  case MeasurementType_PR_measName:
  {
      meas_bin_range_item.meas_type.type = NAME_MEAS_TYPE;
      meas_bin_range_item.meas_type.name.buf = calloc(meas_bin_range_item_asn->measType.choice.measName.size + 1, sizeof(char));
      assert(meas_bin_range_item.meas_type.name.buf != NULL && "Memory exhausted");
      memcpy(meas_bin_range_item.meas_type.name.buf, meas_bin_range_item_asn->measType.choice.measName.buf, meas_bin_range_item_asn->measType.choice.measName.size);
      meas_bin_range_item.meas_type.name.len = meas_bin_range_item_asn->measType.choice.measName.size;
      break;
  }

  case MeasurementType_PR_measID:
  {
      meas_bin_range_item.meas_type.type = ID_MEAS_TYPE;
      memcpy(&meas_bin_range_item.meas_type.id, &meas_bin_range_item_asn->measType.choice.measID, 2);
      break;
  }
        
  default:
      assert(0!=0 && "Unknown Measurement Type");
  }


  // Bin Range
  meas_bin_range_item.bin_range_def = kpm_dec_bin_range_def(&meas_bin_range_item_asn->binRangeDef);


  return meas_bin_range_item;
}
