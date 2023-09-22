#include <assert.h>

#include "matching_cond_frm_3_lst.h"

void free_matching_cond_frm_3(matching_condition_format_3_lst_t* src)
{
  assert(src != NULL);

  // Logical OR

  if (src->logical_OR != NULL)
    free(src->logical_OR);


  // Matching Condition Type
  switch (src->cond_type)
  {
  case LABEL_INFO:
    free_label_info(&src->label_info_lst);
    break;

  case TEST_INFO:
    free_test_info(&src->test_info_lst);
    break;
  
  default:
    assert(false && "Unknown Matching Condition Type");
  }
}

bool eq_matching_cond_frm_3(matching_condition_format_3_lst_t const * m0, matching_condition_format_3_lst_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Matching Condition Type
  if (m0->cond_type != m1->cond_type){
    assert(0!=0 && "For debugging purposes");
    return false;
  }

  switch (m0->cond_type)
  {
  case LABEL_INFO:
    if (eq_label_info(&m0->label_info_lst, &m1->label_info_lst) != true){
    assert(0!=0 && "For debugging purposes");
      return false;
    }
    break;

  case TEST_INFO:
    if (eq_test_info(&m0->test_info_lst, &m1->test_info_lst) != true){
    assert(0!=0 && "For debugging purposes");
      return false;
    }
    break;
  
  default:
    assert(false && "Unknown Matching Condition Type");
  }


  // Logical OR
  if ((m0->logical_OR != NULL || m1->logical_OR != NULL) && *m0->logical_OR != *m1->logical_OR){
    assert(0!=0 && "For debugging purposes");
    return false;
  }

  return true;
}


matching_condition_format_3_lst_t cp_kpm_matching_cond_frm_3(matching_condition_format_3_lst_t const* src)
{
  assert(src != NULL);

  matching_condition_format_3_lst_t dst = {0};

  dst.cond_type = src->cond_type;

  switch (dst.cond_type)
  {
  case LABEL_INFO:
    dst.label_info_lst = cp_label_info(&src->label_info_lst);
    break;

  case TEST_INFO:
    dst.test_info_lst = cp_kpm_test_info(&src->test_info_lst);
    break;
  
  default:
    assert(false && "Unknown Matching Condition Type");
  }


  // Logical OR
  if (src->logical_OR != NULL) {
    dst.logical_OR = malloc (sizeof(dst.logical_OR));
    *dst.logical_OR = *src->logical_OR; 
  }

  return dst;
}

