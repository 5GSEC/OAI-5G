#include <assert.h>

#include "meas_info_frm_1_lst.h"

void free_meas_info_frm_1(meas_info_format_1_lst_t* src)
{
    assert(src != NULL);

    for (size_t i = 0; i<src->label_info_lst_len; i++)
      free_label_info(&src->label_info_lst[i]);
    free(src->label_info_lst);
        
    if (src->meas_type.type == NAME_MEAS_TYPE)
      free_byte_array(src->meas_type.name);

}

bool eq_meas_info_frm_1(meas_info_format_1_lst_t const * m0, meas_info_format_1_lst_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    // Meas Type

    if (eq_meas_type(&m0->meas_type, &m1->meas_type) != true)
        return false;


    // Label Info

    if (m0->label_info_lst_len != m1->label_info_lst_len)
      return false;
    
    for (size_t i = 0; i < m0->label_info_lst_len; i++)
    {
      if (eq_label_info(&m0->label_info_lst[i], &m1->label_info_lst[i]) != true)
        return false;
    }

    return true;
}

meas_info_format_1_lst_t cp_meas_info_format_1_lst(meas_info_format_1_lst_t const* src)
{
  assert(src != NULL);
  meas_info_format_1_lst_t dst = {0}; 

  dst.meas_type = cp_meas_type(&src->meas_type);

  assert(src->label_info_lst_len > 0);

  // [1, 2147483647]
  dst.label_info_lst_len = src->label_info_lst_len; 
  dst.label_info_lst = calloc(dst.label_info_lst_len, sizeof(label_info_lst_t));
  assert(dst.label_info_lst != NULL && "Memory exhausted");

  // 8.3.11
  for(size_t i = 0; i < dst.label_info_lst_len; ++i){
    dst.label_info_lst[i] = cp_label_info(&src->label_info_lst[i]);
  }

  return dst;
}

