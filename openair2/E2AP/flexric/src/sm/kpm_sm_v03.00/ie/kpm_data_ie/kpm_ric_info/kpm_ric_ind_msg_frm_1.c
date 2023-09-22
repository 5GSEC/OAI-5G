#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "kpm_ric_ind_msg_frm_1.h"

// Equality implemented as euristics for the moment. This function is used in test framework.
#define OPTIONAL_CHECK_EQUAL_SIMPLE_TYPE(T0,T1) \
  do { \
    if ((T0) == NULL && (T1) != NULL) \
      return false; \
    if ((T0) != NULL && (T1) == NULL) \
      return false; \
    if ((T0) != NULL && (T1) != NULL) \
      if (*(T0) != *(T1)) \
        return false; \
  } while (0)


void free_kpm_ind_msg_frm_1(kpm_ind_msg_format_1_t* src) 
{
  assert(src != NULL);

  for (size_t i = 0; i<src->meas_data_lst_len; i++)
  {
    free(src->meas_data_lst[i].meas_record_lst);

    if (src->meas_data_lst[i].incomplete_flag != NULL)
      free(src->meas_data_lst[i].incomplete_flag);
  }
  free(src->meas_data_lst);
  

  /*OPTIONAL fields below*/
  if (src->meas_info_lst != NULL){
    for (size_t i=0; i<src->meas_info_lst_len; i++)
    {
      if (src->meas_info_lst[i].meas_type.type ==  NAME_MEAS_TYPE)
        free_byte_array(src->meas_info_lst[i].meas_type.name);
      for (size_t j = 0; j< src->meas_info_lst[i].label_info_lst_len; j++)
        free_label_info(&src->meas_info_lst[i].label_info_lst[j]);
      free(src->meas_info_lst[i].label_info_lst);
    }
    free(src->meas_info_lst);
  }

  if (src->gran_period_ms != NULL)
    free(src->gran_period_ms);
}


bool eq_kpm_ind_msg_frm_1(kpm_ind_msg_format_1_t const* m0, kpm_ind_msg_format_1_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Measurement Data
  if (m0->meas_data_lst_len != m1->meas_data_lst_len)
    return false;

  for (size_t i = 0; i < m0->meas_data_lst_len; ++i)
  {
    if (eq_meas_data_lst(&m0->meas_data_lst[i], &m0->meas_data_lst[i]) != true){
      assert(0!=0);
      return false;
    }
  }


  // Measurement Information
  if (m0->meas_info_lst_len != m1->meas_info_lst_len){
      assert(0!=0);
    return false;
  }
  for (size_t i = 0; i < m0->meas_info_lst_len; ++i)
  {
    // Meas Type
    if (eq_meas_type(&m0->meas_info_lst[i].meas_type, &m1->meas_info_lst[i].meas_type) != true){
      assert(0!=0);
      return false;
    }

    // Label Info

    if (m0->meas_info_lst[i].label_info_lst_len != m1->meas_info_lst[i].label_info_lst_len){
      assert(0!=0);
      return false;
    }
    
    for (size_t j = 0; j < m0->meas_info_lst[i].label_info_lst_len; ++j)
    {
      if (eq_label_info(&m0->meas_info_lst[i].label_info_lst[j], &m1->meas_info_lst[i].label_info_lst[j]) != true){
      assert(0!=0);
        return false;
    }
    }
  }

  // Granularity Period
  if ((m0->gran_period_ms != NULL || m1->gran_period_ms != NULL) && *m0->gran_period_ms != *m1->gran_period_ms){
      assert(0!=0);
    return false;
  }

  return true;
}


