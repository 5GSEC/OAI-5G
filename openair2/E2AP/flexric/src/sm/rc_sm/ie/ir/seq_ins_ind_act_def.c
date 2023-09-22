#include "seq_ins_ind_act_def.h"

#include <assert.h>

void free_seq_ins_ind_act_def(seq_ins_ind_act_def_t* src)
{
  assert(src != NULL);

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  // uint16_t ind_id;

  // List of RAN parameters for
  // Insert Indication
  // [1-65535]
  assert(src->sz_ran_param_ins_ind > 0 && src->sz_ran_param_ins_ind < 65535+1); 

  for(size_t i = 0; i < src->sz_ran_param_ins_ind; ++i){
    free_ran_param_ins_ind(&src->ran_param_ins_ind[i]);
  }

  assert(src->ran_param_ins_ind != NULL);
  free(src->ran_param_ins_ind);

}

bool eq_seq_ins_ind_act_def( seq_ins_ind_act_def_t const* m0, seq_ins_ind_act_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  assert(m0->ind_id > 0);
  assert(m1->ind_id > 0);

  // List of RAN parameters for
  // Insert Indication
  // [1-65535]
  if(m0->sz_ran_param_ins_ind != m1->sz_ran_param_ins_ind) 
    return false;

  for(size_t i = 0; i < m0->sz_ran_param_ins_ind; ++i){
    if(eq_ran_param_ins_ind(&m0->ran_param_ins_ind[i], &m1->ran_param_ins_ind[i] ) == false)
      return false;
  }

  return true;
}

seq_ins_ind_act_def_t cp_seq_ins_ind_act_def(seq_ins_ind_act_def_t const* src)
{
  assert(src != NULL);
  seq_ins_ind_act_def_t dst = {0};

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  assert(src->ind_id > 0);
  dst.ind_id = src->ind_id;

  // List of RAN parameters for
  // Insert Indication
  // [1-65535] 9.2.1.2.4
  // [0-65535] 9.2.1.4.6
  assert(src->sz_ran_param_ins_ind < 65536); 
  if(src->sz_ran_param_ins_ind > 0){
    dst.sz_ran_param_ins_ind = src->sz_ran_param_ins_ind;
    dst.ran_param_ins_ind = calloc(dst.sz_ran_param_ins_ind, sizeof( ran_param_ins_ind_t) );
    assert(dst.ran_param_ins_ind != NULL && "Not implemented");
  }

  for(size_t i = 0; i < dst.sz_ran_param_ins_ind; ++i){
    dst.ran_param_ins_ind[i] = cp_ran_param_ins_ind(&src->ran_param_ins_ind[i]);
  }

  return dst;
}


