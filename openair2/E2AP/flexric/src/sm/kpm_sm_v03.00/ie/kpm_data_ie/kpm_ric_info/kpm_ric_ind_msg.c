#include <assert.h>
#include <stdlib.h>

#include "kpm_ric_ind_msg.h"

void free_kpm_ind_msg(kpm_ind_msg_t * src)
{
    assert(src != NULL);

    switch (src->type)
    {
    case FORMAT_1_INDICATION_MESSAGE:
        free_kpm_ind_msg_frm_1(&src->frm_1);
        break;

    case FORMAT_2_INDICATION_MESSAGE:
        free_kpm_ind_msg_frm_2(&src->frm_2);
        break;

    case FORMAT_3_INDICATION_MESSAGE:
        free_kpm_ind_msg_frm_3(&src->frm_3);
        break;
    
    default:
        assert(false && "Unknown Indication Message Type");
    }
}

bool eq_kpm_ind_msg(kpm_ind_msg_t const * m0, kpm_ind_msg_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);
    
    assert(m0->type == m1->type);

    switch (m0->type)
    {
    case FORMAT_1_INDICATION_MESSAGE:
        if (eq_kpm_ind_msg_frm_1(&m0->frm_1, &m1->frm_1) != true)
            return false;
        break;
    
    case FORMAT_2_INDICATION_MESSAGE:
        if (eq_kpm_ind_msg_frm_2(&m0->frm_2, &m1->frm_2) != true)
            return false;
        break;

    case FORMAT_3_INDICATION_MESSAGE:
        if (eq_kpm_ind_msg_frm_3(&m0->frm_3, &m1->frm_3) != true)
            return false;
        break;

    default:
        assert(false && "Unknown Indication Message Type");
    }

    return true;
}


kpm_ind_msg_t cp_kpm_ind_msg(kpm_ind_msg_t const* src)
{
  assert(src != NULL);

  kpm_ind_msg_t dst = {0};

  dst.type = src->type;

  switch (dst.type)
  {
    case FORMAT_1_INDICATION_MESSAGE:
        dst.frm_1 = cp_kpm_ind_msg_frm_1(&src->frm_1);
        break;
    
    case FORMAT_2_INDICATION_MESSAGE:
        dst.frm_2 = cp_kpm_ind_msg_frm_2(&src->frm_2);
        break;

    case FORMAT_3_INDICATION_MESSAGE:
        dst.frm_3 = cp_kpm_ind_msg_frm_3(&src->frm_3);
        break;
    
    default:
        assert(false && "Unknown Indication Message Type");
  }



  return dst;
}
