/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include "fill_rnd_data_tc.h"
#include "../../src/util/time_now_us.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>


static
void fill_tc_mtr(tc_mtr_t* mtr)
{
  assert(mtr != NULL);

  mtr->time_window_ms = rand() % 124;
  mtr->bnd_flt = 16.3;
}

static
void fill_tc_sch(tc_sch_t* sch)
{
  assert(sch != NULL);
  sch->type = rand()%TC_SCHED_END;
  if(sch->type == TC_SCHED_RR){
//    sch->rr = ;
  } else if(sch->type == TC_SCHED_PRIO){
    sch->prio.len_q_prio = rand()%2;
    sch->prio.q_prio = calloc(sch->prio.len_q_prio, sizeof(sch->prio.q_prio) ); 
    assert(sch->prio.q_prio != NULL && "Memory exhausted");
    for(size_t i = 0; i < sch->prio.len_q_prio; ++i){
      sch->prio.q_prio[i] = rand()%16;
    }
  } else {
    assert(0!=0 && "Unknown scheduler type");
  }
}

static
void fill_tc_pcr(tc_pcr_t* pcr)
{
  assert(pcr != NULL);
  pcr->type = rand()% TC_PCR_END;
  
  if(pcr->type == TC_PCR_DUMMY){
    pcr->id = rand()%16;
    fill_tc_mtr(&pcr->mtr);   
  } else if(pcr->type == TC_PCR_5G_BDP){
    pcr->id = rand()%16;
    fill_tc_mtr(&pcr->mtr);   
  } else {
    assert(0 !=0 && "Unknown pacer");
  }
}

static
void fill_l3(L3_filter_t* l3)
{
  assert(l3 != NULL);
  l3->src_addr = rand()% 4096*1024;
  l3->dst_addr = rand()% 4096*1024;
}

static
void fill_l4(L4_filter_t* l4 )
{
  assert(l4 != NULL);

  l4->src_port = rand()%1024;
  l4->dst_port = rand()%1024;
  l4->protocol = 17;
}

static
void fill_l7(L7_filter_t* l7)
{
  assert(l7 != NULL);

}

static
void fill_tc_cls(tc_cls_t* cls)
{
  assert(cls != NULL);
  cls->type = rand()%TC_CLS_END; 

  if(cls->type == TC_CLS_RR){
    cls->rr.dummy = rand()%16;
  } else if(cls->type == TC_CLS_OSI){

    cls->osi.len = rand()%8;
    if(cls->osi.len > 0){
      cls->osi.flt = calloc(cls->osi.len, sizeof(tc_cls_osi_filter_t) );
      assert(cls->osi.flt != NULL && "memory exhausted");
    }

    for(size_t i = 0; i < cls->osi.len; ++i){
      cls->osi.flt[i].id = rand()%128;
      fill_l3(&cls->osi.flt[i].l3);
      fill_l4(&cls->osi.flt[i].l4);
      fill_l7(&cls->osi.flt[i].l7);
      cls->osi.flt[i].dst_queue = rand()%8;
    }

  } else if(cls->type == TC_CLS_STO){
    cls->sto.dummy = rand()%16;
  } else {
    assert("Unknown classifier type");
  }

}

static
void fill_tc_drp(tc_drp_t* drp)
{
  assert(drp != NULL);
  drp->dropped_pkts = rand()%1000;
}

static
void fill_tc_mrk(tc_mrk_t* mrk)
{
  mrk->marked_pkts = rand()%8;
}

static
void fill_tc_queue(tc_queue_t* q)
{
  assert(q != NULL);

  q->type = rand()%TC_QUEUE_END;
  q->id = rand()%16;

  if(q->type == TC_QUEUE_FIFO){
    q->fifo.bytes = rand()%10000;
    q->fifo.pkts = rand()%10000;
    q->fifo.bytes_fwd = rand()%10000;
    q->fifo.pkts_fwd = rand()%10000;
    q->fifo.avg_sojourn_time = 100.5;
    q->fifo.last_sojourn_time = rand()%100;
    fill_tc_drp(&q->fifo.drp);

  } else if(q->type == TC_QUEUE_CODEL){
    q->codel.bytes = rand()%10000;
    q->codel.pkts = rand()%10000;
    q->codel.bytes_fwd = rand()%10000;
    q->codel.pkts_fwd = rand()%10000;
    q->codel.avg_sojourn_time = 100.5;
    q->codel.last_sojourn_time = rand()%100;
    fill_tc_drp(&q->codel.drp);
  } else if(q->type == TC_QUEUE_ECN_CODEL){
    q->ecn.bytes = rand()%10000;
    q->ecn.pkts = rand()%10000;
    q->ecn.bytes_fwd = rand()%10000;
    q->ecn.pkts_fwd = rand()%10000;
    q->ecn.avg_sojourn_time = 100.5;
    q->ecn.last_sojourn_time = rand()%100;
    fill_tc_mrk(&q->ecn.mrk);
  } else {
    assert(0!=0 && "Unknown queue type");
  }
}

