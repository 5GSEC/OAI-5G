#include "seq_call_proc_type.h"

#include <assert.h>

void free_seq_call_proc_type( seq_call_proc_type_t* src)
{
  assert(src != NULL);
  // Call Process Type ID
  // Mandatory
  // 9.3.15
  // [1- 65535]
  assert(src->id > 0); 

  // Call Process Type Name
  // Mandatory
  // 9.3.19
  // [1-150] 
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // Sequence of Call Process Breakpoints
  // [1-65535]
  assert(src->sz_call_proc_break > 0 && src->sz_call_proc_break < 65536); 

  for(size_t i = 0; i < src->sz_call_proc_break; ++i){
    free_call_proc_break(&src->call_proc_break[i]);
  }
  free(src->call_proc_break);
}

bool eq_seq_call_proc_type( seq_call_proc_type_t const* m0,  seq_call_proc_type_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Call Process Type ID
  // Mandatory
  // 9.3.15
  // [1- 65535]
  assert(m0->id > 0); 
  assert(m1->id > 0); 
  if(m0->id != m1->id )
    return false;

  // Call Process Type Name
  // Mandatory
  // 9.3.19
  assert(m0->name.len > 0 && m0->name.len < 151); 
  assert(m1->name.len > 0 && m1->name.len < 151); 

  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Sequence of Call Process Breakpoints
  // [1-65535]
  assert(m0->sz_call_proc_break > 0 && m0->sz_call_proc_break < 65536);
  assert(m1->sz_call_proc_break > 0 && m1->sz_call_proc_break < 65536);

  if(m0->sz_call_proc_break != m1->sz_call_proc_break)
    return false;

  for(size_t i = 0; i < m0->sz_call_proc_break; ++i){
    if(eq_call_proc_break(&m0->call_proc_break[i], &m1->call_proc_break[i]) == false)
      return false;
  }

  return true;
}


seq_call_proc_type_t cp_seq_call_proc_type(seq_call_proc_type_t const* src)
{
  assert(src != NULL);

  seq_call_proc_type_t dst = {0}; 

  // Call Process Type ID
  // Mandatory
  // 9.3.15
  // [1- 65535]
  assert(src->id > 0);
  dst.id = src->id; 

  // Call Process Type Name
  // Mandatory
  // 9.3.19
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151); 
  dst.name = copy_byte_array(src->name); 

  // Sequence of Call Process Breakpoints
  // [1-65535]
  assert(src->sz_call_proc_break > 0 && src->sz_call_proc_break < 65536); 

  dst.sz_call_proc_break = src->sz_call_proc_break;

  dst.call_proc_break = calloc( dst.sz_call_proc_break, sizeof(call_proc_break_t) );
  assert(dst.call_proc_break != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_call_proc_break; ++i){
    dst.call_proc_break[i] = cp_call_proc_break(&src->call_proc_break[i]);
  }
  return dst;
}

