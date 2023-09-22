#include "seq_report_sty.h"

#include <assert.h>

void free_seq_report_sty(seq_report_sty_t* src)
{
  assert(src != NULL);
  // RIC Report Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  // uint32_t report_type;

  // RIC Report Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // PrintableString(SIZE(1..150,...)) 
  assert(src->name.len > 0 && src->name.len < 151); 
  free_byte_array(src->name);

  // Supported RIC Event Trigger Style Type 
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  // uint32_t ev_trig_type;

  // RIC Report Action Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  // uint32_t act_frmt_type;

  // RIC Indication Header Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  // uint32_t ind_hdr_type;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  // uint32_t ind_msg_type;

  // Sequence of RAN Parameters Supported
  // [0 - 65535]
  assert(src->sz_seq_ran_param <  65536);
  for(size_t i = 0; i < src-> sz_seq_ran_param; ++i){
   free_seq_ran_param_3(&src->ran_param[i]);
  } 
  if(src->ran_param != NULL)
    free(src->ran_param);
}

seq_report_sty_t cp_seq_report_sty(seq_report_sty_t const* src)
{
  assert(src != NULL);
  seq_report_sty_t dst = {0}; 
  // RIC Report Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.report_type = src->report_type; 

  // RIC Report Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // PrintableString(SIZE(1..150,...)) 
  assert(src->name.len > 0 && src->name.len < 151);
  dst.name = copy_byte_array(src->name);

  // Supported RIC Event Trigger Style Type 
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.ev_trig_type = src->ev_trig_type;

  // RIC Report Action Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.act_frmt_type = src->act_frmt_type;  

  // RIC Indication Header Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.ind_hdr_type = src->ind_hdr_type;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.ind_msg_type = src->ind_msg_type;

  // Sequence of RAN Parameters Supported
  // [0 - 65535]
  assert(src->sz_seq_ran_param < 65536);
  if(src->sz_seq_ran_param > 0){
    dst.sz_seq_ran_param = src->sz_seq_ran_param;
    dst.ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_3_t));
    assert(dst.ran_param != NULL && "Memory exhausted");
    
    for(size_t i = 0; i < src-> sz_seq_ran_param; ++i){
      dst.ran_param[i] = cp_seq_ran_param_3(&src->ran_param[i]);
    }

  }

  return dst;
}

bool eq_seq_report_sty(seq_report_sty_t const* m0, seq_report_sty_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RIC Report Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  if(m0->report_type != m1->report_type) 
    return false;

  // RIC Report Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // PrintableString(SIZE(1..150,...)) 
  assert(m0->name.len > 0 && m0->name.len < 151); 
  assert(m1->name.len > 0 && m1->name.len < 151); 
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // Supported RIC Event Trigger Style Type 
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  if(m0->ev_trig_type != m1->ev_trig_type)
    return false;

  // RIC Report Action Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  if(m0->act_frmt_type != m1->act_frmt_type)
    return false;

  // RIC Indication Header Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  if(m0->ind_hdr_type != m1->ind_hdr_type)
    return false;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  if(m0->ind_msg_type != m1->ind_msg_type)
    return false;

  // Sequence of RAN Parameters Supported
  // [0 - 65535]
  assert(m0->sz_seq_ran_param < 65536);
  assert(m1->sz_seq_ran_param < 65536);
 
  if(m0->sz_seq_ran_param != m1->sz_seq_ran_param)
    return false;

  for(size_t i = 0; i < m0->sz_seq_ran_param; ++i){
    if(eq_seq_ran_param_3(&m0->ran_param[i], &m1->ran_param[i]) == false)
      return false;
  }

  return true;
}

