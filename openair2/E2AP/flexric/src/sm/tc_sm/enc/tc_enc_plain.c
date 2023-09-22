#include "tc_enc_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

byte_array_t tc_enc_event_trigger_plain(tc_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
  byte_array_t  ba = {0};
 
  ba.len = sizeof(event_trigger->ms);
  ba.buf = malloc(ba.len);
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, &event_trigger->ms, ba.len);

  return ba;
}

byte_array_t tc_enc_action_def_plain(tc_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t tc_enc_ind_hdr_plain(tc_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);

  byte_array_t ba = {0};

  ba.len = sizeof(tc_ind_hdr_t);
  ba.buf = malloc(sizeof(tc_ind_msg_t));
  assert(ba.buf != NULL && "memory exhausted");
  memcpy(ba.buf, ind_hdr, sizeof(tc_ind_hdr_t));

  return ba;
}

static
size_t cal_sch_payload(tc_sch_t const* s)
{
  assert(s != NULL);
  size_t sz = sizeof(tc_sch_e);
  
  if(s->type == TC_SCHED_RR){

  } else if(s->type == TC_SCHED_PRIO){

    tc_sch_prio_t const* p = &s->prio; 
    sz += sizeof(p->len_q_prio);
    sz += sizeof(*p->q_prio)*p->len_q_prio;
  } else {
    assert(0!=0 && "Unknown Scheduler");
  }

    return sz;
}


static
size_t cal_mtr_payload(tc_mtr_t const* m)
{
  assert(m != NULL);
  size_t sz = sizeof(m->bnd_flt);
  sz += sizeof(m->time_window_ms); 
  return sz;
}

static
size_t cal_pcr_payload(tc_pcr_t const* pcr)
{
  assert(pcr != NULL);
  size_t sz = sizeof(tc_pcr_e);
  sz += sizeof(pcr->id); 
  sz += cal_mtr_payload(&pcr->mtr);

  return sz;
}


static
size_t cal_cls_rr_payload(tc_cls_rr_t const* rr)
{
  assert(rr != NULL);

  return sizeof(rr->dummy);
}

static
size_t cal_cls_osi_payload( tc_cls_osi_t const* osi)
{
  assert(osi != NULL);

  size_t sz = sizeof(osi->len);

  sz += sizeof(tc_cls_osi_filter_t)*osi->len;

  return sz;

//  uint32_t len;
//  tc_cls_osi_filter_t* flt;
//  return sizeof(osi->dummy);
}

static
size_t cal_cls_sto_payload(tc_cls_sto_t const* sto)
{
  assert(sto != NULL);

  return sizeof(sto->dummy);
}

static
size_t cal_cls_payload(tc_cls_t const* c )
{
  assert(c != NULL);
  
  size_t sz = sizeof(tc_cls_e);

  if(c->type == TC_CLS_RR  ){
    sz += cal_cls_rr_payload(&c->rr);
  } else if(c->type == TC_CLS_OSI ){
    sz += cal_cls_osi_payload(&c->osi);
  } else if(c->type == TC_CLS_STO){
    sz += cal_cls_sto_payload(&c->sto);
  } else {
    assert(0!=0 && "Unknown type" );
  }

  return sz;
}

static
size_t cal_drp_payload(tc_drp_t const* drp)
{
  assert(drp != NULL);
  return sizeof(drp->dropped_pkts);
}

static
size_t cal_mrk_payload(tc_mrk_t const* mrk)
{
  assert(mrk != NULL);

  size_t sz = sizeof(mrk->marked_pkts);
  return sz;
}

static
size_t cal_q_payload(tc_queue_t const* q)
{
  assert(q != NULL);
  size_t sz = sizeof(tc_queue_e);

  if(q->type == TC_QUEUE_FIFO){
    sz += sizeof(q->fifo.bytes);
    sz += sizeof(q->fifo.pkts);
    sz += sizeof(q->fifo.bytes_fwd);
    sz += sizeof(q->fifo.pkts_fwd);
    sz += cal_drp_payload(&q->fifo.drp);
    sz += sizeof(q->fifo.avg_sojourn_time);
    sz += sizeof(q->fifo.last_sojourn_time);
  } else if(q->type == TC_QUEUE_CODEL){
    sz += sizeof(q->codel.bytes);
    sz += sizeof(q->codel.pkts);
    sz += sizeof(q->codel.bytes_fwd);
    sz += sizeof(q->codel.pkts_fwd);
    sz += cal_drp_payload(&q->codel.drp);
    sz += sizeof(q->codel.avg_sojourn_time);
    sz += sizeof(q->codel.last_sojourn_time);
  } else if(q->type == TC_QUEUE_ECN_CODEL){
    sz += sizeof(q->ecn.bytes);
    sz += sizeof(q->ecn.pkts);
    sz += sizeof(q->ecn.bytes_fwd);
    sz += sizeof(q->ecn.pkts_fwd);
    sz += cal_mrk_payload(&q->ecn.mrk);
    sz += sizeof(q->ecn.avg_sojourn_time);
    sz += sizeof(q->ecn.last_sojourn_time);
  } else {
    assert(0!=0 && "unknown queue type");
  }
  return sz;
}

static
size_t cal_shp_payload(tc_shp_t const* shp)
{
  assert(shp != NULL);
  
  size_t sz = sizeof(shp->id);
  sz += sizeof(shp->active);
  sz +=  sizeof(shp->max_rate_kbps);
  sz += cal_mtr_payload(&shp->mtr);
  return sz;
}

static
size_t cal_plc_payload(tc_plc_t const* plc)
{
  assert(plc != NULL);
  
  size_t sz = sizeof(plc->id);
  sz += cal_mtr_payload(&plc->mtr);
  sz += cal_drp_payload(&plc->drp);
  sz += cal_mrk_payload(&plc->mrk);
  sz += sizeof(plc->max_rate_kbps);
  sz += sizeof(plc->active);
  sz += sizeof(plc->dst_id);
  sz += sizeof(plc->dev_id);

  return sz;
}


static
size_t cal_ind_msg_payload(tc_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  size_t const sz_sch = cal_sch_payload(&ind_msg->sch);

  size_t const sz_pcr = cal_pcr_payload(&ind_msg->pcr);

  size_t const sz_cls = cal_cls_payload(&ind_msg->cls);  

  size_t const sz_len = sizeof(ind_msg->len_q);

  size_t sz_q_shp_plc = 0; 

  for(size_t i =0; i < ind_msg->len_q; ++i){
    size_t const sz_q = cal_q_payload(&ind_msg->q[i]);
    size_t const sz_shp = cal_shp_payload(&ind_msg->shp[i]);
    size_t const sz_plc = cal_plc_payload(&ind_msg->plc[i]);
    
    sz_q_shp_plc += sz_q + sz_shp + sz_plc;
  }

  size_t const sz_tstamp = sizeof(ind_msg->tstamp);
  return sz_sch + sz_pcr + sz_cls + sz_len + sz_q_shp_plc + sz_tstamp ;
}


static
size_t tc_enc_sch_rr(uint8_t* it, tc_sch_rr_t const* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  return 0;
}

static
size_t tc_enc_sch_prio(uint8_t* it, tc_sch_prio_t const* p)
{
  assert(it != NULL);
  assert(p != NULL);

  memcpy(it, &p->len_q_prio, sizeof(p->len_q_prio));
  it += sizeof(p->len_q_prio);
  size_t sz = sizeof(p->len_q_prio);

  memcpy(it, p->q_prio, sizeof(uint32_t)*p->len_q_prio);
  it += sizeof(uint32_t)*p->len_q_prio;
  sz += sizeof(uint32_t)*p->len_q_prio;

  return sz;
}

static
size_t tc_enc_sch(uint8_t* it, tc_sch_t const* sch)
{
  assert(it != NULL);
  assert(sch != NULL);

  size_t sz = sizeof(tc_sch_e);
  memcpy(it, &sch->type, sizeof(tc_sch_e));
  it += sizeof(tc_sch_e);

  if(sch->type == TC_SCHED_RR ){
    size_t sz_rr = tc_enc_sch_rr(it, &sch->rr);
    it += sz_rr;
    sz += sz_rr;
  } else if(sch->type == TC_SCHED_PRIO){
    size_t sz_prio = tc_enc_sch_prio(it, &sch->prio);
    it += sz_prio;
    sz += sz_prio;
  } else {
    assert(0!=0 && "Unknown scheduler type"); 
  }

  return sz;
}

