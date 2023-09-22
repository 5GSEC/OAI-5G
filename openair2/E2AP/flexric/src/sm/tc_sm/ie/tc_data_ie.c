#include "tc_data_ie.h"

#include "../../../util/alg_ds/alg/eq_float.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static
void free_tc_sch(tc_sch_t const* sch )
{
  assert(sch != NULL);

  if(sch->type == TC_SCHED_RR){

  } else if(sch->type == TC_SCHED_PRIO){
    free(sch->prio.q_prio); 
  } else {
    assert(0!=0 && "Unknown sched type");
  }
}

static
void free_tc_pcr(tc_pcr_t const* pcr)
{
  assert(pcr != NULL);
  // No heap allocated
  (void)pcr;
}

static
void free_tc_cls(tc_cls_t const* cls)
{
  assert(cls != NULL);

  if(cls->type == TC_CLS_OSI){
    if(cls->osi.len > 0)
      free(cls->osi.flt);
  } else {
    // No heap allocated
  }
}

static
void free_tc_shp(tc_shp_t const* shp)
{
  assert(shp != NULL);
  // No heap allocated
  (void)shp;
}

static
void free_tc_plc(tc_plc_t const* plc)
{
  assert(plc != NULL);
  // No heap allocated
  (void)plc;
}

static
void free_tc_q(tc_queue_t const* q)
{
  assert(q != NULL);
  // No heap allocated
  (void)q;
}

void free_tc_ind_msg(tc_ind_msg_t* msg)
{
  assert(msg != NULL);

  free_tc_sch(&msg->sch);
  free_tc_pcr(&msg->pcr);
  free_tc_cls(&msg->cls);

  for(size_t i =0; i < msg->len_q; ++i){
    free_tc_shp(&msg->shp[i]);
    free_tc_plc(&msg->plc[i]);
    free_tc_q(&msg->q[i]);
  }

  free(msg->plc);
  free(msg->shp);
  free(msg->q);
}


void tc_free_ctrl_msg(tc_ctrl_msg_t* msg)
{
  assert(msg != NULL);
  assert(0!=0 && "not implemented");

}

void tc_free_ctrl_out(tc_ctrl_out_t* out)
{
  assert(out != NULL);

  assert(0!=0 && "not implemented");
}

//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_tc_ind_hdr(tc_ind_hdr_t* src)
{
  assert(src != NULL);
  // No memory allocated
  (void)src;
} 


tc_ind_hdr_t cp_tc_ind_hdr(tc_ind_hdr_t const* src)
{
  assert(src != NULL);

  tc_ind_hdr_t dst = {.dummy = src->dummy };

  return dst;
}


bool eq_tc_ind_hdr(tc_ind_hdr_t* m0, tc_ind_hdr_t* m1){
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented");

}



//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////


static
bool eq_tc_sch_rr(tc_sch_rr_t const* m0, tc_sch_rr_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return true;
}

tc_sch_prio_t cp_tc_sch_prio(tc_sch_prio_t const* src)
{
  assert(src != NULL);

  tc_sch_prio_t dst = {.len_q_prio = src->len_q_prio};

  if(dst.len_q_prio > 0){
    dst.q_prio = calloc(dst.len_q_prio, sizeof(uint32_t) );
    assert(dst.q_prio != NULL && "Memory exhausted");
  }

  for(size_t i =0; i< dst.len_q_prio; ++i){
    dst.q_prio[i] = src->q_prio[i];
  }

  return dst;
}

bool eq_tc_sch_prio(tc_sch_prio_t const* m0, tc_sch_prio_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len_q_prio != m1->len_q_prio)
    return false;

  for(size_t i = 0 ; i < m0->len_q_prio; ++i){
    if(m0->q_prio[i] != m1->q_prio[i])
      return false;
  }

  return true;
}

 
static
bool eq_tc_sch(tc_sch_t const* m0, tc_sch_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->type != m1->type)
    return false;

  if(m0->type == TC_SCHED_RR ){
    if (eq_tc_sch_rr(&m0->rr, &m1->rr) == false)
      return false;

  } else if(m0->type == TC_SCHED_PRIO){
    if(eq_tc_sch_prio(&m0->prio, &m1->prio) == false)
      return false;

  } else {
    assert(0!=0 && "Unknown data type");
  }

  return true;
}

static
bool eq_tc_mtr(tc_mtr_t const* m0, tc_mtr_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->time_window_ms != m1->time_window_ms)
    return false;

  if(eq_float(m0->bnd_flt, m1->bnd_flt, 0.01 ) == false)
    return false;

  return true;
}


static
bool eq_tc_pcr(tc_pcr_t const* m0, tc_pcr_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->type != m1->type)
    return false;

  if(m0->id != m1->id)
    return false;

  if(eq_tc_mtr(&m0->mtr, &m1->mtr) == false)
    return false;

  return true;
}


// OSI L3 filter

void free_L3_filter(L3_filter_t* flt)
{
  assert(flt != NULL);
  (void)flt;
}

bool eq_L3_filter(L3_filter_t const* m0, L3_filter_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->src_addr != m1->src_addr)
    return false;

  if(m0->dst_addr != m1->dst_addr)
    return false;

  return true;
}

L3_filter_t cp_L3_filter(L3_filter_t const* src)
{
  assert(src != NULL);

  L3_filter_t dst = {.src_addr = src->src_addr,
                     .dst_addr = src->dst_addr };
  return dst;
}


// OSI L4 filter

