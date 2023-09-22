#include <assert.h>
#include "../../ie/asn/asn_constant.h"
#include "../../ie/asn/BinRangeItem.h"

#include "dec_bin_range_def.h"

bin_range_def_t kpm_dec_bin_range_def(const BinRangeDefinition_t * bin_range_def_asn)
{
  bin_range_def_t bin_range_def = {0};


  // Bin X
  bin_range_def.bin_x_lst_len = bin_range_def_asn->binRangeListX.list.count;
  assert(bin_range_def.bin_x_lst_len >= 1 && bin_range_def.bin_x_lst_len <= maxnoofBin);

  bin_range_def.bin_x_lst = calloc(bin_range_def.bin_x_lst_len, sizeof(bin_distr_t));
  assert(bin_range_def.bin_x_lst != NULL && "Memory exhausted"); 
  
  for (size_t i = 0; i<bin_range_def.bin_x_lst_len; i++)
  {
    BinRangeItem_t * bin_range_item = bin_range_def_asn->binRangeListX.list.array[i]; 
    assert(bin_range_item != NULL && "Memory exhausted");  

    // bin index
    memcpy(&bin_range_def.bin_x_lst[i].bin_index, &bin_range_item->binIndex, 2);

    // start value
    switch (bin_range_item->startValue.present)
    {
    case BinRangeValue_PR_valueInt:
        bin_range_def.bin_x_lst[i].start_value.value = INTEGER_BIN_RANGE;
        memcpy(&bin_range_def.bin_x_lst[i].start_value.int_value, &bin_range_item->startValue.choice.valueInt, 4);
        break;

    case BinRangeValue_PR_valueReal:
        bin_range_def.bin_x_lst[i].start_value.value = REAL_BIN_RANGE;
        memcpy(&bin_range_def.bin_x_lst[i].start_value.real_value, &bin_range_item->startValue.choice.valueReal, 8);
        break;
    
    default:
        break;
    }

    // end value
    switch (bin_range_item->endValue.present)
    {
    case BinRangeValue_PR_valueInt:
        bin_range_def.bin_x_lst[i].end_value.value = INTEGER_BIN_RANGE;
        memcpy(&bin_range_def.bin_x_lst[i].end_value.int_value, &bin_range_item->endValue.choice.valueInt, 4);
        break;

    case BinRangeValue_PR_valueReal:
        bin_range_def.bin_x_lst[i].end_value.value = REAL_BIN_RANGE;
        memcpy(&bin_range_def.bin_x_lst[i].end_value.real_value, &bin_range_item->endValue.choice.valueReal, 8);
        break;
    
    default:
        break;
    }
  }

  // Bin Y
  if (bin_range_def_asn->binRangeListY != NULL)
    assert(false && "Not yet implemented");

  // Bin Z
  if (bin_range_def_asn->binRangeListZ != NULL)
    assert(false && "Not yet implemented");


  return bin_range_def;
}
