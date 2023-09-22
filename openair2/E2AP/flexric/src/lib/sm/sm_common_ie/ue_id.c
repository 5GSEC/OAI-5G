#include <assert.h>
#include <stdlib.h>

#include "ue_id.h"

void free_ue_id_e2sm(ue_id_e2sm_t * src)
{
    assert(src != NULL);

    switch (src->type)
    {
    case GNB_UE_ID_E2SM:
    {
      free_gnb_ue_id_e2sm(&src->gnb);
      break;
    }

    case GNB_DU_UE_ID_E2SM:
    {
      free_gnb_du_ue_id_e2sm(&src->gnb_du);
      break;
    }

    case GNB_CU_UP_UE_ID_E2SM:
    {
      free_gnb_cu_up_ue_id_e2sm(&src->gnb_cu_up);
      break;
    }

    case NG_ENB_UE_ID_E2SM:
    {
      free_ng_enb_ue_id_e2sm(&src->ng_enb);
      break;
    }

    case NG_ENB_DU_UE_ID_E2SM:
    {
      free_ng_enb_du_ue_id_e2sm(&src->ng_enb_du);
      break;
    }

    case EN_GNB_UE_ID_E2SM:
    {
      free_en_gnb_ue_id_e2sm(&src->en_gnb);
      break;
    }

    case ENB_UE_ID_E2SM:
    {
      free_enb_ue_id_e2sm(&src->enb);
      break;
    }

    default:
      assert(false && "Unknown UE ID Type");
    }



}

bool eq_ue_id_e2sm(ue_id_e2sm_t const * m0, ue_id_e2sm_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (m0->type != m1->type)
      return false;

    switch (m0->type)
    {
    case GNB_UE_ID_E2SM:
    {
      if (eq_gnb_ue_id_e2sm(&m0->gnb, &m1->gnb) != true)
        return false;
  
      break;
    }

    case GNB_DU_UE_ID_E2SM:
    {
      if (eq_gnb_du_ue_id_e2sm(&m0->gnb_du, &m1->gnb_du) != true)
        return false;

      break;
    }

    case GNB_CU_UP_UE_ID_E2SM:
    {
      if (eq_gnb_cu_up_ue_id_e2sm(&m0->gnb_cu_up, &m1->gnb_cu_up) != true)
        return false;

      break;
    }

    case NG_ENB_UE_ID_E2SM:
    {
      if (eq_ng_enb_ue_id_e2sm(&m0->ng_enb, &m1->ng_enb) != true)
        return false;

      break;
    }

    case NG_ENB_DU_UE_ID_E2SM:
    {
      if (eq_ng_enb_du_ue_id_e2sm(&m0->ng_enb_du, &m1->ng_enb_du) != true)
        return false;

      break;
    }

    case EN_GNB_UE_ID_E2SM:
    {
      if (eq_en_gnb_ue_id_e2sm(&m0->en_gnb, &m1->en_gnb) != true)
        return false;

      break;
    }

    case ENB_UE_ID_E2SM:
    {
      if (eq_enb_ue_id_e2sm(&m0->enb, &m1->enb) != true)
        return false;

      break;
    }

    default:
      assert(false && "Unknown UE ID Type");
    }

    return true;
}

ue_id_e2sm_t cp_ue_id_e2sm(const ue_id_e2sm_t * src)
{
  assert(src != NULL);

  ue_id_e2sm_t dst = {0};

  dst.type = src->type;

  switch (dst.type)
  {
  case GNB_UE_ID_E2SM:
    dst.gnb = cp_gnb_ue_id_e2sm(&src->gnb);
    break;

  case GNB_DU_UE_ID_E2SM:
    dst.gnb_du = cp_gnb_du_ue_id_e2sm(&src->gnb_du);
    break;

  case GNB_CU_UP_UE_ID_E2SM:
    dst.gnb_cu_up = cp_gnb_cu_up_id_e2sm(&src->gnb_cu_up);
    break;

  case NG_ENB_UE_ID_E2SM:
    dst.ng_enb = cp_ng_enb_ue_id_e2sm(&src->ng_enb);
    break;

  case NG_ENB_DU_UE_ID_E2SM:
    dst.ng_enb_du = cp_ng_enb_du_ue_id_e2sm(&src->ng_enb_du);
    break;
  
  case EN_GNB_UE_ID_E2SM:
    dst.en_gnb = cp_en_gnb_ue_id_e2sm(&src->en_gnb);
    break;

  case ENB_UE_ID_E2SM:
    dst.enb = cp_enb_ue_id_e2sm(&src->enb);
    break;

  default:
    assert(false && "Unknown UE ID Type");
  }



  return dst;
}