void free_L4_filter(L4_filter_t* flt)
{
  assert(flt != NULL);
  (void)flt; // no memory allocated
}

bool eq_L4_filter(L4_filter_t const* m0, L4_filter_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->src_port != m1->src_port)
    return false;

  if(m0->dst_port != m1->dst_port)
    return false;
  
  if(m0->protocol != m1->protocol)
    return false;

  return true;
}

L4_filter_t cp_L4_filter(L4_filter_t const* src)
{
  assert(src != NULL);

  L4_filter_t dst = { .src_port = src->src_port,
                      .dst_port = src->dst_port,
                      .protocol = src->protocol}; 

  return dst;
}


void free_L7_filter(L7_filter_t* flt)
{
  assert(flt != NULL);

  assert(0!=0 && "not implemented");
}

bool eq_L7_filter(L7_filter_t const* m0, L7_filter_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "not implemented");
}

L7_filter_t cp_L7_filter(L7_filter_t const* flt)
{
  assert(flt != NULL);
  assert(0!=0 && "not implemented");
}

bool eq_tc_cls_osi_filter(tc_cls_osi_filter_t const* m0, tc_cls_osi_filter_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->id != m1->id)
    return false;

  // L3
  if(eq_L3_filter(&m0->l3, &m1->l3) == false)
    return false;

   // L4
  if(eq_L4_filter(&m0->l4, &m1->l4) == false)
    return false;

  // L7 
  //if(eq_L7_filter() == false)
  //  return false;


  if(m0->dst_queue != m1->dst_queue)
    return false;

  return true;
}


bool eq_tc_cls_osi(tc_cls_osi_t const* m0, tc_cls_osi_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->len != m1->len)
    return false;

  for(size_t i = 0; i < m0->len; ++i){
    if(eq_tc_cls_osi_filter(&m0->flt[i], &m1->flt[i] ) == false)
      return false;
  }

  return true;
}

bool eq_tc_cls(tc_cls_t const* m0, tc_cls_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->type != m1->type)
    return false;

  if(m0->type == TC_CLS_RR ){
    if(m0->rr.dummy != m1->rr.dummy)
      return false;

  }else if(m0->type == TC_CLS_OSI ){
    if( eq_tc_cls_osi(&m0->osi, &m1->osi) == false)
      return false;

  }else if(m0->type == TC_CLS_STO ){
    if(m0->sto.dummy != m1->sto.dummy)
      return false;

  }else {
    assert(0 != 0 && "Unknown classifier type");
  }

  return true;
}

static
bool eq_tc_shp(tc_shp_t const* m0, tc_shp_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->id != m1->id)
    return false;

  if(m0->active != m1->active)
    return false;

  if(eq_tc_mtr(&m0->mtr, &m1->mtr )  == false)
    return false;

  return true;
}

static
bool eq_tc_drp(tc_drp_t const* m0, tc_drp_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->dropped_pkts == m1->dropped_pkts;
}

static
bool eq_tc_mrk(tc_mrk_t const* m0, tc_mrk_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->marked_pkts == m1->marked_pkts;
}

static
bool eq_tc_plc(tc_plc_t const* m0, tc_plc_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->id != m1->id)
    return false;

  if(eq_tc_mtr(&m0->mtr, &m1->mtr) == false)
    return false;

  if(eq_tc_drp(&m0->drp, &m1->drp) == false)
    return false;

  if(eq_tc_mrk(&m0->mrk, &m1->mrk ) == false)
    return false;

  if(eq_float(m0->max_rate_kbps, m1->max_rate_kbps, 0.01) == false)
    return false;

  return m0->active == m1->active &&
         m0->dst_id == m1->dst_id &&
         m0->dev_id == m1->dev_id;
}

static
bool eq_tc_queue_fifo(tc_queue_fifo_t const* m0,  tc_queue_fifo_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(eq_tc_drp(&m0->drp, &m1->drp) == false)
    return false;

  if(eq_float(m0->avg_sojourn_time, m1->avg_sojourn_time, 0.001 ) == false)
    return false;

  return  m0->bytes == m1->bytes &&
          m0->pkts == m1->pkts &&
          m0->bytes_fwd == m1->bytes_fwd &&
          m0->pkts_fwd == m1->pkts_fwd;
}

static
bool eq_tc_queue_codel(tc_queue_codel_t const* m0, tc_queue_codel_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(eq_tc_drp(&m0->drp, &m1->drp) == false)
    return false;

  if(eq_float(m0->avg_sojourn_time, m1->avg_sojourn_time, 0.001 ) == false)
    return false;

  return  m0->bytes == m1->bytes &&
          m0->pkts == m1->pkts &&
          m0->bytes_fwd == m1->bytes_fwd &&
          m0->pkts_fwd == m1->pkts_fwd;
}

static
bool eq_tc_queue_ecn_codel(tc_queue_ecn_codel_t const* m0, tc_queue_ecn_codel_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(eq_tc_mrk(&m0->mrk, &m1->mrk) == false)
    return false;

  if(eq_float(m0->avg_sojourn_time, m1->avg_sojourn_time, 0.001 ) == false)
    return false;

  return  m0->bytes == m1->bytes &&
          m0->pkts == m1->pkts &&
          m0->bytes_fwd == m1->bytes_fwd &&
          m0->pkts_fwd == m1->pkts_fwd;
}

