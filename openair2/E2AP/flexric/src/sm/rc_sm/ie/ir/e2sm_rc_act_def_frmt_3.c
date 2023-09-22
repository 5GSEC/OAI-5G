#include "e2sm_rc_act_def_frmt_3.h"

#include <assert.h>

void free_e2sm_rc_act_def_frmt_3(e2sm_rc_act_def_frmt_3_t * src)
{
  assert(src != NULL);

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535] 
  // uint16_t id; 

  // List of RAN parameters for Insert
  // Indication
  // [1 - 65535]
  for(size_t i = 0; i < src->sz_ran_param_ins; ++i){
    free_ran_param_ins(&src->ran_param[i]);
  } 

  assert(src->ran_param != NULL);
  free(src->ran_param);

  //  UE ID
  //  Optional
  //  9.3.10
  assert(src->ue_id == NULL && "Not implented"); 

}

bool eq_e2sm_rc_act_def_frmt_3(e2sm_rc_act_def_frmt_3_t const* m0, e2sm_rc_act_def_frmt_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535] 
  if(m0->id != m1->id)
    return false;

  // List of RAN parameters for Insert
  // Indication
  // [1 - 65535]
  assert(m0->sz_ran_param_ins > 0); 
  assert(m1->sz_ran_param_ins > 0); 

  if(m0->sz_ran_param_ins != m1->sz_ran_param_ins)
    return false;

  for(size_t i = 0; i < m0->sz_ran_param_ins; ++i){
    if(eq_ran_param_ins(&m0->ran_param[i], &m1->ran_param[i]) == false)
      return false;
  }

  //  UE ID
  //  Optional
  //  9.3.10
  assert(m0->ue_id == NULL && "Not implemented");
  assert(m1->ue_id == NULL && "Not implemented");

  return true;
}

e2sm_rc_act_def_frmt_3_t cp_e2sm_rc_act_def_frmt_3(e2sm_rc_act_def_frmt_3_t const* src)
{
  assert(src != NULL);
  e2sm_rc_act_def_frmt_3_t dst = {0}; 
  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535] 
  assert(src->id > 0);
  dst.id = src->id;

  // List of RAN parameters for Insert
  // Indication
  // [1 - 65535]
  assert(src->sz_ran_param_ins > 0 && src->sz_ran_param_ins < 65536);
  dst.sz_ran_param_ins = src->sz_ran_param_ins;
  dst.ran_param = calloc(dst.sz_ran_param_ins, sizeof( ran_param_ins_t) );
  assert(dst.ran_param != NULL && "Memory exhausted");

  for(size_t i =0; i < dst.sz_ran_param_ins; ++i){
    dst.ran_param[i] = cp_ran_param_ins(&src->ran_param[i]);
  }

  //  UE ID
  //  Optional
  //  9.3.10
  if(src->ue_id != NULL){
    dst.ue_id = calloc(1, sizeof( ue_id_e2sm_t));
    assert(dst.ue_id != NULL && "Memory exhausted");
    *dst.ue_id = cp_ue_id_e2sm(src->ue_id);
  }

  return dst;
}

