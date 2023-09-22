#include "slice_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static inline
size_t next_pow2(size_t x)
{
  static_assert(sizeof(x) == 8, "Need this size to work correctly");
  x -= 1;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	x |= (x >> 32);
	
	return x + 1;
}

slice_event_trigger_t slice_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  slice_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

slice_action_def_t slice_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  slice_action_def_t act_def;// = {0};
  return act_def;
}

slice_ind_hdr_t slice_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(len == sizeof(slice_ind_hdr_t)); 
  slice_ind_hdr_t ret;
  memcpy(&ret, ind_hdr, len);
  return ret;
}

static
uint8_t* end;

static 
uint8_t* begin;


static
bool it_within_layout(uint8_t const* it)
{
  return it >= begin && it < end;  
}


static inline
size_t fill_static(static_slice_t* sta, uint8_t const* it)
{
  assert(it != NULL);
  assert(sta != NULL);
  assert(it_within_layout(it) == true);

  memcpy(&sta->pos_high, it, sizeof(sta->pos_high));
  it += sizeof(sta->pos_high);
  size_t sz = sizeof(sta->pos_high);

  memcpy(&sta->pos_low, it, sizeof(sta->pos_low));
  it += sizeof(sta->pos_low);
  sz += sizeof(sta->pos_low);

  return sz;
}

static inline
size_t fill_rate(nvs_rate_t* nvs, uint8_t const* it)
{
  assert(it != NULL);
  assert(nvs != NULL);

  memcpy(&nvs->u2.mbps_reference, it, sizeof(nvs->u2.mbps_reference));
  it += sizeof(nvs->u2.mbps_reference);
  size_t sz = sizeof(nvs->u2.mbps_reference);

  memcpy(&nvs->u1.mbps_required, it, sizeof(nvs->u1.mbps_required));
  it += sizeof(nvs->u1.mbps_required);
  sz += sizeof(nvs->u1.mbps_required);

  return sz;
}

static inline
size_t fill_capacity(nvs_capacity_t* nvs, uint8_t const* it)
{
  assert(it != NULL);
  assert(nvs != NULL);

  memcpy(&nvs->u.pct_reserved, it, sizeof(nvs->u.pct_reserved)); 
  it += sizeof(nvs->u.pct_reserved);
  size_t sz = sizeof(nvs->u.pct_reserved);

  return sz;
}

static inline
size_t fill_nvs(nvs_slice_t* nvs, uint8_t const* it)
{
  assert(it != NULL);
  assert(nvs != NULL);

  memcpy(&nvs->conf, it, sizeof(nvs->conf));
  it += sizeof(nvs->conf);


  size_t sz = sizeof(nvs->conf);

  if(nvs->conf == SLICE_SM_NVS_V0_RATE ){
    sz += fill_rate(&nvs->u.rate, it ); 
  } else if(nvs->conf == SLICE_SM_NVS_V0_CAPACITY){
    sz += fill_capacity(&nvs->u.capacity, it); 
  } else {
    assert(0!=0 && "Unknown type"); 
  }

  return sz;
}

static
size_t fill_on_demand(scn19_on_demand_t* scn, uint8_t const* it)
{
  assert(it != NULL);
  assert(scn != NULL);

  memcpy(&scn->log_delta, it, sizeof(scn->log_delta));
  it += sizeof(scn->log_delta);
  size_t sz = sizeof(scn->log_delta);

  memcpy(&scn->pct_reserved, it, sizeof(scn->pct_reserved));
  it += sizeof(scn->pct_reserved);
  sz += sizeof(scn->pct_reserved);

  memcpy(&scn->tau, it, sizeof(scn->tau));
  it += sizeof(scn->tau);
  sz += sizeof(scn->tau);

  return sz;
}


static
size_t fill_scn19(scn19_slice_t* scn, uint8_t const* it)
{
  assert(it != NULL);
  assert(scn != NULL);
 
  memcpy(&scn->conf, it, sizeof(scn->conf)); 
  it += sizeof(scn->conf);
  size_t sz = sizeof(scn->conf);

  if(scn->conf == SLICE_SCN19_SM_V0_DYNAMIC){
    sz += fill_rate(&scn->u.dynamic, it); 
  } else if(scn->conf == SLICE_SCN19_SM_V0_FIXED) {
    sz += fill_static(&scn->u.fixed, it);
  } else if (scn->conf == SLICE_SCN19_SM_V0_ON_DEMAND){
    sz += fill_on_demand(&scn->u.on_demand, it);
  } else {
    assert(0!=0 && "Unknown configuration");
  }

  return sz;
}