static
size_t tc_enc_mtr(uint8_t* it, tc_mtr_t const* mtr)
{
  assert(it != NULL);
  assert(mtr != NULL);

  memcpy(it, mtr->bnd_uint, sizeof(uint8_t)*4);
  it += sizeof(uint8_t)*4;
  size_t sz = sizeof(uint8_t)*4;

  memcpy(it, &mtr->time_window_ms, sizeof(uint32_t) );
  sz += sizeof(uint32_t);
  return sz;
}

static
size_t tc_enc_pcr(uint8_t* it, tc_pcr_t const* pcr)
{
  assert(it != NULL);
  assert(pcr != NULL);

  memcpy(it, &pcr->type, sizeof(tc_pcr_e));
  it += sizeof(tc_pcr_e);
  size_t sz = sizeof(tc_pcr_e);

  memcpy(it, &pcr->id, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  sz += tc_enc_mtr(it, &pcr->mtr);
  return sz;
}

static
size_t tc_enc_cls_rr(uint8_t* it, tc_cls_rr_t const* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(it, &rr->dummy, sizeof(uint32_t));
  return sizeof(uint32_t);
}

static
size_t tc_enc_cls_osi(uint8_t* it, tc_cls_osi_t const* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(it, &osi->len, sizeof(osi->len));
  it += sizeof(osi->len);
  size_t sz = sizeof(osi->len);

  for(size_t i = 0; i < osi->len; ++i){
    memcpy(it, &osi->flt[i], sizeof(tc_cls_osi_filter_t ) );
    it += sizeof(tc_cls_osi_filter_t);
    sz += sizeof(tc_cls_osi_filter_t); 
  }

  return sz;
}

static
size_t tc_enc_cls_sto(uint8_t* it, tc_cls_sto_t const* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy(it, &sto->dummy, sizeof(uint32_t));
  return sizeof(uint32_t);
}

static
size_t tc_enc_cls(uint8_t* it, tc_cls_t const* cls)
{
  assert(it != NULL);
  assert(cls != NULL);

  memcpy(it, &cls->type, sizeof(tc_cls_e) );
  it += sizeof(tc_cls_e);
  size_t sz = sizeof(tc_cls_e);

  if(cls->type == TC_CLS_RR ){
    size_t sz_rr = tc_enc_cls_rr(it, &cls->rr);
    it += sz_rr;
    sz += sz_rr;
  } else if (cls->type == TC_CLS_OSI ){
    size_t sz_osi = tc_enc_cls_osi(it, &cls->osi);
    it += sz_osi;
    sz += sz_osi;
  } else if (cls->type == TC_CLS_STO){
    size_t sz_sto = tc_enc_cls_sto(it, &cls->sto);
    it += sz_sto;
    sz += sz_sto;
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}

static
size_t tc_enc_shp(uint8_t* it, tc_shp_t const* shp)
{
  assert(it != NULL);
  assert(shp != NULL);

  memcpy(it, &shp->id, sizeof(uint32_t) ); 
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(it, &shp->active, sizeof(uint32_t) ); 
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &shp->max_rate_kbps, sizeof(uint32_t) ); 
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);


  sz += tc_enc_mtr(it, &shp->mtr);
  return sz;
}

static
size_t tc_enc_drp(uint8_t* it, tc_drp_t const* drp)
{
  assert(it != NULL);
  assert(drp != NULL);

  memcpy(it, &drp-> dropped_pkts, sizeof(uint32_t ) );
  // it += sizeof(uint32_t)
  size_t sz = sizeof(uint32_t);

  return sz;
}


static
size_t tc_enc_mrk(uint8_t* it, tc_mrk_t const* mrk)
{
  assert(it != NULL);
  assert(mrk != NULL);

  memcpy(it, &mrk->marked_pkts, sizeof(uint32_t));
  size_t sz = sizeof(uint32_t);
  // it += sz;
  return sz;
}

