#include <assert.h>
#include <stddef.h>

#include "cell_global_id.h"

bool eq_cell_global_id(cell_global_id_t const * m0, cell_global_id_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (m0->type != m1->type)
      return false;

    switch (m0->type)
    {
    case NR_CGI_RAT_TYPE:
      if (eq_nr_cgi(&m0->nr_cgi, &m1->nr_cgi) != true)
        return false;
      break;

    case EUTRA_CGI_RAT_TYPE:
      if (eq_eutra_cgi(&m0->eutra, &m1->eutra) != true)
        return false;
      break;
    
    default:
      assert(false && "Unknown Cell Global ID Type");
    }

    return true;
}

cell_global_id_t cp_cell_global_id(const cell_global_id_t * src)
{
  assert(src != NULL);

  cell_global_id_t dst = {0};

  dst.type = src->type;

  switch (dst.type)
  {
  case NR_CGI_RAT_TYPE:
    dst.nr_cgi = cp_nr_cgi(&src->nr_cgi);
    break;

  case EUTRA_CGI_RAT_TYPE:
    dst.eutra = cp_eutra_cgi(&src->eutra);
    break;
  
  default:
    assert(false && "Unknown Cell Global ID Type");
  }



  return dst;
}

void free_cell_global_id(cell_global_id_t * src)
{
  assert(src != NULL);

  if(src->type == NR_CGI_RAT_TYPE){
    free_nr_cgi(&src->nr_cgi);  
  } else if(src->type == EUTRA_CGI_RAT_TYPE){
    free_eutra_cgi(&src->eutra);
  } else {
    assert(0!=0 && "unknown type");
  }

}

