#include <assert.h>

#include "test_info_lst.h"

void free_test_info(test_info_lst_t* src)
{
  assert(src != NULL);

  if (src->test_cond != NULL)
    free(src->test_cond);

  if (src->test_cond_value != NULL)
  {
    // This works but it is a hack
    if (src->int_value != NULL)
      free(src->int_value);
    /*
    if (src->enum_value != NULL)
      free(src->enum_value);

    if (src->bool_value != NULL)
      free(src->bool_value);

    if (src->bit_string_value != NULL)
    {
      free_byte_array(*src->bit_string_value);
      free(src->bit_string_value);
    }

    if (src->octet_string_value != NULL)
    {
      free_byte_array(*src->octet_string_value);
      free(src->octet_string_value);
    }

    if (src->printable_string_value != NULL)
    {
      free_byte_array(*src->printable_string_value);
      free(src->printable_string_value);
    }
    
    if (src->real_value_value != NULL)
      free(src->real_value_value);
*/
    free(src->test_cond_value);
  }


}

bool eq_test_info(const test_info_lst_t *m0, const test_info_lst_t *m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if (m0->test_cond_type != m1->test_cond_type)
    return false;

  switch (m0->test_cond_type)
  {
    case GBR_TEST_COND_TYPE:
      if (m0->GBR != m1->GBR)
        return false;
      break;

    case AMBR_TEST_COND_TYPE:
      if (m0->AMBR != m1->AMBR)
        return false;
      break;

    case IsStat_TEST_COND_TYPE:
      if (m0->IsStat != m1->IsStat)
        return false;
      break;

    case IsCatM_TEST_COND_TYPE:
      if (m0->IsCatM != m1->IsCatM)
        return false;
      break;

    case DL_RSRP_TEST_COND_TYPE:
      if (m0->DL_RSRP != m1->DL_RSRP)
        return false;
      break;

    case DL_RSRQ_TEST_COND_TYPE:
      if (m0->DL_RSRQ != m1->DL_RSRQ)
        return false;
      break;

    case UL_RSRP_TEST_COND_TYPE:
      if (m0->UL_RSRP != m1->UL_RSRP)
        return false;
      break;

    case CQI_TEST_COND_TYPE:
      if (m0->CQI != m1->CQI)
        return false;
      break;

    case fiveQI_TEST_COND_TYPE:
      if (m0->fiveQI != m1->fiveQI)
        return false;
      break;

    case QCI_TEST_COND_TYPE:
      if (m0->QCI != m1->QCI)
        return false;
      break;

    case S_NSSAI_TEST_COND_TYPE:
      if (m0->S_NSSAI != m1->S_NSSAI)
        return false;
      break;

    default:
      assert(false && "Condition type out of the range");
  }


  if (m0->test_cond != NULL || m1->test_cond != NULL) {
    if(m0->test_cond == NULL)
      return false;
    if(m1->test_cond == NULL)
      return false;
    if(*m0->test_cond != *m1->test_cond)
      return false;
  }

  if (m0->test_cond_value != NULL || m1->test_cond_value != NULL){
    if(m0->test_cond_value == NULL)
      return false;
    if(m1->test_cond_value == NULL)
      return false;
    if(*m0->test_cond_value != *m1->test_cond_value)
      return false;

    if(*m0->test_cond_value == INTEGER_TEST_COND_VALUE){
      if(*m0->int_value != *m1->int_value)
        return false;
    } else if(*m0->test_cond_value == ENUMERATED_TEST_COND_VALUE ){
      if(*m0->enum_value != *m1->enum_value)
        return false;

    }else if(*m0->test_cond_value == BOOLEAN_TEST_COND_VALUE ){
      if(*m0->bool_value != *m1->bool_value)
        return false;

    }else if(*m0->test_cond_value ==  BIT_STRING_TEST_COND_VALUE){
      assert(0!=0 && "Not implemented");
      //      if(*m0->int_value != *m1->int_value)
      //        return false;

    }else if(*m0->test_cond_value ==  OCTET_STRING_TEST_COND_VALUE){
      if(eq_byte_array(m0->octet_string_value, m1->octet_string_value) == false)
        return false;

    }else if(*m0->test_cond_value == PRINTABLE_STRING_TEST_COND_VALUE ){
      if(eq_byte_array(m0->printable_string_value, m1->printable_string_value) == false)
        return false;

    } else if(*m0->test_cond_value == REAL_TEST_COND_VALUE ){
      if(*m0->real_value_value != *m1->real_value_value)
        return false;

    } else {
      assert(0 != 0 && "Unknown condition");
    }
  }

  return true;
}

