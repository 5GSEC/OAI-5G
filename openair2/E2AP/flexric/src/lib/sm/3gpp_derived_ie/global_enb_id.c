#include <assert.h>
#include <stdlib.h>

#include "global_enb_id.h"

bool eq_global_enb_id(global_enb_id_t const * m0, global_enb_id_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (eq_e2sm_plmn(&m0->plmn_id, &m1->plmn_id) != true)
      return false;

    if (m0->type != m1->type)
      return false;

    switch (m0->type)
    {
    case MACRO_ENB_TYPE_ID:
      if (m0->macro_enb_id != m1->macro_enb_id)
        return false;
      break;

    case HOME_ENB_TYPE_ID:
      if (m0->home_enb_id != m1->home_enb_id)
        return false;
      break;

    case SHORT_MACRO_ENB_TYPE_ID:
      if (m0->short_macro_enb_id != m1->short_macro_enb_id)
        return false;
      break;

    case LONG_MACRO_ENB_TYPE_ID:
      if (m0->long_macro_enb_id != m1->long_macro_enb_id)
        return false;
      break;
    
    default:
      assert(false && "Unknown eNB Type");
    }

    return true;
}


global_enb_id_t cp_global_enb_ue_id_e2sm(  global_enb_id_t const* src)
{
  assert(src != NULL);

  global_enb_id_t dst = {0}; 


  dst.plmn_id = cp_e2sm_plmn(&src->plmn_id);

  dst.type = src->type;

  if(dst.type == MACRO_ENB_TYPE_ID){
    dst.macro_enb_id = src->macro_enb_id;
  } else if(dst.type == HOME_ENB_TYPE_ID ){
    dst.home_enb_id = src->home_enb_id;
  } else if(dst.type == SHORT_MACRO_ENB_TYPE_ID ){
    dst.short_macro_enb_id = src->short_macro_enb_id;
  } else if(dst.type ==  LONG_MACRO_ENB_TYPE_ID){
    dst.long_macro_enb_id = src->long_macro_enb_id;
  } else {
  assert( 0!=0 && "Unknown type");
  }

  return dst;
}


