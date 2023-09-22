#include "ran_parameter_value_type.h"

#include "ran_parameter_value.h"
#include "ran_param_struct.h"
#include "ran_param_list.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_val_type(ran_param_val_type_t *src)
{
  assert(src != NULL);

  if(src->type == ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE){
    free_ran_parameter_value(src->flag_true);
    free(src->flag_true);
  } else if(src->type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE ){
    free_ran_parameter_value(src->flag_false);
    free(src->flag_false);
  } else if(src->type == STRUCTURE_RAN_PARAMETER_VAL_TYPE ){
    free_ran_param_struct(src->strct);
    free(src->strct);
  } else if(src->type == LIST_RAN_PARAMETER_VAL_TYPE ){
    free_ran_param_list(src->lst);
    free(src->lst);
  } else {
    assert(0 != 0 && "Not implemented");
  }

}

bool eq_ran_param_val_type(ran_param_val_type_t const* m0, ran_param_val_type_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->type != m1->type)
    return false;

  if(m0->type == ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE){
    if(eq_ran_parameter_value(m0->flag_true, m1->flag_true) == false)
      return false;
  } else if(m0->type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE ){
    if(eq_ran_parameter_value(m0->flag_false, m1->flag_false) == false)
      return false;
  }else if(m0->type == STRUCTURE_RAN_PARAMETER_VAL_TYPE){
    if(eq_ran_param_struct(m0->strct, m1->strct) == false)
      return false;
  }else if(m0->type == LIST_RAN_PARAMETER_VAL_TYPE){
    if(eq_ran_param_list(m0->lst, m1->lst) == false)
      return false;
  } else {
    assert(0!=0 && "Unknown type");
  }

  return true;
}

ran_param_val_type_t cp_ran_param_val_type(ran_param_val_type_t const* src)
{
  assert(src != NULL);
  ran_param_val_type_t dst = {0};

  dst.type = src->type;
 
  if(dst.type == ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE){
    dst.flag_true = calloc(1, sizeof(ran_parameter_value_t ));
    assert(dst.flag_true != NULL && "Memory exhausted");
    *dst.flag_true = cp_ran_parameter_value(src->flag_true);
  } else if(dst.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE){
    dst.flag_false = calloc(1, sizeof(ran_parameter_value_t ));
    assert(dst.flag_false != NULL && "Memory exhausted");
    *dst.flag_false = cp_ran_parameter_value(src->flag_false);
  }else if(dst.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE){
    dst.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(dst.strct != NULL && "Memory exhausted");
    *dst.strct = cp_ran_param_struct(src->strct);
  }else if(dst.type == LIST_RAN_PARAMETER_VAL_TYPE){
    dst.lst = calloc(1, sizeof(ran_param_list_t));
    assert(dst.lst != NULL && "Memory exhausted");
    *dst.lst = cp_ran_param_list(src->lst);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}


