#include "network_interface_e2rc.h"

#include <assert.h>
#include <stdlib.h>

void free_network_interface_e2rc( network_interface_e2rc_t* src)
{
  assert(src != NULL);

  // NI Type
  // Mandatory
  // 9.3.32
  // network_interface_type_e ni_type;

  // NI Identifier
  // Optional
  // 9.3.33
  assert(src->ni_id == NULL && "Not implemented");

  // NI Message
  // Optional
  // 9.3.34
  assert(src->ni_msg_id == NULL && "Not implemented");

}

bool eq_network_interface_e2rc( network_interface_e2rc_t const* m0, network_interface_e2rc_t const* m1)
{
  // NI Type
  // Mandatory
  // 9.3.32
  if(m0->ni_type != m1->ni_type) 
    return false;

  // NI Identifier
  // Optional
  // 9.3.33
  assert(m0->ni_id == NULL && "Not implemented");
  assert(m1->ni_id == NULL && "Not implemented");

  // NI Message
  // Optional
  // 9.3.34
  assert(m0->ni_msg_id == NULL && "Not implemented");
  assert(m1->ni_msg_id == NULL && "Not implemented");

  return true;
}

network_interface_e2rc_t cp_network_interface_e2rc(network_interface_e2rc_t const* src)
{
  assert(src != NULL);

  network_interface_e2rc_t dst = {0}; 
  // NI Type
  // Mandatory
  // 9.3.32
  dst.ni_type = src->ni_type;

  // NI Identifier
  // Optional
  // 9.3.33
  assert(src->ni_id == NULL && "Not implemented");

  // NI Message
  // Optional
  // 9.3.34
  assert(src->ni_msg_id == NULL && "Not implemented");

  return dst;
}

