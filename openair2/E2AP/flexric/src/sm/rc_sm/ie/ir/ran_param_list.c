#include "ran_param_list.h"
#include "ran_param_test.h"

#include <assert.h>

void free_ran_param_list(ran_param_list_t* src)
{
  assert(src != NULL);

  // [1- 65535]
  assert(src->sz_lst_ran_param  > 0 && src->sz_lst_ran_param  < 65536);

  for(size_t i  = 0; i < src->sz_lst_ran_param; ++i){
    free_lst_ran_param(&src->lst_ran_param[i]);
  }

  assert(src->lst_ran_param != NULL);
  free(src->lst_ran_param);
}

bool eq_ran_param_list(ran_param_list_t const* m0, ran_param_list_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_lst_ran_param != m1->sz_lst_ran_param )
    return false;

  for(size_t i = 0; i < m0->sz_lst_ran_param; ++i){
    if(eq_lst_ran_param(&m0->lst_ran_param[i], &m1->lst_ran_param[i]) == false)
      return false;
  }

  return true;
}

ran_param_list_t cp_ran_param_list(ran_param_list_t const* src)
{
  assert(src != NULL);
  ran_param_list_t dst = {0}; 

  // [0- 65535]
  if(src->sz_lst_ran_param > 0){
    dst.sz_lst_ran_param = src->sz_lst_ran_param;

    dst.lst_ran_param = calloc(dst.sz_lst_ran_param, sizeof(lst_ran_param_t));
    assert(dst.lst_ran_param != NULL && "Memory exhausted" );
  }

  for(size_t i = 0; i < src->sz_lst_ran_param; ++i){
    dst.lst_ran_param[i] = cp_lst_ran_param(&src->lst_ran_param[i]);
  }

  return dst;
}