static
size_t fill_edf(edf_slice_t* edf, uint8_t const* it)
{
  assert(it != NULL);
  assert(edf != NULL);

  memcpy(&edf->deadline, it, sizeof(edf->deadline));
  it += sizeof(edf->deadline);
  size_t sz = sizeof(edf->deadline);

  memcpy(&edf->guaranteed_prbs, it, sizeof(edf->guaranteed_prbs));
  it += sizeof(edf->guaranteed_prbs);
  sz += sizeof(edf->guaranteed_prbs);

  memcpy(&edf->max_replenish, it, sizeof(edf->max_replenish));
  it += sizeof(edf->max_replenish);
  sz += sizeof(edf->max_replenish);

  memcpy(&edf->len_over, it, sizeof(edf->len_over));
  it += sizeof(edf->len_over);
  sz += sizeof(edf->len_over);

  if(edf->len_over > 0){
    edf->over = calloc(edf->len_over, sizeof(uint32_t));
    assert(edf->over != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < edf->len_over; ++i){
    memcpy(&edf->over[i], it, sizeof(uint32_t));
    it += sizeof(edf->over[i]);
    sz += sizeof(edf->over[i]);
  }

  return sz;
}

static inline
size_t fill_params(slice_params_t* par, uint8_t const* it)
{
  assert(par != NULL);
  assert(it != NULL);

  memcpy(&par->type, it, sizeof(par->type) );
  it += sizeof(par->type);
  size_t sz = sizeof(par->type);

  if(par->type == SLICE_ALG_SM_V0_STATIC ){
    sz += fill_static(&par->u.sta, it);  
  } else if(par->type == SLICE_ALG_SM_V0_NVS ){
    sz += fill_nvs(&par->u.nvs, it);
  } else if(par->type == SLICE_ALG_SM_V0_SCN19  ) {
    sz += fill_scn19(&par->u.scn19, it);
  } else if(par->type == SLICE_ALG_SM_V0_EDF ){
    sz += fill_edf(&par->u.edf, it);
  } else {
    assert(0!=0 && "Unknown parameter type");
  }

  return sz;
}

static inline
size_t fill_slice(fr_slice_t* slc, uint8_t const* it)
{
  assert(slc != NULL);
  assert(it != NULL);

  memcpy(&slc->id, it, sizeof(slc->id));
  it += sizeof(slc->id);
  size_t sz = sizeof(slc->id);

  memcpy(&slc->len_label, it, sizeof(slc->len_label) );
  it += sizeof(slc->len_label);
  sz += sizeof(slc->len_label);

  if(slc->len_label > 0){
    slc->label = calloc(1, slc->len_label + 1);
    assert(slc->label && "Memory exhausted");
  }

  memcpy(slc->label, it, slc->len_label);
  it += slc->len_label;
  sz += slc->len_label;

  memcpy(&slc->len_sched, it, sizeof(slc->len_sched));
  it += sizeof(slc->len_sched);
  sz += sizeof(slc->len_sched);

  if(slc->len_sched > 0){
    slc->sched = calloc(1, slc->len_sched + 1);
    assert(slc->sched != NULL && "Memory exhausted");
  }

  memcpy(slc->sched, it, slc->len_sched);
  it += slc->len_sched;
  sz += slc->len_sched;

  sz += fill_params(&slc->params, it);

  return sz;
}

static inline
size_t fill_ul_dl_slice_conf(ul_dl_slice_conf_t* conf, uint8_t const* it)
{
  assert(conf != NULL);
  assert(it != NULL);

  memcpy(&conf->len_sched_name, it, sizeof(conf->len_sched_name));

  //assert(conf->len_sched_name == 8);

  it += sizeof(conf->len_sched_name);
  size_t sz = sizeof(conf->len_sched_name);

  if(conf->len_sched_name > 0){
    conf->sched_name = calloc(1, conf->len_sched_name + 1);
    assert(conf->sched_name != NULL && "Memory exhausted");
  }

  memcpy(conf->sched_name, it, conf->len_sched_name);
  it += conf->len_sched_name;
  sz += conf->len_sched_name;

  memcpy(&conf->len_slices, it, sizeof(conf->len_slices));
  it += sizeof(conf->len_slices);
  sz += sizeof(conf->len_slices);

  if(conf->len_slices > 0){
    conf->slices = calloc(conf->len_slices, sizeof(fr_slice_t));
    assert(conf->slices != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < conf->len_slices; ++i){
    size_t const tmp = fill_slice(&conf->slices[i], it);
    it += tmp;
    sz += tmp;
  }

  return sz;
}

static
size_t fill_slice_conf(slice_conf_t* conf,  uint8_t const* it)
{
  assert(conf != NULL);
  assert(it != NULL);
  
  size_t sz = fill_ul_dl_slice_conf(&conf->dl, it); 
  it += sz;

  sz += fill_ul_dl_slice_conf(&conf->ul, it); 

  return sz;
}

static inline
size_t fill_ue_slice_assoc(ue_slice_assoc_t* assoc,uint8_t const* it)
{
  assert(it != NULL);
  assert(assoc != NULL);

  memcpy(&assoc->dl_id, it, sizeof(assoc->dl_id));
  it += sizeof(assoc->dl_id);
  size_t sz = sizeof(assoc->dl_id);

  memcpy(&assoc->ul_id, it, sizeof(assoc->ul_id));
  it += sizeof(assoc->ul_id);
  sz += sizeof(assoc->ul_id);

  memcpy(&assoc->rnti, it, sizeof(assoc->rnti));
  it += sizeof(assoc->rnti);
  sz += sizeof(assoc->rnti);

  return sz;
}



static inline
size_t fill_ue_slice_conf(ue_slice_conf_t* slc, uint8_t const* it)
{
  assert(it != NULL);
  assert(slc != NULL);

  memcpy(&slc->len_ue_slice, it, sizeof(slc->len_ue_slice));
  it += sizeof(slc->len_ue_slice);
  size_t sz = sizeof(slc->len_ue_slice);

  if(slc->len_ue_slice > 0){
    slc->ues = calloc(slc->len_ue_slice, sizeof(ue_slice_assoc_t));
    assert(slc->ues != NULL && "memory exhausted");
  }

  for(size_t i = 0; i < slc->len_ue_slice; ++i){
    size_t const tmp = fill_ue_slice_assoc(&slc->ues[i], it);
    it += tmp;
    sz += tmp;
  }

  return sz;
}



slice_ind_msg_t slice_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{

  slice_ind_msg_t ind = {0};

  uint8_t const* it = ind_msg;
  begin = (uint8_t*)it;
  end = begin + len;
  size_t sz = fill_slice_conf(&ind.slice_conf, it);
  it += sz;

  sz = fill_ue_slice_conf(&ind.ue_slice_conf, it);
  it += sz;

  memcpy(&ind.tstamp, it, sizeof(ind.tstamp));
  it += sizeof(ind.tstamp);

  assert(ind_msg + len == it && "Data layout mismacth");

  return ind;
}

slice_call_proc_id_t slice_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
}

slice_ctrl_hdr_t slice_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len == sizeof(slice_ctrl_hdr_t)); 
  slice_ctrl_hdr_t ret;
  memcpy(&ret, ctrl_hdr, len);
  return ret;
}

