#include "ran_func_def_ev_trig.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_func_def_ev_trig(ran_func_def_ev_trig_t* src)
{
  assert(src != NULL);
  
  // Sequence of EVENT TRIGGER styles
  // [1 - 63]
  assert(src->sz_seq_ev_trg_style > 0 && src-> sz_seq_ev_trg_style < 64); 
  for(size_t i = 0; i < src-> sz_seq_ev_trg_style; ++i){
    free_seq_ev_trg_style(&src->seq_ev_trg_style[i]);
  }
  free(src->seq_ev_trg_style);

  // Sequence of RAN Parameters for L2 Variables
  // [0 - 65535]  
  for(size_t i = 0; i < src-> sz_seq_ran_param_l2_var; ++i){
    free_seq_ran_param_3(&src->seq_ran_param_l2_var[i]); 
  }
  if(src->seq_ran_param_l2_var != NULL)
    free(src->seq_ran_param_l2_var);

  //Sequence of Call Process Types
  // [0-65535]
  for(size_t i = 0; i < src-> sz_seq_call_proc_type; ++i){
    free_seq_call_proc_type(&src->seq_call_proc_type[i]);
  }
  if(src->seq_call_proc_type != NULL)
    free(src->seq_call_proc_type);

  // Sequence of RAN Parameters for Identifying UEs
  // 0-65535
  for(size_t i = 0; i < src->sz_seq_ran_param_id_ue; ++i){
    free_seq_ran_param_3(&src->seq_ran_param_id_ue[i]);
  }
  if(src->seq_ran_param_id_ue != NULL){
    free(src->seq_ran_param_id_ue);
  }

  // Sequence of RAN Parameters for Identifying Cells
  for(size_t i = 0; i < src-> sz_seq_ran_param_id_cell; ++i){
    free_seq_ran_param_3(&src->seq_ran_param_id_cell[i]);
  }
  if(src->seq_ran_param_id_cell != NULL)
    free(src->seq_ran_param_id_cell);

}

bool eq_ran_func_def_ev_trig(ran_func_def_ev_trig_t const* m0, ran_func_def_ev_trig_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Sequence of EVENT TRIGGER styles
  // [1 - 63]
  assert(m0->sz_seq_ev_trg_style > 0 && m0->sz_seq_ev_trg_style < 64); 
  assert(m1->sz_seq_ev_trg_style > 0 && m1->sz_seq_ev_trg_style < 64); 
  if(m0->sz_seq_ev_trg_style != m1->sz_seq_ev_trg_style){ 
  assert(0!=0);
    return false;
  }

  for(size_t i = 0; i < m0->sz_seq_ev_trg_style; ++i){
    if(eq_seq_ev_trg_style(&m0->seq_ev_trg_style[i], &m1->seq_ev_trg_style[i]) == false){
  assert(0!=0);
      return false;
    }
  }

  // Sequence of RAN Parameters for L2 Variables
  // [0 - 65535]  
  if(m0->sz_seq_ran_param_l2_var != m1->sz_seq_ran_param_l2_var){
  assert(0!=0);
    return false;
  }

  for(size_t i = 0; i < m0->sz_seq_ran_param_l2_var; ++i){
    if(eq_seq_ran_param_3(&m0->seq_ran_param_l2_var[i], &m1->seq_ran_param_l2_var[i] ) == false){
  assert(0!=0);
      return false;
    }
  }

  //Sequence of Call Process Types
  // [0-65535]
  if(m0-> sz_seq_call_proc_type != m1-> sz_seq_call_proc_type ){
  assert(0!=0);
    return false;
  }

  for(size_t i =0; i < m0-> sz_seq_call_proc_type; ++i){
    if(eq_seq_call_proc_type(&m0->seq_call_proc_type[i], &m1->seq_call_proc_type[i]) == false){
  assert(0!=0);
      return false;
    }
  }

  // Sequence of RAN Parameters for Identifying UEs
  // 0-65535
  if(m0->sz_seq_ran_param_id_ue != m1->sz_seq_ran_param_id_ue){
  assert(0!=0);
    return false;
  }
  for(size_t i = 0; i < m0->sz_seq_ran_param_id_ue; ++i){
    if(eq_seq_ran_param_3(&m0->seq_ran_param_id_ue[i], &m1->seq_ran_param_id_ue[i]) == false){
  assert(0!=0);
      return false;
    }
  }

  // Sequence of RAN Parameters for Identifying Cells
  // 0-65535
  if(m0-> sz_seq_ran_param_id_cell != m1-> sz_seq_ran_param_id_cell){
  assert(0!=0);
    return false;
  }
  for(size_t i = 0; i < m0-> sz_seq_ran_param_id_cell; ++i){
    if(eq_seq_ran_param_3(&m0-> seq_ran_param_id_cell[i], &m1->seq_ran_param_id_cell[i]) == false){
  assert(0!=0);
      return false;
    }
  }

  return true;
}

