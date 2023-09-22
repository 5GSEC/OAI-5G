#include "rrc_state.h"

#include <assert.h>

void free_rrc_state(rrc_state_t* src)
{
  assert(src != NULL);

  // 9.3.37
  //rrc_state_e2sm_rc_e state_chngd_to;

  // 9.3.25
  // Logical OR
  assert(src->log_or == NULL && "Not implemented");

}

bool eq_rrc_state(rrc_state_t const* m0, rrc_state_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // 9.3.37
  if(m0-> state_chngd_to != m1->state_chngd_to)
    return false;

  // 9.3.25
  // Logical OR
  assert(m0->log_or == NULL && "Not implemented");
  assert(m1->log_or == NULL && "Not implemented");

  return true;
}

rrc_state_t cp_rrc_state(rrc_state_t const* src)
{
  assert(src != NULL);
  rrc_state_t dst = {0};

  // 9.3.37
  dst.state_chngd_to = src->state_chngd_to; 

  // 9.3.25
  // Logical OR
  assert(src->log_or == NULL && "Not implemented");

  return dst;
}

