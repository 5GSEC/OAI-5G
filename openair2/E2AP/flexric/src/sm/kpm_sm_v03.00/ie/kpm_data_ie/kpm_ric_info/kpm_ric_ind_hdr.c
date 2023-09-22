#include <assert.h>
#include <stdlib.h>

#include "kpm_ric_ind_hdr.h"

void free_kpm_ind_hdr(kpm_ind_hdr_t * src)
{
  assert(src != NULL);

  switch (src->type)
  {
    case FORMAT_1_INDICATION_HEADER:
      free_kpm_ind_hdr_frm_1(&src->kpm_ric_ind_hdr_format_1);
      break;

    default:
      assert(false && "Unknown Indication Header Type");
  }
}

bool eq_kpm_ind_hdr(kpm_ind_hdr_t const * m0, kpm_ind_hdr_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);
    
    assert(m0->type == m1->type);

    switch (m0->type)
    {
    case FORMAT_1_INDICATION_HEADER:
        if (eq_kpm_ind_hdr_frm_1(&m0->kpm_ric_ind_hdr_format_1, &m1->kpm_ric_ind_hdr_format_1) != true)
            return false;
        break;
    
    default:
        assert(false && "Unknown Indication Header Type");
    }

    return true;
}

kpm_ind_hdr_t cp_kpm_ind_hdr(kpm_ind_hdr_t const* src)
{
  assert(src != NULL);

  kpm_ind_hdr_t dst = {0};

  dst.type = src->type;

  switch (dst.type)
  {
    case FORMAT_1_INDICATION_HEADER:
        dst.kpm_ric_ind_hdr_format_1 = cp_kpm_ind_hdr_frm_1(&src->kpm_ric_ind_hdr_format_1);
        break;
  
    default:
        assert(false && "Unknown Indication Header Type");
  }


  return dst;  
}
