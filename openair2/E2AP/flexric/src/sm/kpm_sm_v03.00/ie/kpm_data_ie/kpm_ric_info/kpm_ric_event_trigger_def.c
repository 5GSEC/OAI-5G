#include "kpm_ric_event_trigger_def.h"

#include <assert.h>
#include <stdlib.h>

void free_kpm_event_trigger_def(kpm_event_trigger_def_t* src)
{
  // No memory allocated in the heap
  (void)src;
}

bool eq_kpm_event_trigger_def( kpm_event_trigger_def_t const* m0,  kpm_event_trigger_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  assert(m0->type == FORMAT_1_RIC_EVENT_TRIGGER  && "Only Format 1 supported ");
  assert(m1->type == FORMAT_1_RIC_EVENT_TRIGGER  && "Only Format 1 supported ");


  if(m0->kpm_ric_event_trigger_format_1.report_period_ms != m1->kpm_ric_event_trigger_format_1.report_period_ms)
    return false;

  return true;
}