static
size_t tc_enc_plc(uint8_t* it, tc_plc_t const* plc)
{
  assert(it != NULL);
  assert(plc != NULL);

  memcpy(it, &plc->id, sizeof(uint32_t));
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  size_t const sz_mtr = tc_enc_mtr(it, &plc->mtr);
  it += sz_mtr;
  sz += sz_mtr;

  size_t const sz_drp = tc_enc_drp(it, &plc->drp);
  it += sz_drp;
  sz += sz_drp;

  size_t const sz_mrk = tc_enc_mrk(it, &plc->mrk);
  it += sz_mrk;
  sz += sz_mrk;


  memcpy(it, &plc->max_rate_kbps, sizeof(float) );
  it += sizeof(float);
  sz += sizeof(float);


  memcpy(it, &plc->active, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &plc->dst_id, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &plc->dev_id, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  return sz;
}


static
size_t tc_enc_q_fifo(uint8_t* it, tc_queue_fifo_t const* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(it, &q->bytes, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(it, &q->pkts, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &q->bytes_fwd, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &q->pkts_fwd, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  size_t const sz_drp = tc_enc_drp(it, &q->drp );
  it += sz_drp;
  sz += sz_drp;

  memcpy(it, &q->avg_sojourn_time, sizeof(float));
  it += sizeof(float);
  sz += sizeof(float);

  memcpy(it, &q->last_sojourn_time, sizeof(int64_t));
//  it += sizeof(int64_t);
  sz += sizeof(int64_t);

  return sz;
}

static
size_t tc_enc_q_codel(uint8_t* it, tc_queue_codel_t const* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(it, &q->bytes, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(it, &q->pkts, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &q->bytes_fwd, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &q->pkts_fwd, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  size_t const sz_drp = tc_enc_drp(it, &q->drp );
  it += sz_drp;
  sz += sz_drp;

  memcpy(it, &q->avg_sojourn_time, sizeof(float));
  it += sizeof(float);
  sz += sizeof(float);

  memcpy(it, &q->last_sojourn_time, sizeof(int64_t));
//  it += sizeof(int64_t);
  sz += sizeof(int64_t);

  return sz;
}

static
size_t tc_enc_q_ecn_codel(uint8_t* it, tc_queue_ecn_codel_t const* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(it, &q->bytes, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  size_t sz = sizeof(uint32_t);

  memcpy(it, &q->pkts, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &q->bytes_fwd, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  memcpy(it, &q->pkts_fwd, sizeof(uint32_t) );
  it += sizeof(uint32_t);
  sz += sizeof(uint32_t);

  size_t const sz_drp = tc_enc_mrk(it, &q->mrk );
  it += sz_drp;
  sz += sz_drp;

  memcpy(it, &q->avg_sojourn_time, sizeof(float));
  it += sizeof(float);
  sz += sizeof(float);

  memcpy(it, &q->last_sojourn_time, sizeof(int64_t));
//  it += sizeof(int64_t);
  sz += sizeof(int64_t);

  return sz;
}

static
size_t tc_enc_q(uint8_t* it, tc_queue_t const* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(it, &q->type, sizeof( tc_queue_e ));
  size_t sz = sizeof(tc_queue_e);
  it += sizeof(tc_queue_e);

  if(q->type == TC_QUEUE_FIFO){
    size_t const sz_fifo = tc_enc_q_fifo(it, &q->fifo);
    sz += sz_fifo;
    it += sz_fifo;
  } else if(q->type == TC_QUEUE_CODEL){
    size_t const sz_codel = tc_enc_q_codel(it, &q->codel);
    sz += sz_codel;
    it += sz_codel;
  } else if(q->type == TC_QUEUE_ECN_CODEL){
    size_t const sz_ecn = tc_enc_q_ecn_codel(it, &q->ecn);
    sz += sz_ecn;
    it += sz_ecn;
  } else {
    assert(0!=0 && "unknown queue type");
  }

  return sz;
}

byte_array_t tc_enc_ind_msg_plain(tc_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  byte_array_t ba = {0};

  size_t sz = cal_ind_msg_payload(ind_msg);

  ba.buf = malloc(sz); 
  ba.len = sz;
  assert(ba.buf != NULL && "Memory exhausted");

  uint8_t* it = ba.buf;

  size_t const sz_sch = tc_enc_sch(it, &ind_msg->sch); 
  it += sz_sch;
  assert(it < ba.buf + ba.len && "iterator out of the chunk of memory");

  size_t const sz_pcr = tc_enc_pcr(it, &ind_msg->pcr);
  it += sz_pcr;
  assert(it < ba.buf + ba.len && "iterator out of the chunk of memory");

  size_t const sz_cls = tc_enc_cls(it, &ind_msg->cls);
  it += sz_cls;
  assert(it < ba.buf + ba.len && "iterator out of the chunk of memory");

  memcpy(it, &ind_msg->len_q, sizeof(uint32_t));
  size_t const sz_len = sizeof(uint32_t);
  it += sz_len;
  assert(it < ba.buf + ba.len && "iterator out of the chunk of memory");

  size_t sz_shp_plc_q = 0;

  for(size_t i = 0; i < ind_msg->len_q; ++i){
    size_t const sz_shp = tc_enc_shp(it, &ind_msg->shp[i]);
    it += sz_shp; 
    assert(it < ba.buf + ba.len && "iterator out of the chunk of memory");

    size_t const sz_plc = tc_enc_plc(it, &ind_msg->plc[i]);
    it += sz_plc;
    assert(it < ba.buf + ba.len && "iterator out of the chunk of memory");

    size_t const sz_q = tc_enc_q(it, &ind_msg->q[i]);
    it += sz_q;
    assert(it < ba.buf + ba.len && "iterator out of the chunk of memory");

    sz_shp_plc_q += sz_shp + sz_plc + sz_q; 
  }

  memcpy(it, &ind_msg->tstamp, sizeof(int64_t));
  size_t sz_tstamp = sizeof(int64_t); 
  it += sizeof(int64_t);
  assert(it == ba.buf + ba.len && "Mismatch of data layout");

  assert(sz == sz_sch + sz_pcr + sz_cls + sz_len + sz_shp_plc_q + sz_tstamp);

  return ba;
}

byte_array_t tc_enc_call_proc_id_plain(tc_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t tc_enc_ctrl_hdr_plain(tc_ctrl_hdr_t const* ctrl_hdr)
{
  assert(ctrl_hdr != NULL);

  byte_array_t ba = {0};

  ba.len = sizeof(tc_ind_hdr_t);
  ba.buf = malloc(sizeof(tc_ind_msg_t));
  assert(ba.buf != NULL && "memory exhausted");
  memcpy(ba.buf, ctrl_hdr, sizeof(tc_ctrl_hdr_t));

  return ba;
}

static
size_t cal_tc_add_ctrl_payload_cls_rr(tc_add_cls_rr_t const* rr)
{
  assert(rr != NULL);

  return sizeof(rr->dummy);
}

static
size_t cal_tc_add_ctrl_payload_cls_osi(tc_add_cls_osi_t const* osi)
{
  assert(osi != NULL);

  return sizeof(osi->l3) +
          sizeof(osi->l4) +
          sizeof(osi->l7) +
          sizeof(osi->dst_queue);
}

static
size_t cal_tc_add_ctrl_payload_cls_sto(tc_add_cls_sto_t const* sto)
{
  assert(sto != NULL);

  return sizeof(sto->dummy);
}

static
size_t cal_tc_add_ctrl_payload_cls(tc_add_ctrl_cls_t const* add)
{
  assert(add != NULL);

  size_t sz = sizeof(add->type);

  if(add->type == TC_CLS_RR){
    sz += cal_tc_add_ctrl_payload_cls_rr(&add->rr);
  } else if (add->type == TC_CLS_OSI) {
    sz += cal_tc_add_ctrl_payload_cls_osi(&add->osi);
  } else if(add->type == TC_CLS_STO ) {
    sz += cal_tc_add_ctrl_payload_cls_sto(&add->sto);
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}


static
size_t cal_tc_mod_ctrl_payload_cls_rr(tc_mod_cls_rr_t const* rr)
{
  return sizeof(rr->dummy);
}

static
size_t cal_tc_mod_ctrl_payload_cls_osi(tc_mod_cls_osi_t const* osi)
{
  return sizeof(osi->filter);
}

static
size_t cal_tc_mod_ctrl_payload_cls_sto(tc_mod_cls_sto_t const* sto)
{
  return sizeof(sto->dummy);
}


static
size_t cal_tc_mod_ctrl_payload_cls(tc_mod_ctrl_cls_t const* mod)
{
  assert(mod != NULL);

  size_t sz = sizeof(mod->type);

  if(mod->type == TC_CLS_RR){
    sz += cal_tc_mod_ctrl_payload_cls_rr(&mod->rr);
  } else if (mod->type == TC_CLS_OSI) {
    sz += cal_tc_mod_ctrl_payload_cls_osi(&mod->osi);
  } else if(mod->type == TC_CLS_STO ) {
    sz += cal_tc_mod_ctrl_payload_cls_sto(&mod->sto);
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}

static
size_t cal_tc_del_ctrl_payload_cls_rr(tc_del_cls_rr_t const* rr)
{
  assert(rr != NULL);
  return sizeof(rr->dummy);
}

static
size_t cal_tc_del_ctrl_payload_cls_osi(tc_del_cls_osi_t const* osi)
{
  assert(osi != NULL);
  
  return sizeof(osi->filter_id);
}

static
size_t cal_tc_del_ctrl_payload_cls_sto(tc_del_cls_sto_t const* sto)
{
  assert(sto != NULL);

  return sizeof(sto->dummy);
}

static
size_t cal_tc_del_ctrl_payload_cls(tc_del_ctrl_cls_t const* del)
{
  assert(del != NULL);

  size_t sz = sizeof(del->type);

  if(del->type == TC_CLS_RR){
    sz += cal_tc_del_ctrl_payload_cls_rr(&del->rr);
  } else if (del->type == TC_CLS_OSI) {
    sz += cal_tc_del_ctrl_payload_cls_osi(&del->osi);
  } else if(del->type == TC_CLS_STO ) {
    sz += cal_tc_del_ctrl_payload_cls_sto(&del->sto);
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}

static
size_t cal_tc_ctrl_payload_cls(tc_ctrl_cls_t const* cls)
{
  assert(cls != NULL);

  size_t sz = sizeof(tc_ctrl_act_e);

  if(cls->act == TC_CTRL_ACTION_SM_V0_ADD ){
    sz += cal_tc_add_ctrl_payload_cls(&cls->add);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += cal_tc_del_ctrl_payload_cls(&cls->del);
  } else if (cls->act == TC_CTRL_ACTION_SM_V0_MOD ){
    sz += cal_tc_mod_ctrl_payload_cls(&cls->mod);
  } else {
    assert(0!=0 && "Unknown action type" );
  }
/*
  if(cls->type == TC_CLS_RR){
    sz += sizeof(cls->rr.dummy);
  } else if(cls->type == TC_CLS_OSI){
    sz += sizeof(cls->osi.dummy);
  } else if(cls->type == TC_CLS_STO){
    sz += sizeof(cls->sto.dummy);
  } else {
    assert(0 != 0 && "Unknown classifier type");
  }
*/
  return sz;
}


static
size_t cal_tc_add_ctrl_payload_plc(tc_add_ctrl_plc_t const* add)
{
  assert(add != NULL );

  return sizeof(add->dummy);
}

static
size_t cal_tc_mod_ctrl_payload_plc(tc_mod_ctrl_plc_t const* mod)
{
  assert(mod != NULL );

  return sizeof(mod->id) +
         sizeof(mod->drop_rate_kbps) +
         sizeof(mod->dev_id) +
         sizeof(mod->dev_rate_kbps) +
         sizeof(mod->active);
}

static
size_t cal_tc_del_ctrl_payload_plc(tc_del_ctrl_plc_t const* del)
{
  assert(del != NULL );

  return sizeof(del->id);
}

static
size_t cal_tc_ctrl_payload_plc(tc_ctrl_plc_t const* plc)
{
  assert(plc != NULL);

  size_t sz = sizeof(plc->act);

  if(plc->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += cal_tc_add_ctrl_payload_plc(&plc->add);
  } else if(plc->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += cal_tc_del_ctrl_payload_plc(&plc->del);
  } else if(plc->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += cal_tc_mod_ctrl_payload_plc(&plc->mod);
  } else {
    assert(0!=0 && "Unknown action type" );
  }
  
  return sz;

/*
  return sizeof(plc->id) + 
         sizeof(plc->drop_rate_kbps) +
         sizeof(plc->dev_id) +
         sizeof(plc->dev_rate_kbps) +
         sizeof(plc->active);
         */
}
static
size_t cal_tc_ctrl_payload_q_fifo(tc_ctrl_queue_fifo_t const* fifo)
{
  assert(fifo != NULL);
  return sizeof(fifo->dummy);
}

static
size_t cal_tc_ctrl_payload_q_codel(tc_ctrl_queue_codel_t const* codel)
{
  assert(codel != NULL);
  size_t sz = sizeof(codel->target_ms);
  sz += sizeof(codel->interval_ms);
  return sz;
//  return sizeof(codel->dummy);
}

static
size_t cal_tc_ctrl_payload_q_ecn_codel(tc_ctrl_queue_ecn_codel_t const* ecn)
{
  assert(ecn != NULL);
  size_t sz = sizeof(ecn->target_ms);
  sz += sizeof(ecn->interval_ms);
  return sz;
}

static
size_t cal_tc_ctrl_payload_q_add(tc_add_ctrl_queue_t const* add)
{
  assert(add != NULL);

  size_t sz = sizeof(tc_queue_e);

  if(add->type == TC_QUEUE_FIFO ){
    sz += cal_tc_ctrl_payload_q_fifo(&add->fifo); 
  } else if (add->type == TC_QUEUE_CODEL){
    sz += cal_tc_ctrl_payload_q_codel(&add->codel);
  } else if (add->type == TC_QUEUE_ECN_CODEL){
    sz += cal_tc_ctrl_payload_q_ecn_codel(&add->ecn);
  } else {
    assert(0!=0 && "Unknwon queue type");
  }

  return sz;
}

static
size_t cal_tc_ctrl_payload_q_mod(tc_mod_ctrl_queue_t const* mod)
{
  assert(mod != NULL);

  size_t sz = sizeof(mod->id);
  sz += sizeof(mod->type);

  if(mod->type == TC_QUEUE_FIFO ){
    sz += cal_tc_ctrl_payload_q_fifo(&mod->fifo); 
  } else if (mod->type == TC_QUEUE_CODEL ){
    sz += cal_tc_ctrl_payload_q_codel(&mod->codel);
  } else if (mod->type == TC_QUEUE_ECN_CODEL){
    sz += cal_tc_ctrl_payload_q_ecn_codel(&mod->ecn);
  } else {
    assert(0!=0 && "Unknwon queue type");
  }

  return sz;
}

static
size_t cal_tc_ctrl_payload_q_del(tc_del_ctrl_queue_t const* del)
{
  assert(del != NULL);

  size_t sz = sizeof(del->id);
  sz += sizeof(del->type);

  return sz;
}

static
size_t cal_tc_ctrl_payload_q(tc_ctrl_queue_t const* q )
{
  assert(q != NULL);

  size_t sz = sizeof(q->act);

  if(q->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += cal_tc_ctrl_payload_q_add(&q->add);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += cal_tc_ctrl_payload_q_mod(&q->mod);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += cal_tc_ctrl_payload_q_del(&q->del);
  } else {
    assert(0!=0 && "Unknown action type");
  }

  return sz;

/*

  size_t sz = sizeof(q->type); 
  sz += sizeof(q->id);

  if(q->type == TC_QUEUE_STATS_FIFO ){
    sz += cal_tc_ctrl_payload_q_fifo(&q->fifo); 
  } else if(q->type == TC_QUEUE_STATS_CODEL){
    sz += cal_tc_ctrl_payload_q_codel(&q->codel); 
  } else {
    assert(0!=0 && "Unknown queue type");
  }

  return sz;
  */

}

static
size_t cal_tc_ctrl_payload_sch_add(tc_add_ctrl_sch_t const* add)
{
  assert(add != NULL);

  return sizeof(add->dummy);
}

static
size_t cal_tc_ctrl_payload_sch_mod_rr(tc_sch_rr_t const* rr)
{
  assert(rr != NULL);

  return sizeof(rr->dummy);
}

static
size_t cal_tc_ctrl_payload_sch_mod_prio(tc_sch_prio_t const* prio)
{
  assert(prio != NULL);

  size_t sz = sizeof(prio->len_q_prio);
  sz += prio->len_q_prio* sizeof(uint32_t);

  return sz;
}

static
size_t cal_tc_ctrl_payload_sch_mod(tc_mod_ctrl_sch_t const* mod)
{
  assert(mod != NULL);

  size_t sz = sizeof(mod->type);

  if(mod->type == TC_SCHED_RR) {
    sz += cal_tc_ctrl_payload_sch_mod_rr(&mod->rr);
  } else if(mod->type == TC_SCHED_PRIO){ 
    sz += cal_tc_ctrl_payload_sch_mod_prio(&mod->prio);
  } else {
    assert(0!=0 && "Unknown sched type" );
  }

  return sz;
}

static
size_t cal_tc_ctrl_payload_sch_del(tc_del_ctrl_sch_t const* del)
{
  assert(del != NULL);

  return sizeof(del->dummy);
}


static
size_t cal_tc_ctrl_payload_sch(tc_ctrl_sch_t const* sch)
{
  assert(sch != NULL);

  size_t sz = sizeof(sch->act);

  if(sch->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += cal_tc_ctrl_payload_sch_add(&sch->add);
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += cal_tc_ctrl_payload_sch_del(&sch->del);
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += cal_tc_ctrl_payload_sch_mod(&sch->mod);
  } else {
    assert( 0!=0 && "Unknown action");
  }

  return sz;



  /*
  size_t sz = sizeof(sch->type);

  if(sch->type == TC_SCHED_RR){

  } else if(sch->type == TC_SCHED_PRIO){
     sz += sizeof(sch->prio.len_q_prio);
     sz += sizeof(sch->prio.q_prio)*sch->prio.len_q_prio;
  } else {
    assert(0!=0 && "Unknown type" );
  }
  return sz;
  */
}

static
size_t cal_tc_ctrl_payload_shp_add(tc_add_ctrl_shp_t const* add)
{
  assert(add != NULL);

  return sizeof(add->dummy);
}

static
size_t cal_tc_ctrl_payload_shp_mod(tc_mod_ctrl_shp_t const* mod)
{
  assert(mod != NULL);

  return sizeof(mod->id) +
          sizeof(mod->time_window_ms) + 
          sizeof(mod->max_rate_kbps ) +
          sizeof(mod->active);
}

static
size_t cal_tc_ctrl_payload_shp_del(tc_del_ctrl_shp_t const* del)
{
  assert(del != NULL);

  return sizeof(del->id);
}



static
size_t cal_tc_ctrl_payload_shp(tc_ctrl_shp_t const* shp)
{
  assert(shp != NULL);

  size_t sz = sizeof(shp->act);

  if(shp->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += cal_tc_ctrl_payload_shp_add(&shp->add);
  } else if(shp->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += cal_tc_ctrl_payload_shp_del(&shp->del);
  } else if(shp->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += cal_tc_ctrl_payload_shp_mod(&shp->mod);
  } else {
    assert( 0!=0 && "Unknown action");
  }

  return sz;

/*
  return sizeof(shp->id) +
         sizeof(shp->time_window_ms) + 
         sizeof(shp->max_rate_kbps) +
         sizeof(shp->active);
*/
}

/*
static
size_t cal_tc_ctrl_payload_pcr_bdp( tc_pcr_5g_bdp_t const* src)
{
  assert(src != NULL);

  return sizeof(src->drb_sz) + sizeof(src->tstamp);
}
*/

static
size_t cal_tc_ctrl_payload_pcr_add(tc_add_ctrl_pcr_t const* add)
{
  assert(add != NULL);

  return sizeof(add->dummy);
}

static
size_t cal_tc_ctrl_payload_pcr_mod_dummy(tc_pcr_dummy_t const* dummy)
{
  assert(dummy != NULL);

  return sizeof(dummy->dummy);
}

static
size_t cal_tc_ctrl_payload_pcr_mod_bdp(tc_pcr_5g_bdp_t const* bdp)
{
  assert(bdp != NULL);

  return sizeof(bdp->drb_sz) +
          sizeof(bdp->tstamp);
}

static
size_t cal_tc_ctrl_payload_pcr_mod(tc_mod_ctrl_pcr_t const* mod)
{
  assert(mod != NULL);

  size_t sz = sizeof(mod->type);

  if(mod->type == TC_PCR_DUMMY ){
    sz +=  cal_tc_ctrl_payload_pcr_mod_dummy(&mod->dummy);
  } else if(mod->type == TC_PCR_5G_BDP){
    sz +=  cal_tc_ctrl_payload_pcr_mod_bdp(&mod->bdp);
  } else {
    assert(0!=0 && "Unknown pacer type" );
  }

  return sz;

}

static
size_t cal_tc_ctrl_payload_pcr_del(tc_del_ctrl_pcr_t const* del)
{
  assert(del != NULL);

  return sizeof(del->dummy);
}

static
size_t cal_tc_ctrl_payload_pcr(tc_ctrl_pcr_t const* pcr)
{
  assert(pcr != NULL);

  size_t sz = sizeof(pcr->act);

  if(pcr->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += cal_tc_ctrl_payload_pcr_add(&pcr->add);
  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += cal_tc_ctrl_payload_pcr_del(&pcr->del);
  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += cal_tc_ctrl_payload_pcr_mod(&pcr->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }

  return sz;

/*
  size_t sz = sizeof(pcr->type);
  if(pcr->type == TC_PCR_DUMMY){

  } else if(pcr->type == TC_PCR_5G_BDP){
      sz +=  cal_tc_ctrl_payload_pcr_bdp(&pcr->bdp);
  } else {
    assert(0!=0 && "Unknown type" );
  }
 */ 
  return sz;
}

static
size_t cal_tc_ctrl_payload(tc_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  size_t sz = sizeof(tc_ctrl_msg_e);

  if(ctrl_msg->type == TC_CTRL_SM_V0_CLS ){
    sz += cal_tc_ctrl_payload_cls(&ctrl_msg->cls); 
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_PLC ){
    sz += cal_tc_ctrl_payload_plc(&ctrl_msg->plc);
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_QUEUE ){
    sz += cal_tc_ctrl_payload_q(&ctrl_msg->q);
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_SCH ){
    sz += cal_tc_ctrl_payload_sch(&ctrl_msg->sch);
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_SHP){
    sz += cal_tc_ctrl_payload_shp(&ctrl_msg->shp);
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_PCR ){
    sz += cal_tc_ctrl_payload_pcr(&ctrl_msg->pcr);
  } else {
    assert(0 != 0 && "Unknown type");
  }

  return sz;
}

/*
static
size_t enc_tc_ctrl_payload_cls_rr(void* it, tc_cls_rr_t const* rr)
{
  assert(rr != NULL);
  assert(it != NULL);

  memcpy(it, &rr->dummy, sizeof(rr->dummy));
  // it += sizeof(rr->dummy);
  return sizeof(rr->dummy);
}

static
size_t enc_tc_ctrl_payload_cls_osi(void* it, tc_cls_osi_t const* osi)
{
  assert(osi != NULL);
  assert(it != NULL);

  memcpy(it, &osi->len, sizeof(osi->len));
  it += sizeof(osi->len);
  size_t sz = sizeof(osi->len);

  for(size_t i = 0; i < osi->len; ++i){
    memcpy(it, &osi->flt[i], sizeof(tc_cls_osi_filter_t ) );
    it += sizeof(tc_cls_osi_filter_t);
    sz += sizeof(tc_cls_osi_filter_t); 
  }

  return sz;

}


static
size_t enc_tc_add_ctrl_payload_cls_sto(void* it, tc_add_cls_sto_t const* sto)
{
  assert(sto != NULL);
  assert(it != NULL);

  memcpy(it, &sto->dummy, sizeof(sto->dummy));
  // it += sizeof(sto->dummy);
  return sizeof(sto->dummy);
}
*/


static
size_t enc_tc_add_ctrl_payload_cls_rr(uint8_t* it, tc_add_cls_rr_t const* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(it, &rr->dummy, sizeof(rr->dummy ));
  size_t sz = sizeof(rr->dummy);
  //it += sizeof(rr->dummy);
  return sz;
}



static
size_t enc_tc_add_ctrl_payload_cls_osi(uint8_t* it, tc_add_cls_osi_t const* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(it, &osi->l3.src_addr, sizeof(osi->l3.src_addr) );
  it += sizeof(osi->l3.src_addr);
  size_t sz = sizeof(osi->l3.src_addr);

  memcpy(it, &osi->l3.dst_addr, sizeof(osi->l3.dst_addr) );
  it += sizeof(osi->l3.dst_addr);
  sz += sizeof(osi->l3.dst_addr);

  memcpy(it, &osi->l4.src_port, sizeof(osi->l4.src_port) );
  it += sizeof(osi->l4.src_port);
  sz += sizeof(osi->l4.src_port);

  memcpy(it, &osi->l4.dst_port, sizeof(osi->l4.dst_port) );
  it += sizeof(osi->l4.dst_port);
  sz += sizeof(osi->l4.dst_port);

  memcpy(it, &osi->l4.protocol, sizeof(osi->l4.protocol) );
  it += sizeof(osi->l4.protocol);
  sz += sizeof(osi->l4.protocol);

  //memcpy(it, osi->l4.protocol, sizeof(osi->l4.protocol) );
  //it += sizeof(osi->l4.protocol);
  //sz += sizeof(osi->l4.protocol);

  memcpy(it, &osi->dst_queue, sizeof(osi->dst_queue) );
  it += sizeof(osi->dst_queue );
  sz += sizeof(osi->dst_queue );

  return sz;
}


static
size_t enc_tc_add_ctrl_payload_cls_sto(uint8_t* it, tc_add_cls_sto_t const* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy(it, &sto->dummy, sizeof(sto->dummy ));
  size_t sz = sizeof(sto->dummy);
  //it += sizeof(rr->dummy);
  return sz;
}


static
size_t enc_tc_add_ctrl_payload_cls(uint8_t* it, tc_add_ctrl_cls_t const* add )
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(it, &add->type, sizeof(add->type));
  it += sizeof(add->type);
  size_t sz = sizeof(add->type);

  if(add->type == TC_CLS_RR){
    sz += enc_tc_add_ctrl_payload_cls_rr(it, &add->rr);
  } else if(add->type == TC_CLS_OSI){
    sz += enc_tc_add_ctrl_payload_cls_osi(it, &add->osi);
  } else if(add->type == TC_CLS_STO){
    sz += enc_tc_add_ctrl_payload_cls_sto(it, &add->sto);
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_cls_rr(uint8_t* it, tc_mod_cls_rr_t const* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(it, &rr->dummy, sizeof(rr->dummy));
  // it += sizeof(rr->dummy);
  size_t sz = sizeof(rr->dummy);
  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_cls_osi(uint8_t* it, tc_mod_cls_osi_t const* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(it, &osi->filter, sizeof(tc_cls_osi_filter_t) );
//  it += sizeof(osi->filter);
  size_t sz = sizeof(osi->filter);
  return sz;
/*
tc_cls_osi_filter_t 

  memcpy(it, &osi->l3.src_addr, sizeof(osi->l3.src_addr) );
  it += sizeof(osi->l3.src_addr);
  sz += sizeof(osi->l3.src_addr);

  memcpy(it, &osi->l3.dst_addr, sizeof(osi->l3.dst_addr) );
  it += sizeof(osi->l3.dst_addr);
  sz += sizeof(osi->l3.dst_addr);

  memcpy(it, &osi->l4.src_port, sizeof(osi->l4.src_port) );
  it += sizeof(osi->l4.src_port);
  sz += sizeof(osi->l4.src_port);

  memcpy(it, &osi->l4.dst_port, sizeof(osi->l4.dst_port) );
  it += sizeof(osi->l4.dst_port);
  sz += sizeof(osi->l4.dst_port);

  memcpy(it, &osi->l4.protocol, sizeof(osi->l4.protocol) );
  it += sizeof(osi->l4.protocol);
  sz += sizeof(osi->l4.protocol);

  //memcpy(it, &osi->l7 , sizeof(osi->l7 ) );
  //it += sizeof(osi->l7 );
  //sz += sizeof(osi->l7);

  memcpy(it, &osi->dst_queue, sizeof(osi->dst_queue) );
  it += sizeof(osi->dst_queue);
  sz += sizeof(osi->dst_queue);


  return sz;
  */
}

static
size_t enc_tc_mod_ctrl_payload_cls_sto(uint8_t* it, tc_mod_cls_sto_t const* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy(it, &sto->dummy, sizeof(sto->dummy));
  // it += sizeof(rr->dummy);
  size_t sz = sizeof(sto->dummy);
  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_cls(uint8_t* it, tc_mod_ctrl_cls_t const* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy(it, &mod->type, sizeof(mod->type ) );
  it += sizeof(mod->type);  
  size_t sz = sizeof(mod->type); 

  if(mod->type == TC_CLS_RR){
    sz += enc_tc_mod_ctrl_payload_cls_rr(it, &mod->rr);
  } else if(mod->type == TC_CLS_OSI){
    sz += enc_tc_mod_ctrl_payload_cls_osi(it, &mod->osi);
  } else if(mod->type == TC_CLS_STO){
    sz += enc_tc_mod_ctrl_payload_cls_sto(it, &mod->sto);
  } else {
    assert(0!=0 && "Unknown classifier type");
  }

  return sz;
}

static
size_t enc_tc_del_ctrl_payload_cls_rr(uint8_t* it, tc_del_cls_rr_t const* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(it, &rr->dummy, sizeof(rr->dummy));
  // it += sizeof(rr->dummy );
  size_t sz = sizeof(rr->dummy);
  return sz;
}

static
size_t enc_tc_del_ctrl_payload_cls_osi(uint8_t* it, tc_del_cls_osi_t const* osi)
{
  assert(it != NULL);
  assert(osi != NULL);

  memcpy(it, &osi->filter_id, sizeof(osi->filter_id) );
  // it += sizeof(osi->filter_id); 
  size_t sz = sizeof(osi->filter_id); 

  return sz;
}

static
size_t enc_tc_del_ctrl_payload_cls_sto(uint8_t* it, tc_del_cls_sto_t const* sto)
{
  assert(it != NULL);
  assert(sto != NULL);

  memcpy(it, &sto->dummy, sizeof(sto->dummy));
  // it += sizeof(sto->dummy);
  size_t sz = sizeof(sto->dummy);
  return sz;
}

static
size_t enc_tc_del_ctrl_payload_cls(uint8_t* it, tc_del_ctrl_cls_t const* del )
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(it, &del->type, sizeof(del->type));
  it += sizeof(del->type);
  size_t sz = sizeof(del->type);

  if(del->type == TC_CLS_RR){
    sz += enc_tc_del_ctrl_payload_cls_rr(it, &del->rr);
  } else if(del->type == TC_CLS_OSI){
    sz += enc_tc_del_ctrl_payload_cls_osi(it, &del->osi);
  } else if (del->type == TC_CLS_STO){
    sz += enc_tc_del_ctrl_payload_cls_sto(it, &del->sto);
  } else {
    assert(0!=0 && "Unknown classifier type" );
  }

  return sz;
}

static
size_t enc_tc_ctrl_payload_cls(void* it, tc_ctrl_cls_t const* cls)
{
  assert(it != NULL);
  assert(cls != NULL);

  memcpy(it, &cls->act, sizeof(cls->act));
  it += sizeof(cls->act);
  size_t sz = sizeof(cls->act);

  if(cls->act == TC_CTRL_ACTION_SM_V0_ADD ){
     sz += enc_tc_add_ctrl_payload_cls(it, &cls->add);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_DEL ){
     sz += enc_tc_del_ctrl_payload_cls(it, &cls->del);
  } else if(cls->act ==  TC_CTRL_ACTION_SM_V0_MOD){
     sz += enc_tc_mod_ctrl_payload_cls(it, &cls->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }

  return sz;

/*
  memcpy(it, &cls->type, sizeof(cls->type) );
  it += sizeof(cls->type);
  size_t sz = sizeof(cls->type);

  if(cls->type == TC_CLS_RR){
     size_t sz_rr = enc_tc_ctrl_payload_cls_rr(it, &cls->rr);  
     it += sz_rr; 
     sz += sz_rr;
  } else if(cls->type == TC_CLS_OSI){
     size_t sz_osi = enc_tc_ctrl_payload_cls_osi(it, &cls->osi); 
     it += sz_osi;
     sz += sz_osi;
  } else if(cls->type == TC_CLS_STO){
    size_t sz_sto = enc_tc_ctrl_payload_cls_sto(it, &cls->sto);
     it += sz_sto;
     sz += sz_sto;
  } 

  return sz;
*/

}

static
size_t enc_tc_add_ctrl_payload_plc(uint8_t* it, tc_add_ctrl_plc_t const* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(it, &add->dummy, sizeof(add->dummy));
// it +=  sizeof(add->dummy) 
  size_t sz = sizeof(add->dummy);
  return sz; 
}

static
size_t enc_tc_mod_ctrl_payload_plc(uint8_t* it, tc_mod_ctrl_plc_t const* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy(it, &mod->id, sizeof(mod->id) );
  it += sizeof(mod->id);
  size_t sz = sizeof(mod->id);

  memcpy(it, &mod->drop_rate_kbps, sizeof(mod->drop_rate_kbps) );
  it += sizeof(mod->drop_rate_kbps);
  sz += sizeof(mod->drop_rate_kbps);

  memcpy(it, &mod->dev_id, sizeof(mod->dev_id) );
  it += sizeof(mod->dev_id);
  sz += sizeof(mod->dev_id);

  memcpy(it, &mod-> dev_rate_kbps, sizeof(mod-> dev_rate_kbps) );
  it += sizeof(mod-> dev_rate_kbps);
  sz += sizeof(mod-> dev_rate_kbps);

  memcpy(it, &mod-> active, sizeof(mod-> active) );
  it += sizeof(mod-> active);
  sz += sizeof(mod-> active);

  return sz;
}

static
size_t enc_tc_del_ctrl_payload_plc(uint8_t* it, tc_del_ctrl_plc_t const* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(it, &del->id, sizeof(del->id ));
// it +=  sizeof(add->dummy) 
  size_t sz = sizeof(del->id);
  return sz; 
}

static
size_t enc_tc_ctrl_payload_plc(uint8_t* it, tc_ctrl_plc_t const* plc)
{
  assert(it != NULL);
  assert(plc != NULL);

  memcpy(it, &plc->act, sizeof(plc->act));
  it += sizeof(plc->act);
  size_t sz = sizeof(plc->act);

  if(plc->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += enc_tc_add_ctrl_payload_plc(it, &plc->add);
  } else if(plc->act == TC_CTRL_ACTION_SM_V0_DEL ){
    sz += enc_tc_del_ctrl_payload_plc(it, &plc->del);
  } else if(plc->act == TC_CTRL_ACTION_SM_V0_MOD ){
    sz += enc_tc_mod_ctrl_payload_plc(it, &plc->mod);
  } else {
    assert(0!=0 && "Unknown action type " );
  }

  return sz;


/*
  memcpy(it, &plc->id, sizeof(plc->id));
  it += sizeof(plc->id);
  size_t sz = sizeof(plc->id);

  memcpy(it, &plc->drop_rate_kbps, sizeof(plc->drop_rate_kbps));
  it += sizeof(plc->drop_rate_kbps);
  sz += sizeof(plc->drop_rate_kbps);

  memcpy(it, &plc->dev_id, sizeof(plc->dev_id ));
  it += sizeof(plc->dev_id);
  sz += sizeof(plc->dev_id);

  memcpy(it, &plc->dev_rate_kbps, sizeof(plc->dev_rate_kbps));
  it += sizeof(plc->dev_rate_kbps);
  sz += sizeof(plc->dev_rate_kbps);

  memcpy(it, &plc->active, sizeof(plc->active));
  it += sizeof(plc->active);
  sz += sizeof(plc->active);

  return sz;
  */
}

static
size_t enc_tc_ctrl_payload_q_fifo(uint8_t* it, tc_ctrl_queue_fifo_t const* fifo)
{
  assert(it != NULL);
  assert(fifo != NULL);

  memcpy(it, &fifo->dummy, sizeof(fifo->dummy));
  // it += sizeof(fifo->dummy);
  return sizeof(fifo->dummy);

}

static
size_t enc_tc_ctrl_payload_q_codel(uint8_t* it, tc_ctrl_queue_codel_t const* codel)
{
  assert(it != NULL);
  assert(codel != NULL);

  memcpy(it, &codel->target_ms, sizeof(codel->target_ms));
  it += sizeof(codel->target_ms);
  size_t sz = sizeof(codel->target_ms);

  memcpy(it, &codel->interval_ms, sizeof(codel->interval_ms));
//  it += sizeof(codel->interval_ms);
  sz += sizeof(codel->interval_ms);

  return sz; //sizeof(codel->dummy);
}

static
size_t enc_tc_ctrl_payload_q_ecn_codel(uint8_t* it, tc_ctrl_queue_ecn_codel_t const* ecn)
{
  assert(it != NULL);
  assert(ecn != NULL);

  memcpy(it, &ecn->target_ms, sizeof(ecn->target_ms));
  it += sizeof(ecn->target_ms);
  size_t sz = sizeof(ecn->target_ms);

  memcpy(it, &ecn->interval_ms, sizeof(ecn->interval_ms));
//  it += sizeof(codel->interval_ms);
  sz += sizeof(ecn->interval_ms);

  return sz; //sizeof(codel->dummy);
}

static
size_t enc_tc_add_ctrl_payload_q(uint8_t* it, tc_add_ctrl_queue_t const* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(it, &add->type, sizeof(add->type)); 
  it += sizeof(add->type);
  size_t sz = sizeof(add->type); 

  if(add->type == TC_QUEUE_CODEL){
    sz += enc_tc_ctrl_payload_q_codel(it, &add->codel);
  } else if (add->type == TC_QUEUE_FIFO){
    sz += enc_tc_ctrl_payload_q_fifo(it, &add->fifo);
  } else if (add->type == TC_QUEUE_ECN_CODEL){
    sz += enc_tc_ctrl_payload_q_ecn_codel(it, &add->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_q(uint8_t* it, tc_mod_ctrl_queue_t const* mod)
{
  assert(mod != NULL);

  memcpy(it, &mod->id, sizeof(mod->id) );
  it += sizeof(mod->id);
  size_t sz = sizeof(mod->id);


  memcpy(it, &mod->type, sizeof(mod->type) );
  it += sizeof(mod->type);
  sz += sizeof(mod->type);

  if(mod->type == TC_QUEUE_CODEL ){
    sz += enc_tc_ctrl_payload_q_codel(it, &mod->codel);
  } else if (mod->type == TC_QUEUE_FIFO){
    sz += enc_tc_ctrl_payload_q_fifo(it, &mod->fifo);
  } else if (mod->type == TC_QUEUE_ECN_CODEL){
    sz += enc_tc_ctrl_payload_q_ecn_codel(it, &mod->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return sz;
}

static
size_t enc_tc_del_ctrl_payload_q(uint8_t* it, tc_del_ctrl_queue_t const* del)
{
  assert(del != NULL);

  memcpy(it, &del->id, sizeof(del->id) );
  it += sizeof(del->id);
  size_t sz = sizeof(del->id);

  memcpy(it, &del->type, sizeof(del->type) );
  it += sizeof(del->type);
  sz += sizeof(del->type);

  return sz;
}


static
size_t enc_tc_ctrl_payload_q(uint8_t* it, tc_ctrl_queue_t const* q)
{
  assert(it != NULL);
  assert(q != NULL);

  memcpy(it, &q->act, sizeof(q->act));
  it += sizeof(q->act);
  size_t sz = sizeof(q->act);

  if(q->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += enc_tc_add_ctrl_payload_q(it, &q->add);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_DEL){
   sz += enc_tc_del_ctrl_payload_q(it, &q->del);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_MOD){
   sz += enc_tc_mod_ctrl_payload_q(it, &q->mod);
  } else {
    assert(0!=0 && "Unknown action");
  }
  return sz;

/*
  memcpy(it, &q->type, sizeof(q->type));
  it += sizeof(q->type);
  size_t sz = sizeof(q->type);

  memcpy(it, &q->id, sizeof(q->id) );
  it += sizeof(q->id);
  sz += sizeof(q->id);

  if(q->type == TC_QUEUE_FIFO){
    size_t const sz_fifo = enc_tc_ctrl_payload_q_fifo(it, &q->fifo);
    sz += sz_fifo;
  } else if (q->type == TC_QUEUE_CODEL ){
    size_t const sz_codel = enc_tc_ctrl_payload_q_codel(it, &q->codel);
    sz += sz_codel;
  } else {
    assert(0!=0 && "Unknown type" );
  }
*/
}

static
size_t enc_tc_add_ctrl_payload_sch(uint8_t* it, tc_add_ctrl_sch_t const* add)
{
  assert(it != NULL);
  assert(add != NULL); 

  memcpy(it, &add->dummy, sizeof(add->dummy) );
// it +=  sizeof(add->dummy);
  size_t sz = sizeof(add->dummy);
  return sz; 
}

static
size_t enc_tc_mod_ctrl_payload_sch_rr(uint8_t* it, tc_sch_rr_t const* rr)
{
  assert(it != NULL);
  assert(rr != NULL);

  memcpy(it, &rr->dummy, sizeof(rr->dummy) );
  // it += sizeof(rr->dummy);
  size_t sz = sizeof(rr->dummy);
  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_sch_prio(uint8_t* it, tc_sch_prio_t const* prio)
{
  assert(it != NULL);
  assert(prio != NULL);

  memcpy(it, &prio->len_q_prio, sizeof(prio->len_q_prio) );
  it += sizeof(prio->len_q_prio);
  size_t sz = sizeof(prio->len_q_prio);

  for(size_t i = 0; i < prio->len_q_prio; ++i){
    memcpy(it, &prio->q_prio[i], sizeof(prio->q_prio[0]) );
    it += sizeof(prio->q_prio[0]);
    sz += sizeof(prio->q_prio[0]);
  }

  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_sch(uint8_t* it, tc_mod_ctrl_sch_t const* mod)
{
  assert(it != NULL);
  assert(mod != NULL); 

  memcpy(it, &mod->type, sizeof(mod->type) );
  it += sizeof(mod->type );
  size_t sz = sizeof(mod->type );

  if(mod->type == TC_SCHED_RR){
    sz += enc_tc_mod_ctrl_payload_sch_rr(it, &mod->rr);
  } else if(mod->type == TC_SCHED_PRIO){
    sz += enc_tc_mod_ctrl_payload_sch_prio(it, &mod->prio);
  } else {
    assert(0!=0 && "Unknown scheduler type" );
  }

  return sz;
}

static
size_t enc_tc_del_ctrl_payload_sch(uint8_t* it, tc_del_ctrl_sch_t const* del)
{
  assert(it != NULL);
  assert(del != NULL); 

  memcpy(it, &del->dummy, sizeof(del->dummy ) );
  // it += sizeof(del->dummy );
  size_t sz = sizeof(del->dummy);
  return sz;
}




static
size_t enc_tc_ctrl_payload_sch(uint8_t* it, tc_ctrl_sch_t const* sch)
{
  assert(it != NULL);
  assert(sch != NULL);

  memcpy(it, &sch->act, sizeof(sch->act) );
  it += sizeof(sch->act);
  size_t sz = sizeof(sch->act);

  if(sch->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += enc_tc_add_ctrl_payload_sch(it, &sch->add);
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += enc_tc_del_ctrl_payload_sch(it, &sch->del);
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += enc_tc_mod_ctrl_payload_sch(it, &sch->mod);
  } else {
    assert(0!=0 && "Unknown scheduler action");
  }

  return sz;

/*
  memcpy(it, &sch->type, sizeof(sch->type) );
  it += sizeof(sch->type);
  size_t sz = sizeof(sch->type);

  if(sch->type == TC_SCHED_RR){


  } else if (sch->type == TC_SCHED_PRIO ){
    memcpy(it, &sch->prio.len_q_prio, sizeof(sch->prio.len_q_prio));
    it += sizeof(sch->prio.len_q_prio);
    size_t sz_prio = sizeof(sch->prio.len_q_prio);

    memcpy(it, &sch->prio.q_prio, sizeof(sch->prio.q_prio)*sch->prio.len_q_prio);
    it += sizeof(sch->prio.q_prio)*sch->prio.len_q_prio;
    sz_prio += sizeof(sch->prio.q_prio)*sch->prio.len_q_prio;

    sz += sz_prio;
  } else {
    assert(0!=0 && "Unknown type" );
  }

  return sz; 
  */
}

static
size_t enc_tc_add_ctrl_payload_shp(uint8_t* it, tc_add_ctrl_shp_t const* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(it, &add->dummy, sizeof(add->dummy) );
  // it += sizeof(add->dummy)
  size_t sz = sizeof(add->dummy);
  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_shp(uint8_t* it, tc_mod_ctrl_shp_t const* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy(it, &mod->id, sizeof( mod->id) );
  it += sizeof(mod->id);
  size_t sz = sizeof(mod->id);

  memcpy(it, &mod-> time_window_ms, sizeof( mod-> time_window_ms) );
  it += sizeof(mod-> time_window_ms);
  sz += sizeof(mod-> time_window_ms);

  memcpy(it, &mod-> max_rate_kbps, sizeof( mod-> max_rate_kbps) );
  it += sizeof(mod-> max_rate_kbps);
  sz += sizeof(mod-> max_rate_kbps);

  memcpy(it, &mod-> active, sizeof( mod-> active) );
  it += sizeof(mod-> active);
  sz += sizeof(mod-> active);

  return sz;
}

static
size_t enc_tc_del_ctrl_payload_shp(uint8_t* it, tc_del_ctrl_shp_t const* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(it, &del->id, sizeof(del->id));
  // it += sizeof(del->id ) 
  size_t sz = sizeof(del->id);
  return sz;
}

static
size_t enc_tc_ctrl_payload_shp(void* it, tc_ctrl_shp_t const* shp)
{
  assert(it != NULL);
  assert(shp != NULL);

  memcpy(it, &shp->act, sizeof(shp->act) );
  it += sizeof(shp->act); 
  size_t sz = sizeof(shp->act); 

  if(shp->act == TC_CTRL_ACTION_SM_V0_ADD){
    sz += enc_tc_add_ctrl_payload_shp(it, &shp->add);
  } else if(shp->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += enc_tc_del_ctrl_payload_shp(it, &shp->del);
  } else if(shp->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += enc_tc_mod_ctrl_payload_shp(it, &shp->mod);
  } else {
    assert(0!=0 && "Unknown shaper type" );
  }

  return sz;

/*
  memcpy(it, &shp->id, sizeof(shp->id) );
  it += sizeof(shp->id);
  size_t sz = sizeof(shp->id);

  memcpy(it, &shp->time_window_ms, sizeof(shp->time_window_ms));
  it += sizeof(shp->time_window_ms);
  sz += sizeof(shp->time_window_ms);

  memcpy(it, &shp->max_rate_kbps, sizeof(shp->max_rate_kbps));
  it += sizeof(shp->max_rate_kbps);
  sz += sizeof(shp->max_rate_kbps);

  memcpy(it, &shp->active, sizeof(shp->active));
  it += sizeof(shp->active);
  sz += sizeof(shp->active);

  return sz;
  */

}

static
size_t enc_tc_add_ctrl_payload_pcr(uint8_t* it, tc_add_ctrl_pcr_t const* add)
{
  assert(it != NULL);
  assert(add != NULL);

  memcpy(it, &add->dummy, sizeof(add->dummy));
  // it += sizeof(add->dummy )
  size_t sz = sizeof(add->dummy);
  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_pcr_dummy(uint8_t* it, tc_pcr_dummy_t const* dummy)
{
  assert(it != NULL);
  assert(dummy != NULL);

  memcpy(it, &dummy->dummy, sizeof(dummy->dummy));
  // it += 
  size_t sz = sizeof(dummy->dummy);
  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_pcr_bdp(uint8_t* it, tc_pcr_5g_bdp_t const* bdp)
{
  assert(it != NULL);
  assert(bdp != NULL);

  memcpy(it, &bdp->drb_sz, sizeof(bdp->drb_sz));
  it += sizeof(bdp->drb_sz);
  size_t sz = sizeof(bdp->drb_sz);

  memcpy(it, &bdp->tstamp, sizeof(bdp->tstamp));
  it += sizeof(bdp->tstamp);
  sz += sizeof(bdp->tstamp);

  return sz;
}

static
size_t enc_tc_mod_ctrl_payload_pcr(uint8_t* it, tc_mod_ctrl_pcr_t const* mod)
{
  assert(it != NULL);
  assert(mod != NULL);

  memcpy(it, &mod->type, sizeof(mod->type));
  it += sizeof(mod->type );
  size_t sz =  sizeof(mod->type);

  if(mod->type == TC_PCR_DUMMY){
    sz += enc_tc_mod_ctrl_payload_pcr_dummy(it, &mod->dummy);
  } else if(mod->type == TC_PCR_5G_BDP){
    sz += enc_tc_mod_ctrl_payload_pcr_bdp(it, &mod->bdp);
  } else {
    assert(0!=0 && "unknown pacer type" );
  }
  
  return sz;
}

static
size_t enc_tc_del_ctrl_payload_pcr(uint8_t* it, tc_del_ctrl_pcr_t const* del)
{
  assert(it != NULL);
  assert(del != NULL);

  memcpy(it, &del->dummy, sizeof(del->dummy) );
  // it += 
  size_t sz = sizeof(del->dummy);
  return sz;
}

static
size_t enc_tc_ctrl_payload_pcr(void* it, tc_ctrl_pcr_t const* pcr)
{
  assert(it != NULL);
  assert(pcr != NULL);

  memcpy(it, &pcr->act, sizeof(pcr->act));
  it += sizeof(pcr->act);
  size_t sz = sizeof(pcr->act);

  if(pcr->act == TC_CTRL_ACTION_SM_V0_ADD ){
    sz += enc_tc_add_ctrl_payload_pcr(it, &pcr->add );
  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_DEL){
    sz += enc_tc_del_ctrl_payload_pcr(it, &pcr->del );
  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_MOD){
    sz += enc_tc_mod_ctrl_payload_pcr(it, &pcr->mod );
  } else {
    assert(0!=0 && "Unknown pacer action" );
  }

  return sz;

/*

  memcpy(it, &pcr->type, sizeof(pcr->type) );
  it += sizeof(pcr->type); 
  size_t sz = sizeof(pcr->type);

  if(pcr->type == TC_PCR_DUMMY){

  } else if(pcr->type == TC_PCR_5G_BDP ){
    memcpy(it, &pcr->bdp.drb_sz, sizeof(pcr->bdp.drb_sz) );
    it += sizeof(pcr->bdp.drb_sz);
    sz += sizeof(pcr->bdp.drb_sz);

    memcpy(it, &pcr->bdp.tstamp, sizeof(pcr->bdp.tstamp) );
    it += sizeof(pcr->bdp.tstamp);
    sz += sizeof(pcr->bdp.tstamp);

  } else {
    assert(0!=0 && "Unknown type" );
  }

  return sz;
  */

}

byte_array_t tc_enc_ctrl_msg_plain(tc_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  byte_array_t ba = {0};

  size_t const sz = cal_tc_ctrl_payload(ctrl_msg);
  ba.len = sz;
  ba.buf = malloc(sz);
  assert(ba.buf != NULL && "Memory exhausted");

  void* it = ba.buf;
  memcpy(it, &ctrl_msg->type, sizeof(ctrl_msg->type) );
  it += sizeof(ctrl_msg->type); 

  if(ctrl_msg->type == TC_CTRL_SM_V0_CLS ){
    it += enc_tc_ctrl_payload_cls(it, &ctrl_msg->cls); 
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_PLC ){
    it += enc_tc_ctrl_payload_plc(it,  &ctrl_msg->plc);
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_QUEUE ){
    it += enc_tc_ctrl_payload_q(it, &ctrl_msg->q );
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_SCH ){
    it += enc_tc_ctrl_payload_sch(it, &ctrl_msg->sch);
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_SHP){
    it += enc_tc_ctrl_payload_shp(it, &ctrl_msg->shp);
  } else if (ctrl_msg->type == TC_CTRL_SM_V0_PCR ){
    it += enc_tc_ctrl_payload_pcr(it, &ctrl_msg->pcr);
  } else {
    assert(0 != 0 && "Unknown type");
  }

  assert(it == ba.buf + ba.len);

  return ba;
}

byte_array_t tc_enc_ctrl_out_plain(tc_ctrl_out_t const* ctrl) 
{
  assert(ctrl != NULL );
  byte_array_t ba = {0};

  ba.buf = malloc(sizeof(ctrl->out));
  assert(ba.buf != NULL && "Memory exhausted" );
  ba.len = sizeof(ctrl->out);

  memcpy(ba.buf, &ctrl->out, sizeof(ctrl->out) );

  return ba;
}

byte_array_t tc_enc_func_def_plain(tc_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

