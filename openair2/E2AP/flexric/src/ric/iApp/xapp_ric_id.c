
#include "xapp_ric_id.h"

#include <assert.h>
#include <stddef.h>

int cmp_xapp_ric_gen_id(xapp_ric_id_t const* m0,  xapp_ric_id_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->xapp_id < m1->xapp_id) return 1;
  if(m0->xapp_id > m1->xapp_id) return -1;

  return cmp_ric_gen_id(&m0->ric_id, &m1->ric_id);
}

int cmp_xapp_ric_gen_id_wrapper(void const* m0, void const* m1)
{
  return  cmp_xapp_ric_gen_id(m0, m1);
}

bool eq_xapp_ric_gen_id(xapp_ric_id_t const* m0, xapp_ric_id_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->xapp_id != m1->xapp_id)
    return false;

  return eq_ric_gen_id(&m0->ric_id, &m1->ric_id);
}

bool eq_xapp_ric_gen_id_wrapper(void const* m0, void const* m1)
{
  return  eq_xapp_ric_gen_id(m0, m1);
}


