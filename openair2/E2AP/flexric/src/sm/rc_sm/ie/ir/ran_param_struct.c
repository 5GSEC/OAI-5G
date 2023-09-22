#include "ran_param_struct.h"
#include "ran_param_test.h"

#include <assert.h>

void free_ran_param_struct(ran_param_struct_t* src)
{
  assert(src != NULL);

  // [1-65535]
  assert(src->sz_ran_param_struct > 0 && src-> sz_ran_param_struct < 65536);
  for(size_t i = 0;i < src->sz_ran_param_struct; ++i ){
    free_seq_ran_param(&src->ran_param_struct[i]);
  }

  assert(src->ran_param_struct != NULL);
  free(src->ran_param_struct);
}

bool eq_ran_param_struct(ran_param_struct_t const* m0, ran_param_struct_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ran_param_struct != m1->sz_ran_param_struct)
    return false;

  assert(m0->sz_ran_param_struct > 0 && m0->sz_ran_param_struct < 65536);
  assert(m1->sz_ran_param_struct > 0 && m1->sz_ran_param_struct < 65536);

  for(size_t i = 0; i < m0->sz_ran_param_struct; ++i){
    if(eq_seq_ran_param(&m0->ran_param_struct[i], &m1->ran_param_struct[i]) == false )
      return false;
  }

  return true;
}

ran_param_struct_t cp_ran_param_struct(ran_param_struct_t const* src)
{
  assert(src != NULL);
  ran_param_struct_t dst = {0}; 

  // [1-65535]
  assert(src->sz_ran_param_struct > 0 && src->sz_ran_param_struct < 65536);

  dst.ran_param_struct = calloc(src->sz_ran_param_struct, sizeof(seq_ran_param_t ) );
  assert(dst.ran_param_struct != NULL && "Memory exhausted");

  dst.sz_ran_param_struct = src->sz_ran_param_struct;

  for(size_t i = 0; i < src->sz_ran_param_struct; ++i){
    dst.ran_param_struct[i] = cp_seq_ran_param(&src->ran_param_struct[i]);
  }

  return dst;
}

