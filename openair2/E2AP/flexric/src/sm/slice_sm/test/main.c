#include "../../../../test/common/fill_ind_data.h"
#include "../../slice_sm/slice_sm_agent.h"
#include "../../slice_sm/slice_sm_ric.h"
#include "../../../util/alg_ds/alg/defer.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


// For testing purposes
static
slice_ind_data_t cp;

/*
static
int64_t time_now_us(void)
{
  struct timespec tms;

  if (clock_gettime(CLOCK_REALTIME,&tms)) {
    return -1;
  }
  int64_t micros = tms.tv_sec * 1000000;
  micros += tms.tv_nsec/1000;
  if (tms.tv_nsec % 1000 >= 500) {
    ++micros;
  }
  return micros;
}
*/


void free_ag_slice(void)
{


}

/*
static
void fill_static_slice(static_slice_t* sta)
{
  assert(sta != NULL);

  sta->pos_high = abs(rand()%25);
  sta->pos_low = abs(rand()%25);
}


static
void fill_nvs_slice(nvs_slice_t* nvs)
{
  assert(nvs != NULL);

  const uint32_t type = abs(rand() % SLICE_SM_NVS_V0_END);

  if(type == SLICE_SM_NVS_V0_RATE ){
    nvs->conf = SLICE_SM_NVS_V0_RATE; 
    nvs->u.rate.u2.mbps_reference = 0.8; 
//      10.0*((float)rand()/(float)RAND_MAX); 
    nvs->u.rate.u1.mbps_required = 10.0;
    //((float)rand()/(float)RAND_MAX); 
  } else if(type ==SLICE_SM_NVS_V0_CAPACITY ){
    nvs->conf = SLICE_SM_NVS_V0_CAPACITY; 
    nvs->u.capacity.u.pct_reserved = 15.0;
    //((float)rand()/(float)RAND_MAX);
  } else {
    assert(0!=0 && "Unknown type");
  }

}

static
void fill_scn19_slice(scn19_slice_t* scn19)
{
  assert(scn19 != NULL);

  const uint32_t type = abs(rand()% SLICE_SCN19_SM_V0_END);

  if(type == SLICE_SCN19_SM_V0_DYNAMIC ){
    scn19->conf = SLICE_SCN19_SM_V0_DYNAMIC ;
    scn19->u.dynamic.u2.mbps_reference = 10.0 * fabs((float)rand()/(float)RAND_MAX); 
    scn19->u.dynamic.u1.mbps_required = 8.0 * fabs((float)rand()/(float)RAND_MAX); 
  } else if(type == SLICE_SCN19_SM_V0_FIXED ) {
    scn19->conf = SLICE_SCN19_SM_V0_FIXED; 
    scn19->u.fixed.pos_high = abs(rand()%14);
    scn19->u.fixed.pos_low = abs(rand()%10);
  } else if(type ==SLICE_SCN19_SM_V0_ON_DEMAND){
    scn19->conf = SLICE_SCN19_SM_V0_ON_DEMAND;
//    scn19->u.on_demand.log_delta_byte = abs(rand()%121);
    scn19->u.on_demand.log_delta = 1.0 * fabs((float)rand()/RAND_MAX);
    scn19->u.on_demand.tau = abs(rand()%256);
    scn19->u.on_demand.pct_reserved = fabs((float)rand()/(float)RAND_MAX);
  } else {
    assert(0 != 0 && "Unknown type!!");
  }

}

static 
void fill_edf_slice(edf_slice_t* edf)
{
  assert(edf != NULL);

  int mod = 32;
  edf->deadline = abs(rand()%mod);
  edf->guaranteed_prbs = abs(rand()%mod);
  edf->max_replenish = abs(rand()%mod);

  edf->len_over = abs(rand()%mod);

  if(edf->len_over > 0){
    edf->over = calloc(edf->len_over, sizeof(uint32_t));
    assert(edf->over != NULL && "Memory exhausted");
  }

  for(uint32_t i = 0; i < edf->len_over; ++i){
    edf->over[i] = abs(rand()%mod);
  }
}

static
void fill_ul_dl_slice(ul_dl_slice_conf_t* slice)
{
  assert(slice != NULL);

  char const* name = "MY SLICE";
  slice->len_sched_name = strlen(name);
  slice->sched_name = malloc(strlen(name));
  assert(slice->sched_name != NULL && "memory exhausted");
  memcpy(slice->sched_name, name, strlen(name));

  slice->len_slices = 1; 
  //slice->len_slices = abs(rand()%8);

  if(slice->len_slices > 0){
    slice->slices = calloc(slice->len_slices, sizeof(fr_slice_t));
    assert(slice->slices != NULL && "memory exhausted");
  }

  for(uint32_t i = 0; i < slice->len_slices; ++i){
    slice->slices[i].id = abs(rand()%1024);
    fr_slice_t* s = &slice->slices[i];

    const char* label = "This is my label";
    s->len_label = strlen(label);
    s->label = malloc(s->len_label);
    assert(s->label != NULL && "Memory exhausted");
    memcpy(s->label, label, s->len_label );

    const char* sched_str = "Scheduler string";
    s->len_sched = strlen(sched_str); 
    s->sched = malloc(s->len_sched);
    assert(s->sched != NULL && "Memory exhausted");
    memcpy(s->sched, sched_str, s->len_sched);

    uint32_t type = abs(rand()% SLICE_ALG_SM_V0_END);

    if(type ==  SLICE_ALG_SM_V0_EDF || type == SLICE_ALG_SM_V0_NONE )
      type = SLICE_ALG_SM_V0_STATIC; 

 //   type = SLICE_ALG_SM_V0_STATIC; 
 //   type = SLICE_ALG_SM_V0_SCN19;

    if(type == SLICE_ALG_SM_V0_NONE ){
      s->params.type =SLICE_ALG_SM_V0_NONE; 
    } else if (type == SLICE_ALG_SM_V0_STATIC ){
      s->params.type = SLICE_ALG_SM_V0_STATIC; 
      fill_static_slice(&s->params.u.sta);
    } else if (type == SLICE_ALG_SM_V0_NVS){
      s->params.type =  SLICE_ALG_SM_V0_NVS; 
      fill_nvs_slice(&s->params.u.nvs);
    } else if (type == SLICE_ALG_SM_V0_SCN19) {
      s->params.type = SLICE_ALG_SM_V0_SCN19; 
      fill_scn19_slice(&s->params.u.scn19);
    } else if (type == SLICE_ALG_SM_V0_EDF){
      s->params.type =  SLICE_ALG_SM_V0_EDF; 
      fill_edf_slice(&s->params.u.edf);
    } else {
      assert(0 != 0 && "Unknown type encountered");
    }
  }
}

static
void fill_slice_conf(slice_conf_t* conf)
{
  assert(conf != NULL);

  fill_ul_dl_slice(&conf->ul);
  fill_ul_dl_slice(&conf->dl);
}

static
void fill_ue_slice_conf(ue_slice_conf_t* conf)
{
  assert(conf != NULL);
  conf->len_ue_slice = abs(rand()%10);
  if(conf->len_ue_slice > 0){
    conf->ues = calloc(conf->len_ue_slice, sizeof(ue_slice_assoc_t));
    assert(conf->ues);
  }

  for(uint32_t i = 0; i < conf->len_ue_slice; ++i){
    conf->ues[i].rnti = abs(rand()%1024);  
    conf->ues[i].dl_id = abs(rand()%16); 
    conf->ues[i].ul_id = abs(rand()%16); 
  }

}


void fill_slice_ind_data(slice_ind_data_t* ind_msg)
{
  assert(ind_msg != NULL);

  srand(time(0));

  fill_slice_conf(&ind_msg->msg.slice_conf);
  fill_ue_slice_conf(&ind_msg->msg.ue_slice_conf);

  ind_msg->msg.tstamp = time_now_us();

  cp.msg = cp_slice_ind_msg(&ind_msg->msg);
}
*/