static
bool eq_tc_q(tc_queue_t const* m0, tc_queue_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->type != m1->type)
    return false;

  if(m0->type == TC_QUEUE_FIFO ){
    if(eq_tc_queue_fifo(&m0->fifo, &m1->fifo) == false)
      return false;

  } else if(m0->type == TC_QUEUE_CODEL){
    if(eq_tc_queue_codel(&m0->codel, &m1->codel) == false)
      return false;

  } else if(m0->type == TC_QUEUE_ECN_CODEL){
    if(eq_tc_queue_ecn_codel(&m0->ecn, &m1->ecn) == false)
      return false;
  } else {
    assert(0!=0 && "Unknown queue type");
  }

  return true;
}

bool eq_tc_ind_msg(tc_ind_msg_t const* m0, tc_ind_msg_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->tstamp != m1->tstamp){
    assert(0!=0);
    return false;
  }

  if(eq_tc_sch(&m0->sch, &m1->sch) == false){
    assert(0!=0);
    return false;
  }

  if(eq_tc_pcr(&m0->pcr, &m1->pcr) == false){
    assert(0!=0);
    return false;
  }

  if(eq_tc_cls(&m0->cls, &m1->cls) == false){
    assert(0!=0);
    return false;
  }

  if(m0->len_q != m1->len_q){
    assert(0!=0);
    return false;
  }

  for(size_t i = 0; i < m0->len_q; ++i){
    if(eq_tc_shp(&m0->shp[i], &m1->shp[i] ) == false){
    assert(0!=0);
      return false;
    }

    if(eq_tc_plc(&m0->plc[i], &m1->plc[i]) == false){
    assert(0!=0);
      return false;
    }

    if(eq_tc_q(&m0->q[i], &m1->q[i]) == false){
    assert(0!=0);
      return false;
    }
  }

  return true;
}


tc_sch_rr_t cp_tc_sch_rr(tc_sch_rr_t const* src)
{
  assert(src != NULL);
  tc_sch_rr_t dst  = {.dummy = src->dummy};
  return dst;
}

/*
tc_sch_prio_t cp_tc_sch_prio(tc_sch_prio_t const* src)
{
  assert(src != NULL);

  tc_sch_prio_t dst = {0};

  dst.len_q_prio = src->len_q_prio;
  if(dst.len_q_prio > 0){
    dst.q_prio = calloc(dst.len_q_prio, sizeof(*dst.q_prio) );
  }
  for(size_t i = 0; i < dst.len_q_prio; ++i){
    dst.q_prio[i] = src->q_prio[i];
  }

  return dst;
} 
*/ 

