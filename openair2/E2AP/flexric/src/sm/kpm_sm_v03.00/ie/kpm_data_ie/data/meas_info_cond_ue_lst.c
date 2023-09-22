#include <assert.h>

#include "meas_info_cond_ue_lst.h"

void free_kpm_meas_info_cond_ue(meas_info_cond_ue_lst_t* src)
{
  assert(src != NULL);

  // Measurement Type
  if (src->meas_type.type ==  NAME_MEAS_TYPE)
    free_byte_array(src->meas_type.name);

  // Matching Condition Format 3
  // [1, 32768]
  for (size_t i = 0; i<src->matching_cond_lst_len; i++)
    free_matching_cond_frm_3(&src->matching_cond_lst[i]);
  free(src->matching_cond_lst);
  
  // List of matched UE IDs
  // Optional [0, 65535]
  if (src->ue_id_matched_lst != NULL)
  {
    for (size_t i = 0; i<src->ue_id_matched_lst_len; i++)
      free_ue_id_e2sm(&src->ue_id_matched_lst[i]);
  }
  free(src->ue_id_matched_lst);

  // Sequence of Matched UE IDs for Granularity Periods
  // not yet implemented in ASN.1 - possible extension
  if (src->ue_id_gran_period_lst != NULL || src->ue_id_gran_period_lst_len != 0)
  {
    for (size_t i = 0; i<src->ue_id_gran_period_lst_len; i++)
    {
      free_kpm_ue_id_gran_period_lst(&src->ue_id_gran_period_lst[i]);
    }
    free(src->ue_id_gran_period_lst);
  }

}


bool eq_kpm_meas_info_cond_ue(meas_info_cond_ue_lst_t const* m0, meas_info_cond_ue_lst_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Measurement Type
  if (eq_meas_type(&m0->meas_type, &m1->meas_type) != true){
    assert(0!=0 && "For debugging");
    return false;
  }

  // Matching Condition Format 3
  // [1, 32768]
  if (m0->matching_cond_lst_len != m1->matching_cond_lst_len){
    assert(0!=0 && "For debugging");
    return false;
  }

  for (size_t i = 0; i<m0->matching_cond_lst_len; i++)
  {
    if (eq_matching_cond_frm_3(&m0->matching_cond_lst[i], &m1->matching_cond_lst[i]) != true){
    assert(0!=0 && "For debugging");
      return false;
    }
  }

  // List of matched UE IDs
  // Optional [0, 65535]
  if (m0->ue_id_matched_lst_len != m1->ue_id_matched_lst_len){
    assert(0!=0 && "For debugging");
    return false;
  }

  for (size_t i = 0; i<m0->ue_id_matched_lst_len; i++){
    if (eq_ue_id_e2sm(&m0->ue_id_matched_lst[i], &m1->ue_id_matched_lst[i]) != true){
    assert(0!=0 && "For debugging");
      return false;
    }
  }

  // Sequence of Matched UE IDs for Granularity Periods
  // not yet implemented in ASN.1 - possible extension
  if (m0->ue_id_gran_period_lst != NULL || m0->ue_id_gran_period_lst_len != 0 || m1->ue_id_gran_period_lst_len != 0 || m1->ue_id_gran_period_lst != NULL)
  {
    for (size_t i = 0; i<m0->ue_id_gran_period_lst_len; i++)
    {
      if (eq_kpm_ue_id_gran_period_lst(&m0->ue_id_gran_period_lst[i], &m1->ue_id_gran_period_lst[i]) != true){
        assert(0!=0 && "For debugging");
        return false;
      }
    }
  }


  return true;
}


meas_info_cond_ue_lst_t cp_kpm_meas_info_cond_ue(meas_info_cond_ue_lst_t const* src)
{
  assert(src != NULL);

  meas_info_cond_ue_lst_t dst = {0};

  // Meas Type
  dst.meas_type = cp_meas_type(&src->meas_type);

  // [1, 32768]
  // Matching Condition Format 3
  assert(src->matching_cond_lst_len > 0 && src->matching_cond_lst_len < 32769); 
  dst.matching_cond_lst_len = src->matching_cond_lst_len;

  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_3_lst_t));
  assert(dst.matching_cond_lst != NULL && "Memory exhausted");
//  memcpy(dst.matching_cond_lst, src->matching_cond_lst, dst.matching_cond_lst_len * sizeof(matching_condition_format_3_lst_t));
      
  for (size_t j = 0; j<dst.matching_cond_lst_len; j++)
  {
    dst.matching_cond_lst[j] = cp_kpm_matching_cond_frm_3(&src->matching_cond_lst[j]);
  }

  // List of matched UE IDs
  dst.ue_id_matched_lst_len = src->ue_id_matched_lst_len;

  dst.ue_id_matched_lst = calloc(dst.ue_id_matched_lst_len, sizeof(ue_id_e2sm_t));
  assert(dst.ue_id_matched_lst != NULL && "Memory exhausted");
//  memcpy(dst.ue_id_matched_lst, src->ue_id_matched_lst, dst.ue_id_matched_lst_len * sizeof(ue_id_e2sm_t));

  for (size_t j = 0; j< dst.ue_id_matched_lst_len; j++)
  {
    dst.ue_id_matched_lst[j] = cp_ue_id_e2sm(&src->ue_id_matched_lst[j]);
  }

  // Sequence of Matched UE IDs for Granularity Periods
  dst.ue_id_gran_period_lst_len = src->ue_id_gran_period_lst_len;
  if(dst.ue_id_gran_period_lst_len > 0){
    dst.ue_id_gran_period_lst = calloc(dst.ue_id_gran_period_lst_len, sizeof(ue_id_gran_period_lst_t));
    assert(dst.ue_id_gran_period_lst != NULL && "Memory exhausted");
  }
//  memcpy(dst.ue_id_gran_period_lst, src->ue_id_gran_period_lst, dst.ue_id_gran_period_lst_len * sizeof(ue_id_gran_period_lst_t));

  for (size_t j = 0; j<dst.ue_id_gran_period_lst_len; j++)
  {
    dst.ue_id_gran_period_lst[j] = cp_kpm_ue_id_gran_period_lst(&src->ue_id_gran_period_lst[j]);
  }

  return dst;
}
