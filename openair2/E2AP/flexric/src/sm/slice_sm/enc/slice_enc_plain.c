#include "slice_enc_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

byte_array_t slice_enc_event_trigger_plain(slice_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
  byte_array_t  ba = {0};
 
  ba.len = sizeof(event_trigger->ms);
  ba.buf = malloc(ba.len);
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, &event_trigger->ms, ba.len);

  return ba;
}

byte_array_t slice_enc_action_def_plain(slice_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t slice_enc_ind_hdr_plain(slice_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);

  byte_array_t ba = {0};

  ba.len = sizeof(slice_ind_hdr_t);
  ba.buf = malloc(sizeof(slice_ind_msg_t));
  assert(ba.buf != NULL && "memory exhausted");
  memcpy(ba.buf, ind_hdr, sizeof(slice_ind_hdr_t));

  return ba;
}

static inline
size_t cal_static(static_slice_t const* sta)
{
  assert(sta != NULL);
  return sizeof(sta->pos_high) + sizeof(sta->pos_low);
}

static inline
size_t cal_nvs(nvs_slice_t const* nvs)
{
  assert(nvs != NULL);

  size_t sz = sizeof(nvs->conf);
  if(nvs->conf == SLICE_SM_NVS_V0_RATE){
    sz += sizeof(nvs->u.rate.u2.mbps_reference);
    sz += sizeof(nvs->u.rate.u1.mbps_required);
  } else if (nvs->conf == SLICE_SM_NVS_V0_CAPACITY){
    sz += sizeof(nvs->u.capacity.u.pct_reserved);
  } else { 
    assert(0!=0 && "Unknown nvs conf");
  }

  return sz ;
}

static inline
size_t cal_scn19(scn19_slice_t const* scn)
{
  assert(scn != NULL);

  size_t sz = sizeof(scn->conf);
  if(scn->conf == SLICE_SCN19_SM_V0_DYNAMIC){
    sz += sizeof(scn->u.dynamic.u2.mbps_reference);
    sz += sizeof(scn->u.dynamic.u1.mbps_required);
  } else if (scn->conf == SLICE_SCN19_SM_V0_FIXED){
    sz += sizeof(scn->u.fixed.pos_high);
    sz += sizeof(scn->u.fixed.pos_low);
  } else if(scn->conf == SLICE_SCN19_SM_V0_ON_DEMAND ) { 
    sz += sizeof(scn->u.on_demand.log_delta);
    sz += sizeof(scn->u.on_demand.pct_reserved);
    sz += sizeof(scn->u.on_demand.tau);
  } else {
    assert(0!=0 && "Unknown nvs conf");
  }
  return sz;
}

static inline
size_t cal_edf(edf_slice_t const* edf)
{
  assert(edf != NULL);

  size_t sz = sizeof(edf->deadline);
  sz += sizeof(edf->guaranteed_prbs);
  sz += sizeof(edf->max_replenish);
  sz += sizeof(edf->len_over);
  for(size_t i = 0; i < edf->len_over; ++i){
    sz += sizeof(edf->over[i]);
  }
  return sz;
}

static
size_t cal_params(slice_params_t* par)
{
  assert(par != NULL);

  size_t sz = 0;
  if(par->type == SLICE_ALG_SM_V0_STATIC){
    sz += cal_static(&par->u.sta);
  } else if (par->type == SLICE_ALG_SM_V0_NVS){
    sz += cal_nvs(&par->u.nvs);
  } else if (par->type == SLICE_ALG_SM_V0_SCN19 ){
    sz += cal_scn19(&par->u.scn19);
  } else if (par->type == SLICE_ALG_SM_V0_EDF ){
    sz += cal_edf(&par->u.edf);
  } else {
    assert(0 != 0 && "Unknown slicing type");
    // edf_slice_t edf;
    //  SLICE_SM_V0_EDF = 4
  }

  return sizeof(par->type) + sz;
}

static
size_t cal_slc(size_t len, fr_slice_t slc[len])
{
  size_t total = 0;
  for(size_t i = 0; i < len; ++i){
    size_t const sz_id = sizeof(slc[i].id);
    size_t const sz_lbl = sizeof(slc[i].len_label);
    size_t const sz_lab = slc[i].len_label;
    size_t const sz_sch = sizeof(slc[i].len_sched);
    size_t const sz_sched = slc[i].len_sched;
    size_t const sz_par = cal_params(&slc[i].params);
    total += sz_id + sz_lbl + sz_lab + sz_sch + sz_sched + sz_par;
  }

  return total;
}

