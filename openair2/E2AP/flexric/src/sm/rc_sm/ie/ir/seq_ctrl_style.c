#include "seq_ctrl_style.h"

#include <assert.h>

void free_seq_ctrl_style(seq_ctrl_style_t* src)
{
  assert(src != NULL);

  // RIC Control Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // uint32_t style_type;

  //RIC Control Style Name
  //Mandatory
  //9.3.4
  // [1 -150]
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // Sequence of Control Actions
  // [0-65535]
  assert(src->sz_seq_ctrl_act < 65536); 
  for(size_t i = 0; i < src->sz_seq_ctrl_act; ++i){
    free_seq_ctrl_act_2(&src->seq_ctrl_act[i]);
  }
  if(src->seq_ctrl_act != NULL)
    free(src->seq_ctrl_act);

  // RIC Control Header Format Type
  // Mandatory
  // 9.3.5
  // uint32_t hdr;

  // RIC Control Message Format Type
  // Mandatory
  // 9.3.5
  // uint32_t msg;

  // RIC Call Process ID Format Type
  // Optional
  //call_proc_id_frmt_t* call_proc_id_type;
  assert(src->call_proc_id_type == NULL && "Not implemented"); 

  // RIC Control Outcome Format Type
  // Mandatory
  // 9.3.5
  // uint32_t out_frmt;

  // Sequence of Associated RAN 
  // Parameters for Control Outcome
  // [0- 255]
  assert(src->sz_ran_param_ctrl_out < 256); 
  for(size_t i = 0; i < src->sz_ran_param_ctrl_out; ++i){
    free_seq_ran_param_3(&src->ran_param_ctrl_out[i]);
  }
  if(src->ran_param_ctrl_out != NULL) 
    free(src->ran_param_ctrl_out);

}


bool eq_seq_ctrl_style(seq_ctrl_style_t const* m0, seq_ctrl_style_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RIC Control Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  if(m0->style_type != m1->style_type)
    return false;

  //RIC Control Style Name
  //Mandatory
  //9.3.4
  // [1 -150]
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Sequence of Control Actions
  // [0-65535]
  assert(m0->sz_seq_ctrl_act < 65536); 
  assert(m1->sz_seq_ctrl_act < 65536); 
  if(m0->sz_seq_ctrl_act != m1->sz_seq_ctrl_act)
    return false;
  for(size_t i = 0; i < m0->sz_seq_ctrl_act; ++i){
    if(eq_seq_ctrl_act_2(&m0->seq_ctrl_act[i], &m1->seq_ctrl_act[i] ) == false)
      return false;
  }

  // RIC Control Header Format Type
  // Mandatory
  // 9.3.5
  if(m0->hdr != m1->hdr) 
    return false;

  // RIC Control Message Format Type
  // Mandatory
  // 9.3.5
  if(m0->msg != m1->msg)
    return false;

  // RIC Call Process ID Format Type
  // Optional
  assert(m0->call_proc_id_type == NULL && "Not implemented");
  assert(m1->call_proc_id_type == NULL && "Not implemented");

  // RIC Control Outcome Format Type
  // Mandatory
  // 9.3.5
  if(m0->out_frmt != m1->out_frmt)
    return false;

  // Sequence of Associated RAN 
  // Parameters for Control Outcome
  // [0- 255]
  assert(m0->sz_ran_param_ctrl_out < 256);
  assert(m1->sz_ran_param_ctrl_out < 256);
  if(m0->sz_ran_param_ctrl_out  != m1->sz_ran_param_ctrl_out) 
    return false;

  for(size_t i = 0; i < m0->sz_ran_param_ctrl_out; ++i){
    if(eq_seq_ran_param_3(&m0->ran_param_ctrl_out[i], &m1->ran_param_ctrl_out[i]) == false)
      return false;
  }

  return true;
}

seq_ctrl_style_t cp_seq_ctrl_style(seq_ctrl_style_t const* src)
{
  assert(src != NULL);

  seq_ctrl_style_t dst = {0}; 
  // RIC Control Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  dst.style_type = src->style_type ;

  //RIC Control Style Name
  //Mandatory
  //9.3.4
  // [1 -150]
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);

  // Sequence of Control Actions
  // [0-65535]
  assert(src->sz_seq_ctrl_act < 65535);
  if(src->sz_seq_ctrl_act > 0){
    dst.sz_seq_ctrl_act = src->sz_seq_ctrl_act;
    dst.seq_ctrl_act = calloc(dst.sz_seq_ctrl_act , sizeof(seq_ctrl_act_2_t));
    assert(dst.seq_ctrl_act != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < src->sz_seq_ctrl_act; ++i){
    dst.seq_ctrl_act[i] = cp_seq_ctrl_act_2(&src->seq_ctrl_act[i]);
  }

  // RIC Control Header Format Type
  // Mandatory
  // 9.3.5
  dst.hdr = src->hdr;

  // RIC Control Message Format Type
  // Mandatory
  // 9.3.5
  dst.msg = src->msg;

  // RIC Call Process ID Format Type
  // Optional
  assert(src-> call_proc_id_type == NULL && "Not implemented");

  // RIC Control Outcome Format Type
  // Mandatory
  // 9.3.5
  dst.out_frmt = src->out_frmt;

  // Sequence of Associated RAN 
  // Parameters for Control Outcome
  // [0- 255]
  assert(src->sz_ran_param_ctrl_out < 256);
  if(src->sz_ran_param_ctrl_out > 0){
    dst.sz_ran_param_ctrl_out = src->sz_ran_param_ctrl_out;
    dst.ran_param_ctrl_out = calloc(dst.sz_ran_param_ctrl_out, sizeof(seq_ran_param_3_t));
    assert(dst.ran_param_ctrl_out != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_ran_param_ctrl_out; ++i){
    dst.ran_param_ctrl_out[i] = cp_seq_ran_param_3(&src->ran_param_ctrl_out[i]);
  }

  return dst;
}

