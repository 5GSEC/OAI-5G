#include "tc_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
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

tc_event_trigger_t tc_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  tc_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

tc_action_def_t tc_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  tc_action_def_t act_def;// = {0};
  return act_def;
}

tc_ind_hdr_t tc_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(len == sizeof(tc_ind_hdr_t)); 
  tc_ind_hdr_t ret;
  memcpy(&ret, ind_hdr, len);
  return ret;
}

static
size_t tc_dec_sch_rr(uint8_t const* it, tc_sch_rr_t* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  return 0;
}

static
size_t tc_dec_sch_prio(uint8_t const* it, tc_sch_prio_t* p)
{
  assert(it != NULL);
  assert(p != NULL);

  memcpy(&p->len_q_prio, it, sizeof(p->len_q_prio));
  it += sizeof(p->len_q_prio);
  size_t sz = sizeof(p->len_q_prio);

  if(p->len_q_prio > 0)
    p->q_prio = calloc(p->len_q_prio, sizeof(uint32_t) );

  memcpy(p->q_prio, it, sizeof(uint32_t)*p->len_q_prio);
  it += sizeof(uint32_t)*p->len_q_prio;
  sz += sizeof(uint32_t)*p->len_q_prio;

  return sz;
}


static
size_t tc_dec_sch(uint8_t const* it, tc_sch_t* sch)
{
  assert(it != NULL);
  assert(sch != NULL);

  memcpy(&sch->type, it, sizeof(tc_sch_e));
  it += sizeof(tc_sch_e);
  size_t sz = sizeof(tc_sch_e);

  if(sch->type == TC_SCHED_RR ){
    size_t sz_rr = tc_dec_sch_rr(it, &sch->rr);
    it += sz_rr;
    sz += sz_rr;
  } else if(sch->type == TC_SCHED_PRIO){
    size_t sz_prio = tc_dec_sch_prio(it, &sch->prio);
    it += sz_prio;
    sz += sz_prio;
  } else {
    assert(0!=0 && "Unknown scheduler type"); 
  }

  return sz;
}

static
size_t tc_dec_mtr(uint8_t const* it, tc_mtr_t* mtr)
{
  assert(it != NULL);
  assert(mtr != NULL);

  memcpy(mtr->bnd_uint, it, sizeof(uint8_t)*4);
  it += sizeof(uint8_t)*4;
  size_t sz = sizeof(uint8_t)*4;

  memcpy(&mtr->time_window_ms, it, sizeof(uint32_t) );
  sz += sizeof(uint32_t);
  return sz;
}

