#include "slice_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

void free_slice_event_trigger(slice_event_trigger_t* src)
{
  assert(src != NULL);

  assert(0 != 0 && "not implemented");
}

slice_event_trigger_t cp_slice_event_trigger( slice_event_trigger_t* src)
{
  assert(src != NULL);

  assert(0 != 0 && "not implemented");

  slice_event_trigger_t ans;
  return ans;
}

bool eq_slice_event_trigger(slice_event_trigger_t* m0, slice_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0 != 0 && "not implemented");

  return false;
}



//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

void free_slice_action_def(slice_action_def_t* src)
{
  assert(src != NULL);

  assert(0 != 0 && "not implemented");
}

slice_action_def_t cp_slice_action_def(slice_action_def_t* src)
{
  assert(src != NULL);
  assert(0 != 0 && "not implemented");

  slice_action_def_t ans;
  return ans;
}

bool eq_slice_action_def(slice_event_trigger_t* m0,  slice_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0 != 0 && "not implemented");

  return false;
}

static
void free_slice_param(slice_params_t* param)
{
  assert(param != NULL);

  if(param->type == SLICE_ALG_SM_V0_NONE){
   // no memory to be freed 
  } else if(param->type == SLICE_ALG_SM_V0_STATIC){
   // no memory to be freed 
  } else if(param->type == SLICE_ALG_SM_V0_NVS){
   // no memory to be freed 
  } else if(param->type == SLICE_ALG_SM_V0_SCN19){
   // no memory to be freed 
  } else if(param->type == SLICE_ALG_SM_V0_EDF){
    if(param->u.edf.len_over > 0 ){
      assert(param->u.edf.over != NULL);
      free(param->u.edf.over);
    }
  } else {
    assert("Unknown slice param");
  }
}

static
void free_ul_dl_slice_conf(ul_dl_slice_conf_t* conf)
{
  assert(conf != NULL);

  if(conf->len_sched_name > 0){
    free(conf->sched_name); 
  }

  if(conf->len_slices > 0){
    assert(conf->slices != NULL);  
   
    for(size_t i = 0; i < conf->len_slices; ++i){

      fr_slice_t* slice = &conf->slices[i];
      if(slice->len_label > 0){
        assert(slice->label != NULL);
        free(slice->label);
      } 

      if(slice->len_sched > 0){
        assert(slice->sched != NULL);
        free(slice->sched);
      }
      free_slice_param(&slice->params);
    }
    free(conf->slices);
  }
}

static
void free_slice_conf(slice_conf_t* conf)
{
 assert(conf != NULL);
 free_ul_dl_slice_conf(&conf->ul);
 free_ul_dl_slice_conf(&conf->dl);
}

static
void free_ue_slice_conf(ue_slice_conf_t* conf)
{
  assert(conf != NULL);
  if(conf->len_ue_slice > 0){
    assert(conf->ues != NULL);
    free(conf->ues);
  }
}

void free_slice_ind_msg(slice_ind_msg_t* msg)
{
  assert(msg != NULL);
  free_slice_conf(&msg->slice_conf);
  free_ue_slice_conf(&msg->ue_slice_conf);
}

static
void free_del_slice(del_slice_conf_t* slice)
{
  assert(slice != NULL);
  if(slice->len_dl > 0){
    assert(slice->dl != NULL);
    free(slice->dl);
  }

  if(slice->len_ul > 0){
    assert(slice->ul != NULL);
    free(slice->ul);
  }
} 

void slice_free_ctrl_msg(slice_ctrl_msg_t* msg)
{
  assert(msg != NULL);

  if(msg->type == SLICE_CTRL_SM_V0_ADD ){
    free_slice_conf(&msg->u.add_mod_slice);
  } else if(msg->type == SLICE_CTRL_SM_V0_DEL){
    free_del_slice(&msg->u.del_slice);
  } else if(msg->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
    free_ue_slice_conf(&msg->u.ue_slice);
  } else {
    assert(0 != 0 && "Unknown slice control type");
  }
}

