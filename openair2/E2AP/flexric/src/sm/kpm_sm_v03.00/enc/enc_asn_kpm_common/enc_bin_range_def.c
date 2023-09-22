#include <assert.h>
#include "../../ie/asn/asn_constant.h"
#include "../../ie/asn/BinRangeItem.h"

#include "enc_bin_range_def.h"

BinRangeDefinition_t kpm_enc_bin_range_def(const bin_range_def_t * bin_range_def)
{
  BinRangeDefinition_t bin_range_def_asn = {0};


  // Bin X
  assert(bin_range_def->bin_x_lst_len >= 1 && bin_range_def->bin_x_lst_len <= maxnoofBin);


  for (size_t i = 0; i<bin_range_def->bin_x_lst_len; i++)
  {
    BinRangeItem_t * bin_range_item = calloc(1, sizeof(BinRangeItem_t));
    assert(bin_range_item != NULL && "Memory exhausted");  

    // bin index
    memcpy(&bin_range_item->binIndex, &bin_range_def->bin_x_lst[i].bin_index, 2);
      
    // start value
    switch (bin_range_def->bin_x_lst[i].start_value.value)
    {
    case INTEGER_BIN_RANGE:
        bin_range_item->startValue.present = BinRangeValue_PR_valueInt;
        memcpy(&bin_range_item->startValue.choice.valueInt, &bin_range_def->bin_x_lst[i].start_value.int_value, 4);
        break;  

    case REAL_BIN_RANGE:
        bin_range_item->startValue.present = BinRangeValue_PR_valueReal;
        memcpy(&bin_range_item->startValue.choice.valueReal, &bin_range_def->bin_x_lst[i].start_value.real_value, 8);
        break;
      
    default:
        break;
    }  

    // end value
    switch (bin_range_def->bin_x_lst[i].end_value.value)
    {
    case INTEGER_BIN_RANGE:
        bin_range_item->endValue.present = BinRangeValue_PR_valueInt;
        memcpy(&bin_range_item->endValue.choice.valueInt, &bin_range_def->bin_x_lst[i].end_value.int_value, 4);
        break;  

    case REAL_BIN_RANGE:
        bin_range_item->endValue.present = BinRangeValue_PR_valueReal;
        memcpy(&bin_range_item->endValue.choice.valueReal, &bin_range_def->bin_x_lst[i].end_value.real_value, 8);
        break;
      
    default:
        break;
    }

    int rc1 = ASN_SEQUENCE_ADD(&bin_range_def_asn.binRangeListX.list, bin_range_item);
    assert(rc1 == 0);
  }
    
  // Bin Y
  if (bin_range_def->bin_y_lst != NULL || bin_range_def->bin_y_lst_len != 0)
    assert(false && "Not yet implemented");

  // Bin Z
  if (bin_range_def->bin_z_lst != NULL || bin_range_def->bin_z_lst_len != 0)
    assert(false && "Not yet implemented");



  return bin_range_def_asn;
}
