#include <assert.h>

#include "meas_type.h"

bool eq_meas_type(meas_type_t const * m0, meas_type_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (m0->type != m1->type)
      return false;


    switch (m0->type)
    {
    case NAME_MEAS_TYPE:
      if (eq_byte_array(&m0->name, &m1->name) != true)
        return false;
      break;

    case ID_MEAS_TYPE:
      if (m0->id != m1->id)
        return false;
      break;
    
    default:
      assert(false && "Unknown Measurement Type");
    }

    return true;
}

meas_type_t cp_meas_type(meas_type_t const* src)
{
    assert(src != NULL);
    meas_type_t ret = {0};

    ret.type = src->type;

    if (ret.type == NAME_MEAS_TYPE)
    {
      ret.name = copy_byte_array(src->name);
    }
    else if (ret.type == ID_MEAS_TYPE)
    {
      ret.id = src->id;
    }

    return ret;
}