void slice_free_ctrl_out(slice_ctrl_out_t* out)
{
  assert(out != NULL);
  if(out->len_diag > 0){
    assert(out->diagnostic != NULL);
    free(out->diagnostic);
  }
}

//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_slice_ind_hdr(slice_ind_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
} 

slice_ind_hdr_t cp_slice_ind_hdr(slice_ind_hdr_t const* src)
{
  assert(src != NULL);
  slice_ind_hdr_t dst = {.dummy = src->dummy}; 
  return dst;
}

bool eq_slice_ind_hdr(slice_ind_hdr_t* m0, slice_ind_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->dummy == m1->dummy;
}


//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////

static
bool eq_static_slice(static_slice_t const* m0, static_slice_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->pos_high == m1->pos_high 
        && m0->pos_low == m1->pos_low;
}

static
bool eq_nvs_rate(nvs_rate_t const* m0, nvs_rate_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->u2.mbps_reference == m1->u2.mbps_reference 
    && m0->u1.mbps_required == m1->u1.mbps_required;
}


static
bool eq_nvs_cap(nvs_capacity_t const* m0, nvs_capacity_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->u.pct_reserved == m1->u.pct_reserved;
}

static
bool eq_nvs_slice(nvs_slice_t const* m0, nvs_slice_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->conf != m1->conf)
    return false;

  bool ret = false;
  if(m0->conf == SLICE_SM_NVS_V0_RATE){
    ret = eq_nvs_rate(&m0->u.rate, &m1->u.rate);
  } else if (m0->conf == SLICE_SM_NVS_V0_CAPACITY){
    ret = eq_nvs_cap(&m0->u.capacity, &m1->u.capacity);
  } else {
    assert("Unknown type");
  }

  return ret;
}

bool eq_scn19_on_demand(scn19_on_demand_t const* m0, scn19_on_demand_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->log_delta == m1->log_delta 
        && m0->pct_reserved == m1->pct_reserved
        && m0->tau == m1->tau;
}

static
bool eq_scn19_slice(scn19_slice_t const* m0, scn19_slice_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->conf != m1->conf)
    return false;

  bool ret = false;
  if(m0->conf == SLICE_SCN19_SM_V0_DYNAMIC){
    ret = eq_nvs_rate(&m0->u.dynamic, &m1->u.dynamic); 

    if(ret == false)
      printf("Returning false from scn19 dyn\n");

  } else if(m0->conf == SLICE_SCN19_SM_V0_FIXED ){
    ret = eq_static_slice(&m0->u.fixed, &m1->u.fixed); 

    if(ret == false)
      printf("Returning false from scn19 static\n");

  } else if (m0->conf == SLICE_SCN19_SM_V0_ON_DEMAND ){
    ret = eq_scn19_on_demand(&m0->u.on_demand, &m1->u.on_demand);

     if(ret == false)
      printf("Returning false from scn19 on demand\n");
  }

     return ret; 
}

static
bool eq_edf_slice(edf_slice_t const* m0, edf_slice_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  for(size_t i = 0; i < m0->len_over; ++i){
    if(m0->over[i] != m1->over[i])
      return false;
  }

  return m0->deadline == m1->deadline
         && m0->guaranteed_prbs == m1->guaranteed_prbs
         && m0->max_replenish == m1->max_replenish
         && m0->len_over == m1->len_over;
}

static
bool eq_slice_params(slice_params_t const* m0, slice_params_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->type != m1->type)
    return false;

  bool ret = false;
  if(m0->type == SLICE_ALG_SM_V0_STATIC){
    ret = eq_static_slice(&m0->u.sta, &m1->u.sta);
  } else if(m0->type == SLICE_ALG_SM_V0_NVS){
    ret = eq_nvs_slice(&m0->u.nvs, &m1->u.nvs); 
  } else if(m0->type == SLICE_ALG_SM_V0_SCN19){
    ret = eq_scn19_slice(&m0->u.scn19, &m1->u.scn19);
  } else if(m0->type == SLICE_ALG_SM_V0_EDF){
    ret = eq_edf_slice(&m0->u.edf, &m1->u.edf);
  } else {
    assert("Unknown type");
  }

  return ret;
}