static
void fill_tc_shp(tc_shp_t* s)
{
  assert(s != NULL);

  s->id = rand()%16;
  s->active = rand()%2;
  fill_tc_mtr(&s->mtr);
}

static
void fill_tc_plc(tc_plc_t* p)
{
  assert(p != NULL);

  p->id = rand() % 1024;
  fill_tc_mtr(&p->mtr);
  fill_tc_drp(&p->drp);
  fill_tc_mrk(&p->mrk);
  p->max_rate_kbps = 124.5;
  p->active = rand()%2;
  p->dst_id = rand()%16;
  p->dev_id = rand()%16;
}


static
void fill_tc_shp_q_plc(tc_ind_msg_t* msg)
{
  assert(msg != NULL); 
  msg->len_q = rand()%16;

  msg->q = calloc(msg->len_q, sizeof(tc_queue_t));
  assert(msg->q != NULL && "Memory exhausted");
  
  msg->shp = calloc(msg->len_q, sizeof(tc_shp_t));
  assert(msg->shp != NULL && "Memory exhausted");

  msg->plc = calloc(msg->len_q, sizeof(tc_plc_t));
  assert(msg->plc != NULL && "Memory exhausted");

  for(size_t i = 0; i < msg->len_q; ++i){
    fill_tc_queue(&msg->q[i]);
    fill_tc_shp(&msg->shp[i]);
    fill_tc_plc(&msg->plc[i]);
  }

}



void fill_tc_ind_data(tc_ind_data_t* ind_msg)
{
  assert(ind_msg != NULL);

  srand(time(0));

  ind_msg->msg.tstamp = time_now_us();

  fill_tc_sch(&ind_msg->msg.sch);
  fill_tc_pcr(&ind_msg->msg.pcr);
  fill_tc_cls(&ind_msg->msg.cls);
  fill_tc_shp_q_plc(&ind_msg->msg);

}


static
void fill_tc_ctrl_cls_add(tc_add_ctrl_cls_t* add)
{
  assert(add != NULL);

  tc_cls_e type = rand() % TC_CLS_END;
  add->type = type;

  if(type == TC_CLS_RR){
    add->rr.dummy = rand()%16;
  } else if(type == TC_CLS_OSI){
    add->osi.dst_queue = rand()%16;
    fill_l3(&add->osi.l3);
    fill_l4(&add->osi.l4);
    fill_l7(&add->osi.l7);
  } else if(type == TC_CLS_STO){
    add->sto.dummy = rand()%16;
  } else {
    assert(0!=0 && "Unknown classifier type");    
  }
}

static
void fill_tc_ctrl_cls_del(tc_del_ctrl_cls_t* del)
{
  assert(del != NULL);

  tc_cls_e type = rand() % TC_CLS_END;
  del->type = type;

  if(type == TC_CLS_RR){
    del->rr.dummy = rand()%16;
  } else if(type == TC_CLS_OSI){
    del->osi.filter_id = rand()%256;
  } else if(type == TC_CLS_STO){
    del->sto.dummy = rand()%16;
  } else {
    assert(0!=0 && "Unknown classifier type");    
  }

}

static
void fill_tc_ctrl_cls_mod(tc_mod_ctrl_cls_t* mod)
{
  assert(mod != NULL);

  tc_cls_e type = rand() % TC_CLS_END;
  mod->type = type;

  if(type == TC_CLS_RR){
    mod->rr.dummy = rand()%16;
  } else if(type == TC_CLS_OSI){
    mod->osi.filter.id = rand()%256;
    mod->osi.filter.dst_queue = rand()%256;
    fill_l3(&mod->osi.filter.l3) ;
    fill_l4(&mod->osi.filter.l4) ;
    fill_l7(&mod->osi.filter.l7) ;
  } else if(type == TC_CLS_STO){
    mod->sto.dummy = rand()%16;
  } else {
    assert(0!=0 && "Unknown classifier type");    
  }
}

static
void fill_tc_ctrl_cls(tc_ctrl_cls_t* cls)
{
  assert(cls != NULL);

  cls->act = rand()%TC_CTRL_ACTION_SM_V0_END;

  if(cls->act == TC_CTRL_ACTION_SM_V0_ADD){
    fill_tc_ctrl_cls_add(&cls->add);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_DEL){
    fill_tc_ctrl_cls_del(&cls->del);
  } else if(cls->act == TC_CTRL_ACTION_SM_V0_MOD){
    fill_tc_ctrl_cls_mod(&cls->mod);
  } else {
    assert(0!=0 && "Unknown action type" ); 
  }


/*
  uint32_t type = rand() % TC_CLS_END;

  cls->type = type;

  if(type == TC_CLS_RR){
    cls->rr.dummy = rand()%16;
  } else if (type == TC_CLS_OSI){
    cls->osi.dummy = rand()%16;
  } else if (type == TC_CLS_STO){
    cls->sto.dummy = rand()%16;
  } else {
    assert("Unknown classifier type"); 
  }
*/


}

