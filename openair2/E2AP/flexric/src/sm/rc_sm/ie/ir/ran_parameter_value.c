#include "ran_parameter_value.h"

#include <assert.h>
#include <math.h>

void free_ran_parameter_value(ran_parameter_value_t* src)
{
  assert(src != NULL);

  if(src->type ==  BOOLEAN_RAN_PARAMETER_VALUE){

  } else if(src->type == INTEGER_RAN_PARAMETER_VALUE ){

  } else if(src->type == REAL_RAN_PARAMETER_VALUE ){

  }else if(src->type == BIT_STRING_RAN_PARAMETER_VALUE ){
    free_byte_array(src->bit_str_ran);
  }else if(src->type ==  OCTET_STRING_RAN_PARAMETER_VALUE){
    free_byte_array(src->octet_str_ran );
  }else if(src->type == PRINTABLESTRING_RAN_PARAMETER_VALUE){
    free_byte_array(src->printable_str_ran);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

bool eq_ran_parameter_value(ran_parameter_value_t const* m0, ran_parameter_value_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false; 


  if(m0->type != m1->type)
    return false;

  if(m0->type == BOOLEAN_RAN_PARAMETER_VALUE){
    if(m0->bool_ran != m1->bool_ran)
      return false;

  }else if(m0->type == INTEGER_RAN_PARAMETER_VALUE){
    if(m0->int_ran != m1->int_ran)
      return false;

  }else if(m0->type == REAL_RAN_PARAMETER_VALUE){
    assert(!isnan(m0->real_ran)); // && !isinf(m0->real_ran));
    assert(!isnan(m1->real_ran)); // && !isinf(m1->real_ran));

    if(m0->real_ran != m1->real_ran) // equality here is right, since we are just transporting bits. 
      return false;

  }else if(m0->type == BIT_STRING_RAN_PARAMETER_VALUE){
    if(eq_byte_array(&m0->bit_str_ran, &m1->bit_str_ran) == false)
      return false;

  }else if(m0->type == OCTET_STRING_RAN_PARAMETER_VALUE){
    if(eq_byte_array(&m0->octet_str_ran, &m1->octet_str_ran) == false)
      return false;

  }else if(m0->type == PRINTABLESTRING_RAN_PARAMETER_VALUE){
    if(eq_byte_array(&m0->printable_str_ran, &m1->printable_str_ran) == false)
      return false;

  } else {
    assert(0 != 0 && "Unknown type");
  }

  return true;
}


ran_parameter_value_t cp_ran_parameter_value(ran_parameter_value_t const* src)
{
  assert(src != NULL);

  ran_parameter_value_t dst = {.type = src->type};

  if(dst.type == BOOLEAN_RAN_PARAMETER_VALUE ){
    dst.bool_ran = src->bool_ran;
  } else if(dst.type == INTEGER_RAN_PARAMETER_VALUE){
    dst.int_ran = src->int_ran;
  } else if(dst.type == REAL_RAN_PARAMETER_VALUE){
    dst.real_ran = src->real_ran;
  } else if(dst.type == BIT_STRING_RAN_PARAMETER_VALUE){
    dst.bit_str_ran = copy_byte_array(src->bit_str_ran);
  } else if(dst.type == OCTET_STRING_RAN_PARAMETER_VALUE){
    dst.octet_str_ran = copy_byte_array(src->octet_str_ran);
  } else if(dst.type == PRINTABLESTRING_RAN_PARAMETER_VALUE){
    dst.printable_str_ran = copy_byte_array(src->printable_str_ran);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