static
bool eq_slice(fr_slice_t const* m0, fr_slice_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->id != m1->id)
    return false;

  if(m0->len_label != m1->len_label)
    return false;

  if(memcmp(m0->label, m1->label, m0->len_label) != 0)
    return false;

  if(m0->len_sched != m1->len_sched)
    return false;

  return eq_slice_params(&m0->params, &m1->params); 
}

static
bool eq_ul_dl_slice_conf(ul_dl_slice_conf_t const* m0, ul_dl_slice_conf_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_sched_name != m1->len_sched_name)
    return false;

  if(m0->len_slices != m1->len_slices)
    return false;

  if(memcmp(m0->sched_name, m1->sched_name, m0->len_sched_name) != 0)
    return false;

  for(size_t i = 0; i < m0->len_slices; ++i) {
    if(eq_slice(&m0->slices[i], &m1->slices[i] ) == false){
      printf("eq slice returning false \n");
      return false;
    }
  }

  return true;
}

static
bool eq_slice_conf(slice_conf_t const* m0, slice_conf_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return eq_ul_dl_slice_conf(&m0->ul, &m1->ul) && eq_ul_dl_slice_conf(&m0->dl, &m1->dl); 
}

static
bool eq_ue_slice_assoc(ue_slice_assoc_t const* m0, ue_slice_assoc_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->dl_id == m1->dl_id
        && m0->rnti == m1->rnti
        && m0->ul_id == m1->ul_id;
}

static
bool eq_ue_slice_conf(ue_slice_conf_t const* m0, ue_slice_conf_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_ue_slice != m1->len_ue_slice)
    return false;

  for(size_t i = 0; i < m0->len_ue_slice; ++i){
    if(eq_ue_slice_assoc(&m0->ues[i], &m1->ues[i]) == false)
      return false;
  }

  return true;
}


bool eq_slice_ind_msg(slice_ind_msg_t const* m0, slice_ind_msg_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return eq_slice_conf(&m0->slice_conf, &m1->slice_conf) 
         && eq_ue_slice_conf(&m0->ue_slice_conf, &m1->ue_slice_conf)
         && m0->tstamp == m1->tstamp;
}



