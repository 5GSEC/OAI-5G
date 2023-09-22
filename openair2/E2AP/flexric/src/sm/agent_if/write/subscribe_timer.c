#include "subscribe_timer.h"

#include <assert.h>

void free_subscribe_timer(subscribe_timer_t* src)
{
  assert(src != NULL);

  //int64_t ms;
  //sub_data_e type;
  // Number of elements.
  // Just one is supported
  assert(src->sz < 2); 
  if(src->type == KPM_V3_0_SUB_DATA_ENUM){
    assert(src->kpm_ad != NULL);
    free_kpm_action_def(src->kpm_ad);
  } else if(src->type == NONE_SUB_DATA_ENUM ){

  } else {
    assert(0!=0 && "Unknown type");
  }

}

