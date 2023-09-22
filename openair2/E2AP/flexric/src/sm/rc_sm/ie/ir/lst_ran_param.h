#ifndef LIST_OF_RAN_PARAMETER_H
#define LIST_OF_RAN_PARAMETER_H

#include "ran_param_struct.h"

#include <stdint.h>

typedef struct{

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  //1.. 4294967295

  uint32_t ran_param_id;

  // RAN Parameter Structure
  // Mandatory
  // 9.3.12
  ran_param_struct_t ran_param_struct;

} lst_ran_param_t;

void free_lst_ran_param(lst_ran_param_t* src);

bool eq_lst_ran_param(lst_ran_param_t const* m0, lst_ran_param_t const* m1);

lst_ran_param_t cp_lst_ran_param(lst_ran_param_t const* src);

#endif

