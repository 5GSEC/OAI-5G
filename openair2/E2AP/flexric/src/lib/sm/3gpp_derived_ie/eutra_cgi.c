#include <assert.h>
#include <stddef.h>

#include "eutra_cgi.h"


void free_eutra_cgi(eutra_cgi_t* src)
{
  assert(src != NULL);

  free_e2sm_plmn(&src->plmn_id); //6.2.3.1

  //  uint32_t eutra_cell_id; // bit string of 28 bits

}


bool eq_eutra_cgi(eutra_cgi_t const * m0, eutra_cgi_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (eq_e2sm_plmn(&m0->plmn_id, &m1->plmn_id) != true)
        return false;

    if (m0->eutra_cell_id != m1->eutra_cell_id)
      return false;

    return true;
}


eutra_cgi_t cp_eutra_cgi(eutra_cgi_t const* src)
{
  assert(src != NULL);

  eutra_cgi_t dst = {0}; 

  dst.plmn_id = src->plmn_id; // 6.2.3.1
  
  dst.eutra_cell_id = src->eutra_cell_id; // bit string of 28 bits

  return dst;
}