static
size_t cal_ul_dl(ul_dl_slice_conf_t const* slc)
{
  assert(slc != NULL);

  size_t const len_sn = sizeof(slc->len_sched_name);
  size_t const len_slc = sizeof(slc->len_slices);  
  size_t const sz_slice = cal_slc(slc->len_slices, slc->slices);

  return len_sn + slc->len_sched_name + len_slc + sz_slice ; 
}

static
size_t cal_slice_conf(slice_conf_t const* slc)
{
  assert(slc != NULL);
  size_t ul = cal_ul_dl(&slc->ul);
  size_t dl = cal_ul_dl(&slc->dl);

  return ul+dl;
}

static
size_t cal_ue_slice_assoc(ue_slice_assoc_t* assoc)
{
  assert(assoc != NULL);
  return sizeof(assoc->dl_id) + sizeof(assoc->ul_id) + sizeof(assoc->rnti);
}

static
size_t cal_ue_slice_conf(ue_slice_conf_t const* slc)
{
  assert(slc != NULL);

  size_t sz = sizeof(slc->len_ue_slice);
  for(size_t i = 0; i < slc->len_ue_slice; ++i){
    sz += cal_ue_slice_assoc(&slc->ues[i]);
  }

  return sz;
}

static
size_t cal_ind_msg_payload(slice_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  size_t sz_conf = cal_slice_conf(&ind_msg->slice_conf);
  size_t sz_ues = cal_ue_slice_conf(&ind_msg->ue_slice_conf);
  size_t sz_tstamp = sizeof(int64_t);

  return sz_conf + sz_ues + sz_tstamp;
}

static
uint8_t* end;


static inline
size_t fill_static(uint8_t* it, static_slice_t* sta)
{
  assert(it != NULL);
  assert(sta != NULL);

  assert(it < end);

  memcpy(it, &sta->pos_high, sizeof(sta->pos_high));
  it += sizeof(sta->pos_high);
  size_t sz = sizeof(sta->pos_high);

  memcpy(it, &sta->pos_low, sizeof(sta->pos_low));
  it += sizeof(sta->pos_low);
  sz += sizeof(sta->pos_low);

  return sz;
}

static inline
size_t fill_rate(uint8_t* it, nvs_rate_t const* nvs)
{
  assert(it != NULL);
  assert(nvs != NULL);


  memcpy(it, &nvs->u2.mbps_reference, sizeof(nvs->u2.mbps_reference));
  it += sizeof(nvs->u2.mbps_reference);
  size_t sz = sizeof(nvs->u2.mbps_reference);

  memcpy(it, &nvs->u1.mbps_required, sizeof(nvs->u1.mbps_required));
  it += sizeof(nvs->u1.mbps_required);
  sz += sizeof(nvs->u1.mbps_required);

  return sz;
}

static inline
size_t fill_capacity(uint8_t* it, nvs_capacity_t const* nvs)
{
  assert(it != NULL);
  assert(nvs != NULL);
  
  memcpy(it, &nvs->u.pct_reserved, sizeof(nvs->u.pct_reserved)); 
  it += sizeof(nvs->u.pct_reserved);
  size_t sz = sizeof(nvs->u.pct_reserved);

  return sz;
}

static inline
size_t fill_nvs(uint8_t* it, nvs_slice_t const* nvs)
{
  assert(it != NULL);
  assert(nvs != NULL);
  

  memcpy(it, &nvs->conf, sizeof(nvs->conf));
  it += sizeof(nvs->conf);
  size_t sz = sizeof(nvs->conf);

  if(nvs->conf == SLICE_SM_NVS_V0_RATE ){
    sz += fill_rate(it, &nvs->u.rate); 
  } else if(nvs->conf == SLICE_SM_NVS_V0_CAPACITY){
    sz += fill_capacity(it, &nvs->u.capacity); 
  } else {
    assert(0!=0 && "Unknown type"); 
  }

  return sz;
}