static
size_t fill_del_slice(del_slice_conf_t* conf, uint8_t const* it)
{
  assert(conf != NULL);
  assert(it != NULL);
  
  memcpy(&conf->len_dl, it, sizeof(conf->len_dl));
  it += sizeof(conf->len_dl);
  size_t sz = sizeof(conf->len_dl);

  if(conf->len_dl > 0){
    conf->dl = calloc(conf->len_dl, sizeof(uint32_t));
    assert(conf->dl != NULL && "memory exhausted");
  }

  for(size_t i = 0; i < conf->len_dl; ++i){
    memcpy(&conf->dl[i], it, sizeof(uint32_t));
    it += sizeof(uint32_t);
    sz += sizeof(uint32_t);
  }


  memcpy(&conf->len_ul, it, sizeof(conf->len_ul));
  it += sizeof(conf->len_ul);
  sz += sizeof(conf->len_ul);


  if(conf->len_ul > 0){
    conf->ul = calloc(conf->len_ul, sizeof(uint32_t));
    assert(conf->ul != NULL && "memory exhausted");
  }

  for(size_t i = 0; i < conf->len_ul; ++i){
    memcpy(&conf->ul[i], it, sizeof(uint32_t));
    it += sizeof(uint32_t);
    sz += sizeof(uint32_t);
  }

  return sz;
}

slice_ctrl_msg_t slice_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  slice_ctrl_msg_t ctrl = {0}; 
  
  uint8_t const* it = ctrl_msg; 
  begin = (uint8_t*)ctrl_msg;
  end = (uint8_t*)ctrl_msg + len;

  memcpy(&ctrl.type, it, sizeof(ctrl.type));
  it += sizeof(ctrl.type);
  size_t sz = sizeof(ctrl.type);

  if(ctrl.type == SLICE_CTRL_SM_V0_ADD){
    sz += fill_slice_conf(&ctrl.u.add_mod_slice, it); 
  } else if (ctrl.type == SLICE_CTRL_SM_V0_DEL){
    sz += fill_del_slice(&ctrl.u.del_slice, it); 
  } else if(ctrl.type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
    sz += fill_ue_slice_conf(&ctrl.u.ue_slice, it);
  } else {
    assert(0!=0 && "Unknonw type");
  }

  assert(sz == len && "Data layout mismatch");

  return ctrl;
}

slice_ctrl_out_t slice_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(ctrl_out!= NULL);

  slice_ctrl_out_t ret = {0}; 

  uint8_t* it = (uint8_t*)ctrl_out;
  memcpy(&ret.len_diag, it, sizeof(ret.len_diag));
  it += sizeof(ret.len_diag);

  if(ret.len_diag > 0){
    ret.diagnostic = malloc(ret.len_diag);
    assert(ret.diagnostic != NULL && "memory exhausted");
    memcpy(ret.diagnostic, it, ret.len_diag);
  }

  return ret;
}

slice_func_def_t slice_dec_func_def_plain(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(func_def != NULL);
}

