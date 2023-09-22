#include "enc_ran_function_name.h"

#include <assert.h>
#include <stdlib.h>

static inline
OCTET_STRING_t copy_ba_to_ostring(byte_array_t ba)
{
  OCTET_STRING_t os = { .size = ba.len }; 
  os.buf = malloc(ba.len);
  memcpy(os.buf, ba.buf, ba.len);
  return os;
}


RANfunction_Name_t enc_ran_func_name(ran_function_name_t const* src)
{
  assert(src != NULL);

  RANfunction_Name_t dst = {0};
 
  // RAN Function Short Name
  // Mandatory
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151);
  dst.ranFunction_ShortName = copy_ba_to_ostring(src->name); 

  //RAN Function Service Model OID
  //Mandatory
  // [1-1000]
  assert(src->oid.len > 0 && src->oid.len < 1001); 
  dst.ranFunction_E2SM_OID = copy_ba_to_ostring(src->oid);
	
  //RAN Function Description
  //Mandatory
  // [1-150]
  assert(src->description.len > 0 && src->description.len < 151);
  dst.ranFunction_Description = copy_ba_to_ostring(src->description);

  // RAN Function Instance
  // Optional
  // INTEGER
  assert(src->instance == NULL && "not implemented");

  return dst;
}

