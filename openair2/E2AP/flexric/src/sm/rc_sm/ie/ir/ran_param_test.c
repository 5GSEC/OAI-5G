#include "ran_param_test.h"

#include <assert.h>

void free_ran_param_test(ran_param_test_t * src)
{
  assert(src != NULL);

  //RAN Parameter ID
  //Mandatory
  //9.3.9
 // PrintableString(SIZE(1.. 150, ...))
//  free_byte_array(src->ran_param_id); 

  if(src->type == LIST_RAN_PARAMETER_TYPE ){
    free_ran_param_test_lst(&src->lst);

  } else if(src->type == STRUCTURE_RAN_PARAMETER_TYPE ){
    free_ran_param_test_strct(&src->strct);

  }else if(src->type == ELEMENT_WITH_KEY_FLAG_TRUE_RAN_PARAMETER_TYPE ){
   free_ran_parameter_value(&src->flag_true);

  }else if(src->type == ELEMENT_WITH_KEY_FLAG_FALSE_RAN_PARAMETER_TYPE){
    free_ran_param_elm_key_false(&src->flag_false);

  } else {
    assert(0!=0 && "Unknown type");
  }

}

bool eq_ran_param_test(ran_param_test_t const* m0, ran_param_test_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  //RAN Parameter ID
  //Mandatory
  //Bug in the standard. 9.3.9 wrote in section 9. 
  //9.3.8 in ASN language. Probably 9.3.8 intended
  //1.. 4294967295
  if(m0->ran_param_id != m1->ran_param_id) 
    return false;

  if(m0->type != m1->type)
    return false;

  if(m0->type == LIST_RAN_PARAMETER_TYPE){
    if(eq_ran_param_test_lst(&m0->lst, &m1->lst) == false ){
      assert(0!=0);
      return false;
    }
  } else if(m0->type == STRUCTURE_RAN_PARAMETER_TYPE){
    if(eq_ran_param_test_strct(&m0->strct, &m1->strct) == false){
      assert(0!=0);
      return false;
    }

  }else if(m0->type == ELEMENT_WITH_KEY_FLAG_TRUE_RAN_PARAMETER_TYPE ){
    if(eq_ran_parameter_value(&m0->flag_true, &m1->flag_true ) == false ){
      assert(0!=0);
      return false;
    }

  }else if(m0->type ==  ELEMENT_WITH_KEY_FLAG_FALSE_RAN_PARAMETER_TYPE){
    if(eq_ran_param_elm_key_false(&m0->flag_false, &m1->flag_false) == false ){
      assert(0!=0);
      return false;
    }

  } else {
    assert(0!=0 && "Unknown type");
  }

  return true;
}

ran_param_test_t cp_ran_param_test(ran_param_test_t const* src)
{
  assert(src != NULL);

  ran_param_test_t dst = {0};
  //RAN Parameter ID
  //Mandatory
  //Bug in the standard. 9.3.9 wrote in section 9. 
  //9.3.8 in ASN language. Probably 9.3.8 intended
  //1.. 4294967295
  assert(src->ran_param_id > 0);
  dst.ran_param_id = src->ran_param_id;

  dst.type = src->type;
  if(dst.type == LIST_RAN_PARAMETER_TYPE){
    dst.lst = cp_ran_param_test_lst(&src->lst);
  } else if(dst.type == STRUCTURE_RAN_PARAMETER_TYPE){
    dst.strct = cp_ran_param_test_strct(&src->strct);  
  } else if(dst.type == ELEMENT_WITH_KEY_FLAG_TRUE_RAN_PARAMETER_TYPE){
    dst.flag_true = cp_ran_parameter_value(&src->flag_true);  
  } else if(dst.type == ELEMENT_WITH_KEY_FLAG_FALSE_RAN_PARAMETER_TYPE){
    dst.flag_false = cp_ran_param_elm_key_false(&src->flag_false);
  } else {
    assert(0 != 0 && "Unknown type");
  }

  return dst;
}