static
slice_ctrl_req_data_t cp_ctrl;


static
void ctrl_slice(slice_ctrl_req_data_t const* ctrl)
{
  assert(ctrl != NULL);

  bool const ans_hdr = eq_slice_ctrl_hdr(&cp_ctrl.hdr, &ctrl->hdr);
  assert(ans_hdr == true);

  bool const ans_msg = eq_slice_ctrl_msg(&cp_ctrl.msg, &ctrl->msg);
  assert(ans_msg == true);
}



//
// Functions 


/////
// AGENT
////

static
void read_RAN(sm_ag_if_rd_t* read)
{
  assert(read != NULL);
  assert(read->type == SLICE_STATS_V0);

  fill_slice_ind_data(&read->slice_stats);
  cp.msg = cp_slice_ind_msg(&read->slice_stats.msg);
}


static 
sm_ag_if_ans_t write_RAN(sm_ag_if_wr_t const* data)
{
  assert(data != NULL);
  assert(data->type == SLICE_CTRL_REQ_V0);

  if(data->type ==  SLICE_CTRL_REQ_V0){
    //printf("SLICE Control called \n");
    ctrl_slice(&data->slice_req_ctrl);
  } else {
    assert(0!=0 && "Unknown data type");
  }

  sm_ag_if_ans_t ans = {.type = SLICE_AGENT_IF_CTRL_ANS_V0}; 
 
  const char* str = "THIS IS ANS STRING";
  ans.slice.len_diag = strlen(str);
  ans.slice.diagnostic = malloc(strlen(str));
  assert(ans.slice.diagnostic != NULL && "Memory exhausted");

  return ans;
}

