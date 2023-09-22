#include <assert.h>

#include "dec_test_info.h"

#include "../../ie/asn/TestCond-Value.h"

static inline
byte_array_t copy_ostring_to_ba(OCTET_STRING_t src)
{
  byte_array_t dst = { .len = src.size}; 
  dst.buf = malloc(src.size);
  memcpy(dst.buf, src.buf, src.size);
  return dst;
}

test_info_lst_t kpm_dec_test_info_asn(const TestCondInfo_t * test_info_asn)
{
  test_info_lst_t test_info = {0};

  switch (test_info_asn->testType.present)
  {
    case TestCond_Type_PR_gBR:
      test_info.test_cond_type = GBR_TEST_COND_TYPE;
      test_info.GBR = test_info_asn->testType.choice.gBR;
      break;

    case TestCond_Type_PR_aMBR:
      test_info.test_cond_type = AMBR_TEST_COND_TYPE;
      test_info.AMBR = test_info_asn->testType.choice.aMBR;
      break;

    case TestCond_Type_PR_isStat:
      test_info.test_cond_type = IsStat_TEST_COND_TYPE;
      test_info.IsStat = test_info_asn->testType.choice.isStat;
      break;

    case TestCond_Type_PR_isCatM:
      test_info.test_cond_type = IsCatM_TEST_COND_TYPE;
      test_info.IsCatM = test_info_asn->testType.choice.isCatM;
      break;

    case TestCond_Type_PR_rSRP:
      test_info.test_cond_type = DL_RSRP_TEST_COND_TYPE;
      test_info.DL_RSRP = test_info_asn->testType.choice.rSRP;
      break;

    case TestCond_Type_PR_rSRQ:
      test_info.test_cond_type = DL_RSRQ_TEST_COND_TYPE;
      test_info.DL_RSRQ = test_info_asn->testType.choice.rSRQ;
      break;

    case TestCond_Type_PR_ul_rSRP:
      test_info.test_cond_type = UL_RSRP_TEST_COND_TYPE;
      test_info.UL_RSRP = test_info_asn->testType.choice.ul_rSRP;
      break;

    case TestCond_Type_PR_cQI:
      test_info.test_cond_type = CQI_TEST_COND_TYPE;
      test_info.CQI = test_info_asn->testType.choice.cQI;
      break;

    case TestCond_Type_PR_fiveQI:
      test_info.test_cond_type = fiveQI_TEST_COND_TYPE;
      test_info.fiveQI = test_info_asn->testType.choice.fiveQI;
      break;

    case TestCond_Type_PR_qCI:
      test_info.test_cond_type = QCI_TEST_COND_TYPE;
      test_info.QCI = test_info_asn->testType.choice.qCI;
      break;

    case TestCond_Type_PR_sNSSAI:
      test_info.test_cond_type = S_NSSAI_TEST_COND_TYPE;
      test_info.S_NSSAI = test_info_asn->testType.choice.sNSSAI;
      break;

    default:
      assert(false && "Condition type out of the range");
      break;
  }

  if (test_info_asn->testExpr != NULL) {
    test_info.test_cond = calloc(1, sizeof(test_cond_e));
    assert(test_info.test_cond != NULL && "Memory exhausted");

    if(*test_info_asn->testExpr == TestCond_Expression_equal){
      *test_info.test_cond = EQUAL_TEST_COND;
    } else if(*test_info_asn->testExpr == TestCond_Expression_greaterthan){
      *test_info.test_cond = GREATERTHAN_TEST_COND;
    } else if(*test_info_asn->testExpr == TestCond_Expression_lessthan){
      *test_info.test_cond = LESSTHAN_TEST_COND;
    } else if(*test_info_asn->testExpr == TestCond_Expression_contains){
      *test_info.test_cond = CONTAINS_TEST_COND;
    } else if(*test_info_asn->testExpr ==TestCond_Expression_present){
      *test_info.test_cond = PRESENT_TEST_COND;
    } else {
      assert(0 != 0 && "Unknown test condition enum type");
    }
  }

  if (test_info_asn->testValue != NULL) {

    test_info.test_cond_value = calloc(1, sizeof(test_cond_value_e));
    assert(test_info.test_cond_value != NULL && "Memory exhausted");

    if(test_info_asn->testValue->present == TestCond_Value_PR_valueInt){
      *test_info.test_cond_value = INTEGER_TEST_COND_VALUE;
      test_info.int_value = calloc(1, sizeof(int64_t));
      assert(test_info.int_value != NULL && "Memory exhausted");
      *test_info.int_value = test_info_asn->testValue->choice.valueInt; 
    } else if(test_info_asn->testValue->present == TestCond_Value_PR_valueEnum ){
      *test_info.test_cond_value = ENUMERATED_TEST_COND_VALUE;
      test_info.enum_value = calloc(1, sizeof(int64_t));
      assert(test_info.enum_value != NULL && "Memory exhausted");
      *test_info.enum_value = test_info_asn->testValue->choice.valueEnum; 

    } else if(test_info_asn->testValue->present ==  TestCond_Value_PR_valueBool){
      *test_info.test_cond_value = BOOLEAN_TEST_COND_VALUE;
      test_info.bool_value = calloc(1, sizeof(bool));
      assert(test_info.bool_value != NULL && "Memory exhausted");
      *test_info.bool_value = test_info_asn->testValue->choice.valueBool; 

    } else if(test_info_asn->testValue->present ==  TestCond_Value_PR_valueBitS ){
      *test_info.test_cond_value = BIT_STRING_TEST_COND_VALUE;
      assert(0 != 0 && "Not implemented");
      //test_info.= calloc(1, sizeof());
      //assert(test_info.!= NULL && "Memory exhausted");
      //*test_info.= test_info_asn->testValue->choice.; 
    } else if(test_info_asn->testValue->present ==  TestCond_Value_PR_valueOctS){
      *test_info.test_cond_value = OCTET_STRING_TEST_COND_VALUE;
      test_info.octet_string_value = calloc(1, sizeof(byte_array_t));
      assert(test_info.octet_string_value != NULL && "Memory exhausted");
      *test_info.octet_string_value = copy_ostring_to_ba(test_info_asn->testValue->choice.valueOctS); 

    } else if(test_info_asn->testValue->present == TestCond_Value_PR_valuePrtS){
      *test_info.test_cond_value = PRINTABLE_STRING_TEST_COND_VALUE;
      test_info.printable_string_value = calloc(1, sizeof(byte_array_t));
      assert(test_info.printable_string_value != NULL && "Memory exhausted");
      *test_info.printable_string_value =  copy_ostring_to_ba(test_info_asn->testValue->choice.valuePrtS); 

    } else if(test_info_asn->testValue->present ==  TestCond_Value_PR_valueReal){
      *test_info.test_cond_value = REAL_TEST_COND_VALUE;
      test_info.real_value_value = calloc(1, sizeof(double));
      assert(test_info.real_value_value != NULL && "Memory exhausted");
      *test_info.real_value_value = test_info_asn->testValue->choice.valueReal; 

    } else {
      assert(0!=0 && "Unknown type");
    } 

  }

  return test_info;
}


