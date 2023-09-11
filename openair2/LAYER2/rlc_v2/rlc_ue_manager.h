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

#ifndef _RLC_UE_MANAGER_H_
#define _RLC_UE_MANAGER_H_

#include "rlc_entity.h"

typedef void rlc_ue_manager_t;

typedef struct rlc_ue_t {
  int rnti;
  int module_id;   /* necesarry for the L2 simulator - not clean, to revise */
  rlc_entity_t *srb[2];
  rlc_entity_t *drb[5];
} rlc_ue_t;

/***********************************************************************/
/* manager functions                                                   */
/***********************************************************************/

rlc_ue_manager_t *new_rlc_ue_manager(int enb_flag);

int rlc_manager_get_enb_flag(rlc_ue_manager_t *m);

void rlc_manager_lock(rlc_ue_manager_t *m);
void rlc_manager_unlock(rlc_ue_manager_t *m);

rlc_ue_t *rlc_manager_get_ue(rlc_ue_manager_t *m, int rnti);
void rlc_manager_remove_ue(rlc_ue_manager_t *m, int rnti);

/***********************************************************************/
/* ue functions                                                        */
/***********************************************************************/

void rlc_ue_add_srb_rlc_entity(rlc_ue_t *ue, int srb_id, rlc_entity_t *entity);
void rlc_ue_add_drb_rlc_entity(rlc_ue_t *ue, int drb_id, rlc_entity_t *entity);

#endif /* _RLC_UE_MANAGER_H_ */
