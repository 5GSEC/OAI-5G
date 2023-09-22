#include <assert.h>

#include "meas_bin_range_info_lst.h"

void free_kpm_meas_bin_range_info(meas_bin_range_info_lst_t* src)
{
  assert(src != NULL);

  // Measurement Type
  if (src->meas_type.type == NAME_MEAS_TYPE)
    free_byte_array(src->meas_type.name);

  // Bin Range
  free_kpm_bin_range_def(&src->bin_range_def);


}

bool eq_kpm_meas_bin_range_info(meas_bin_range_info_lst_t const * m0, meas_bin_range_info_lst_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Measurement Type

  if (eq_meas_type(&m0->meas_type, &m1->meas_type) != true)
    return false;

  // Bin Range
  if (eq_kpm_bin_range_def(&m0->bin_range_def, &m1->bin_range_def) != true)
    return false;

  return true;
}