static
size_t tc_dec_pcr(uint8_t const* it, tc_pcr_t* pcr)
{
  assert(it != NULL);
  assert(pcr != NULL);

  memcpy(&pcr->type, it, sizeof(tc_pcr_e));
  it += sizeof(tc_pcr_e);
  size_t sz = sizeof(tc_pcr_e);

  memcpy(&pcr->id, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  sz += tc_dec_mtr(it, &pcr->mtr);
  return sz;
}

static
size_t tc_dec_cls_rr(uint8_t const* it, tc_cls_rr_t* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(&rr->dummy, it, sizeof(uint32_t));
  return sizeof(uint32_t);
}

static
size_t tc_dec_cls_osi(uint8_t const* it, tc_cls_osi_t* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(&osi->len, it, sizeof(osi->len));
  it += sizeof(osi->len);
  size_t sz = sizeof(osi->len); 

  if(osi->len > 0){
    osi->flt = calloc(osi->len, sizeof( tc_cls_osi_filter_t) );
    assert(osi->flt != NULL && "Memory exhausted");
  }


  for(size_t i = 0; i < osi->len; ++i){
    memcpy(&osi->flt[i], it, sizeof(tc_cls_osi_filter_t) );
    it += sizeof(tc_cls_osi_filter_t);
    sz += sizeof(tc_cls_osi_filter_t);
  }

  return sz;
}

static
size_t tc_dec_cls_sto(uint8_t const* it, tc_cls_sto_t* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy(&sto->dummy, it, sizeof(uint32_t));
  return sizeof(uint32_t);
}

static
size_t tc_dec_cls(uint8_t const *it, tc_cls_t* cls)
{
  assert(it != NULL);
  assert(cls != NULL);

  memcpy(&cls->type, it, sizeof(tc_cls_e) );
  it += sizeof(tc_cls_e);
  size_t sz = sizeof(tc_cls_e);

  if(cls->type == TC_CLS_RR ){
    size_t sz_rr = tc_dec_cls_rr(it, &cls->rr);
    it += sz_rr;
    sz += sz_rr;
  } else if (cls->type == TC_CLS_OSI ){
    size_t sz_osi = tc_dec_cls_osi(it, &cls->osi);
    it += sz_osi;
    sz += sz_osi;
  } else if (cls->type == TC_CLS_STO){
    size_t sz_sto = tc_dec_cls_sto(it, &cls->sto);
    it += sz_sto;
    sz += sz_sto;
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}


static
size_t tc_dec_shp(uint8_t const* it, tc_shp_t* shp)
{
  assert(it != NULL);
  assert(shp != NULL);

  memcpy(&shp->id, it, sizeof(uint32_t) ); 
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(&shp->active, it, sizeof(uint32_t) ); 
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&shp->max_rate_kbps, it, sizeof(uint32_t) ); 
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  sz += tc_dec_mtr(it, &shp->mtr);
  return sz;
}

static
size_t tc_dec_drp(uint8_t const* it, tc_drp_t* drp)
{
  assert(it != NULL);
  assert(drp != NULL);

  memcpy(&drp->dropped_pkts, it, sizeof(uint32_t ) );
  // it += sizeof(uint32_t)
  size_t sz = sizeof(uint32_t);

  return sz;
}

static
size_t tc_dec_mrk(uint8_t const* it, tc_mrk_t* mrk)
{
  assert(it != NULL);
  assert(mrk != NULL);

  memcpy(&mrk->marked_pkts, it, sizeof(uint32_t));
  size_t sz = sizeof(uint32_t);
  // it += sz;
  return sz;
}

static
size_t tc_dec_plc(uint8_t const* it, tc_plc_t* plc)
{
  assert(it != NULL);
  assert(plc != NULL);

  memcpy(&plc->id, it, sizeof(uint32_t));
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  size_t const sz_mtr = tc_dec_mtr(it, &plc->mtr);
  it += sz_mtr;
  sz += sz_mtr;

  size_t const sz_drp = tc_dec_drp(it, &plc->drp);
  it += sz_drp;
  sz += sz_drp;

  size_t const sz_mrk = tc_dec_mrk(it, &plc->mrk);
  it += sz_mrk;
  sz += sz_mrk;

  memcpy(&plc->max_rate_kbps, it, sizeof(float) );
  it += sizeof(float);
  sz += sizeof(float);

  memcpy(&plc->active, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&plc->dst_id, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&plc->dev_id, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  return sz;
}

static
size_t tc_dec_q_fifo(uint8_t const* it, tc_queue_fifo_t* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(&q->bytes, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(&q->pkts, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&q->bytes_fwd, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&q->pkts_fwd, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  size_t const sz_drp = tc_dec_drp(it, &q->drp );
  it += sz_drp;
  sz += sz_drp;

  memcpy(&q->avg_sojourn_time, it, sizeof(float));
  it += sizeof(float);
  sz += sizeof(float);

  memcpy(&q->last_sojourn_time, it, sizeof(int64_t));
  it += sizeof(int64_t);
  sz += sizeof(int64_t);

  return sz;
}

static
size_t tc_dec_q_codel(uint8_t const* it, tc_queue_codel_t* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(&q->bytes, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(&q->pkts, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&q->bytes_fwd, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&q->pkts_fwd, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  size_t const sz_drp = tc_dec_drp(it, &q->drp );
  it += sz_drp;
  sz += sz_drp;

  memcpy(&q->avg_sojourn_time, it, sizeof(float));
  it += sizeof(float);
  sz += sizeof(float);

  memcpy(&q->last_sojourn_time, it, sizeof(int64_t));
  it += sizeof(int64_t);
  sz += sizeof(int64_t);


  return sz;
}

static
size_t tc_dec_q_ecn_codel(uint8_t const* it, tc_queue_ecn_codel_t* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(&q->bytes, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(&q->pkts, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&q->bytes_fwd, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(&q->pkts_fwd, it, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  size_t const sz_drp = tc_dec_mrk(it, &q->mrk );
  it += sz_drp;
  sz += sz_drp;

  memcpy(&q->avg_sojourn_time, it, sizeof(float));
  it += sizeof(float);
  sz += sizeof(float);

  memcpy(&q->last_sojourn_time, it, sizeof(int64_t));
  it += sizeof(int64_t);
  sz += sizeof(int64_t);


  return sz;
}

static
size_t tc_dec_q(uint8_t const* it, tc_queue_t* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(&q->type, it, sizeof( tc_queue_e ));
  size_t sz = sizeof(tc_queue_e);
  it += sizeof(tc_queue_e);

  if(q->type == TC_QUEUE_FIFO){
    size_t const sz_fifo = tc_dec_q_fifo(it, &q->fifo);
    sz += sz_fifo;
    it += sz_fifo;
  } else if(q->type == TC_QUEUE_CODEL){
    size_t const sz_codel = tc_dec_q_codel(it, &q->codel);
    sz += sz_codel;
    it += sz_codel;
  } else if(q->type == TC_QUEUE_ECN_CODEL){
    size_t const sz_ecn = tc_dec_q_ecn_codel(it, &q->ecn);
    sz += sz_ecn;
    it += sz_ecn;
  } else {
    assert(0!=0 && "unknown queue type");
  }

  return sz;
}

tc_ind_msg_t tc_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{
  tc_ind_msg_t ind = {0};

  uint8_t const* it = ind_msg;

  size_t const sz_sch = tc_dec_sch(it, &ind.sch);
  it += sz_sch;
  assert(it >= ind_msg && it < ind_msg + len && "iterator out of memory chunk");

  size_t const sz_pcr = tc_dec_pcr(it, &ind.pcr);
  it += sz_pcr;
  assert(it >= ind_msg && it < ind_msg + len && "iterator out of memory chunk");

  size_t const sz_cls = tc_dec_cls(it, &ind.cls);
  it += sz_cls;
  assert(it >= ind_msg && it < ind_msg + len && "iterator out of memory chunk");

  memcpy(&ind.len_q, it, sizeof(uint32_t));
  size_t const sz_len = sizeof(uint32_t);
  it += sz_len;
  assert(it >= ind_msg && it < ind_msg + len && "iterator out of memory chunk");

  if(ind.len_q > 0){
    ind.shp = calloc(ind.len_q, sizeof(tc_shp_t));
    ind.plc = calloc(ind.len_q, sizeof(tc_plc_t));
    ind.q = calloc(ind.len_q, sizeof(tc_queue_t));
  }

  size_t sz_shp_plc_q = 0;
  for(size_t i = 0 ; i < ind.len_q; ++i){
    size_t const sz_shp = tc_dec_shp(it, &ind.shp[i]);
    it += sz_shp; 
    assert(it >= ind_msg && it < ind_msg + len && "iterator out of memory chunk");

    size_t const sz_plc = tc_dec_plc(it, &ind.plc[i]);
    it += sz_plc;
    assert(it >= ind_msg && it < ind_msg + len && "iterator out of memory chunk");

    size_t const sz_q = tc_dec_q(it, &ind.q[i]);
    it += sz_q;
    assert(it >= ind_msg && it < ind_msg + len && "iterator out of memory chunk");

    //printf("sz_q = %lu sz_shp = %lu sz_plc = %lu \n",sz_q, sz_shp, sz_plc);

    sz_shp_plc_q += sz_shp + sz_plc + sz_q; 
  }

  memcpy(&ind.tstamp, it, sizeof(int64_t));
  size_t sz_tstamp = sizeof(int64_t); 
  it += sizeof(int64_t);

  assert(len == sz_sch + sz_pcr + sz_cls + sz_len + sz_shp_plc_q + sz_tstamp);

  assert(ind_msg + len == it && "Data layout mismacth");
  return ind;
}

tc_call_proc_id_t tc_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
}

tc_ctrl_hdr_t tc_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len == sizeof(tc_ctrl_hdr_t)); 
  tc_ctrl_hdr_t ret;
  memcpy(&ret, ctrl_hdr, len);
  return ret;
}

/*
static
size_t dec_tc_ctrl_payload_cls_rr(void const* it, tc_cls_rr_t* rr)  
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(&rr->dummy, it, sizeof(rr->dummy));
  // it += sizeof(rr->dummy);
  return sizeof(rr->dummy);
}


static
size_t dec_tc_ctrl_payload_cls_osi(void const* it, tc_cls_osi_t* osi)
{
  assert(osi != NULL);
  assert(it != NULL);

  memcpy(&osi->dummy, it, sizeof(osi->dummy));
  // it += sizeof(osi->dummy);
  return sizeof(osi->dummy);
}

static
size_t dec_tc_ctrl_payload_cls_sto(void const* it, tc_cls_sto_t* sto)
{
  assert(sto != NULL);
  assert(it != NULL);

  memcpy(&sto->dummy, it, sizeof(sto->dummy));
  // it += sizeof(sto->dummy);
  return sizeof(sto->dummy);
}
*/

static
size_t dec_tc_add_ctrl_payload_cls_rr(uint8_t const* it, tc_add_cls_rr_t* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(&rr->dummy, it, sizeof(rr->dummy ));
  size_t sz = sizeof(rr->dummy);
  //it += sizeof(rr->dummy);
  return sz;
}

static
size_t dec_tc_add_ctrl_payload_cls_osi(uint8_t const* it, tc_add_cls_osi_t* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(&osi->l3.src_addr, it, sizeof(osi->l3.src_addr) );
  it += sizeof(osi->l3.src_addr);
  size_t sz = sizeof(osi->l3.src_addr);

  memcpy(&osi->l3.dst_addr, it, sizeof(osi->l3.dst_addr) );
  it += sizeof(osi->l3.dst_addr);
  sz += sizeof(osi->l3.dst_addr);

  memcpy(&osi->l4.src_port, it, sizeof(osi->l4.src_port) );
  it += sizeof(osi->l4.src_port);
  sz += sizeof(osi->l4.src_port);

  memcpy(&osi->l4.dst_port, it, sizeof(osi->l4.dst_port) );
  it += sizeof(osi->l4.dst_port);
  sz += sizeof(osi->l4.dst_port);

  memcpy(&osi->l4.protocol, it, sizeof(osi->l4.protocol) );
  it += sizeof(osi->l4.protocol);
  sz += sizeof(osi->l4.protocol);

  //memcpy(it, osi->l4.protocol, sizeof(osi->l4.protocol) );
  //it += sizeof(osi->l4.protocol);
  //sz += sizeof(osi->l4.protocol);

  memcpy(&osi->dst_queue, it, sizeof(osi->dst_queue) );
  it += sizeof(osi->dst_queue );
  sz += sizeof(osi->dst_queue );

  return sz;
}

static
size_t dec_tc_add_ctrl_payload_cls_sto(uint8_t const* it, tc_add_cls_sto_t* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy(&sto->dummy, it, sizeof(sto->dummy ));
  size_t sz = sizeof(sto->dummy);
  //it += sizeof(rr->dummy);
  return sz;
}

static
size_t dec_tc_add_ctrl_payload_cls(uint8_t const* it,tc_add_ctrl_cls_t* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(&add->type, it, sizeof(add->type));
  it += sizeof(add->type);
  size_t sz = sizeof(add->type);

  if(add->type == TC_CLS_RR){
    sz += dec_tc_add_ctrl_payload_cls_rr(it, &add->rr);
  } else if(add->type == TC_CLS_OSI){
    sz += dec_tc_add_ctrl_payload_cls_osi(it, &add->osi);
  } else if(add->type == TC_CLS_STO){
    sz += dec_tc_add_ctrl_payload_cls_sto(it, &add->sto);
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}


static
size_t dec_tc_del_ctrl_payload_cls_rr(uint8_t const* it, tc_del_cls_rr_t* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy( &rr->dummy, it, sizeof(rr->dummy));
  // it += sizeof(rr->dummy );
  size_t sz = sizeof(rr->dummy);
  return sz;
}

static
size_t dec_tc_del_ctrl_payload_cls_osi(uint8_t const* it, tc_del_cls_osi_t* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(&osi->filter_id, it, sizeof(osi->filter_id) );
  // it += sizeof(osi->filter_id); 
  size_t sz = sizeof(osi->filter_id); 

  return sz;
}

static
size_t dec_tc_del_ctrl_payload_cls_sto(uint8_t const* it, tc_del_cls_sto_t* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy( &sto->dummy, it, sizeof(sto->dummy));
  // it += sizeof(sto->dummy);
  size_t sz = sizeof(sto->dummy);
  return sz;
}



static
size_t dec_tc_del_ctrl_payload_cls(uint8_t const* it, tc_del_ctrl_cls_t* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(&del->type, it, sizeof(del->type));
  it += sizeof(del->type);
  size_t sz = sizeof(del->type);

  if(del->type == TC_CLS_RR){
    sz += dec_tc_del_ctrl_payload_cls_rr(it, &del->rr);
  } else if(del->type == TC_CLS_OSI){
    sz += dec_tc_del_ctrl_payload_cls_osi(it, &del->osi);
  } else if (del->type == TC_CLS_STO){
    sz += dec_tc_del_ctrl_payload_cls_sto(it, &del->sto);
  } else {
    assert(0!=0 && "Unknown classifier type" );
  }

  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_cls_rr(uint8_t const* it, tc_mod_cls_rr_t* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(&rr->dummy, it, sizeof(rr->dummy));
  // it += sizeof(rr->dummy);
  size_t sz = sizeof(rr->dummy);
  return sz;
}


static
size_t dec_tc_mod_ctrl_payload_cls_osi(uint8_t const* it, tc_mod_cls_osi_t* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(&osi->filter, it, sizeof(tc_cls_osi_filter_t) );
//  it += sizeof(osi->filter);
  size_t sz = sizeof(osi->filter);
  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_cls_sto(uint8_t const* it, tc_mod_cls_sto_t* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy(&sto->dummy, it, sizeof(sto->dummy));
  // it += sizeof(rr->dummy);
  size_t sz = sizeof(sto->dummy);
  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_cls(uint8_t const* it, tc_mod_ctrl_cls_t* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy(&mod->type, it, sizeof(mod->type ) );
  it += sizeof(mod->type);  
  size_t sz = sizeof(mod->type); 

  if(mod->type == TC_CLS_RR){
    sz += dec_tc_mod_ctrl_payload_cls_rr(it, &mod->rr);
  } else if(mod->type == TC_CLS_OSI){
    sz += dec_tc_mod_ctrl_payload_cls_osi(it, &mod->osi);
  } else if(mod->type == TC_CLS_STO){
    sz += dec_tc_mod_ctrl_payload_cls_sto(it, &mod->sto);
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}



static
size_t dec_tc_ctrl_payload_cls(void const* it, tc_ctrl_cls_t* cls )
{
  assert(it != NULL);
  assert(cls != NULL);

  memcpy(&cls->act, it, sizeof(cls->act) );
  it += sizeof(cls->act);
  size_t sz = sizeof(cls->act);

  if(cls->act == TC_CTRL_ACTION_SM_V0_ADD ){
     sz += dec_tc_add_ctrl_payload_cls(it, &cls->add);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_DEL ){
     sz += dec_tc_del_ctrl_payload_cls(it, &cls->del);
  } else if(cls->act ==  TC_CTRL_ACTION_SM_V0_MOD){
     sz += dec_tc_mod_ctrl_payload_cls(it, &cls->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }

  return sz;




  /*
  if(cls->type == TC_CLS_RR){
     size_t sz_rr = dec_tc_ctrl_payload_cls_rr(it, &cls->rr);  
     it += sz_rr; 
     sz += sz_rr;
  } else if(cls->type == TC_CLS_OSI){
     size_t sz_osi = dec_tc_ctrl_payload_cls_osi(it, &cls->osi); 
     it += sz_osi;
     sz += sz_osi;
  } else if(cls->type == TC_CLS_STO){
    size_t sz_sto = dec_tc_ctrl_payload_cls_sto(it, &cls->sto);
     it += sz_sto;
     sz += sz_sto;
  } 

  return sz;
  */
}

 /* 
static
size_t dec_tc_ctrl_payload_plc(void const* it, tc_ctrl_plc_t* plc)
{
  assert(it != NULL);
  assert(plc != NULL);

  memcpy(&plc->id, it, sizeof(plc->id));
  it += sizeof(plc->id);
  size_t sz = sizeof(plc->id);

  memcpy(&plc->drop_rate_kbps, it, sizeof(plc->drop_rate_kbps));
  it += sizeof(plc->drop_rate_kbps);
  sz += sizeof(plc->drop_rate_kbps);

  memcpy(&plc->dev_id, it, sizeof(plc->dev_id ));
  it += sizeof(plc->dev_id);
  sz += sizeof(plc->dev_id);

  memcpy(&plc->dev_rate_kbps, it, sizeof(plc->dev_rate_kbps ));
  it += sizeof(plc->dev_rate_kbps);
  sz += sizeof(plc->dev_rate_kbps);

  memcpy(&plc->active, it, sizeof(plc->active));
  it += sizeof(plc->active);
  sz += sizeof(plc->active);

  return sz;
}

*/


static
size_t dec_tc_ctrl_payload_q_fifo(void const* it, tc_ctrl_queue_fifo_t* fifo)
{
  assert(it != NULL);
  assert(fifo != NULL);

  memcpy(&fifo->dummy, it, sizeof(fifo->dummy));
  // it += sizeof(fifo->dummy);
  return sizeof(fifo->dummy);

}

static
size_t dec_tc_ctrl_payload_q_codel(void const* it, tc_ctrl_queue_codel_t* codel)
{
  assert(it != NULL);
  assert(codel != NULL);

  memcpy(&codel->target_ms, it, sizeof(codel->target_ms));
  size_t sz = sizeof(codel->target_ms);
  it += sizeof(codel->target_ms);

  memcpy(&codel->interval_ms, it, sizeof(codel->interval_ms));
  sz += sizeof(codel->interval_ms);
  //it += sizeof(codel->target_ms);

  return sz;
}

static
size_t dec_tc_ctrl_payload_q_ecn_codel(void const* it, tc_ctrl_queue_ecn_codel_t* ecn)
{
  assert(it != NULL);
  assert(ecn != NULL);

  memcpy(&ecn->target_ms, it, sizeof(ecn->target_ms));
  size_t sz = sizeof(ecn->target_ms);
  it += sizeof(ecn->target_ms);

  memcpy(&ecn->interval_ms, it, sizeof(ecn->interval_ms));
  sz += sizeof(ecn->interval_ms);
  //it += sizeof(codel->target_ms);

  return sz;
}

/*
static
size_t dec_tc_ctrl_payload_q_add(uint8_t const *it, tc_add_ctrl_queue_t* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(&add->type,it, sizeof(add->type));
  size_t sz = sizeof(add->type);
  it += sizeof(add->type);

  if(add->type == TC_QUEUE_FIFO){
    sz += dec_tc_ctrl_payload_q_fifo(it , &add->fifo);
  } else if(add->type == TC_QUEUE_CODEL){
    sz += dec_tc_ctrl_payload_q_codel(it , &add->codel);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return sz;
}

static
size_t dec_tc_ctrl_payload_q_del(uint8_t const *it, tc_del_ctrl_queue_t* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(&del->id, it, sizeof(del->id));
  size_t sz = sizeof(del->id);
  it += sizeof(del->id);

  memcpy(&del->type,it, sizeof(del->type));
  sz += sizeof(del->type);
  //it += sizeof(del->type);
  return sz;
}

static
size_t dec_tc_ctrl_payload_q_mod(uint8_t const *it, tc_mod_ctrl_queue_t* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy(&mod->id, it, sizeof(mod->id));
  size_t sz = sizeof(mod->id);
  it += sizeof(mod->id);

  memcpy(&mod->type,it, sizeof(mod->type));
  sz += sizeof(mod->type);
  it += sizeof(mod->type);

  if(mod->type == TC_QUEUE_FIFO){
    sz += dec_tc_ctrl_payload_q_fifo(it , &mod->fifo);
  } else if(mod->type == TC_QUEUE_CODEL){
    sz += dec_tc_ctrl_payload_q_codel(it , &mod->codel);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return sz;
}

*/

/*
static
size_t dec_tc_ctrl_payload_q(void const* it, tc_ctrl_queue_t* q, tc_ctrl_act_e act)
{
  assert(it != NULL);
  assert(q != NULL);

  if(act == TC_CTRL_ACTION_SM_V0_ADD ){
    return dec_tc_ctrl_payload_q_add(it, &q->add );
  } else if (act == TC_CTRL_ACTION_SM_V0_DEL ){
    return dec_tc_ctrl_payload_q_del(it, &q->del);
  } else if(act == TC_CTRL_ACTION_SM_V0_MOD){
    return dec_tc_ctrl_payload_q_mod(it, &q->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }

  assert(0!=0 && "Impossible path");
  exit(-1);

  return 0;
  }
  */

  /*
  memcpy(&q->type, it, sizeof(q->type));
  it += sizeof(q->type);
  size_t sz = sizeof(q->type);

  memcpy(&q->id, it, sizeof(q->id) );
  it += sizeof(q->id);
  sz += sizeof(q->id);

  if(q->type == TC_QUEUE_STATS_FIFO){
    size_t const sz_fifo = dec_tc_ctrl_payload_q_fifo(it, &q->fifo);
    sz += sz_fifo;
  } else if (q->type == TC_QUEUE_STATS_CODEL ){
    size_t const sz_codel = dec_tc_ctrl_payload_q_codel(it, &q->codel);
    sz += sz_codel;
  } else {
    assert(0!=0 && "Unknown type" );
  }

  return sz;
*/

/*
static
size_t dec_tc_ctrl_payload_sch(void const* it, tc_ctrl_sch_t* sch)
{
  assert(it != NULL);
  assert(sch != NULL);

  memcpy(&sch->type, it, sizeof(sch->type) );
  it += sizeof(sch->type);
  size_t sz = sizeof(sch->type);

  if(sch->type == TC_SCHED_RR){


  } else if (sch->type == TC_SCHED_PRIO ){
    memcpy(&sch->prio.len_q_prio, it, sizeof(sch->prio.len_q_prio));
    it += sizeof(sch->prio.len_q_prio);
    sz += sizeof(sch->prio.len_q_prio);

    memcpy(&sch->prio.q_prio, it, sizeof(sch->prio.q_prio)*sch->prio.len_q_prio);
    it += sizeof(sch->prio.q_prio)*sch->prio.len_q_prio;
    sz += sizeof(sch->prio.q_prio)*sch->prio.len_q_prio;

  } else {
    assert(0!=0 && "Unknown type" );
  }

  return sz; 
}
*/
/*
static
size_t dec_tc_ctrl_payload_shp(void const* it, tc_ctrl_shp_t* shp)
{
  assert(it != NULL);
  assert(shp != NULL);

  memcpy(&shp->id, it, sizeof(shp->id) );
  it += sizeof(shp->id);
  size_t sz = sizeof(shp->id);

  memcpy(&shp->time_window_ms, it, sizeof(shp->time_window_ms));
  it += sizeof(shp->time_window_ms);
  sz += sizeof(shp->time_window_ms);

  memcpy(&shp->max_rate_kbps, it, sizeof(shp->max_rate_kbps));
  it += sizeof(shp->max_rate_kbps);
  sz += sizeof(shp->max_rate_kbps);

  memcpy(&shp->active, it, sizeof(shp->active));
  it += sizeof(shp->active);
  sz += sizeof(shp->active);

  return sz;
}
*/
/*
static
size_t dec_tc_ctrl_payload_pcr(void const* it, tc_ctrl_pcr_t* pcr)
{
  assert(it != NULL);
  assert(pcr != NULL);

  memcpy(&pcr->type, it, sizeof(pcr->type) );
  it += sizeof(pcr->type); 
  size_t sz = sizeof(pcr->type);

  if(pcr->type == TC_PCR_DUMMY){

  } else if(pcr->type == TC_PCR_5G_BDP ){
    memcpy(&pcr->bdp.drb_sz, it, sizeof(pcr->bdp.drb_sz) );
    it += sizeof(pcr->bdp.drb_sz);
    sz += sizeof(pcr->bdp.drb_sz);

    memcpy( &pcr->bdp.tstamp, it, sizeof(pcr->bdp.tstamp) );
    it += sizeof(pcr->bdp.tstamp);
    sz += sizeof(pcr->bdp.tstamp);

  } else {
    assert(0!=0 && "Unknown type" );
  }

  return sz;
}
*/
static
size_t dec_tc_add_ctrl_payload_plc(uint8_t const* it, tc_add_ctrl_plc_t* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(&add->dummy, it, sizeof(add->dummy));
// it +=  sizeof(add->dummy) 
  size_t sz = sizeof(add->dummy);
  return sz; 
}

static
size_t dec_tc_del_ctrl_payload_plc(uint8_t const* it, tc_del_ctrl_plc_t* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(&del->id, it, sizeof(del->id ));
// it +=  sizeof(add->dummy) 
  size_t sz = sizeof(del->id);
  return sz; 
}

static
size_t dec_tc_mod_ctrl_payload_plc(uint8_t const* it, tc_mod_ctrl_plc_t* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy(&mod->id, it, sizeof(mod->id) );
  it += sizeof(mod->id);
  size_t sz = sizeof(mod->id);

  memcpy( &mod->drop_rate_kbps, it, sizeof(mod->drop_rate_kbps) );
  it += sizeof(mod->drop_rate_kbps);
  sz += sizeof(mod->drop_rate_kbps);

  memcpy( &mod->dev_id, it, sizeof(mod->dev_id) );
  it += sizeof(mod->dev_id);
  sz += sizeof(mod->dev_id);

  memcpy( &mod-> dev_rate_kbps, it, sizeof(mod->dev_rate_kbps) );
  it += sizeof(mod-> dev_rate_kbps);
  sz += sizeof(mod-> dev_rate_kbps);

  memcpy( &mod-> active, it, sizeof(mod->active));
  it += sizeof(mod-> active);
  sz += sizeof(mod-> active);

  return sz;
}



static
size_t dec_tc_ctrl_payload_plc(uint8_t const* it, tc_ctrl_plc_t* plc)
{
  assert(it != NULL);
  assert(plc != NULL);

  memcpy(&plc->act, it, sizeof(plc->act));
  it += sizeof(plc->act);
  size_t sz = sizeof(plc->act);

  if(plc->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += dec_tc_add_ctrl_payload_plc(it, &plc->add);
  } else if(plc->act == TC_CTRL_ACTION_SM_V0_DEL ){
    sz += dec_tc_del_ctrl_payload_plc(it, &plc->del);
  } else if(plc->act == TC_CTRL_ACTION_SM_V0_MOD ){
    sz += dec_tc_mod_ctrl_payload_plc(it, &plc->mod);
  } else {
    assert(0!=0 && "Unknown action type " );
  }

  return sz;
}

static
size_t dec_tc_add_ctrl_payload_q(uint8_t const* it, tc_add_ctrl_queue_t* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(&add->type, it, sizeof(add->type)); 
  it += sizeof(add->type);
  size_t sz = sizeof(add->type); 

  if(add->type == TC_QUEUE_FIFO){
    sz += dec_tc_ctrl_payload_q_fifo(it, &add->fifo);
  } else if (add->type == TC_QUEUE_CODEL){
    sz += dec_tc_ctrl_payload_q_codel(it, &add->codel);
  } else if (add->type == TC_QUEUE_ECN_CODEL){
    sz += dec_tc_ctrl_payload_q_ecn_codel(it, &add->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return sz;
}

static
size_t dec_tc_del_ctrl_payload_q(uint8_t const* it, tc_del_ctrl_queue_t* del)
{
  assert(del != NULL);

  memcpy(&del->id, it, sizeof(del->id) );
  it += sizeof(del->id);
  size_t sz = sizeof(del->id);

  memcpy(&del->type, it, sizeof(del->type) );
  it += sizeof(del->type);
  sz += sizeof(del->type);

  return sz;
}


static
size_t dec_tc_mod_ctrl_payload_q(uint8_t const* it, tc_mod_ctrl_queue_t* mod)
{
  assert(mod != NULL);

  memcpy(&mod->id, it, sizeof(mod->id) );
  it += sizeof(mod->id);
  size_t sz = sizeof(mod->id);

  memcpy(&mod->type, it, sizeof(mod->type) );
  it += sizeof(mod->type);
  sz += sizeof(mod->type);

  if(mod->type == TC_QUEUE_FIFO){
    sz += dec_tc_ctrl_payload_q_fifo(it, &mod->fifo);
  } else if (mod->type == TC_QUEUE_CODEL){
    sz += dec_tc_ctrl_payload_q_codel(it, &mod->codel);
  } else if (mod->type == TC_QUEUE_ECN_CODEL){
    sz += dec_tc_ctrl_payload_q_ecn_codel(it, &mod->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return sz;
}

static
size_t dec_tc_ctrl_payload_q(uint8_t const* it, tc_ctrl_queue_t* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(&q->act, it, sizeof(q->act));
  it += sizeof(q->act);
  size_t sz = sizeof(q->act);

  if(q->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += dec_tc_add_ctrl_payload_q(it, &q->add);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_DEL){
   sz += dec_tc_del_ctrl_payload_q(it, &q->del);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_MOD){
   sz += dec_tc_mod_ctrl_payload_q(it, &q->mod);
  } else {
    assert(0!=0 && "Unknown action");
  }
  return sz;
}

static
size_t dec_tc_add_ctrl_payload_sch(uint8_t const* it, tc_add_ctrl_sch_t* add)
{
  assert(it != NULL);
  assert(add != NULL); 

  memcpy(&add->dummy, it, sizeof(add->dummy));
// it +=  sizeof(add->dummy);
  size_t sz = sizeof(add->dummy);
  return sz; 
}

static
size_t dec_tc_del_ctrl_payload_sch(uint8_t const* it, tc_del_ctrl_sch_t* del)
{
  assert(it != NULL);
  assert(del != NULL); 

  memcpy(&del->dummy, it, sizeof(del->dummy ) );
  // it += sizeof(del->dummy );
  size_t sz = sizeof(del->dummy);
  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_sch_rr(uint8_t const* it, tc_sch_rr_t* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(&rr->dummy, it, sizeof(rr->dummy) );
  // it += sizeof(rr->dummy);
  size_t sz = sizeof(rr->dummy);
  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_sch_prio(uint8_t const* it, tc_sch_prio_t* prio)
{
  assert(it != NULL);
  assert(prio != NULL);

  memcpy(&prio->len_q_prio, it, sizeof(prio->len_q_prio));
  it += sizeof(prio->len_q_prio);
  size_t sz = sizeof(prio->len_q_prio);

  if(prio->len_q_prio > 0){
    prio->q_prio = calloc(prio->len_q_prio, sizeof( uint32_t) );
    assert(prio->q_prio != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < prio->len_q_prio; ++i){
    memcpy(&prio->q_prio[i], it, sizeof(prio->q_prio[0]) );
    it += sizeof(prio->q_prio[0]);
    sz += sizeof(prio->q_prio[0]);
  }

  return sz;
}


static
size_t dec_tc_mod_ctrl_payload_sch(uint8_t const* it, tc_mod_ctrl_sch_t* mod)
{
  assert(it != NULL);
  assert(mod != NULL); 

  memcpy(&mod->type, it, sizeof(mod->type));
  it += sizeof(mod->type );
  size_t sz = sizeof(mod->type );

  if(mod->type == TC_SCHED_RR){
    sz += dec_tc_mod_ctrl_payload_sch_rr(it, &mod->rr);
  } else if(mod->type == TC_SCHED_PRIO){
    sz += dec_tc_mod_ctrl_payload_sch_prio(it, &mod->prio);
  } else {
    assert(0!=0 && "Unknown scheduler type" );
  }

  return sz;
}





static
size_t dec_tc_ctrl_payload_sch(uint8_t const* it, tc_ctrl_sch_t* sch)
{
  assert(it != NULL);
  assert(sch != NULL);

  memcpy(&sch->act, it, sizeof(sch->act));
  it += sizeof(sch->act);
  size_t sz = sizeof(sch->act);

  if(sch->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += dec_tc_add_ctrl_payload_sch(it, &sch->add);
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += dec_tc_del_ctrl_payload_sch(it, &sch->del);
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += dec_tc_mod_ctrl_payload_sch(it, &sch->mod);
  } else {
    assert(0!=0 && "Unknown scheduler action");
  }

  return sz;
}

static
size_t dec_tc_add_ctrl_payload_shp(uint8_t const* it, tc_add_ctrl_shp_t* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(&add->dummy, it, sizeof(add->dummy) );
  // it += sizeof(add->dummy)
  size_t sz = sizeof(add->dummy);
  return sz;
}

static
size_t dec_tc_del_ctrl_payload_shp(uint8_t const* it, tc_del_ctrl_shp_t* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(&del->id, it, sizeof(del->id));
  // it += sizeof(del->id ) 
  size_t sz = sizeof(del->id);
  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_shp(uint8_t const* it, tc_mod_ctrl_shp_t* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy( &mod->id, it, sizeof( mod->id) );
  it += sizeof(mod->id);
  size_t sz = sizeof(mod->id);

  memcpy( &mod-> time_window_ms, it, sizeof( mod-> time_window_ms) );
  it += sizeof(mod-> time_window_ms);
  sz += sizeof(mod-> time_window_ms);

  memcpy( &mod-> max_rate_kbps, it, sizeof( mod-> max_rate_kbps) );
  it += sizeof(mod-> max_rate_kbps);
  sz += sizeof(mod-> max_rate_kbps);

  memcpy( &mod-> active, it, sizeof( mod-> active) );
  it += sizeof(mod-> active);
  sz += sizeof(mod-> active);

  return sz;
}

static
size_t dec_tc_ctrl_payload_shp(void const* it, tc_ctrl_shp_t* shp)
{
  assert(it != NULL);
  assert(shp != NULL);

  memcpy(&shp->act, it, sizeof(shp->act));
  it += sizeof(shp->act); 
  size_t sz = sizeof(shp->act); 

  if(shp->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += dec_tc_add_ctrl_payload_shp(it, &shp->add);
  } else if(shp->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += dec_tc_del_ctrl_payload_shp(it, &shp->del);
  } else if(shp->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += dec_tc_mod_ctrl_payload_shp(it, &shp->mod);
  } else {
    assert(0!=0 && "Unknown shaper type" );
  }

  return sz;
}

static
size_t dec_tc_add_ctrl_payload_pcr(uint8_t const* it, tc_add_ctrl_pcr_t* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(&add->dummy, it, sizeof(add->dummy));
  // it += sizeof(add->dummy )
  size_t sz = sizeof(add->dummy);
  return sz;
}

static
size_t dec_tc_del_ctrl_payload_pcr(uint8_t const* it, tc_del_ctrl_pcr_t* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(&del->dummy, it, sizeof(del->dummy) );
  // it += 
  size_t sz = sizeof(del->dummy);
  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_pcr_dummy(uint8_t const* it, tc_pcr_dummy_t* dummy)
{
  assert(it != NULL);
  assert(dummy != NULL);

  memcpy(&dummy->dummy, it, sizeof(dummy->dummy));
  // it += 
  size_t sz = sizeof(dummy->dummy);
  return sz;
}

static
size_t dec_tc_mod_ctrl_payload_pcr_bdp(uint8_t const* it, tc_pcr_5g_bdp_t* bdp)
{
  assert(it != NULL);
  assert(bdp != NULL);

  memcpy(&bdp->drb_sz, it, sizeof(bdp->drb_sz));
  it += sizeof(bdp->drb_sz);
  size_t sz = sizeof(bdp->drb_sz);

  memcpy(&bdp->tstamp, it, sizeof(bdp->tstamp));
  it += sizeof(bdp->tstamp);
  sz += sizeof(bdp->tstamp);

  return sz;
}



static
size_t dec_tc_mod_ctrl_payload_pcr(uint8_t const* it, tc_mod_ctrl_pcr_t* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy( &mod->type, it, sizeof(mod->type));
  it += sizeof(mod->type);
  size_t sz = sizeof(mod->type);

  if(mod->type == TC_PCR_DUMMY){
    sz += dec_tc_mod_ctrl_payload_pcr_dummy(it, &mod->dummy);
  } else if(mod->type == TC_PCR_5G_BDP){
    sz += dec_tc_mod_ctrl_payload_pcr_bdp(it, &mod->bdp);
  } else {
    assert(0!=0 && "unknown pacer type" );
  }
  
  return sz;
}



static
size_t dec_tc_ctrl_payload_pcr(uint8_t const* it, tc_ctrl_pcr_t* pcr)
{
  assert(it != NULL);
  assert(pcr != NULL);

  memcpy(&pcr->act, it, sizeof(pcr->act));
  it += sizeof(pcr->act);
  size_t sz = sizeof(pcr->act);

  if(pcr->act == TC_CTRL_ACTION_SM_V0_ADD ){
    sz += dec_tc_add_ctrl_payload_pcr(it, &pcr->add );
  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += dec_tc_del_ctrl_payload_pcr(it, &pcr->del );
  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += dec_tc_mod_ctrl_payload_pcr(it, &pcr->mod );
  } else {
    assert(0!=0 && "Unknown pacer action" );
  }

  return sz;
}


tc_ctrl_msg_t tc_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  tc_ctrl_msg_t ctrl = {0}; 
  assert(len >0);
  assert(ctrl_msg != NULL);

  void const* it = ctrl_msg;

  memcpy(&ctrl.type, it, sizeof(ctrl.type));
  it += sizeof(ctrl.type);

  if(ctrl.type == TC_CTRL_SM_V0_CLS ){
    it += dec_tc_ctrl_payload_cls(it, &ctrl.cls); 
  } else if (ctrl.type == TC_CTRL_SM_V0_PLC ){
    it += dec_tc_ctrl_payload_plc(it,  &ctrl.plc);
  } else if (ctrl.type == TC_CTRL_SM_V0_QUEUE ){
    it += dec_tc_ctrl_payload_q(it, &ctrl.q);
  } else if (ctrl.type == TC_CTRL_SM_V0_SCH ){
    it += dec_tc_ctrl_payload_sch(it, &ctrl.sch);
  } else if (ctrl.type == TC_CTRL_SM_V0_SHP){
    it += dec_tc_ctrl_payload_shp(it, &ctrl.shp);
  } else if (ctrl.type == TC_CTRL_SM_V0_PCR ){
    it += dec_tc_ctrl_payload_pcr(it, &ctrl.pcr);
  } else {
    assert(0 != 0 && "Unknown type");
  }

  assert(ctrl_msg + len == it);

  return ctrl;
}

tc_ctrl_out_t tc_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(ctrl_out!= NULL);
  assert(len > 0);
  assert(len == sizeof(tc_ctrl_out_e));

  tc_ctrl_out_t ret = {0}; 
  memcpy(&ret.out, ctrl_out, len);

  return ret;
}

tc_func_def_t tc_dec_func_def_plain(size_t len, uint8_t const func_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(func_def != NULL);
}