kpm_ind_msg_format_1_t cp_kpm_ind_msg_frm_1(kpm_ind_msg_format_1_t const* src) 
{
  assert(src != NULL);
  kpm_ind_msg_format_1_t dst = {0};

  assert(src->meas_data_lst_len > 0 && src->meas_data_lst_len < 65536);
  dst.meas_data_lst_len = src->meas_data_lst_len;
  dst.meas_data_lst = calloc(dst.meas_data_lst_len, sizeof(meas_data_lst_t));
  assert(dst.meas_data_lst != NULL && "Memory exhausted");
  for(size_t i = 0; i < dst.meas_data_lst_len; ++i){
    dst.meas_data_lst[i] = cp_meas_data_lst(&src->meas_data_lst[i]);          
  }

  // [0, 65535]
  if(src->meas_info_lst_len > 0){
    assert(src->meas_info_lst_len < 65536);
    dst.meas_info_lst_len = src->meas_info_lst_len;  
    dst.meas_info_lst = calloc(dst.meas_info_lst_len, sizeof(meas_info_format_1_lst_t  ));
    assert(dst.meas_info_lst != NULL && "Memory exhausted");
    for(size_t i = 0; i < dst.meas_info_lst_len; ++i){
      dst.meas_info_lst[i] = cp_meas_info_format_1_lst(&src->meas_info_lst[i]); 
    }
  }

  //uint32_t *gran_period_ms;  // 8.3.8  -  OPTIONAL
  if (src->gran_period_ms) {
    dst.gran_period_ms = malloc(sizeof(dst.gran_period_ms));
    assert(dst.gran_period_ms != NULL && "Memory exhausted");
    *dst.gran_period_ms = *src->gran_period_ms; 
  }

  return dst;
  /*
  if (src->meas_data_lst_len)
  {
    ret.meas_data_lst_len = src->meas_data_lst_len;

    ret.meas_data_lst = calloc(src->meas_data_lst_len, sizeof(meas_data_lst_t));
    assert(ret.meas_data_lst != NULL && "Memory exhausted");
   
    for(size_t i = 0; i < src->meas_data_lst_len; ++i){
     ret.meas_data_lst[i] = ;  
    }

    memcpy (ret.meas_data_lst, src->meas_data_lst, src->meas_data_lst_len * sizeof(meas_data_lst_t));
    
    for (size_t i = 0; i<ret.meas_data_lst_len; i++)
    {
      ret.meas_data_lst[i].meas_record_len = src->meas_data_lst[i].meas_record_len;
      ret.meas_data_lst[i].meas_record_lst = calloc(src->meas_data_lst[i].meas_record_len, sizeof(meas_record_lst_t));
      memcpy (ret.meas_data_lst[i].meas_record_lst, src->meas_data_lst[i].meas_record_lst, src->meas_data_lst[i].meas_record_len * sizeof(meas_record_lst_t));
    }
  }

  if (src->meas_info_lst_len)
  {
    ret.meas_info_lst_len = src->meas_info_lst_len;
    
    ret.meas_info_lst = calloc(src->meas_info_lst_len, sizeof(meas_info_format_1_lst_t));
    memcpy (ret.meas_info_lst, src->meas_info_lst, src->meas_info_lst_len * sizeof(meas_info_format_1_lst_t));
    
    
    for (size_t i = 0; i<ret.meas_info_lst_len; i++)
    {
      // Meas Type
      ret.meas_info_lst[i].meas_type = cp_meas_type(&src->meas_info_lst[i].meas_type);
      
      // Label Information
      ret.meas_info_lst[i].label_info_lst_len = src->meas_info_lst[i].label_info_lst_len;
      
      ret.meas_info_lst[i].label_info_lst = calloc(src->meas_info_lst[i].label_info_lst_len, sizeof(label_info_lst_t));
      memcpy (ret.meas_info_lst[i].label_info_lst, src->meas_info_lst[i].label_info_lst, src->meas_info_lst[i].label_info_lst_len * sizeof(label_info_lst_t));
      
      
      for (size_t j = 0; j < src->meas_info_lst[i].label_info_lst_len; j++)
      {
        cp_label_info(&ret.meas_info_lst[i].label_info_lst[j], &src->meas_info_lst[i].label_info_lst[j]);  
      }
    }
  }
  
  return ret;
  */
}
