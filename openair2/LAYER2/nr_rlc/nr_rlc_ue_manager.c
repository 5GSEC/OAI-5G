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

#include "nr_rlc_ue_manager.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "LOG/log.h"

typedef struct {
  pthread_mutex_t lock;
  nr_rlc_ue_t     **ue_list;
  int             ue_count;
  int             enb_flag;
} nr_rlc_ue_manager_internal_t;

nr_rlc_ue_manager_t *new_nr_rlc_ue_manager(int enb_flag)
{
  nr_rlc_ue_manager_internal_t *ret;

  ret = calloc(1, sizeof(nr_rlc_ue_manager_internal_t));
  if (ret == NULL) {
    LOG_E(RLC, "%s:%d:%s: out of memory\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  if (pthread_mutex_init(&ret->lock, NULL)) abort();
  ret->enb_flag = enb_flag;

  return ret;
}

int nr_rlc_manager_get_enb_flag(nr_rlc_ue_manager_t *_m)
{
  nr_rlc_ue_manager_internal_t *m = _m;
  return m->enb_flag;
}

void nr_rlc_manager_lock(nr_rlc_ue_manager_t *_m)
{
  nr_rlc_ue_manager_internal_t *m = _m;
  if (pthread_mutex_lock(&m->lock)) {
    LOG_E(RLC, "%s:%d:%s: fatal\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }
}

void nr_rlc_manager_unlock(nr_rlc_ue_manager_t *_m)
{
  nr_rlc_ue_manager_internal_t *m = _m;
  if (pthread_mutex_unlock(&m->lock)) {
    LOG_E(RLC, "%s:%d:%s: fatal\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }
}

/* must be called with lock acquired */
nr_rlc_ue_t *nr_rlc_manager_get_ue(nr_rlc_ue_manager_t *_m, int rnti)
{
  /* TODO: optimze */
  nr_rlc_ue_manager_internal_t *m = _m;
  int i;

  for (i = 0; i < m->ue_count; i++)
    if (m->ue_list[i]->rnti == rnti)
      return m->ue_list[i];

  LOG_D(RLC, "%s:%d:%s: new UE with RNTI 0x%x\n", __FILE__, __LINE__, __FUNCTION__, rnti);

  m->ue_count++;
  m->ue_list = realloc(m->ue_list, sizeof(nr_rlc_ue_t *) * m->ue_count);
  if (m->ue_list == NULL) {
    LOG_E(RLC, "%s:%d:%s: out of memory\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }
  m->ue_list[m->ue_count-1] = calloc(1, sizeof(nr_rlc_ue_t));
  if (m->ue_list[m->ue_count-1] == NULL) {
    LOG_E(RLC, "%s:%d:%s: out of memory\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  m->ue_list[m->ue_count-1]->rnti = rnti;

  return m->ue_list[m->ue_count-1];
}

/* must be called with lock acquired */
void nr_rlc_manager_remove_ue(nr_rlc_ue_manager_t *_m, int rnti)
{
  nr_rlc_ue_manager_internal_t *m = _m;
  nr_rlc_ue_t *ue;
  int i;
  int j;

  for (i = 0; i < m->ue_count; i++)
    if (m->ue_list[i]->rnti == rnti)
      break;

  if (i == m->ue_count) {
    LOG_W(RLC, "%s:%d:%s: warning: ue %x not found\n",
          __FILE__, __LINE__, __FUNCTION__,
          rnti);
    return;
  }

  ue = m->ue_list[i];

  if (ue->srb0 != NULL) {
    /* deliver_sdu_data for srb0 is allocated, needs a free() */
    free(ue->srb0->deliver_sdu_data);
    ue->srb0->delete(ue->srb0);
  }

  for (j = 0; j < 3; j++)
    if (ue->srb[j] != NULL)
      ue->srb[j]->delete(ue->srb[j]);

  for (j = 0; j < MAX_DRBS_PER_UE; j++)
    if (ue->drb[j] != NULL)
      ue->drb[j]->delete(ue->drb[j]);

  free(ue);

  m->ue_count--;
  if (m->ue_count == 0) {
    free(m->ue_list);
    m->ue_list = NULL;
    return;
  }

  memmove(&m->ue_list[i], &m->ue_list[i+1],
          (m->ue_count - i) * sizeof(nr_rlc_ue_t *));
  m->ue_list = realloc(m->ue_list, m->ue_count * sizeof(nr_rlc_ue_t *));
  if (m->ue_list == NULL) {
    LOG_E(RLC, "%s:%d:%s: fatal\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }
}

/* must be called with lock acquired */
void nr_rlc_ue_add_srb_rlc_entity(nr_rlc_ue_t *ue, int srb_id, nr_rlc_entity_t *entity)
{
  if (srb_id < 0 || srb_id > 3) {
    LOG_E(RLC, "%s:%d:%s: fatal, bad srb id\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  /* special case: srb0 */
  if (srb_id == 0) {
    if (ue->srb0 != NULL) {
      LOG_E(RLC, "fatal, srb0 already present\n");
      exit(1);
    }

    ue->srb0 = entity;

    return;
  }

  srb_id--;

  if (ue->srb[srb_id] != NULL) {
    LOG_E(RLC, "%s:%d:%s: fatal, srb already present\n",
          __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  ue->srb[srb_id] = entity;
}

/* must be called with lock acquired */
void nr_rlc_ue_add_drb_rlc_entity(nr_rlc_ue_t *ue, int drb_id, nr_rlc_entity_t *entity)
{
  if (drb_id < 1 || drb_id > MAX_DRBS_PER_UE) {
    LOG_E(RLC, "%s:%d:%s: fatal, bad drb id\n", __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  drb_id--;

  if (ue->drb[drb_id] != NULL) {
    LOG_E(RLC, "%s:%d:%s: fatal, drb already present\n",
          __FILE__, __LINE__, __FUNCTION__);
    exit(1);
  }

  ue->drb[drb_id] = entity;
}
