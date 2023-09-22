#include "ran_param_cond_crit_def.h"

#include <assert.h>

void free_ran_param_cond_crit_def(ran_param_cond_crit_def_t* src)
{
  assert(src != NULL);

  assert(src->sz_ran_param_test > 0 && src->sz_ran_param_test < 256);

  // RAN Parameter Test
  // 9.3.30
  for(size_t i = 0; i < src->sz_ran_param_test; ++i){
    free_ran_param_test(&src->ran_param_test[i]); 
  }

  assert(src->ran_param_test != NULL);
  free(src->ran_param_test);
}

bool eq_ran_param_cond_crit_def(ran_param_cond_crit_def_t const* m0, ran_param_cond_crit_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  if(m0->sz_ran_param_test != m1->sz_ran_param_test)
    return false;

  for(size_t i = 0; i < m0->sz_ran_param_test; ++i){
    if(eq_ran_param_test(&m0->ran_param_test[i], &m1->ran_param_test[i] ) == false)
      return false;
  }

  return true;
}

ran_param_cond_crit_def_t cp_ran_param_cond_crit_def(ran_param_cond_crit_def_t const* src)
{
  assert(src != NULL);
ran_param_cond_crit_def_t dst = {0};

// [1 - 255 ]
  assert(src->sz_ran_param_test > 0 && src->sz_ran_param_test < 256);
  dst.sz_ran_param_test = src->sz_ran_param_test; 
  dst.ran_param_test = calloc(dst.sz_ran_param_test, sizeof(ran_param_test_t));
  assert(dst.ran_param_test != NULL && "Memory exhausted" );

  // RAN Parameter Test
  // 9.3.30
  for(size_t i = 0; i < dst.sz_ran_param_test ; ++i){
    dst.ran_param_test[i] = cp_ran_param_test(&src->ran_param_test[i]);
  }  

return dst;
}

