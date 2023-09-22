#include <assert.h>

#include "bin_range_def.h"

void free_kpm_bin_range_def(bin_range_def_t* src)
{
  assert(src != NULL);

  // Bin X
  free(src->bin_x_lst);

  // Bin Y
  if (src->bin_y_lst != NULL)
  {
    for (size_t i = 0; i<src->bin_y_lst_len; i++)
      free(&src->bin_y_lst[i]);
    free(src->bin_y_lst);
  }

  // Bin Z
  if (src->bin_z_lst != NULL)
  {
    for (size_t i = 0; i<src->bin_z_lst_len; i++)
      free(&src->bin_z_lst[i]);
    free(src->bin_z_lst);
  }

}

bool eq_kpm_bin_range_def(bin_range_def_t const * m0, bin_range_def_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Bin X
  if (m0->bin_x_lst_len != m1->bin_x_lst_len)
    return false;

  for (size_t i = 0; i<m0->bin_x_lst_len; i++)
  {
    if (m0->bin_x_lst[i].bin_index != m1->bin_x_lst[i].bin_index)
      return false;

    if (m0->bin_x_lst[i].start_value.value != m1->bin_x_lst[i].start_value.value)
      return false;

    // start value
    switch (m0->bin_x_lst[i].start_value.value)
    {
    case INTEGER_BIN_RANGE:
        if (m0->bin_x_lst[i].start_value.int_value != m1->bin_x_lst[i].start_value.int_value)
          return false;
        break;
    
    case REAL_BIN_RANGE:
        if (m0->bin_x_lst[i].start_value.real_value != m1->bin_x_lst[i].start_value.real_value)
          return false;
        break;

    default:
        break;
    }

    // end value
    switch (m0->bin_x_lst[i].end_value.value)
    {
    case INTEGER_BIN_RANGE:
        if (m0->bin_x_lst[i].end_value.int_value != m1->bin_x_lst[i].end_value.int_value)
          return false;
        break;
    
    case REAL_BIN_RANGE:
        if (m0->bin_x_lst[i].end_value.real_value != m1->bin_x_lst[i].end_value.real_value)
          return false;
        break;

    default:
        break;
    }
  }

  // Bin Y
  if ((m0->bin_y_lst != NULL || m1->bin_y_lst != NULL) && m0->bin_y_lst_len != m1->bin_y_lst_len)
    assert(false && "Not yet implemented");

  // Bin Z
  if ((m0->bin_z_lst != NULL || m1->bin_z_lst != NULL) && m0->bin_z_lst_len != m1->bin_z_lst_len)
    assert(false && "Not yet implemented");


  return true;
}