static inline
size_t fill_on_demand(uint8_t* it, scn19_on_demand_t* scn)
{
  assert(it != NULL);
  assert(scn != NULL);

  memcpy(it, &scn->log_delta, sizeof(scn->log_delta));
  it += sizeof(scn->log_delta);
  size_t sz = sizeof(scn->log_delta);

  memcpy(it, &scn->pct_reserved, sizeof(scn->pct_reserved));
  it += sizeof(scn->pct_reserved);
  sz += sizeof(scn->pct_reserved);

  memcpy(it, &scn->tau, sizeof(scn->tau));
  it += sizeof(scn->tau);
  sz += sizeof(scn->tau);

  return sz;
}

static inline
size_t fill_scn19(uint8_t* it, scn19_slice_t* scn)
{
  assert(it != NULL);
  assert(scn != NULL);
 
  memcpy(it, &scn->conf, sizeof(scn->conf)); 
  it += sizeof(scn->conf);
  size_t sz = sizeof(scn->conf);

  if(scn->conf == SLICE_SCN19_SM_V0_DYNAMIC){
    sz += fill_rate(it, &scn->u.dynamic); 
  } else if(scn->conf == SLICE_SCN19_SM_V0_FIXED) {
    sz += fill_static(it, &scn->u.fixed);
  } else if (scn->conf == SLICE_SCN19_SM_V0_ON_DEMAND){
    sz += fill_on_demand(it, &scn->u.on_demand);
  } else {
    assert(0!=0 && "Unknown configuration");
  }

  return sz;
}