ran_func_def_ev_trig_t cp_ran_func_def_ev_trig(ran_func_def_ev_trig_t const* src)
{
  assert(src != NULL);
  ran_func_def_ev_trig_t dst = {0};

  // Sequence of EVENT TRIGGER styles
  // [1 - 63]
  assert(src->sz_seq_ev_trg_style > 0 && src->sz_seq_ev_trg_style < 64); 
  dst.sz_seq_ev_trg_style = src->sz_seq_ev_trg_style;  
  dst.seq_ev_trg_style = calloc(dst.sz_seq_ev_trg_style, sizeof(seq_ev_trg_style_t) ); 
  assert(dst.seq_ev_trg_style != NULL && "memory exhausted");

  for(size_t i = 0; i <  src->sz_seq_ev_trg_style; ++i){
    dst.seq_ev_trg_style[i] = cp_seq_ev_trg_style(&src->seq_ev_trg_style[i]);
  }

  // Sequence of RAN Parameters for L2 Variables
  // [0 - 65535]  
  assert(src-> sz_seq_ran_param_l2_var < 65536);
  dst.sz_seq_ran_param_l2_var = src-> sz_seq_ran_param_l2_var;
  dst. seq_ran_param_l2_var = calloc(dst.sz_seq_ran_param_l2_var, sizeof(seq_ran_param_3_t  ) ); 
  assert(dst. seq_ran_param_l2_var != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst. sz_seq_ran_param_l2_var; ++i){
    dst. seq_ran_param_l2_var[i] = cp_seq_ran_param_3(&src->seq_ran_param_l2_var[i]);
  }

  //Sequence of Call Process Types
  // [0-65535]
  assert(src->sz_seq_call_proc_type < 65536);
  dst.sz_seq_call_proc_type = src->sz_seq_call_proc_type;
  dst.seq_call_proc_type = calloc(dst. sz_seq_call_proc_type, sizeof(seq_call_proc_type_t ) );
  assert(dst.seq_call_proc_type != NULL && "memory exhausted");

  for(size_t i = 0; i < dst. sz_seq_call_proc_type; ++i){
    dst. seq_call_proc_type[i] = cp_seq_call_proc_type(&src->seq_call_proc_type[i]);
  }

  // Sequence of RAN Parameters for Identifying UEs
  // 0-65535
  assert(src->sz_seq_ran_param_id_ue < 65536);
  dst.sz_seq_ran_param_id_ue = src->sz_seq_ran_param_id_ue;
  dst.seq_ran_param_id_ue = calloc(dst.sz_seq_ran_param_id_ue, sizeof(seq_ran_param_3_t ) );
  assert(dst.seq_ran_param_id_ue != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ran_param_id_ue; ++i){
    dst.seq_ran_param_id_ue[i] = cp_seq_ran_param_3(&src->seq_ran_param_id_ue[i]) ; 
  }

  // Sequence of RAN Parameters for Identifying Cells
  // 0-65535
  assert(src-> sz_seq_ran_param_id_cell < 65536);
  dst.sz_seq_ran_param_id_cell = src-> sz_seq_ran_param_id_cell;
  dst.seq_ran_param_id_cell = calloc(dst. sz_seq_ran_param_id_cell, sizeof(seq_ran_param_3_t ));
  assert(dst.seq_ran_param_id_cell != NULL && "Memory exhausted");

  for(size_t i = 0; i < src-> sz_seq_ran_param_id_cell; ++i){
    dst.seq_ran_param_id_cell[i] = cp_seq_ran_param_3(&src->seq_ran_param_id_cell[i]);
  }

  return dst;
}

