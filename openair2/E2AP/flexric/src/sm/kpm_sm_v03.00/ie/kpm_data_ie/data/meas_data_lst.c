#include <assert.h>
#include <math.h>

#include "meas_data_lst.h"


meas_record_lst_t cp_meas_record_lst(meas_record_lst_t const* src)
{
  assert(src != NULL);
  meas_record_lst_t dst = {.value = src->value};

  if(src->value == INTEGER_MEAS_VALUE){
    dst.int_val = src->int_val;
  } else if(src->value == REAL_MEAS_VALUE){
    dst.real_val = src->real_val;
  } else if(src->value == NO_VALUE_MEAS_VALUE){
    assert(src->no_value == NULL);
    dst.no_value = NULL;
  } else {
    assert(0!=0 && "Unknown value");
  }

  return dst;
}

bool eq_meas_data_lst(meas_data_lst_t const* m0, meas_data_lst_t const* m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (m0->meas_record_len != m1->meas_record_len)
      return false;
    for (size_t j = 0; j < m0->meas_record_len; ++j)
    {
      
      if (m0->meas_record_lst[j].value != m1->meas_record_lst[j].value)
        return false;

      switch (m0->meas_record_lst[j].value)
      {
        case INTEGER_MEAS_VALUE:
          if (m0->meas_record_lst[j].int_val != m1->meas_record_lst[j].int_val)
            return false;
          break;

        case REAL_MEAS_VALUE:
          if (fabs(m0->meas_record_lst[j].real_val - m1->meas_record_lst[j].real_val) > 0.0001f)
            return false;
          break;

        case NO_VALUE_MEAS_VALUE:
          if (m0->meas_record_lst[j].no_value != m1->meas_record_lst[j].no_value)
            return false;
          break;

        default:
          assert(false && "Unknown Measurement Value");
      }

      
    }


    if ((m0->incomplete_flag != NULL || m1->incomplete_flag != NULL) && *m0->incomplete_flag != *m1->incomplete_flag)
      return false;


    return true;
}

meas_data_lst_t cp_meas_data_lst(meas_data_lst_t const* src)
{
  assert(src != NULL);
  meas_data_lst_t dst = {0}; 

  // [1, 65535]
  assert(src->meas_record_len > 0 && src->meas_record_len <  65535);
  dst.meas_record_len = src->meas_record_len;	
  dst.meas_record_lst = calloc(dst.meas_record_len, sizeof(meas_record_lst_t));
  assert(dst.meas_record_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_record_len; ++i){
   dst.meas_record_lst[i] = cp_meas_record_lst(&src->meas_record_lst[i]);   
  }

  if(src->incomplete_flag != NULL){
    dst.incomplete_flag = calloc(1, sizeof(enum_value_e));
    assert(dst.incomplete_flag != NULL && "Memory exhausted");
    *dst.incomplete_flag = *src->incomplete_flag;
  }

  return dst;
}


