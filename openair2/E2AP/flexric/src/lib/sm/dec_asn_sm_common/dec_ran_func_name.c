#include "dec_ran_func_name.h"

#include <assert.h>
#include <stdlib.h>

static inline
byte_array_t copy_ostring_to_ba(OCTET_STRING_t src)
{
  byte_array_t dst = { .len = src.size}; 
  dst.buf = malloc(src.size);
  memcpy(dst.buf, src.buf, src.size);
  return dst;
}


ran_function_name_t dec_ran_func_name(RANfunction_Name_t const* src)
{
  assert(src != NULL); 

  ran_function_name_t dst = {0}; 

  // RAN Function Short Name
  // Mandatory
  // [1-150]
  assert(src->ranFunction_ShortName.size > 0 && src->ranFunction_ShortName.size < 151);
  dst.name = copy_ostring_to_ba(src->ranFunction_ShortName);

  //RAN Function Service Model OID
  //Mandatory
  // [1-1000]
  assert(src->ranFunction_E2SM_OID.size > 0 && src->ranFunction_E2SM_OID.size < 1001);
  dst.oid = copy_ostring_to_ba(src->ranFunction_E2SM_OID);
	
  //RAN Function Description
  //Mandatory
  // [1-150]
  assert(src->ranFunction_Description.size > 0 && src->ranFunction_Description.size < 151);
  dst.description = copy_ostring_to_ba(src->ranFunction_Description);

  // RAN Function Instance
  // Optional
  // INTEGER
  assert(src->ranFunction_Instance == NULL && "Not implemented");

  return dst;
}

