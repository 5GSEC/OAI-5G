#include "seq_ins_ind_act_2.h"

#include <assert.h>


void free_seq_ins_ind_act_2(seq_ins_ind_act_2_t* src)
{
  assert(src != NULL);


  //  Insert Indication ID
  //  Mandatory
  //  9.3.16
  //  1.. 65535
  assert(src->ins_ind_id > 0);

  // List of RAN parameters requested
  // [0-65535]
  for(size_t i = 0; i < src->sz_ran_param_req; ++i){
    free_ran_param_req(&src->ran_param_req[i]);
  }

  if(src->ran_param_req != NULL)
    free(src->ran_param_req);
}


bool eq_seq_ins_ind_act_2(seq_ins_ind_act_2_t const* m0, seq_ins_ind_act_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Indicated Insert Style
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  if(m0->ins_ind_id != m1->ins_ind_id)
    return false;

  // Sequence of Insert Indication Actions
  // [0-65535]
  assert(m0->sz_ran_param_req < 65536); 
  assert(m1->sz_ran_param_req < 65536); 

  if(m0->sz_ran_param_req != m1->sz_ran_param_req)
    return false;

  for(size_t i = 0; i < m0->sz_ran_param_req; ++i){
    if(eq_ran_param_req(&m0->ran_param_req[i], &m1->ran_param_req[i]) == false)
      return false;
  }

  return true;
}


seq_ins_ind_act_2_t cp_seq_ins_ind_act_2(seq_ins_ind_act_2_t const* src)
{
  assert(src != NULL);
  seq_ins_ind_act_2_t dst = {0}; 
  //  Insert Indication ID
  //  Mandatory
  //  9.3.16
  //  1.. 65535
  assert(src->ins_ind_id > 0);
  dst.ins_ind_id = src->ins_ind_id;

  // List of RAN parameters requested
  // [0-65535]
  if(src->sz_ran_param_req > 0){
    dst.sz_ran_param_req = src->sz_ran_param_req;
    dst.ran_param_req = calloc(dst.sz_ran_param_req, sizeof(ran_param_req_t ));
    assert(dst.ran_param_req != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_ran_param_req; ++i){
    dst.ran_param_req[i] = cp_ran_param_req(&src->ran_param_req[i]);
  }

  return dst;
}

