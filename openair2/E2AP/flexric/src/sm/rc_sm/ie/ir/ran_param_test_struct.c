#include "ran_param_test_struct.h"
#include "ran_param_test.h"

#include <assert.h>

void free_ran_param_test_strct(ran_param_test_strct_t* src)
{
  assert(src != NULL);

  // [1- 65535]
  assert(src->sz_strct > 0 && src->sz_strct <  65535 +1);
  // RAN Parameter Testing
  // 9.3.30
  for(size_t i = 0; i < src->sz_strct; ++i) {
    free_ran_param_test(&src->ran_param_test[i]);
  }

  free(src->ran_param_test);

}

bool eq_ran_param_test_strct(ran_param_test_strct_t const* m0, ran_param_test_strct_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_strct != m1->sz_strct)
    return false;

  for(size_t i = 0; i < m0->sz_strct; ++i){
    if(eq_ran_param_test(&m0->ran_param_test[i], &m1->ran_param_test[i]) == false)
      return false;
  }

  return true;
}

ran_param_test_strct_t cp_ran_param_test_strct( ran_param_test_strct_t const* src)
{
  assert(src != NULL);

  ran_param_test_strct_t dst = {0}; 

  // [1- 65535]
  assert(src->sz_strct > 0 && src->sz_strct <  65536);
  dst.sz_strct = src->sz_strct;

  // RAN Parameter Testing
  // 9.3.30
  dst.ran_param_test = calloc(dst.sz_strct, sizeof(ran_param_test_t) );
  assert(dst.ran_param_test != NULL && "memory exhausted");

  // RAN Parameter Testing
  // 9.3.30
  for(size_t i = 0; i < dst.sz_strct; ++i){
    dst.ran_param_test[i] = cp_ran_param_test(&src->ran_param_test[i]);
  } 

  return dst;
}

