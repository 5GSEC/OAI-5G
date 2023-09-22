#include <assert.h>

#include "kpm_ran_function_def.h"

void free_kpm_ran_function_def(kpm_ran_function_def_t* src)
{
  assert(src != NULL);
  
  // RAN Function Name
  free_ran_function_name(&src->name);

  // RIC Event Trigger Style List
  if (src->ric_event_trigger_style_list != NULL || src->sz_ric_event_trigger_style_list != 0)
  {
    for (size_t i = 0; i<src->sz_ric_event_trigger_style_list; i++)
      free_byte_array(src->ric_event_trigger_style_list[i].style_name);

    free(src->ric_event_trigger_style_list);
  }

  // RIC Report Style List
  if (src->ric_report_style_list != NULL || src->sz_ric_report_style_list != 0)
  {
    for (size_t i = 0; i<src->sz_ric_report_style_list; i++)
    {
      free_byte_array(src->ric_report_style_list[i].report_style_name);

      for (size_t j = 0; j<src->ric_report_style_list[i].meas_info_for_action_lst_len; j++)
      {
        free_byte_array(src->ric_report_style_list[i].meas_info_for_action_lst[j].name);

        if (src->ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL)
          free(src->ric_report_style_list[i].meas_info_for_action_lst[j].id);

        if (src->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def != NULL)
          free_kpm_bin_range_def(src->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def);
      }
      free(src->ric_report_style_list[i].meas_info_for_action_lst);
    }
    free(src->ric_report_style_list);
  }

}

bool eq_kpm_ran_function_def(kpm_ran_function_def_t const * m0, kpm_ran_function_def_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // RAN Function Name
  if (eq_ran_function_name(&m0->name, &m1->name) != true)
    return false;

  // RIC Event Trigger Style List
  if (m0->ric_event_trigger_style_list != NULL || m1->ric_event_trigger_style_list != NULL)
  {
    if (m0->sz_ric_event_trigger_style_list != m1->sz_ric_event_trigger_style_list)
      return false;

    for (size_t i = 0; i<m0->sz_ric_event_trigger_style_list; i++)
    {
      // RIC Event Trigger Style
      if (m0->ric_event_trigger_style_list[i].style_type != m1->ric_event_trigger_style_list[i].style_type)
        return false;

      switch (m0->ric_event_trigger_style_list[i].style_type)
      {
      case STYLE_1_RIC_EVENT_TRIGGER:
      {
        // RIC Event Trigger Style Name
        if (eq_byte_array(&m0->ric_event_trigger_style_list[i].style_name, &m1->ric_event_trigger_style_list[i].style_name) != true)
          return false;


        // RIC Event Trigger Format
        if (m0->ric_event_trigger_style_list[i].format_type != m1->ric_event_trigger_style_list[i].format_type)
          return false;
          
        break;
      }
      
      default:
        assert(false && "Unknown RIC Event Trigger Style Type");
      }
    }
  }

  // RIC Report Style List
  if (m0->ric_report_style_list != NULL || m1->ric_report_style_list != NULL)
  {
    if (m0->sz_ric_report_style_list != m1->sz_ric_report_style_list)
      return false;

    for (size_t i = 0; i<m0->sz_ric_report_style_list; i++)
    {
      // RIC Report Styles
      if (m0->ric_report_style_list[i].report_style_type != m1->ric_report_style_list[i].report_style_type)
        return false;

      if (m0->ric_report_style_list[i].act_def_format_type != m1->ric_report_style_list[i].act_def_format_type)
        return false;

      if (m0->ric_report_style_list[i].ind_hdr_format_type != m1->ric_report_style_list[i].ind_hdr_format_type)
        return false;

      if (m0->ric_report_style_list[i].ind_msg_format_type != m1->ric_report_style_list[i].ind_msg_format_type)
        return false;

      // RIC REPORT Style Name
      if (eq_byte_array(&m0->ric_report_style_list[i].report_style_name, &m1->ric_report_style_list[i].report_style_name) != true)
        return false;

      // Measurement Information for Action
      if (m0->ric_report_style_list[i].meas_info_for_action_lst_len != m1->ric_report_style_list[i].meas_info_for_action_lst_len)
        return false;

      for (size_t j = 0; j<m0->ric_report_style_list[i].meas_info_for_action_lst_len; j++)
      {
        // Measurement Type Name
        if (eq_byte_array(&m0->ric_report_style_list[i].meas_info_for_action_lst[j].name, &m1->ric_report_style_list[i].meas_info_for_action_lst[j].name) != true)
          return false;

        // Measurement Type ID
        if ((m0->ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL || m1->ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL) && *m0->ric_report_style_list[i].meas_info_for_action_lst[j].id != *m1->ric_report_style_list[i].meas_info_for_action_lst[j].id)
          return false;

        // Bin Range Definition
        // not yet implemented in ASN.1
        if (m0->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def != NULL || m1->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def != NULL)
        {
          if (eq_kpm_bin_range_def(m0->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def, m1->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def) != true)
            return false;
        }

      }
    
    }
  }

  return true;
}

