#include <assert.h>
#include <stdlib.h>

#include "ran_function_name.h"

void free_ran_function_name(ran_function_name_t* src)
{
  assert(src != NULL);

  free_byte_array(src->name);

  free_byte_array(src->oid);

  free_byte_array(src->description);

  if (src->instance != NULL)
    free(src->instance);
}

bool eq_ran_function_name(ran_function_name_t const * m0, ran_function_name_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if (eq_byte_array(&m0->name, &m1->name) != true)
    return false;

  if (eq_byte_array(&m0->oid, &m1->oid) != true)
    return false;

  if (eq_byte_array(&m0->description, &m1->description) != true)
    return false;

  if ((m0->instance != NULL || m0->instance != NULL) && *m0->instance != *m1->instance)
    return false;

  return true;
}

ran_function_name_t cp_ran_function_name(ran_function_name_t const* src)
{
  assert(src != NULL);

  ran_function_name_t dst = {0}; 

  // RAN Function Short Name
  // Mandatory
  // PrintableString [1-150]
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);

  // RAN Function Service Model OID
  // Mandatory
  // PrintableString [1-1000]
  assert(src->oid.len > 0 && src->oid.len < 1001);
  dst.oid = copy_byte_array(src->oid);

  // RAN Function Description
  // Mandatory
  // PrintableString [1- 150]
  assert(src->description.len > 0 && src->description.len < 151);
  dst.description = copy_byte_array(src->description);

  // RAN Function Instance
  // Optional
  // INTEGER
  assert(src->instance == NULL && "Not implemented");
  //  OPTIONAL: it is suggested to be used when E2 Node declares multiple RAN 
  //  Function ID supporting the same  E2SM specification   ask Mikel */
  //  long* instance;	

  return dst;
}

