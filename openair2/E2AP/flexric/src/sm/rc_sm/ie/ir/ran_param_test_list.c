#include "ran_param_test_list.h"

#include <assert.h>

#include "ran_param_test.h"

void free_ran_param_test_lst( ran_param_test_lst_t* src)
{
  assert(src != NULL);
  assert(src->sz_lst > 0 && src->sz_lst < 65535+1);

  for(size_t i = 0; i < src->sz_lst; ++i){
    free_ran_param_test(&src->ran_param_test[i]);
  }
  free(src->ran_param_test);

}

bool eq_ran_param_test_lst(ran_param_test_lst_t const* m0, ran_param_test_lst_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_lst != m1->sz_lst)
    return false;

  for(size_t i = 0; i < m0->sz_lst; ++i){
    if(eq_ran_param_test(&m0->ran_param_test[i], &m1->ran_param_test[i]  )  == false)
      return false;
  }

  return true;


}

ran_param_test_lst_t cp_ran_param_test_lst(ran_param_test_lst_t const* src)
{
  assert(src != NULL);
  ran_param_test_lst_t dst = {0}; 

  // [1- 65535]
  assert(src->sz_lst > 0 && src->sz_lst < 65536); 
  dst.sz_lst = src->sz_lst;
  // RAN Parameter Testing
  // 9.3.30
  dst.ran_param_test = calloc(dst.sz_lst, sizeof(ran_param_test_t)); 
  assert(dst.ran_param_test != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_lst; ++i){
    dst.ran_param_test[i] = cp_ran_param_test(&src->ran_param_test[i]);
  }

  return dst;
}

