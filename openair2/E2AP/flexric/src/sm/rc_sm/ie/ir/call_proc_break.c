#include "call_proc_break.h"

#include <assert.h>

void free_call_proc_break(call_proc_break_t* src)
{
  assert(src != NULL);

  // Call Process Breakpoint ID
  // Mandatory
  // 9.3.49
  // [1 - 65535]
  assert(src->id > 0);
  //uint16_t id;

  // Call Process Breakpoint Name
  // Mandatory
  // 9.3.50
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // Sequence of Associated RAN Parameters
  // [0-65535]
  for(size_t i = 0; i < src->sz_param; ++i){
    free_seq_ran_param_3(&src->param[i]);
  }

  if(src->param)
    free(src->param);

}


bool eq_call_proc_break(call_proc_break_t const* m0, call_proc_break_t const* m1)
{
  // Call Process Breakpoint ID
  // Mandatory
  // 9.3.49
  // [1 - 65535]
  assert(m0->id > 0);
  assert(m1->id > 0);
  if(m0->id != m1->id) 
    return false;

  // Call Process Breakpoint Name
  // Mandatory
  // 9.3.50
  // [1-150]
  assert(m0->name.len > 0 && m0->name.len < 151); 
  assert(m1->name.len > 0 && m1->name.len < 151); 
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Sequence of Associated RAN Parameters
  // [0-65535]
  if(m0->sz_param != m1->sz_param)
    return false;

  for(size_t i = 0; i < m0->sz_param; ++i){
    if(eq_seq_ran_param_3(&m0->param[i], &m1->param[i]) == false)
      return false;
  }

  return true;
}

call_proc_break_t cp_call_proc_break(call_proc_break_t const* src)
{
  assert(src != NULL);

  call_proc_break_t dst = {0}; 
  // Call Process Breakpoint ID
  // Mandatory
  // 9.3.49
  // [1 - 65535]
  assert(src->id > 0);
  dst.id = src->id;

  // Call Process Breakpoint Name
  // Mandatory
  // 9.3.50
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151); 
  dst.name = copy_byte_array(src->name);

  // Sequence of Associated RAN Parameters
  // [0-65535]
  if(src->sz_param > 0){
    dst.sz_param = src->sz_param;
    dst.param = calloc(dst.sz_param , sizeof(seq_ran_param_3_t));
    assert(dst.param != NULL && "Memory exhausted");
  } 

  for(size_t i = 0; i < src->sz_param; ++i){
    dst.param[i] = cp_seq_ran_param_3(&src->param[i]);
  }

  return dst;
}