static
tc_sch_t cp_tc_sch(tc_sch_t const* src)
{
  assert(src != NULL);
  tc_sch_t dst = {0};

  dst.type = src->type;
  
  if(src->type == TC_SCHED_RR ){
    dst.rr = cp_tc_sch_rr(&src->rr);
  } else if(src->type == TC_SCHED_PRIO){
    dst.prio = cp_tc_sch_prio(&src->prio);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static
tc_mtr_t cp_tc_mtr(tc_mtr_t const* src)
{
  assert(src != NULL);

  tc_mtr_t dst = {0};
  dst.bnd_flt = src->bnd_flt;
  dst.time_window_ms = src->time_window_ms;

  return dst;
}

static
tc_pcr_t cp_tc_pcr(tc_pcr_t const* src)
{
  assert(src != NULL);

  tc_pcr_t dst = {.type = src->type,
                  .id = src->id};

  dst.mtr = cp_tc_mtr(&src->mtr);

  return dst;
}

tc_cls_osi_filter_t cp_tc_cls_osi_filter(tc_cls_osi_filter_t const* src)
{
  assert(src != NULL);

  tc_cls_osi_filter_t dst = {.id = src->id,
                              .dst_queue = src->dst_queue}; 

  dst.l3 = cp_L3_filter(&src->l3);
  dst.l4 = cp_L4_filter(&src->l4);
//  dst.l7 = cp_L7_filter(&src->l7);

  return dst;
}

static
tc_cls_osi_t cp_tc_cls_osi(tc_cls_osi_t const* src)
{
  assert(src != NULL);

  tc_cls_osi_t dst = {0}; 

  dst.len = src->len;

  if(dst.len > 0){
    dst.flt = calloc(dst.len, sizeof( tc_cls_osi_filter_t) );
    assert(dst.flt != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.len; ++i){
    dst.flt[i] = cp_tc_cls_osi_filter(&src->flt[i] );
  }

  return dst;
}

static
tc_cls_t cp_tc_cls(tc_cls_t const* src)
{
  assert(src != NULL);

  tc_cls_t dst = {0};

  dst.type = src->type;

  if(dst.type == TC_CLS_RR){
    dst.rr.dummy = src->rr.dummy;
  } else if(dst.type == TC_CLS_OSI){
    dst.osi = cp_tc_cls_osi(&src->osi);
//    dst.osi.dummy = src->osi.dummy;

  } else if(dst.type == TC_CLS_STO) {
    dst.sto.dummy = src->sto.dummy;
  } else {
    assert(0!= 0 && "Unknown classifier type");
  }
  
  return dst;
}

static
tc_drp_t cp_tc_drp(tc_drp_t const* src)
{
  assert(src != NULL);

  tc_drp_t dst = {0};

  dst.dropped_pkts = src->dropped_pkts;

  return dst;
}
static
tc_mrk_t cp_tc_mrk(tc_mrk_t const* src)
{
  assert(src != NULL);

  tc_mrk_t dst = {0}; 
  dst.marked_pkts = src->marked_pkts;

  return dst;
}

tc_queue_t cp_tc_queue(tc_queue_t const* src)
{
  assert(src != NULL);
  tc_queue_t dst = {0}; 

  dst.type = src->type;

  if(dst.type == TC_QUEUE_FIFO ){
    dst.fifo.bytes = src->fifo.bytes; 
    dst.fifo.pkts = src->fifo.pkts; 
    dst.fifo.bytes_fwd = src->fifo.bytes_fwd; 
    dst.fifo.pkts_fwd = src->fifo.pkts_fwd; 
    dst.fifo.avg_sojourn_time = src->fifo.avg_sojourn_time; 
    dst.fifo.drp = cp_tc_drp(&src->fifo.drp);
  } else if(dst.type == TC_QUEUE_CODEL){
    dst.codel.bytes = src->codel.bytes; 
    dst.codel.pkts = src->codel.pkts; 
    dst.codel.bytes_fwd = src->codel.bytes_fwd; 
    dst.codel.pkts_fwd = src->codel.pkts_fwd; 
    dst.codel.avg_sojourn_time = src->codel.avg_sojourn_time; 
    dst.codel.drp = cp_tc_drp(&src->codel.drp);
  } else if(dst.type == TC_QUEUE_ECN_CODEL){
    dst.ecn.bytes = src->ecn.bytes; 
    dst.ecn.pkts = src->ecn.pkts; 
    dst.ecn.bytes_fwd = src->ecn.bytes_fwd; 
    dst.ecn.pkts_fwd = src->ecn.pkts_fwd; 
    dst.ecn.avg_sojourn_time = src->ecn.avg_sojourn_time; 
    dst.ecn.mrk = cp_tc_mrk(&src->ecn.mrk);
  } else {
    assert(0!=0 && "Unknown queu type");
  }

  return dst;
}

static
tc_shp_t cp_tc_shp(tc_shp_t const* src)
{
  tc_shp_t dst = {0};

  dst.mtr = cp_tc_mtr(&src->mtr);

  dst.id = src->id;
  assert(src->active == 0 || src->active == 1);
  dst.active = src->active;
  return dst;
}

static
tc_plc_t cp_tc_plc(tc_plc_t const* src)
{
  assert(src != NULL);

  tc_plc_t dst = {0};

  dst.id = src->id;

  // meter
  dst.mtr = cp_tc_mtr(&src->mtr);

  // dropper
  dst.drp = cp_tc_drp(&src->drp);

  // marker
  dst.mrk = cp_tc_mrk(&src->mrk);

  dst.max_rate_kbps = src->max_rate_kbps;
  assert(src->active == 0 || src->active == 1);
  dst.active = src->active;;
  dst.dst_id = src->dst_id;
  dst.dev_id = src->dev_id;

  return dst;
}

void cp_tc_shp_q_plc(tc_ind_msg_t const* src, tc_ind_msg_t* dst)
{
  assert(src!= NULL);
  assert(dst != NULL);

  dst->len_q = src->len_q;

  if(dst->len_q > 0){
    uint32_t const sz = dst->len_q; 

    dst->q = calloc(sz, sizeof(*dst->q) );
    assert(dst->q != NULL && "memory exhausted");

    dst->shp = calloc(sz, sizeof(*dst->shp) );
    assert(dst->shp != NULL && "memory exhausted");

    dst->plc = calloc(sz, sizeof(*dst->plc) );
    assert(dst->shp != NULL && "memory exhausted");
  }

  for(size_t i =0; i < dst->len_q; ++i){
    dst->q[i] = cp_tc_queue(&src->q[i]);
    dst->shp[i] = cp_tc_shp(&src->shp[i]);
    dst->plc[i] = cp_tc_plc(&src->plc[i]);
  }

}


tc_ind_msg_t cp_tc_ind_msg(tc_ind_msg_t const* src)
{
  assert(src != NULL);

  tc_ind_msg_t dst = {0};

  dst.sch = cp_tc_sch(&src->sch);
  dst.pcr = cp_tc_pcr(&src->pcr);
  dst.cls = cp_tc_cls(&src->cls);
  cp_tc_shp_q_plc(src, &dst);

  dst.tstamp = src->tstamp; 

  assert(eq_tc_ind_msg(src, &dst ) );
  return dst;

}

//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

void free_tc_call_proc_id( tc_call_proc_id_t* src)
{
  if(src == NULL) 
    return;

  assert(0!=0 && "Not implemented");
}


tc_call_proc_id_t cp_tc_call_proc_id( tc_call_proc_id_t const* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implememnted");
}


//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////

void free_tc_ctrl_hdr(tc_ctrl_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
} 

tc_ctrl_hdr_t cp_tc_ctrl_hdr(tc_ctrl_hdr_t const* src)
{
  assert(src != NULL);
  tc_ctrl_hdr_t dst = {.dummy = src->dummy};
  return dst; 
}

bool eq_tc_ctrl_hdr(tc_ctrl_hdr_t const* m0, tc_ctrl_hdr_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  return m0->dummy == m1->dummy;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////

tc_add_cls_osi_t cp_tc_add_cls_osi( tc_add_cls_osi_t const* src)
{
  assert(src != NULL);

  tc_add_cls_osi_t dst = {.dst_queue = src-> dst_queue}; 

  dst.l3 = cp_L3_filter(&src->l3);
  dst.l4 = cp_L4_filter(&src->l4);
//  dst.l7 = cp_L7_filter(&src->l7);

  return dst;
}

tc_add_ctrl_cls_t cp_tc_add_ctrl_cls(tc_add_ctrl_cls_t const* src)
{
  assert(src != NULL);

  tc_add_ctrl_cls_t dst = {.type = src->type}; 

  if(dst.type == TC_CLS_RR ){
    dst.rr.dummy = src->rr.dummy;
  } else if(dst.type == TC_CLS_OSI){
    dst.osi = cp_tc_add_cls_osi(&src->osi);
  } else if(dst.type == TC_CLS_STO ){
    dst.sto.dummy = src->sto.dummy;
  } else {
    assert(0!=0 && "Unknown classifier type");
  }
  return dst;
}

tc_mod_ctrl_cls_t cp_tc_mod_ctrl_cls(tc_mod_ctrl_cls_t const* src)
{
  assert(src != NULL);

  tc_mod_ctrl_cls_t  dst = {.type = src->type };

  if(dst.type == TC_CLS_RR){
    dst.rr.dummy = src->rr.dummy;
  } else if(dst.type == TC_CLS_OSI){
    dst.osi.filter = cp_tc_cls_osi_filter(&src->osi.filter);
  } else if(dst.type == TC_CLS_STO){
    dst.sto.dummy = src->sto.dummy;
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

tc_del_ctrl_cls_t cp_tc_del_ctrl_cls(tc_del_ctrl_cls_t const* src)
{
  assert(src != NULL);

  tc_del_ctrl_cls_t  dst = {.type = src->type };

  if(dst.type == TC_CLS_RR){
    dst.rr.dummy = src->rr.dummy;
  } else if(dst.type == TC_CLS_OSI){
    dst.osi.filter_id = src->osi.filter_id;
  } else if(dst.type == TC_CLS_STO){
    dst.sto.dummy = src->sto.dummy;
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}


tc_ctrl_cls_t cp_tc_ctrl_cls(tc_ctrl_cls_t const* src)
{
  assert(src != NULL);

  tc_ctrl_cls_t dst = {.act = src->act};

  if(dst.act == TC_CTRL_ACTION_SM_V0_ADD ){
    dst.add = cp_tc_add_ctrl_cls(&src->add);
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_DEL){
    dst.del = cp_tc_del_ctrl_cls(&src->del);
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_MOD){
    dst.mod = cp_tc_mod_ctrl_cls(&src->mod);
  } else {
    assert(0!=0 && "Unknown action");
  }

  /*
  if(dst.type == TC_CLS_RR){
    dst.rr.dummy = src->rr.dummy;
  } else if(dst.type == TC_CLS_OSI){
    dst.osi.dummy = src->osi.dummy;
  } else if(dst.type == TC_CLS_STO){
    dst.sto.dummy = src->sto.dummy;
  } else {
    assert(0!=0 && "unknown type");
  }
*/
  return dst;
}


tc_mod_ctrl_plc_t cp_tc_mod_ctrl_plc(tc_mod_ctrl_plc_t const* src)
{
  assert(src != NULL);
  tc_mod_ctrl_plc_t dst = {
    .id = src->id,
    .drop_rate_kbps = src->drop_rate_kbps,  
    .dev_id = src->dev_id,
    .dev_rate_kbps = src-> dev_rate_kbps,  
    .active = src->active
  }; 
  return dst;
}

tc_ctrl_plc_t cp_tc_ctrl_plc(tc_ctrl_plc_t const* src)
{
  assert(src != NULL);

  tc_ctrl_plc_t dst = {.act = src->act };

  if(dst.act == TC_CTRL_ACTION_SM_V0_ADD){
    dst.add.dummy = src->add.dummy;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_DEL){
    dst.del.id = src->del.id;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_MOD){
    dst.mod = cp_tc_mod_ctrl_plc(&src->mod);
  } else{
    assert(0!=0 && "Unknown action");
  }


/*



  tc_ctrl_plc_t dst = {.id = src->id, 
                       .drop_rate_kbps = src->drop_rate_kbps, 
                       .dev_id= src->dev_id ,
                       .dev_rate_kbps= src->dev_rate_kbps,
                       .active = src->active,
                      }; 
                      */

  return dst;
}

static
tc_ctrl_queue_fifo_t cp_tc_ctrl_q_fifo(tc_ctrl_queue_fifo_t const* src)
{
  assert(src != NULL);
  tc_ctrl_queue_fifo_t dst = {.dummy = src->dummy};

  return dst;
}

static
tc_ctrl_queue_codel_t cp_tc_ctrl_q_codel(tc_ctrl_queue_codel_t const* src)
{
  assert(src != NULL);
  tc_ctrl_queue_codel_t dst = {.target_ms = src->target_ms,
                               .interval_ms = src->interval_ms };

  return dst;
}

static
tc_ctrl_queue_ecn_codel_t cp_tc_ctrl_q_ecn_codel(tc_ctrl_queue_ecn_codel_t const* src)
{
  assert(src != NULL);
  tc_ctrl_queue_ecn_codel_t dst = { .target_ms = src->target_ms,
                                    .interval_ms = src->interval_ms };

  return dst;
}

static
tc_add_ctrl_queue_t cp_tc_add_ctrl_queue(tc_add_ctrl_queue_t const* src)
{
  assert(src != NULL);

  tc_add_ctrl_queue_t dst = {.type = src->type}; 

  if(dst.type == TC_QUEUE_FIFO){
    dst.fifo = cp_tc_ctrl_q_fifo(&src->fifo); 
  } else if(dst.type == TC_QUEUE_CODEL ){
    dst.codel = cp_tc_ctrl_q_codel(&src->codel); 
  } else if(dst.type == TC_QUEUE_ECN_CODEL ){
    dst.ecn = cp_tc_ctrl_q_ecn_codel(&src->ecn); 
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static
tc_del_ctrl_queue_t cp_tc_del_ctrl_queue(tc_del_ctrl_queue_t const* src)  
{
  assert(src != NULL);

  tc_del_ctrl_queue_t dst = {.id = src->id,
                              .type = src->type}; 

  return dst;
}

static
tc_mod_ctrl_queue_t cp_tc_mod_ctrl_queue(tc_mod_ctrl_queue_t const* src)
{
  assert(src != NULL);

  tc_mod_ctrl_queue_t dst = {.id = src->id,
                             .type = src->type}; 

  if(dst.type == TC_QUEUE_FIFO){
    dst.fifo = cp_tc_ctrl_q_fifo(&src->fifo); 
  } else if (dst.type == TC_QUEUE_CODEL ){
    dst.codel = cp_tc_ctrl_q_codel(&src->codel); 
  } else if (dst.type == TC_QUEUE_ECN_CODEL ){
    dst.ecn = cp_tc_ctrl_q_ecn_codel(&src->ecn); 
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static
tc_ctrl_queue_t cp_tc_ctrl_q(tc_ctrl_queue_t const* src )
{
  tc_ctrl_queue_t dst = {.act = src->act};

  if(dst.act == TC_CTRL_ACTION_SM_V0_ADD){
    dst.add = cp_tc_add_ctrl_queue(&src->add);  
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_DEL){
    dst.del = cp_tc_del_ctrl_queue(&src->del);  
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_MOD ) {
    dst.mod = cp_tc_mod_ctrl_queue(&src->mod);  
  } else {
    assert(0!=0 && "Unknown type");
  }
  return dst;
}

tc_mod_ctrl_sch_t cp_tc_mod_ctrl_sch(tc_mod_ctrl_sch_t const* src)
{
  assert(src != NULL);

  tc_mod_ctrl_sch_t dst = {.type = src->type};

  if(dst.type == TC_SCHED_RR){
    dst.rr.dummy = src->rr.dummy;
  } else if(dst.type == TC_SCHED_PRIO ){
    dst.prio = cp_tc_sch_prio(&src->prio) ;
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static
tc_ctrl_sch_t cp_tc_ctrl_sch(tc_ctrl_sch_t const* src )
{
  assert(src != NULL);

  tc_ctrl_sch_t dst = {.act = src->act}; 

  if(dst.act == TC_CTRL_ACTION_SM_V0_ADD){
    dst.add.dummy = src->add.dummy;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_DEL){
    dst.del.dummy = src->del.dummy;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_MOD){
    dst.mod = cp_tc_mod_ctrl_sch(&src->mod) ;
  } else {
    assert(0!=0 && "Unknown action");
  }

/*
  if(dst.type == TC_SCHED_RR){

  } else if (dst.type == TC_SCHED_PRIO){
    dst.prio.len_q_prio = src->prio.len_q_prio;
    if(dst.prio.len_q_prio > 0){
     dst.prio.q_prio = calloc(dst.prio.len_q_prio, sizeof(uint32_t)); 
     assert(dst.prio.q_prio != NULL && "Memory exhausted");
    }
    for(size_t i = 0; i < dst.prio.len_q_prio; ++i){
      dst.prio.q_prio[i] = src->prio.q_prio[i];
    }
  } else {
    assert(0!=0 && "Unknown scheduler type");
  }
*/
  return dst;
}


tc_mod_ctrl_shp_t cp_tc_mod_ctrl_shp(tc_mod_ctrl_shp_t const* src)
{
  tc_mod_ctrl_shp_t dst = {.id = src->id,
                            .time_window_ms = src->time_window_ms,
                            . max_rate_kbps = src->max_rate_kbps,
                            .active = src->active};
    return dst;
}

tc_ctrl_shp_t cp_tc_ctrl_shp(tc_ctrl_shp_t const* src)
{
  assert(src != NULL);

  tc_ctrl_shp_t dst = {.act = src->act};

  if(dst.act == TC_CTRL_ACTION_SM_V0_ADD){
    dst.add.dummy = src->add.dummy;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_DEL){
    dst.del.id = src->del.id;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_MOD){
    dst.mod = cp_tc_mod_ctrl_shp(&src->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }

  return dst;

  /*
  tc_ctrl_shp_t dst = {
    .id = src->id,
    .time_window_ms = src->time_window_ms, 
    .max_rate_kbps = src->max_rate_kbps,
    .active = src->active
  }; 
  return dst;
  */

}

tc_pcr_5g_bdp_t cp_tc_pcr_5g_bdp( tc_pcr_5g_bdp_t const* src)
{
  assert(src != NULL);

  tc_pcr_5g_bdp_t dst = {.drb_sz = src->drb_sz,
                        .tstamp = src->tstamp};

  return dst;
}

tc_mod_ctrl_pcr_t cp_tc_mod_ctrl_pcr(tc_mod_ctrl_pcr_t const* src)
{
  assert(src != NULL);

  tc_mod_ctrl_pcr_t dst = {.type = src->type}; 

  if(dst.type == TC_PCR_DUMMY){
    dst.dummy.dummy = src->dummy.dummy;
  } else if(dst.type == TC_PCR_5G_BDP){
    dst.bdp = cp_tc_pcr_5g_bdp(&src->bdp);
  } else {
    assert(0!=0 && "Unknown pacer type");
  }

  return dst;
}

tc_ctrl_pcr_t cp_tc_ctrl_pcr(tc_ctrl_pcr_t const* src)
{
  assert(src != NULL);

  tc_ctrl_pcr_t dst = {.act = src->act}; 

  if(dst.act == TC_CTRL_ACTION_SM_V0_ADD){
    dst.add.dummy = src->add.dummy;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_DEL){
    dst.del.dummy = src->del.dummy;
  } else if(dst.act == TC_CTRL_ACTION_SM_V0_MOD){
    dst.mod = cp_tc_mod_ctrl_pcr(&src->mod);
  } else {
    assert(0!=0 && "Unknown action" );
  }
  
  return dst;


/*
  tc_ctrl_pcr_t dst = { .type = src->type};
  if(dst.type == TC_PCR_DUMMY ){

  } else if(dst.type == TC_PCR_5G_BDP ){ 
    dst.bdp.drb_sz = src->bdp.drb_sz;
    dst.bdp.tstamp = src->bdp.tstamp;
  } else {
    assert(0!=0 && "Unknown type" );
  }
      
  return dst;
  */
}

void free_tc_add_cls_osi(tc_add_cls_osi_t* src)
{
  assert(src != NULL);
  // No heap memory allocated
  (void)src;
}

void free_tc_add_ctrl_cls(tc_add_ctrl_cls_t* src)
{
  assert(src != NULL);

  if(src->type == TC_CLS_RR){
    // No heap allocated
  } else if(src->type == TC_CLS_OSI){
     free_tc_add_cls_osi(&src->osi);
  } else if(src->type == TC_CLS_STO){
    // No heap allocated
  } else {
    assert(0!=0 && "unknown type");
  }

}

void free_tc_cls_osi_filter( tc_cls_osi_filter_t* flt)
{
  assert(flt != NULL);
  (void)flt;
}

void free_tc_mod_ctrl_cls(tc_mod_ctrl_cls_t* src)
{
  assert(src != NULL);

  if(src->type == TC_CLS_RR){
    // No heap allocated
  } else if(src->type == TC_CLS_OSI){
  free_tc_cls_osi_filter(&src->osi.filter);
  } else if(src->type == TC_CLS_STO){
    // No heap allocated
  } else {
    assert(0!=0 && "unknown classifier type");
  }
}

void free_tc_ctrl_cls(tc_ctrl_cls_t* cls)
{
  assert(cls != NULL); 

  if(cls->act == TC_CTRL_ACTION_SM_V0_ADD){
    free_tc_add_ctrl_cls(&cls->add);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_DEL){

  } else if(cls->act == TC_CTRL_ACTION_SM_V0_MOD){
    free_tc_mod_ctrl_cls(&cls->mod);
  } else {
    assert(0!=0 && "unknown action");
  }


/*
  // No heap memory allocated
  if(cls->type == TC_CLS_RR ){

  } else if(cls->type == TC_CLS_OSI){

  } else if(cls->type == TC_CLS_STO){

  } else {
    assert(0!=0 && "Unknown type");
  }
*/
}

static
void free_tc_ctrl_plc(tc_ctrl_plc_t* plc )
{
  assert(plc != NULL); 
  // No heap memory allocated
  (void)plc;
}

static
void free_tc_ctrl_q_add(tc_add_ctrl_queue_t* add)
{
  assert(add != NULL);
  // No heap memory allocated
  (void)add;
}

static
void free_tc_ctrl_q_mod(tc_mod_ctrl_queue_t* mod)
{
  assert(mod != NULL);
  // No heap memory allocated
  (void)mod;
}

static
void free_tc_ctrl_q_del(tc_del_ctrl_queue_t* del)
{
  assert(del != NULL);
  // No heap memory allocated
  (void)del;
}

static
void free_tc_ctrl_q(tc_ctrl_queue_t* q)
{
  assert(q != NULL); 

  if(q->act == TC_CTRL_ACTION_SM_V0_ADD ){
   free_tc_ctrl_q_add(&q->add);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_DEL){
   free_tc_ctrl_q_del(&q->del);
  } else if(q->act == TC_CTRL_ACTION_SM_V0_MOD){
   free_tc_ctrl_q_mod(&q->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  } 



 /* 
  // No heap memory allocated
  if(q->type == TC_QUEUE_FIFO ){
    //tc_ctrl_queue_fifo_t fifo;
  } else if(q->type == TC_QUEUE_CODEL){
    //tc_ctrl_queue_codel_t codel;
  } else {
    assert(0!=0 && "Unknown type");
  }
*/

}

void free_tc_ctrl_sch_prio(tc_sch_prio_t* prio)
{
  assert(prio != NULL);

  if(prio->len_q_prio > 0)
    free(prio->q_prio);
}


void free_tc_mod_ctrl_sch(tc_mod_ctrl_sch_t* src)
{
  assert(src != NULL);

  if(src->type == TC_SCHED_RR){
    // No heap allocated
  } else if(src->type == TC_SCHED_PRIO) {
     free_tc_ctrl_sch_prio(&src->prio);
  } else {
    assert(0!=0 && "unknown type");
  }
}

void free_tc_ctrl_sch(tc_ctrl_sch_t* sch)
{
  assert(sch != NULL); 

  if(sch->act == TC_CTRL_ACTION_SM_V0_ADD){
    // No heap allocated 
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_DEL){
    // No heap allocated 
  } else if(sch->act == TC_CTRL_ACTION_SM_V0_MOD){
      free_tc_mod_ctrl_sch(&sch->mod);
  } else {
    assert(0!=0 && "unknown action");
  }

/*
  if(sch->type == TC_SCHED_RR){
    // No heap allocated
  } else if (sch->type == TC_SCHED_PRIO ){
     free_tc_ctrl_sch_prio(&sch->prio);
  } else {
    assert(0!=0 && "Unknown type");
  }
*/

}

void free_tc_ctrl_shp(tc_ctrl_shp_t* shp)
{
  assert(shp != NULL); 
  //No heap memory allocated
  (void)shp;
}

static
void free_tc_ctrl_pcr(tc_ctrl_pcr_t* pcr)
{
  assert(pcr != NULL); 

  // No heap allocated
  (void)pcr;
/*
  if(pcr->act == TC_CTRL_ACTION_SM_V0_ADD){

  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_DEL){

  } else if(pcr->act == TC_CTRL_ACTION_SM_V0_MOD){

  } else {
    assert(0!=0 && "Unknown action");
  }
*/

  /*
  if(pcr->type == TC_PCR_DUMMY ){
    // No heap memory allocated
  } else if (pcr->type == TC_PCR_5G_BDP){
    // No heap memory allocated
    //tc_pcr_5g_bdp_t bdp;
  } else {
    assert(0!=0 && "Unknown type");
  }
*/

}


void free_tc_ctrl_msg(tc_ctrl_msg_t* src)
{
  assert(src != NULL);

  if(src->type == TC_CTRL_SM_V0_CLS ){
    free_tc_ctrl_cls(&src->cls); 
  } else if(src->type == TC_CTRL_SM_V0_PLC){
    free_tc_ctrl_plc(&src->plc);
  } else if(src->type == TC_CTRL_SM_V0_QUEUE){
    free_tc_ctrl_q(&src->q);
  } else if(src->type == TC_CTRL_SM_V0_SCH){
    free_tc_ctrl_sch(&src->sch);
  } else if(src->type == TC_CTRL_SM_V0_SHP){
    free_tc_ctrl_shp(&src->shp);
  } else if(src->type == TC_CTRL_SM_V0_PCR){
    free_tc_ctrl_pcr(&src->pcr);
  } else {
    assert(0 != 0 && "Unknown type"); 
  }

} 

tc_ctrl_msg_t cp_tc_ctrl_msg(tc_ctrl_msg_t const* src)
{
  assert(src != NULL);

  tc_ctrl_msg_t dst = {.type = src->type} ;

  if(dst.type == TC_CTRL_SM_V0_CLS ){
    dst.cls = cp_tc_ctrl_cls(&src->cls); 
  } else if(dst.type == TC_CTRL_SM_V0_PLC){
    dst.plc = cp_tc_ctrl_plc(&src->plc);
  } else if(dst.type == TC_CTRL_SM_V0_QUEUE){
    dst.q = cp_tc_ctrl_q(&src->q);
  } else if(dst.type == TC_CTRL_SM_V0_SCH){
    dst.sch = cp_tc_ctrl_sch(&src->sch);
  } else if(dst.type == TC_CTRL_SM_V0_SHP){
    dst.shp = cp_tc_ctrl_shp(&src->shp);
  } else if(dst.type == TC_CTRL_SM_V0_PCR){
    dst.pcr = cp_tc_ctrl_pcr(&src->pcr);
  } else {
    assert(0 != 0 && "Unknown type"); 
  }

  return dst; 
}



bool eq_tc_ctrl_msg(tc_ctrl_msg_t const* m0, tc_ctrl_msg_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  assert(0!=0 && "Not implemented" );

  bool ans= false;
  return ans;
}




//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////


void free_tc_ctrl_out(tc_ctrl_out_t* src)
{
  assert(src != NULL);

  //if(src->len_diag > 0){
  //  assert(src->diagnostic != NULL);
  //  free(src->diagnostic);
  //}
} 

tc_ctrl_out_t cp_tc_ctrl_out(tc_ctrl_out_t const* src)
{
  assert(src != NULL);
  tc_ctrl_out_t dst = {.out = src->out }; 
/*(
  if(src->len_diag > 0){
    dst.diagnostic = malloc(src->len_diag);
    assert(dst.diagnostic != NULL);
    memcpy(dst.diagnostic, src->diagnostic, src->len_diag);
  }
*/
  return dst;
}

bool eq_tc_ctrl_out(tc_ctrl_out_t* m0, tc_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->out == m1->out;
/*
  if(m0->len_diag != m1->len_diag) return false;

  if(m0->len_diag > 0){
   return (memcmp(m0->diagnostic, m1->diagnostic, m0->len_diag) == 0);
  }

  return true;
*/
}


///////////////
// RIC Indication
///////////////

void free_tc_ind_data(tc_ind_data_t* ind)
{
  assert(ind != NULL);

  free_tc_ind_hdr(&ind->hdr);
  free_tc_ind_msg(&ind->msg);
  free_tc_call_proc_id(ind->proc_id);
}

tc_ind_data_t cp_tc_ind_data(tc_ind_data_t const* src)
{
  assert(src != NULL);

  tc_ind_data_t dst = {0}; 

  dst.hdr = cp_tc_ind_hdr(&src->hdr);
  dst.msg = cp_tc_ind_msg(&src->msg);

  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(*dst.proc_id) );
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_tc_call_proc_id(src->proc_id);
  }

  return dst;
}

