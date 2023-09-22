#include "seq_ins_sty.h"

#include <assert.h>


void free_seq_ins_sty(seq_ins_sty_t* src)
{
  assert(src != NULL);
  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  // uint32_t style_type;

  // RIC Insert Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151);
  free_byte_array(src->name);

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // uint32_t ev_trig_style_type;

  // RIC Action Definition Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // uint32_t act_def_frmt_type;

  // Sequence of Insert Indications
  // [0-65535]
  for(size_t i = 0; i < src->sz_seq_ins_ind; ++i){
    free_seq_ins_ind(&src->seq_ins_ind[i]);
  } 
  if(src->sz_seq_ins_ind > 0)
    free(src->seq_ins_ind);

  // RIC Indication Header Format Type
  // Mandatoyr
  // 9.3.5
  // 6.2.2.4.
  // uint32_t ind_hdr_frmt_type;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // uint32_t ind_msg_frmt_type;

  // RIC Call Process ID Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // uint32_t call_proc_id_type;
}

bool eq_seq_ins_sty(seq_ins_sty_t const* m0, seq_ins_sty_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  if(m0->style_type != m1->style_type)
    return false;

  // RIC Insert Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // [1-150]
  assert(m0->name.len > 0 && m0->name.len < 151);
  assert(m1->name.len > 0 && m1->name.len < 151);
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  if(m0->ev_trig_style_type != m1->ev_trig_style_type)
    return false;

  // RIC Action Definition Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  if(m0->act_def_frmt_type != m1->act_def_frmt_type)
    return false;

  // Sequence of Insert Indications
  // [0-65535]
  assert(m0->sz_seq_ins_ind < 65536); 
  assert(m1->sz_seq_ins_ind < 65536); 
  
  if(m0->sz_seq_ins_ind != m1->sz_seq_ins_ind)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ins_ind; ++i){
    if(eq_seq_ins_ind(&m0->seq_ins_ind[i], &m1->seq_ins_ind[i]) == false)
      return false;
  }

  // RIC Indication Header Format Type
  // Mandatoyr
  // 9.3.5
  // 6.2.2.4.
  if(m0->ind_hdr_frmt_type != m1->ind_hdr_frmt_type)
    return false;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  if(m0->ind_msg_frmt_type != m1->ind_msg_frmt_type)
    return false;

  // RIC Call Process ID Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  if(m0->call_proc_id_type != m1->call_proc_id_type)
    return false;

  return true;
}

seq_ins_sty_t cp_seq_ins_sty(seq_ins_sty_t const* src)
{
  assert(src != NULL);
seq_ins_sty_t dst = {0}; 
  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.style_type = src->style_type;

  // RIC Insert Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // [1-150]
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  dst.ev_trig_style_type = src->ev_trig_style_type;

  // RIC Action Definition Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  dst.act_def_frmt_type = src->act_def_frmt_type;

  // Sequence of Insert Indications
  // [0-65535]
  assert(src->sz_seq_ins_ind < 65536);
 
  if(src->sz_seq_ins_ind > 0){
    dst.sz_seq_ins_ind = src->sz_seq_ins_ind;
    dst.seq_ins_ind = calloc(src->sz_seq_ins_ind, sizeof(seq_ins_ind_t));
    assert(dst.seq_ins_ind != NULL && "memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_ins_ind; ++i){
    dst.seq_ins_ind[i] = cp_seq_ins_ind(&src->seq_ins_ind[i]);
  }

  // RIC Indication Header Format Type
  // Mandatoyr
  // 9.3.5
  // 6.2.2.4.
  dst.ind_hdr_frmt_type = src->ind_hdr_frmt_type;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  dst.ind_msg_frmt_type = src->ind_msg_frmt_type;

  // RIC Call Process ID Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  dst.call_proc_id_type = src->call_proc_id_type;

  return dst;
}