/*
static
void fill_slice_del(del_slice_conf_t* conf)
{
  assert(conf != NULL);

  uint32_t const len_dl = rand()%5;
  conf->len_dl = len_dl;
  if(conf->len_dl > 0)
    conf->dl = calloc(len_dl, sizeof(uint32_t));

  uint32_t const len_ul = rand()%5;
  conf->len_ul = len_ul;
  if(conf->len_ul > 0)
    conf->ul = calloc(len_ul, sizeof(uint32_t));
}
*/

/////////////////////////////
// Check Functions
// //////////////////////////

static
void check_eq_ran_function(sm_agent_t const* ag, sm_ric_t const* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
  assert(ag->ran_func_id == ric->ran_func_id);
}

// RIC -> E2
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
 
  sm_subs_data_t data = ric->proc.on_subscription(ric, "2_ms");
  ag->proc.on_subscription(ag, &data); 

  free_sm_subs_data(&data);
}

// E2 -> RIC
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  sm_ind_data_t sm_data = ag->proc.on_indication(ag);
  defer({ free_sm_ind_data(&sm_data); }); 

  sm_ag_if_rd_t msg = ric->proc.on_indication(ric, &sm_data);

  slice_ind_data_t* data = &msg.slice_stats;
  assert(msg.type == SLICE_STATS_V0);

  assert(eq_slice_ind_msg(&cp.msg, &data->msg) == true);

  free_slice_ind_hdr(&data->hdr); 
  free_slice_ind_msg(&data->msg); 

  free_slice_ind_hdr(&cp.hdr);
  free_slice_ind_msg(&cp.msg);
}

/*
static
slice_ctrl_req_data_t generate_slice_ctrl()
{
   uint32_t type = rand()%SLICE_CTRL_SM_V0_END;
   slice_ctrl_req_data_t ret = {0}; 
   ret.msg.type = type;

   if(type == SLICE_CTRL_SM_V0_ADD){
     fill_slice_conf(&ret.msg.u.add_mod_slice);
     printf("SLICE_CTRL_SM_V0_ADD \n");
   } else if (type == SLICE_CTRL_SM_V0_DEL){
     fill_slice_del(&ret.msg.u.del_slice);
     printf("SLICE_CTRL_SM_V0_DEL \n");
   } else if (type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
     fill_ue_slice_conf(&ret.msg.u.ue_slice); 
     printf("SLICE_CTRL_SM_V0_MOD \n");
   } else {
      assert(0!=0 && "Unknown type");
   }

   return ret;
}
*/

static
void check_ctrl(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  sm_ag_if_wr_t ctrl = {.type = SLICE_CTRL_REQ_V0 };

  fill_slice_ctrl(&ctrl.slice_req_ctrl);

//  ctrl.slice_req_ctrl = generate_slice_ctrl();

  cp_ctrl.hdr = cp_slice_ctrl_hdr(&ctrl.slice_req_ctrl.hdr);
  cp_ctrl.msg = cp_slice_ctrl_msg(&ctrl.slice_req_ctrl.msg);

  sm_ctrl_req_data_t ctrl_req = ric->proc.on_control_req(ric, &ctrl);

  sm_ctrl_out_data_t out_data = ag->proc.on_control(ag, &ctrl_req);

  sm_ag_if_ans_t ans = ric->proc.on_control_out(ric, &out_data);
  assert(ans.type == SLICE_AGENT_IF_CTRL_ANS_V0 );

  if(ctrl_req.len_hdr > 0)
    free(ctrl_req.ctrl_hdr);

  if(ctrl_req.len_msg > 0)
    free(ctrl_req.ctrl_msg);

  if(out_data.len_out > 0)
    free(out_data.ctrl_out);

  free_slice_ctrl_out(&ans.slice);

  free_slice_ctrl_hdr(&ctrl.slice_req_ctrl.hdr); 
  free_slice_ctrl_msg(&ctrl.slice_req_ctrl.msg); 

  free_slice_ctrl_hdr(&cp_ctrl.hdr);
  free_slice_ctrl_msg(&cp_ctrl.msg);
}

int main()
{
  sm_io_ag_t io_ag = {.read = read_RAN, .write = write_RAN};  
  sm_agent_t* sm_ag = make_slice_sm_agent(io_ag);

  sm_ric_t* sm_ric = make_slice_sm_ric();

  for(int i = 0; i < 64*1024; ++i){
    check_eq_ran_function(sm_ag, sm_ric);
    check_subscription(sm_ag, sm_ric);
    check_indication(sm_ag, sm_ric);
    check_ctrl(sm_ag, sm_ric);
  }

  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);

  free_ag_slice();

  printf("Success\n");
  return EXIT_SUCCESS;
}

