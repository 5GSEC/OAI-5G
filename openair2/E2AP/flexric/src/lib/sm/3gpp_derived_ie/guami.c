#include <assert.h>
#include <stdlib.h>

#include "guami.h"

bool eq_guami(guami_t const * m0, guami_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if (eq_e2sm_plmn(&m0->plmn_id, &m1->plmn_id) != true)
    return false;

  if (m0->amf_region_id != m1->amf_region_id)
    return false;

  if (m0->amf_set_id != m1->amf_set_id)
    return false;

  if (m0->amf_ptr != m1->amf_ptr)
    return false;

  return true;
}


guami_t cp_guami(guami_t const* src)
{
  assert(src != NULL);

  guami_t dst = {0}; 

  // Mandatory
  // PLMN Identity 6.2.3.1
  dst.plmn_id = cp_e2sm_plmn(&src->plmn_id);

  // Mandatory
  // AMF Region ID BIT STRING (SIZE(8))
  dst.amf_region_id = src->amf_region_id;

  // Mandatory
  //  AMF Set ID BIT STRING (SIZE(10))
  dst.amf_set_id = src->amf_set_id;

  // Mandatory
  // AMF Pointer BIT STRING (SIZE(6))
  dst.amf_ptr = src->amf_ptr;

  return dst;
}