static
void fill_tc_ctrl_plc(tc_ctrl_plc_t* plc)
{
  assert(plc != NULL);

  tc_ctrl_act_e act = rand() % TC_CTRL_ACTION_SM_V0_END;
  plc->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
    plc->add.dummy = rand()%16;  
  } else if(act == TC_CTRL_ACTION_SM_V0_DEL){
    plc->del.id = rand()%8;
  } else if(act == TC_CTRL_ACTION_SM_V0_MOD){
    plc->mod.id = rand()%16;
    plc->mod.drop_rate_kbps = rand()%16;
    plc->mod.dev_id = rand()%16 ;
    plc->mod.dev_rate_kbps = rand()% 4096;
    plc->mod.active = rand()%2;
  } else {
    assert(0!=0 && "Unknown action type");
  }
}

static
void fill_tc_ctrl_queue_fifo( tc_ctrl_queue_fifo_t* fifo)
{
  assert(fifo != NULL);
  fifo->dummy = rand()%128;
}

static
void fill_tc_ctrl_queue_codel(tc_ctrl_queue_codel_t* codel)
{
  assert(codel != NULL);

  codel->interval_ms = rand()%256 + 100; 
  codel-> target_ms = rand()%10 + 5;
}

static
void fill_tc_ctrl_queue_ecn_codel(tc_ctrl_queue_ecn_codel_t* ecn)
{
  assert(ecn != NULL);

  ecn->interval_ms = rand()%256 + 100; 
  ecn->target_ms = rand()%10 + 5;
}

