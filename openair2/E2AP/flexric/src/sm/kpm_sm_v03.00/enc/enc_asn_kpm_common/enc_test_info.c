#include <assert.h>

#include "enc_test_info.h"

#include "../../ie/asn/TestCond-Value.h"

static inline
OCTET_STRING_t copy_ba_to_ostring(byte_array_t ba)
{
  OCTET_STRING_t os = { .size = ba.len }; 
  os.buf = malloc(ba.len);
  memcpy(os.buf, ba.buf, ba.len);
  return os;
}

TestCondInfo_t kpm_enc_test_info_asn(const test_info_lst_t * test_info)
{
  TestCondInfo_t test_info_asn = {0};

  switch (test_info->test_cond_type)
  {
    case GBR_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_gBR;
      test_info_asn.testType.choice.gBR = test_info->GBR;
      break;

    case AMBR_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_aMBR;
      test_info_asn.testType.choice.aMBR = test_info->AMBR;
      break;

    case IsStat_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_isStat;
      test_info_asn.testType.choice.isStat = test_info->IsStat;
      break;

    case IsCatM_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_isCatM;
      test_info_asn.testType.choice.isCatM = test_info->IsCatM;
      break;

    case DL_RSRP_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_rSRP;
      test_info_asn.testType.choice.rSRP = test_info->DL_RSRP;
      break;

    case DL_RSRQ_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_rSRQ;
      test_info_asn.testType.choice.rSRQ = test_info->DL_RSRQ;
      break;

    case UL_RSRP_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_ul_rSRP;
      test_info_asn.testType.choice.ul_rSRP = test_info->UL_RSRP;
      break;

    case CQI_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_cQI;
      test_info_asn.testType.choice.cQI = test_info->CQI;
      break;

    case fiveQI_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_fiveQI;
      test_info_asn.testType.choice.fiveQI = test_info->fiveQI;
      break;

    case QCI_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_qCI;
      test_info_asn.testType.choice.qCI = test_info->QCI;
      break;

    case S_NSSAI_TEST_COND_TYPE:
      test_info_asn.testType.present = TestCond_Type_PR_sNSSAI;
      test_info_asn.testType.choice.sNSSAI = test_info->S_NSSAI;
      break;

    default:
      assert(false && "Condition type out of the range");
      break;

  }

  // Optional
  if (test_info->test_cond != NULL){
    test_info_asn.testExpr = calloc(1, sizeof(TestCond_Expression_t));
    assert(test_info_asn.testExpr != NULL && "Memory exhausted");

    if(*test_info->test_cond == EQUAL_TEST_COND){
      *test_info_asn.testExpr = TestCond_Expression_equal; 
    } else if(*test_info->test_cond == GREATERTHAN_TEST_COND){
      *test_info_asn.testExpr =TestCond_Expression_greaterthan; 
    } else if(*test_info->test_cond == LESSTHAN_TEST_COND){
      *test_info_asn.testExpr =TestCond_Expression_lessthan; 
    } else if(*test_info->test_cond == CONTAINS_TEST_COND){
      *test_info_asn.testExpr =TestCond_Expression_contains; 
    } else if(*test_info->test_cond == PRESENT_TEST_COND){
      *test_info_asn.testExpr =TestCond_Expression_present; 
    } else {
      assert(0 != 0 && "Unknown test condition enum type");
    }
  }

  if (test_info->test_cond_value != NULL){
    test_info_asn.testValue = calloc(1, sizeof(TestCond_Value_t));
    assert(test_info_asn.testValue != NULL && "Memory exhausted");

    if(*test_info->test_cond_value == INTEGER_TEST_COND_VALUE){
      test_info_asn.testValue->present = TestCond_Value_PR_valueInt;
      test_info_asn.testValue->choice.valueInt = *test_info->int_value;
    } else if(*test_info->test_cond_value == ENUMERATED_TEST_COND_VALUE){
      test_info_asn.testValue->present =TestCond_Value_PR_valueEnum;
      test_info_asn.testValue->choice.valueEnum = *test_info->enum_value;
    }else if(*test_info->test_cond_value == BOOLEAN_TEST_COND_VALUE){
      test_info_asn.testValue->present =TestCond_Value_PR_valueBool;
      test_info_asn.testValue->choice.valueBool = *test_info->bool_value;
    }else if(*test_info->test_cond_value == BIT_STRING_TEST_COND_VALUE){
      test_info_asn.testValue->present = TestCond_Value_PR_valueBitS;
      assert(0 != 0 && "Bit string not implemented");
      //      test_info_asn.testValue->choice.valueBitS = copy_ba_to_ostring(*test_info->bit_string_value);
    }else if(*test_info->test_cond_value == OCTET_STRING_TEST_COND_VALUE){
      test_info_asn.testValue->present =TestCond_Value_PR_valueOctS;
      test_info_asn.testValue->choice.valueOctS = copy_ba_to_ostring(*test_info->octet_string_value);
    }else if(*test_info->test_cond_value == PRINTABLE_STRING_TEST_COND_VALUE){
      test_info_asn.testValue->present =TestCond_Value_PR_valuePrtS;
      test_info_asn.testValue->choice.valuePrtS = copy_ba_to_ostring(*test_info->printable_string_value);
    }else if(*test_info->test_cond_value == REAL_TEST_COND_VALUE){
      test_info_asn.testValue->present = TestCond_Value_PR_valueReal ;
      test_info_asn.testValue->choice.valueReal = *test_info->real_value_value;
    } else {
      assert(0 != 0 && "Unknown value");
    }

  }

  return test_info_asn;
}