static
scn19_slice_t cp_scn19_slice(scn19_slice_t const* src)
{
  assert(src != NULL);
  scn19_slice_t dst = {0};

  dst.conf = src->conf;
  if(dst.conf == SLICE_SCN19_SM_V0_DYNAMIC){
    dst.u.dynamic.u2.mbps_reference = src->u.dynamic.u2.mbps_reference;
    dst.u.dynamic.u1.mbps_required = src->u.dynamic.u1.mbps_required;
  } else if(dst.conf == SLICE_SCN19_SM_V0_FIXED ) {
    dst.u.fixed.pos_high = src->u.fixed.pos_high;
    dst.u.fixed.pos_low = src->u.fixed.pos_low;
  } else if(dst.conf == SLICE_SCN19_SM_V0_ON_DEMAND ) {
    dst.u.on_demand.log_delta = src->u.on_demand.log_delta; 
    dst.u.on_demand.pct_reserved = src->u.on_demand.pct_reserved; 
    dst.u.on_demand.tau = src->u.on_demand.tau;
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static
slice_params_t cp_slice_params(slice_params_t const* src)
{
  assert(src != NULL);

  slice_params_t dst = {0};

  dst.type = src->type;

  if(src->type == SLICE_ALG_SM_V0_STATIC  ){
    dst.u.sta.pos_high = src->u.sta.pos_high;
    dst.u.sta.pos_low = src->u.sta.pos_low; 
  } else if(src->type == SLICE_ALG_SM_V0_NVS ){
    if(src->u.nvs.conf == SLICE_SM_NVS_V0_RATE){
      dst.u.nvs.conf =  SLICE_SM_NVS_V0_RATE;
      dst.u.nvs.u.rate.u2.mbps_reference = src->u.nvs.u.rate.u2.mbps_reference;
      dst.u.nvs.u.rate.u1.mbps_required = src->u.nvs.u.rate.u1.mbps_required;
    } else if(src->u.nvs.conf == SLICE_SM_NVS_V0_CAPACITY){
      dst.u.nvs.conf =  SLICE_SM_NVS_V0_CAPACITY;
      dst.u.nvs.u.capacity.u.pct_reserved = src->u.nvs.u.capacity.u.pct_reserved;
    } else {
      assert(0!=0 && "Not implemented or unknown");
    }
  } else if(src->type == SLICE_ALG_SM_V0_SCN19){
      dst.u.scn19 = cp_scn19_slice(&src->u.scn19);
  } else if(src->type == SLICE_ALG_SM_V0_EDF){
      dst.u.edf.deadline = src->u.edf.deadline;
      dst.u.edf.guaranteed_prbs = src->u.edf.guaranteed_prbs;
      dst.u.edf.max_replenish = src->u.edf.max_replenish;
      dst.u.edf.len_over = src->u.edf.len_over;
      for(size_t i = 0; i < src->u.edf.len_over; ++i)
        dst.u.edf.over[i] = src->u.edf.over[i];
  } else {
    assert(0!=0 && "Not implemented or unknown");
  }

  assert(eq_slice_params(&dst, src) == true);

  return dst;
}

static
fr_slice_t cpy_slice(fr_slice_t const* src)
{
  assert(src != NULL);
  fr_slice_t dst = {0};

  dst.id = src->id;
  dst.len_label = src->len_label;
  if(dst.len_label > 0){
    dst.label = malloc(src->len_label);
    memcpy(dst.label, src->label, src->len_label);
  }

  dst.len_sched = src->len_sched;
  if(src->len_sched > 0){
    dst.sched = malloc(src->len_sched);
    assert(dst.sched != NULL);
    memcpy(dst.sched, src->sched, src->len_sched);
  }

  dst.params = cp_slice_params(&src->params);

  assert(eq_slice(src, &dst) == true);

  return dst;
}

static
ul_dl_slice_conf_t cpy_ul_dl_slice_conf(ul_dl_slice_conf_t const* src)
{
  assert(src != NULL);
  ul_dl_slice_conf_t dst = {0};

  dst.len_sched_name = src->len_sched_name;
  dst.len_slices = src->len_slices;

  if(src->len_sched_name > 0){
    dst.sched_name = malloc(src->len_sched_name);
    assert(dst.sched_name != NULL && "memory exhausted");
    memcpy(dst.sched_name, src->sched_name, src->len_sched_name);
  }

  if (src->len_slices > 0) {
    dst.slices = calloc(src->len_slices, sizeof(fr_slice_t));
    assert(dst.slices != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < src->len_slices; ++i){
    dst.slices[i] = cpy_slice(&src->slices[i]); 
  }

  assert(eq_ul_dl_slice_conf(src, &dst) == true);
  return dst;
}

slice_conf_t cp_slice_conf(slice_conf_t const* src)
{
  assert(src != NULL);
  slice_conf_t dst = {0};

  dst.dl = cpy_ul_dl_slice_conf(&src->dl); 
  dst.ul = cpy_ul_dl_slice_conf(&src->ul); 

  assert(eq_slice_conf(src, &dst));

  return dst;
}

static
ue_slice_assoc_t cp_ue_slice_assoc(ue_slice_assoc_t const* src)
{
  assert(src != NULL);
  ue_slice_assoc_t dst = {0};

  dst.ul_id = src->ul_id;
  dst.dl_id = src->dl_id;
  dst.rnti = src->rnti;

  return dst;
}

ue_slice_conf_t cp_ue_slice_conf(ue_slice_conf_t const* src)
{
  assert(src != NULL);
  ue_slice_conf_t dst = {0}; 

  dst.len_ue_slice = src->len_ue_slice; 
  if(src->len_ue_slice > 0){
    dst.ues = calloc(src->len_ue_slice, sizeof(ue_slice_assoc_t)); 
    assert(dst.ues != NULL);
  }

  for(size_t i =0; i < src->len_ue_slice; ++i){
    dst.ues[i] = cp_ue_slice_assoc(&src->ues[i]);
  }

  return dst;
}

slice_ind_msg_t cp_slice_ind_msg(slice_ind_msg_t const* src)
{
  assert(src != NULL);

  slice_ind_msg_t out = {0};

  out.slice_conf = cp_slice_conf(&src->slice_conf);
  out.ue_slice_conf = cp_ue_slice_conf(&src->ue_slice_conf);
  out.tstamp = src->tstamp;

  assert(eq_slice_ind_msg(src, &out) );

  return out;
}


//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

void free_slice_call_proc_id( slice_call_proc_id_t* src)
{
  if(src == NULL) 
    return;

  assert(0!=0 && "Not implemented");
}

slice_call_proc_id_t cp_slice_call_proc_id( slice_call_proc_id_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented");
  slice_call_proc_id_t ans = {0}; 
  return ans;
}

bool eq_slice_call_proc_id(slice_call_proc_id_t* m0, slice_call_proc_id_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented");
  return false;
}






//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////

void free_slice_ctrl_hdr(slice_ctrl_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
} 

slice_ctrl_hdr_t cp_slice_ctrl_hdr(slice_ctrl_hdr_t* src)
{
  assert(src != NULL);
  slice_ctrl_hdr_t dst = {.dummy = src->dummy};
  return dst; 
}

bool eq_slice_ctrl_hdr(slice_ctrl_hdr_t const* m0, slice_ctrl_hdr_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->dummy == m1->dummy;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////

static
void free_del_slice_conf(del_slice_conf_t* conf)
{
  assert(conf != NULL);
  
  if(conf->len_dl > 0){
    assert(conf->dl != NULL);
    free(conf->dl);
  }

  if(conf->len_ul > 0){
    assert(conf->ul != NULL);
    free(conf->ul);
  }

}

void free_slice_ctrl_msg(slice_ctrl_msg_t* src)
{
  assert(src != NULL);

  if(src->type == SLICE_CTRL_SM_V0_ADD){
    free_slice_conf(&src->u.add_mod_slice);
  }else if(src->type == SLICE_CTRL_SM_V0_DEL){
    free_del_slice_conf(&src->u.del_slice);
  } else if(src->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
    free_ue_slice_conf(&src->u.ue_slice);
  } else {
    assert(0!=0 && "Unknown type");
  }
} 

static
del_slice_conf_t cp_del_slice(del_slice_conf_t const* src)
{
  assert(src != NULL);

  del_slice_conf_t dst = {0};

  if(src->len_dl > 0){
    dst.len_dl = src->len_dl;
    dst.dl = calloc(src->len_dl, sizeof(uint32_t));
    assert(dst.dl != NULL && "Memory exhausted");
    for(size_t i = 0; i < src->len_dl; ++i){
      dst.dl[i] = src->dl[i];
    }
  }

  if(src->len_ul > 0){
    dst.len_ul = src->len_ul;
    dst.ul = calloc(dst.len_ul, sizeof(uint32_t)); 
    assert(dst.ul != NULL && "Memory exhausted");
    for(size_t i = 0; i < src->len_ul; ++i){
      dst.ul[i] = src->ul[i];
    }
  }

  return dst;
}

slice_ctrl_msg_t cp_slice_ctrl_msg(slice_ctrl_msg_t* src)
{
  assert(src != NULL);

  slice_ctrl_msg_t dst = {0};
  dst.type = src->type;

  if(src->type == SLICE_CTRL_SM_V0_ADD){
   dst.u.add_mod_slice = cp_slice_conf(&src->u.add_mod_slice);
  } else if(src->type == SLICE_CTRL_SM_V0_DEL){
    dst.u.del_slice = cp_del_slice(&src->u.del_slice);
  } else if(src->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
    dst.u.ue_slice = cp_ue_slice_conf(&src->u.ue_slice);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static 
bool eq_del_slice(del_slice_conf_t const* m0, del_slice_conf_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_dl != m1->len_dl) return false;

  for(size_t i = 0; i < m0->len_dl; ++i){
    if(m0->dl[i] != m1->dl[i]) return false;
  }

  if(m0->len_ul != m1->len_ul) return false;

  for(size_t i = 0; i < m0->len_ul; ++i){
    if(m0->ul[i] != m1->ul[i]) return false;
  }

  return true;
}

bool eq_slice_ctrl_msg(slice_ctrl_msg_t const* m0, slice_ctrl_msg_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  bool ans= false;
  if(m0->type == SLICE_CTRL_SM_V0_ADD){
    ans = eq_slice_conf(&m0->u.add_mod_slice, &m1->u.add_mod_slice);    
  } else if(m0->type == SLICE_CTRL_SM_V0_DEL){
    ans = eq_del_slice ( &m0->u.del_slice, &m1->u.del_slice); 
  } else if(m0->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
    ans = eq_ue_slice_conf(&m0->u.ue_slice, &m1->u.ue_slice);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return ans;
}

//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////


void free_slice_ctrl_out(slice_ctrl_out_t* src)
{
  assert(src != NULL);

  if(src->len_diag > 0){
    assert(src->diagnostic != NULL);
    free(src->diagnostic);
  }
} 

slice_ctrl_out_t cp_slice_ctrl_out(slice_ctrl_out_t* src)
{
  assert(src != NULL);
  slice_ctrl_out_t dst = {.len_diag = src->len_diag }; 
  if(src->len_diag > 0){
    dst.diagnostic = malloc(src->len_diag);
    assert(dst.diagnostic != NULL);
    memcpy(dst.diagnostic, src->diagnostic, src->len_diag);
  }

  return dst;
}

bool eq_slice_ctrl_out(slice_ctrl_out_t* m0, slice_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_diag != m1->len_diag) return false;

  if(m0->len_diag > 0){
   return (memcmp(m0->diagnostic, m1->diagnostic, m0->len_diag) == 0);
  }

  return true;
}

//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

void free_slice_func_def( slice_func_def_t* src)
{
  assert(src != NULL);
  if(src->len_supported_alg > 0)
    free(src->supported_alg);
}

slice_func_def_t cp_slice_func_def(slice_func_def_t* src)
{
  assert(src != NULL);

  slice_func_def_t ans = {.len_supported_alg = src->len_supported_alg}; 
  if(ans.len_supported_alg > 0){
    ans.supported_alg = calloc(ans.len_supported_alg, sizeof( slice_algorithm_e ));
    assert( ans.supported_alg != NULL && "Memory exhauested");
  }
  for(size_t i = 0; i < ans.len_supported_alg; ++i){
    ans.supported_alg[i] = src->supported_alg[i];
  }

  return ans;
}

bool eq_slice_func_def(slice_func_def_t* m0, slice_func_def_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_supported_alg != m1->len_supported_alg)
    return false;

  for(size_t i = 0; i < m0->len_supported_alg; ++i){
    if(m0->supported_alg[i] != m1->supported_alg[i] )
      return false;
  }
  return true;
}

///////////////
// RIC Indication
///////////////

void free_slice_ind_data(slice_ind_data_t* ind)
{
  assert(ind != NULL);

  free_slice_ind_hdr(&ind->hdr);
  free_slice_ind_msg(&ind->msg);
  free_slice_call_proc_id(ind->proc_id);
}

slice_ind_data_t cp_slice_ind_data(slice_ind_data_t const* src)
{
  assert(src != NULL);


  slice_ind_data_t dst = {0};
  dst.hdr = cp_slice_ind_hdr(&src->hdr);
  dst.msg = cp_slice_ind_msg(&src->msg);

  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(slice_call_proc_id_t));
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_slice_call_proc_id(src->proc_id);
  }

  return dst;
}

