#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "kpm_ric_action_def_frm_1.h"

void free_kpm_action_def_frm_1(kpm_act_def_format_1_t* src) 
{
  assert(src != NULL);

  // Measurement Information
  for (size_t i = 0; i<src->meas_info_lst_len; i++)
  {
    free_meas_info_frm_1(&src->meas_info_lst[i]);
  }
  free(src->meas_info_lst);
  
  
  // Cell Global ID
  if (src->cell_global_id != NULL)
    free(src->cell_global_id);

  
  // Measurement Bin Range
  if (src->meas_bin_info_lst != NULL)
  {
    for (size_t i = 0; i<src->meas_bin_range_info_lst_len; i++)
      free_kpm_meas_bin_range_info(&src->meas_bin_info_lst[i]);
    free(src->meas_bin_info_lst);
  }

}

bool eq_kpm_action_def_frm_1(kpm_act_def_format_1_t const * m0, kpm_act_def_format_1_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Measurement Information
  if (m0->meas_info_lst_len != m1->meas_info_lst_len)
    return false;

  for (size_t i = 0; i < m0->meas_info_lst_len; i++)
  {
    if (eq_meas_info_frm_1(&m0->meas_info_lst[i], &m1->meas_info_lst[i]) != true)
      return false;
  }

  // Granularity Period
  if (m0->gran_period_ms != m1->gran_period_ms)
    return false;

  // Cell Global ID
  if (m0->cell_global_id != NULL || m1->cell_global_id != NULL)
  {
    if (eq_cell_global_id(m0->cell_global_id, m1->cell_global_id) != true)
      return false;
  }

  // Measurement Bin Range Information
  if (m0->meas_bin_info_lst != NULL || m1->meas_bin_info_lst != NULL)
  {
    if (m0->meas_bin_range_info_lst_len != m1->meas_bin_range_info_lst_len)
      return false;

    for (size_t i = 0; i<m0->meas_bin_range_info_lst_len; i++)
    {
      if (eq_kpm_meas_bin_range_info(&m0->meas_bin_info_lst[i], &m1->meas_bin_info_lst[i]) != true)
        return false;
    }

  }

  return true;
}
