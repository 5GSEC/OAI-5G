#include "eq_float.h"

#include <assert.h>
#include <float.h>
#include <math.h>


bool eq_float(float a, float b, double eps)
{
  assert(isnan(a) == false);
  assert(isnan(b) == false);
  assert(isnan(eps) == false);

  float const absA = fabs(a);
  float const absB = fabs(b);
  double const diff = fabs(a - b);

  if(a == b){ 
    // Infinite case and normal equality
    return true;
  }else if(a == 0 || b == 0 || diff < FLT_MIN ){
    // a or b are zero or are extremely close to each other.
    return (diff < (eps * FLT_MIN));
  } else {
    return (diff / (double)(absA + absB) < eps);
  }
}

