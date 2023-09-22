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

#include "../../../src/xApp/e42_xapp_api.h"
#include "../../../src/sm/slice_sm/slice_sm_id.h"
#include "../../../src/util/alg_ds/alg/defer.h"
#include "../../../src/util/time_now_us.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


static
tc_ctrl_msg_t gen_add_fifo_queue(void)
{

  tc_ctrl_msg_t ans = {.type = TC_CTRL_SM_V0_QUEUE,
                       .q.act = TC_CTRL_ACTION_SM_V0_ADD}; 

  tc_add_ctrl_queue_t* q = &ans.q.add;

  q->type = TC_QUEUE_FIFO;

  assert(ans.type == TC_CTRL_SM_V0_QUEUE );

  return ans;
}

static
tc_ctrl_msg_t gen_shaper(void)
{
  tc_ctrl_msg_t ans = {.type =  TC_CTRL_SM_V0_SHP,
                       .shp.act =  TC_CTRL_ACTION_SM_V0_MOD}; 

  tc_mod_ctrl_shp_t* mod = &ans.shp.mod;

  mod->id = 2;
  mod->time_window_ms = 100; 
  mod->max_rate_kbps = 15000;
  mod->active = 1;
  return ans;
}

static
tc_ctrl_msg_t gen_add_osi_cls(uint32_t src_port) 
{
  tc_ctrl_msg_t ans = {.type = TC_CTRL_SM_V0_CLS,
		.cls.act = TC_CTRL_ACTION_SM_V0_ADD}; 

	tc_add_ctrl_cls_t* add = &ans.cls.add; 
	add->type = TC_CLS_OSI;  
	add->osi.dst_queue = 2;
	add->osi.l3.src_addr = -1;  
	add->osi.l3.dst_addr = -1;
	add->osi.l4.src_port = src_port; 
	add->osi.l4.dst_port = -1;
	add->osi.l4.protocol = -1;
	return ans;
}


int main(int argc, char *argv[])
{
  //Init the xApp
  init_xapp_api(argc, argv);
  sleep(1);

  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });

  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len );

  e2_node_connected_t* n = &nodes.n[0];
  for(size_t i = 0; i < n->len_rf; ++i)
    printf("Registered ran func id = %d \n ", n->ack_rf[i].id );

 const int TC_SM_ID_TEST = 146;

 for(int i =0 ; i < 3; ++i){
 // Create second FIFO queue
   sm_ag_if_wr_t wr = {.type = TC_CTRL_REQ_V0 };
   wr.tc_req_ctrl.msg = gen_add_fifo_queue();
   control_sm_xapp_api(&nodes.n[0].id, TC_SM_ID_TEST, &wr);
   free_tc_ctrl_msg(&wr.tc_req_ctrl.msg);
 }

 uint32_t src_port = atoi(argv[1]);
 // Rule to segregate on the classifier
 sm_ag_if_wr_t wr_cls = {.type = TC_CTRL_REQ_V0 };
 wr_cls.tc_req_ctrl.msg = gen_add_osi_cls(src_port);
 control_sm_xapp_api(&nodes.n[0].id, TC_SM_ID_TEST, &wr_cls);
 free_tc_ctrl_msg(&wr_cls.tc_req_ctrl.msg);

 
 // Rule to segregate on the classifier
 wr_cls.type = TC_CTRL_REQ_V0 ;
 wr_cls.tc_req_ctrl.msg = gen_add_osi_cls(src_port+1);
 control_sm_xapp_api(&nodes.n[0].id, TC_SM_ID_TEST, &wr_cls);
 free_tc_ctrl_msg(&wr_cls.tc_req_ctrl.msg);

 // Rule to segregate on the classifier
 wr_cls.type = TC_CTRL_REQ_V0;
 wr_cls.tc_req_ctrl.msg = gen_add_osi_cls(src_port+2);
 control_sm_xapp_api(&nodes.n[0].id, TC_SM_ID_TEST, &wr_cls);
 free_tc_ctrl_msg(&wr_cls.tc_req_ctrl.msg);

 // Rule to limit the BW in the shaper
 sm_ag_if_wr_t wr_shp = {.type = TC_CTRL_REQ_V0 };
 wr_shp.tc_req_ctrl.msg = gen_shaper();
 control_sm_xapp_api(&nodes.n[0].id, TC_SM_ID_TEST, &wr_shp);
 free_tc_ctrl_msg(&wr_shp.tc_req_ctrl.msg);

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);     

  printf("Test xApp limit BW run SUCCESSFULLY\n");
}