static inline
size_t fill_edf(uint8_t* it, edf_slice_t* edf)
{
  assert(it != NULL);
  assert(edf != NULL);

  memcpy(it, &edf->deadline, sizeof(edf->deadline));
  it += sizeof(edf->deadline);
  size_t sz = sizeof(edf->deadline);

  memcpy(it, &edf->guaranteed_prbs, sizeof(edf->guaranteed_prbs));
  it += sizeof(edf->guaranteed_prbs);
  sz += sizeof(edf->guaranteed_prbs);

  memcpy(it, &edf->max_replenish, sizeof(edf->max_replenish));
  it += sizeof(edf->max_replenish);
  sz += sizeof(edf->max_replenish);

  memcpy(it, &edf->len_over, sizeof(edf->len_over));
  it += sizeof(edf->len_over);
  sz += sizeof(edf->len_over);

  if(edf->len_over > 0){
    edf->over = calloc(edf->len_over, sizeof(uint32_t));
    assert(edf->over != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < edf->len_over; ++i){
    memcpy(it, &edf->over[i], sizeof(uint32_t));
    it += sizeof(edf->over[i]);
    sz += sizeof(edf->over[i]);
  }

  return sz;
}

static
size_t fill_params(uint8_t* it, slice_params_t* par)
{
  assert(it != NULL);
  assert(par != NULL);


  memcpy(it, &par->type, sizeof(par->type) );
  it += sizeof(par->type);
  size_t sz = sizeof(par->type);

  if(par->type == SLICE_ALG_SM_V0_STATIC ){
    sz += fill_static(it, &par->u.sta);  
  } else if(par->type == SLICE_ALG_SM_V0_NVS ){
    sz += fill_nvs(it, &par->u.nvs);
  } else if(par->type == SLICE_ALG_SM_V0_SCN19  ) {
    sz += fill_scn19(it, &par->u.scn19);
  } else if(par->type == SLICE_ALG_SM_V0_EDF ){
    sz += fill_edf(it, &par->u.edf);
  } else {
    assert(0!=0 && "Unknown parameter type");
  }

  return sz; 
}

static
size_t fill_slice(uint8_t* it, fr_slice_t* slc)
{
  assert(it != NULL);
  assert(slc != NULL);
  assert(it < end);

  memcpy(it, &slc->id, sizeof(slc->id));
  it += sizeof(slc->id);
  size_t sz = sizeof(slc->id);

  memcpy(it, &slc->len_label, sizeof(slc->len_label) );
  it += sizeof(slc->len_label);
  sz += sizeof(slc->len_label);

  memcpy(it, slc->label, slc->len_label);
  it += slc->len_label;
  sz += slc->len_label;

  memcpy(it, &slc->len_sched, sizeof(slc->len_sched));
  it += sizeof(slc->len_sched);
  sz += sizeof(slc->len_sched);

  memcpy(it, slc->sched, slc->len_sched);
  it += slc->len_sched;
  sz += slc->len_sched;

  sz += fill_params(it, &slc->params);

  return sz;
}

static
size_t fill_ul_dl_slice_conf(uint8_t* it, ul_dl_slice_conf_t const* conf)
{
  assert(it != NULL);
  assert(conf != NULL);

  memcpy(it, &conf->len_sched_name, sizeof(conf->len_sched_name)); 
  it += sizeof(conf->len_sched_name);
  size_t sz = sizeof(conf->len_sched_name);

  //printf(" conf->len_sched_name = %d\n", conf->len_sched_name );
  //fflush(stdout);

  memcpy(it, conf->sched_name, conf->len_sched_name);
  it += conf->len_sched_name;
  sz += conf->len_sched_name;

  memcpy(it, &conf->len_slices, sizeof(conf->len_slices));
  it += sizeof(conf->len_slices);
  sz += sizeof(conf->len_slices);

  for(size_t i = 0; i < conf->len_slices; ++i){
    size_t const tmp = fill_slice(it, &conf->slices[i]);
    it += tmp;
    sz += tmp;
  }
  return sz;
}

static
size_t fill_slice_conf(uint8_t* it, slice_conf_t const* conf)
{
  size_t sz = fill_ul_dl_slice_conf(it, &conf->dl);
  it += sz;

  sz += fill_ul_dl_slice_conf(it, &conf->ul);
  return sz;
}

static inline
size_t fill_ue_slice_assoc(uint8_t* it, ue_slice_assoc_t const* assoc)
{
  assert(it != NULL);
  assert(assoc != NULL);

  assert(it < end);

  memcpy(it, &assoc->dl_id, sizeof(assoc->dl_id));
  it += sizeof(assoc->dl_id);
  size_t sz = sizeof(assoc->dl_id);

  memcpy(it, &assoc->ul_id, sizeof(assoc->ul_id));
  it += sizeof(assoc->ul_id);
  sz += sizeof(assoc->ul_id);

  memcpy(it, &assoc->rnti, sizeof(assoc->rnti));
  it += sizeof(assoc->rnti);
  sz += sizeof(assoc->rnti);

  return sz;
}

static inline
size_t fill_ue_slice_conf(uint8_t* it, ue_slice_conf_t const* slc)
{
  assert(it != NULL);
  assert(slc != NULL);

  memcpy(it, &slc->len_ue_slice, sizeof(slc->len_ue_slice));
  it += sizeof(slc->len_ue_slice);
  size_t sz = sizeof(slc->len_ue_slice);

  for(size_t i = 0; i < slc->len_ue_slice; ++i){
    size_t const tmp = fill_ue_slice_assoc(it, &slc->ues[i]);
    
    it += tmp;
    sz += tmp;
  }

  return sz;
}


byte_array_t slice_enc_ind_msg_plain(slice_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  byte_array_t ba = {0};

  size_t sz = cal_ind_msg_payload(ind_msg);

  ba.buf = malloc(sz); 
  assert(ba.buf != NULL && "Memory exhausted");
  end = ba.buf + sz;

  uint8_t* it = ba.buf;
  size_t pos1 = fill_slice_conf(it, &ind_msg->slice_conf); 
  it += pos1;
  size_t pos2 = fill_ue_slice_conf(it, &ind_msg->ue_slice_conf);

  it += pos2;
  // tstamp
  memcpy(it, &ind_msg->tstamp, sizeof(ind_msg->tstamp));
  it += sizeof(ind_msg->tstamp);
  assert(it == ba.buf + sz && "Mismatch of data layout");

  ba.len = sz;
  return ba;
}

byte_array_t slice_enc_call_proc_id_plain(slice_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t slice_enc_ctrl_hdr_plain(slice_ctrl_hdr_t const* ctrl_hdr)
{
  assert(ctrl_hdr != NULL);

  byte_array_t ba = {0};

  ba.len = sizeof(slice_ind_hdr_t);
  ba.buf = malloc(sizeof(slice_ind_msg_t));
  assert(ba.buf != NULL && "memory exhausted");
  memcpy(ba.buf, ctrl_hdr, sizeof(slice_ctrl_hdr_t));

  return ba;
}

static
size_t cal_del_slice(del_slice_conf_t const* del)
{
  assert(del != 0);
  
  size_t sz = sizeof(del->len_dl);
  sz += del->len_dl*sizeof(uint32_t);

  sz += sizeof(del->len_ul);
  sz += del->len_ul*sizeof(uint32_t);

  return sz;
}

static
size_t cal_ctrl_msg_payload(slice_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  size_t sz = sizeof(ctrl_msg->type);

  if(ctrl_msg->type == SLICE_CTRL_SM_V0_ADD){
    sz += cal_slice_conf(&ctrl_msg->u.add_mod_slice);
  } else if(ctrl_msg->type == SLICE_CTRL_SM_V0_DEL ){
    sz += cal_del_slice(&ctrl_msg->u.del_slice);
  } else if(ctrl_msg->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC) {
    sz += cal_ue_slice_conf(&ctrl_msg->u.ue_slice);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return sz;
}

static
size_t fill_del_slice_conf(uint8_t* it, del_slice_conf_t const* conf)
{
  assert(it != NULL);
  assert(conf != NULL);

  memcpy(it, &conf->len_dl, sizeof(conf->len_dl));
  it += sizeof(conf->len_dl);
  size_t sz = sizeof(conf->len_dl);

  for(size_t i = 0; i < conf->len_dl; ++i){
    memcpy(it, &conf->dl[i], sizeof(uint32_t)); 
    it += sizeof(uint32_t);
    sz += sizeof(uint32_t);
  }

  memcpy(it, &conf->len_ul, sizeof(conf->len_ul));
  it += sizeof(conf->len_ul);
  sz += sizeof(conf->len_ul);

  for(size_t i = 0; i < conf->len_ul; ++i){
    memcpy(it, &conf->ul[i], sizeof(uint32_t)); 
    it += sizeof(uint32_t);
    sz += sizeof(uint32_t);
  }

  return sz;
}

static
size_t fill_slice_ctrl_msg(uint8_t* it, slice_ctrl_msg_t const* msg )
{
  assert(it != NULL);
  assert(msg != NULL);

  memcpy(it, &msg->type, sizeof(msg->type));
  it += sizeof(msg->type);
  size_t sz = sizeof(msg->type);

  if(msg->type == SLICE_CTRL_SM_V0_ADD ){
    sz += fill_slice_conf(it, &msg->u.add_mod_slice);
  } else if(msg->type == SLICE_CTRL_SM_V0_DEL  ){
    sz += fill_del_slice_conf(it, &msg->u.del_slice);
  } else if(msg->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC ) {
    sz += fill_ue_slice_conf(it, &msg->u.ue_slice);
  } else {
    assert(0 != 0 && "Unknown type");
  } 

  return sz;
}

byte_array_t slice_enc_ctrl_msg_plain(slice_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  byte_array_t ba = {0};
 
  size_t const sz = cal_ctrl_msg_payload(ctrl_msg);

  ba.buf = malloc(sz);
  assert(ba.buf != NULL && "Memory exhausted");
  ba.len = sz;

  end = ba.buf + sz;

  uint8_t* it = ba.buf;
  size_t const pos = fill_slice_ctrl_msg(it, ctrl_msg);

  assert(pos == sz && "Mismatch of data layout");

  return ba;
}

byte_array_t slice_enc_ctrl_out_plain(slice_ctrl_out_t const* ctrl) 
{
  assert(ctrl != NULL );
  byte_array_t ba = {0};

  ba.len = sizeof(ctrl->len_diag) + ctrl->len_diag;

  ba.buf = malloc(ba.len);
  assert(ba.buf != NULL && "Memory exhausted");
  uint8_t* it = ba.buf;

  memcpy(it, &ctrl->len_diag, sizeof(ctrl->len_diag));
  it += sizeof(ctrl->len_diag);

  memcpy(it, ctrl->diagnostic, ctrl->len_diag);
  it += ctrl->len_diag;

  assert(it == ba.buf + ba.len);

  return ba;
}

byte_array_t slice_enc_func_def_plain(slice_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