test_info_lst_t cp_kpm_test_info(const test_info_lst_t * src)
{
  assert(src != NULL);

  test_info_lst_t dst = {0};

  dst.test_cond_type = src->test_cond_type;

  switch (dst.test_cond_type)
  {
  case GBR_TEST_COND_TYPE:
    dst.GBR = src->GBR;
    break;

  case AMBR_TEST_COND_TYPE:
    dst.AMBR = src->AMBR;
    break;

  case IsStat_TEST_COND_TYPE:
    dst.IsStat = src->IsStat;
    break;

  case IsCatM_TEST_COND_TYPE:
    dst.IsCatM = src->IsCatM;
    break;

  case DL_RSRP_TEST_COND_TYPE:
    dst.DL_RSRP = src->DL_RSRP;
    break;

  case DL_RSRQ_TEST_COND_TYPE:
    dst.DL_RSRQ = src->DL_RSRQ;
    break;

  case UL_RSRP_TEST_COND_TYPE:
    dst.UL_RSRP = src->UL_RSRP;
    break;

  case CQI_TEST_COND_TYPE:
    dst.CQI = src->CQI;
    break;

  case fiveQI_TEST_COND_TYPE:
    dst.fiveQI = src->fiveQI;
    break;

  case QCI_TEST_COND_TYPE:
    dst.QCI = src->QCI;
    break;

  case S_NSSAI_TEST_COND_TYPE:
    dst.S_NSSAI = src->S_NSSAI;
    break;
  
  default:
    assert(false && "Condition type out of the range");
  }

  if (src->test_cond != NULL){
    dst.test_cond = calloc(1, sizeof(test_cond_e)); 
    assert(dst.test_cond != NULL && "Memory exhausted");
    *dst.test_cond = *src->test_cond; 
  }

  if (src->test_cond_value != NULL){
    dst.test_cond_value = calloc(1, sizeof(test_cond_value_e));
    assert(dst.test_cond_value != NULL && "Memory exhausted");
    *dst.test_cond_value = *src->test_cond_value;
    if(*src->test_cond_value == INTEGER_TEST_COND_VALUE){
      dst.int_value = calloc(1, sizeof(int64_t));
      assert(dst.int_value != NULL && "Memory exhausted");
      *dst.int_value = *src->int_value; 

    } else if(*src->test_cond_value == ENUMERATED_TEST_COND_VALUE ){
      dst.enum_value = calloc(1, sizeof(int64_t));
      assert(dst.enum_value!= NULL && "Memory exhausted");
      *dst.enum_value= *src->enum_value; 

    } else if(*src->test_cond_value ==  BOOLEAN_TEST_COND_VALUE){
      dst.bool_value = calloc(1, sizeof(bool));
      assert(dst.bool_value!= NULL && "Memory exhausted");
      *dst.bool_value = *src->bool_value; 

    } else if(*src->test_cond_value ==  BIT_STRING_TEST_COND_VALUE){
      assert(0 !=0 && "Not implemented");

    } else if(*src->test_cond_value ==  OCTET_STRING_TEST_COND_VALUE){
      dst.octet_string_value = calloc(1, sizeof(byte_array_t));
      assert(dst.octet_string_value != NULL && "Memory exhausted");
      *dst.octet_string_value = copy_byte_array(*src->octet_string_value); 

    } else if(*src->test_cond_value ==  PRINTABLE_STRING_TEST_COND_VALUE){
      dst.printable_string_value = calloc(1, sizeof(byte_array_t));
      assert(dst.printable_string_value != NULL && "Memory exhausted");
      *dst.printable_string_value = copy_byte_array(*src->printable_string_value); 

    } else if(*src->test_cond_value == REAL_TEST_COND_VALUE){
      dst.real_value_value = calloc(1, sizeof(double));
      assert(dst.real_value_value!= NULL && "Memory exhausted");
      *dst.real_value_value = *src->real_value_value; 

    } else {
      assert(0 !=0 && "Unknown condition");
    }
  }
  return dst;
}

