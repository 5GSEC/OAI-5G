
#include "seq_ev_trg_style.h"

#include <assert.h>
#include <stdlib.h>

void free_seq_ev_trg_style(seq_ev_trg_style_t* src)
{
  assert(src != NULL);
  // RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  //  INTEGER
  // uint32_t style;

  // RIC Event Trigger Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3
  //PrintableString(SIZE(1..150,...))
  assert(src->name.len > 0 && src->name.len < 151);
  free_byte_array(src->name);

  // RIC Event Trigger Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  //uint32_t format;
}

bool eq_seq_ev_trg_style( seq_ev_trg_style_t const* m0,  seq_ev_trg_style_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  //  INTEGER
  if(m0->style != m1->style)
    return false;

  // RIC Event Trigger Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3
  //PrintableString(SIZE(1..150,...))
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);

  if(eq_byte_array(&m0->name, &m1->name) == false) 
    return false;

  // RIC Event Trigger Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  if(m0->format != m1->format)
    return false;

  return true;
}

seq_ev_trg_style_t cp_seq_ev_trg_style(seq_ev_trg_style_t const* src)
{
  assert(src != NULL);
  seq_ev_trg_style_t dst = {0}; 

  // RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  //  INTEGER
  dst.style = src->style;

  // RIC Event Trigger Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3
  //PrintableString(SIZE(1..150,...))
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);

  // RIC Event Trigger Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.format = src->format;

  return dst;
}

