#include <assert.h>
#include <stdlib.h>

#include "e2sm_gummei.h"

bool eq_e2sm_gummei(e2sm_gummei_t const * m0, e2sm_gummei_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (eq_e2sm_plmn(&m0->plmn_id, &m1->plmn_id) != true)
      return false;

    if (m0->mme_group_id != m1->mme_group_id)
      return false;

    if (m0->mme_code != m1->mme_code)
      return false;


    return true;
}


e2sm_gummei_t cp_e2sm_gummei(e2sm_gummei_t const* src)
{
  assert(src != NULL);

  e2sm_gummei_t dst = {0}; 

  dst.plmn_id = cp_e2sm_plmn(&src->plmn_id);

  dst.mme_group_id = src->mme_group_id;

  dst.mme_code = src->mme_code;

  return dst;
}

