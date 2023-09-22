#include <assert.h>

#include "ue_id_gran_period_lst.h"

void free_kpm_ue_id_gran_period_lst(ue_id_gran_period_lst_t* src)
{
  assert(src != NULL);

  if (src->matched_ue_lst.ue_lst != NULL)
  {
    for (size_t i = 0; i<src->matched_ue_lst.ue_lst_len; i++)
    {
      free_ue_id_e2sm(&src->matched_ue_lst.ue_lst[i]);
    }
    
    free(src->matched_ue_lst.ue_lst);
  }

}

bool eq_kpm_ue_id_gran_period_lst(ue_id_gran_period_lst_t const* m0, ue_id_gran_period_lst_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if (m0->num_matched_ue != m1->num_matched_ue)
    return false;

  switch (m0->num_matched_ue)
  {
  case NONE_MATCHED_UE:
    if (m0->no_matched_ue != m1->no_matched_ue)
      return false;
    break;

  case ONE_OR_MORE_MATCHED_UE:
    if (m0->matched_ue_lst.ue_lst_len != m1->matched_ue_lst.ue_lst_len)
      return false;

    for (size_t i = 0; i<m0->matched_ue_lst.ue_lst_len; i++)
    {
        if (eq_ue_id_e2sm(&m0->matched_ue_lst.ue_lst[i], &m1->matched_ue_lst.ue_lst[i]) != true)
          return false;
    }
    break;
  
  default:
    break;
  }


  return true;
}

ue_id_gran_period_lst_t cp_kpm_ue_id_gran_period_lst(const ue_id_gran_period_lst_t * src)
{
  assert(src != NULL);

  ue_id_gran_period_lst_t dst = {0};

  dst.num_matched_ue = src->num_matched_ue;

  switch (dst.num_matched_ue)
  {
  case NONE_MATCHED_UE:
    dst.no_matched_ue = src->no_matched_ue;
    break;
  
  case ONE_OR_MORE_MATCHED_UE:{
    dst.matched_ue_lst.ue_lst_len = src->matched_ue_lst.ue_lst_len;
    if(dst.matched_ue_lst.ue_lst_len > 0){
    dst.matched_ue_lst.ue_lst = calloc(dst.matched_ue_lst.ue_lst_len, sizeof(ue_id_e2sm_t));
    assert(dst.matched_ue_lst.ue_lst != NULL && "Memory exhausted");
    }

    for (size_t i = 0; i<dst.matched_ue_lst.ue_lst_len; i++)
    {
      dst.matched_ue_lst.ue_lst[i] = cp_ue_id_e2sm(&src->matched_ue_lst.ue_lst[i]);
    }
    break;
    }
  default:
    break;
  }


  return dst;
}