static
ric_event_trigger_style_item_t cp_ric_event_trigger_style_list(ric_event_trigger_style_item_t const* src)
{
  assert(src != NULL);
  ric_event_trigger_style_item_t dst = {0}; 

  //style_ric_event_trigger_e style_type; // 8.3.3
  dst.style_type = src->style_type;

  //byte_array_t style_name;  // 8.3.4
  dst.style_name = copy_byte_array(src->style_name);

  //format_ric_event_trigger_e format_type;  // 8.3.5
  dst.format_type = src->format_type;

  return dst;
}

static
meas_info_for_action_lst_t cp_meas_info_for_action_lst( meas_info_for_action_lst_t const* src)
{
  assert(src != NULL);
  meas_info_for_action_lst_t dst = {0}; 

  //byte_array_t name; // 8.3.9
  dst.name = copy_byte_array(src->name);

  // uint16_t *id; // 8.3.10  -  OPTIONAL
  assert(src->id == NULL && "Not implemented");

  // bin_range_def_t *bin_range_def;  // 8.3.26  -  OPTIONAL; not yet implemented in ASN.1
  assert(src->bin_range_def == NULL && "Not implemented");

  return dst;
}

static
ric_report_style_item_t cp_ric_report_style_list(ric_report_style_item_t const* src)
{
  assert(src != NULL);
  ric_report_style_item_t dst = {0}; 

  //ric_service_report_e report_style_type;  // 8.3.3
  dst.report_style_type = src->report_style_type; 

  //byte_array_t report_style_name;  // 8.3.4
  dst.report_style_name = copy_byte_array(src->report_style_name); 


  //  format_action_def_e act_def_format_type;  // 8.3.5
  dst.act_def_format_type = src->act_def_format_type;

  //size_t meas_info_for_action_lst_len;  // [1, 65535]
  assert(src-> meas_info_for_action_lst_len > 0 &&  src->meas_info_for_action_lst_len <  65536);
  dst.meas_info_for_action_lst_len = src->meas_info_for_action_lst_len;

  dst.meas_info_for_action_lst = calloc(dst.meas_info_for_action_lst_len, sizeof(meas_info_for_action_lst_t) );
  assert(dst.meas_info_for_action_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_info_for_action_lst_len; ++i){
    dst.meas_info_for_action_lst[i] = cp_meas_info_for_action_lst(&src->meas_info_for_action_lst[i]);
  }

  // format_ind_hdr_e ind_hdr_format_type;  // 8.3.5
  dst.ind_hdr_format_type = src->ind_hdr_format_type; 

  // format_ind_msg_e ind_msg_format_type;  // 8.3.5
  dst.ind_msg_format_type = src->ind_msg_format_type;

  return dst;
}

kpm_ran_function_def_t cp_kpm_ran_function_def( kpm_ran_function_def_t  const* src)
{
  assert(src != NULL);
  kpm_ran_function_def_t dst = {0}; 

  //ran_function_name_t name;  // 8.3.2
  dst.name =  cp_ran_function_name(&src->name);

  assert(src->sz_ric_event_trigger_style_list < 64);
  if(src->sz_ric_event_trigger_style_list > 0){
    dst.sz_ric_event_trigger_style_list = src->sz_ric_event_trigger_style_list;
    dst.ric_event_trigger_style_list = calloc(dst.sz_ric_event_trigger_style_list, sizeof(ric_event_trigger_style_item_t));
    assert(dst.ric_event_trigger_style_list != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < src->sz_ric_event_trigger_style_list; ++i){
    dst.ric_event_trigger_style_list[i] = cp_ric_event_trigger_style_list(&src->ric_event_trigger_style_list[i]);
  }

  // [0-63]
  assert(src->sz_ric_report_style_list < 64);
  if(src->sz_ric_report_style_list > 0){
    dst.sz_ric_report_style_list = src->sz_ric_report_style_list;
    dst.ric_report_style_list = calloc(dst.sz_ric_report_style_list  , sizeof(ric_report_style_item_t));
  }
  for(size_t i = 0; i < dst. sz_ric_report_style_list; ++i){
    dst.ric_report_style_list[i] = cp_ric_report_style_list(&src->ric_report_style_list[i]);  
  }

  return dst;
}


