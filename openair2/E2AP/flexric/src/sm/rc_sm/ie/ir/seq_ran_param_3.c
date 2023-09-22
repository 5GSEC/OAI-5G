#include "seq_ran_param_3.h"

#include <assert.h>

void free_seq_ran_param_3(seq_ran_param_3_t* src)
{
  assert(src != NULL);

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1- 4294967295]
  // uint32_t id; 

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  // [1-150] 
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(src->def == NULL && "Not implemented"); 

}

bool eq_seq_ran_param_3(seq_ran_param_3_t const* m0, seq_ran_param_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1- 4294967295]
  assert(m0->id > 0);
  assert(m1->id > 0);
  if(m0->id != m1->id) 
    return false;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  // [1-150] 
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);
  if(eq_byte_array(&m0->name, &m1->name) == false )
    return false;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(m0->def == NULL && "Not implemented"); 
  assert(m1->def == NULL && "Not implemented"); 

  return true;
}

seq_ran_param_3_t cp_seq_ran_param_3(seq_ran_param_3_t const* src)
{
  assert(src != NULL);

  seq_ran_param_3_t dst = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1- 4294967295]
  assert(src->id > 0);  
  dst.id = src->id;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  // [1-150] 
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);  

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(src->def == NULL && "Not implemented");

  return dst;
}