static
void fill_tc_ctrl_queue_add(tc_add_ctrl_queue_t* add)
{
  assert(add != NULL);

  add->type = rand() % TC_QUEUE_END;

  if(add->type == TC_QUEUE_FIFO ){
    fill_tc_ctrl_queue_fifo(&add->fifo);
  } else if(add->type == TC_QUEUE_CODEL){
    fill_tc_ctrl_queue_codel(&add->codel);
  } else if(add->type == TC_QUEUE_ECN_CODEL){
    fill_tc_ctrl_queue_ecn_codel(&add->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }
}

static
void fill_tc_ctrl_queue_del(tc_del_ctrl_queue_t* del)
{
  assert(del != NULL);

  del->id = rand() % 10;
  del->type = rand() % TC_QUEUE_END;
}

static
void fill_tc_ctrl_queue_mod(tc_mod_ctrl_queue_t* mod)
{
  assert(mod != NULL);

  mod->id = rand() % 10;
  mod->type = rand() % TC_QUEUE_END;

  if(mod->type == TC_QUEUE_FIFO ){
    fill_tc_ctrl_queue_fifo(&mod->fifo);
  } else if(mod->type == TC_QUEUE_CODEL){
    fill_tc_ctrl_queue_codel(&mod->codel);
  } else if(mod->type == TC_QUEUE_ECN_CODEL){
    fill_tc_ctrl_queue_ecn_codel(&mod->ecn);
  } else {
    assert(0!=0 && "Unknown type");
  }

}


static
void fill_tc_ctrl_queue(tc_ctrl_queue_t* q)
{
  assert(q != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  q->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
     fill_tc_ctrl_queue_add(&q->add);
  } else if (act == TC_CTRL_ACTION_SM_V0_DEL){ 
     fill_tc_ctrl_queue_del(&q->del);
  } else if (act == TC_CTRL_ACTION_SM_V0_MOD){
     fill_tc_ctrl_queue_mod(&q->mod);
  } else {
    assert(0!=0 && "Unknown actio type");
  }

/*
  tc_queue_e type = rand() % TC_QUEUE_END;
  q->type = type;
  q->id = rand() % 16;

  if(type == TC_QUEUE_FIFO){
    fill_tc_ctrl_queue_fifo(&q->fifo);
  } else if(type == TC_QUEUE_CODEL){
    fill_tc_ctrl_queue_codel(&q->codel);
  } else {
    assert(0!=0 && "Unknown queue type");
  }
*/

}

static
void fill_tc_ctrl_sch_add(tc_add_ctrl_sch_t* add)
{
  assert(add != NULL);

  add->dummy = rand()%16;
}

static
void fill_tc_ctrl_sch_del(tc_del_ctrl_sch_t* del)
{
  assert(del != NULL);

  del->dummy = rand()%16;
}

static
void fill_tc_ctrl_sch_mod(tc_mod_ctrl_sch_t* mod)
{
  assert(mod != NULL);

  tc_sch_e type = rand()%TC_SCHED_END;
  mod->type = type;

  if(type == TC_SCHED_RR){
    mod->rr.dummy = rand()%8;
  } else if(type == TC_SCHED_PRIO ){

  mod->prio.len_q_prio = rand()%8;
  if(mod->prio.len_q_prio > 0){
    mod->prio.q_prio = calloc(mod->prio.len_q_prio, sizeof(uint32_t));
    assert(mod->prio.q_prio != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < mod->prio.len_q_prio; ++i){
    mod->prio.q_prio[i] = rand()%16; 
  }

  } else {
    assert(0!=0 && "scheduler type unknown");
  }

}


static
void fill_tc_ctrl_sch(tc_ctrl_sch_t* sch)
{
  assert(sch != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  sch->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
     fill_tc_ctrl_sch_add(&sch->add);
  } else if (act == TC_CTRL_ACTION_SM_V0_DEL){ 
     fill_tc_ctrl_sch_del(&sch->del);
  } else if (act == TC_CTRL_ACTION_SM_V0_MOD){
     fill_tc_ctrl_sch_mod(&sch->mod);
  } else {
    assert(0!=0 && "Unknown actio type");
  }
/*
  sch->type = rand()%TC_SCHED_END;

  if(sch->type == TC_SCHED_RR){

  } else if(sch->type == TC_SCHED_PRIO){

  } else {
    assert(0!=0 && "Unknown scheduler type");
  }
*/

}

static
void fill_tc_mod_ctrl_shp(tc_mod_ctrl_shp_t* mod)
{
  assert(mod != NULL);

  mod->id = rand()%128;
  mod->time_window_ms = rand()%128; 
  mod->max_rate_kbps = rand()%1024;
  mod->active = rand()%2;
} 

static
void fill_tc_ctrl_shp(tc_ctrl_shp_t* shp)
{
  assert(shp != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  shp->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
    shp->add.dummy = rand()%16; 
  } else if(act == TC_CTRL_ACTION_SM_V0_DEL){
    shp->del.id = rand()%16; 
  } else if(act == TC_CTRL_ACTION_SM_V0_MOD){
     fill_tc_mod_ctrl_shp(&shp->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }
}

static
void fill_tc_mod_ctrl_pcr(tc_mod_ctrl_pcr_t* mod)
{
  assert(mod != NULL);

  tc_pcr_e type = rand()% TC_PCR_END;
  mod->type = type;

  if(type == TC_PCR_DUMMY){
    mod->dummy.dummy = rand()%16;
  } else if(type == TC_PCR_5G_BDP) {
    mod->bdp. drb_sz = rand()% 2048;
    mod->bdp.tstamp = 10000 + rand()% 2048;
  } else {
    assert(0!=0 && "unknown type");
  }

}


static
void fill_tc_ctrl_pcr(tc_ctrl_pcr_t* pcr)
{
  assert(pcr != NULL);

  tc_ctrl_act_e act = rand()%TC_CTRL_ACTION_SM_V0_END;
  pcr->act = act;

  if(act == TC_CTRL_ACTION_SM_V0_ADD){
    pcr->add.dummy = rand()%32;
  } else if(act == TC_CTRL_ACTION_SM_V0_DEL){
    pcr->del.dummy = rand()%32;
  } else if(act == TC_CTRL_ACTION_SM_V0_MOD){
    fill_tc_mod_ctrl_pcr(&pcr->mod);
  } else {
    assert(0!=0 && "Unknown action type");
  }

}

void fill_tc_ctrl(tc_ctrl_req_data_t* ctrl)
{
  assert(ctrl != NULL);

  ctrl->hdr.dummy = rand() % 16;

  uint32_t const type = rand()%TC_CTRL_SM_V0_END;
  ctrl->msg.type = type;

  if(type == TC_CTRL_SM_V0_CLS){
    fill_tc_ctrl_cls(&ctrl->msg.cls);
  } else if(type == TC_CTRL_SM_V0_PLC) {
    fill_tc_ctrl_plc(&ctrl->msg.plc);
  } else if(type == TC_CTRL_SM_V0_QUEUE){
    fill_tc_ctrl_queue(&ctrl->msg.q);
  } else if(type == TC_CTRL_SM_V0_SCH){
    fill_tc_ctrl_sch(&ctrl->msg.sch);
  } else if(type == TC_CTRL_SM_V0_SHP){
    fill_tc_ctrl_shp(&ctrl->msg.shp);
  } else if(type == TC_CTRL_SM_V0_PCR){
    fill_tc_ctrl_pcr(&ctrl->msg.pcr);
  } else {
    assert(0!=0 && "Unknown type");
  }

}


