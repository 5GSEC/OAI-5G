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

/*! \file f1ap_common.c
 * \brief f1ap procedures for both CU and DU
 * \author EURECOM/NTUST
 * \date 2018
 * \version 0.1
 * \company Eurecom
 * \email: navid.nikaein@eurecom.fr, bing-kai.hong@eurecom.fr
 * \note
 * \warning
 */

#include "f1ap_common.h"

static f1ap_cudu_inst_t *f1_du_inst[NUMBER_OF_gNB_MAX]= {0};
static f1ap_cudu_inst_t *f1_cu_inst[NUMBER_OF_gNB_MAX]= {0};

uint8_t F1AP_get_next_transaction_identifier(instance_t mod_idP, instance_t cu_mod_idP) {
  static uint8_t transaction_identifier[NUMBER_OF_gNB_MAX];
  transaction_identifier[mod_idP+cu_mod_idP] =
    (transaction_identifier[mod_idP+cu_mod_idP] + 1) % F1AP_TRANSACTION_IDENTIFIER_NUMBER;
  return transaction_identifier[mod_idP+cu_mod_idP];
}

f1ap_cudu_inst_t *getCxt(F1_t isCU, instance_t instanceP) {
  //Fixme: 4G F1 has race condtions, someone may debug it, using this test
  /*
    static pid_t t=-1;
    pid_t tNew=gettid();
    AssertFatal ( t==-1 || t==tNew, "This is not thread safe\n");
    t=tNew;
  */
  AssertFatal( instanceP < sizeofArray(f1_cu_inst), "");
  return isCU == CUtype ? f1_cu_inst[ instanceP]:  f1_du_inst[ instanceP];
}

void createF1inst(F1_t isCU, instance_t instanceP, f1ap_setup_req_t *req) {
  if (isCU == CUtype) {
    AssertFatal(f1_cu_inst[instanceP] == NULL, "Double call to F1 CU init\n");
    f1_cu_inst[instanceP]=( f1ap_cudu_inst_t *) calloc(1, sizeof( f1ap_cudu_inst_t));
    //memcpy(f1_cu_inst[instanceP]->setupReq, req, sizeof(f1ap_setup_req_t) );
  } else {
    AssertFatal(f1_du_inst[instanceP] == NULL, "Double call to F1 DU init\n");
    f1_du_inst[instanceP]=( f1ap_cudu_inst_t *) calloc(1,  sizeof(f1ap_cudu_inst_t));
    memcpy(&f1_du_inst[instanceP]->setupReq, req, sizeof(f1ap_setup_req_t) );
  }
}

int f1ap_add_ue(F1_t isCu,
                instance_t          instanceP,
                rnti_t               rntiP) {
  f1ap_cudu_inst_t *f1_inst=getCxt(isCu, instanceP);

  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].rnti == rntiP) {
      f1_inst->f1ap_ue[i].f1ap_uid = i;
      LOG_I(F1AP, "Updating the index of UE with RNTI %x and du_ue_f1ap_id %ld\n", f1_inst->f1ap_ue[i].rnti, f1_inst->f1ap_ue[i].du_ue_f1ap_id);
      return i;
    }
  }

  // We didn't find the rnti
  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].rnti == 0 ) {
      f1_inst->f1ap_ue[i].rnti = rntiP;
      f1_inst->f1ap_ue[i].f1ap_uid = i;
      f1_inst->f1ap_ue[i].du_ue_f1ap_id = rntiP;
      f1_inst->f1ap_ue[i].cu_ue_f1ap_id = rntiP;
      f1_inst->num_ues++;
      LOG_I(F1AP, "Adding a new UE with RNTI %x and cu/du ue_f1ap_id %ld\n", f1_inst->f1ap_ue[i].rnti, f1_inst->f1ap_ue[i].du_ue_f1ap_id);
      return i;
    }
  }

  return -1;
}


int f1ap_remove_ue(F1_t isCu, instance_t instanceP,
                   rnti_t            rntiP) {
  f1ap_cudu_inst_t *f1_inst=getCxt(isCu, instanceP);

  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].rnti == rntiP) {
      f1_inst->f1ap_ue[i].rnti = 0;
      break;
    }
  }

  f1_inst->num_ues--;
  return 0;
}

int f1ap_get_du_ue_f1ap_id(F1_t isCu, instance_t instanceP,
                           rnti_t            rntiP) {
  f1ap_cudu_inst_t *f1_inst=getCxt(isCu, instanceP);

  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].rnti == rntiP) {
      return f1_inst->f1ap_ue[i].du_ue_f1ap_id;
    }
  }

  return -1;
}

int f1ap_get_cu_ue_f1ap_id(F1_t isCu, instance_t instanceP,
                           rnti_t            rntiP) {
  f1ap_cudu_inst_t *f1_inst=getCxt(isCu, instanceP);

  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].rnti == rntiP) {
      return f1_inst->f1ap_ue[i].cu_ue_f1ap_id;
    }
  }

  return -1;
}

int f1ap_get_rnti_by_du_id(F1_t isCu, instance_t instanceP,
                           instance_t       du_ue_f1ap_id ) {
  f1ap_cudu_inst_t *f1_inst=getCxt(isCu, instanceP);

  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].du_ue_f1ap_id == du_ue_f1ap_id) {
      return f1_inst->f1ap_ue[i].rnti;
    }
  }

  return -1;
}

int f1ap_get_rnti_by_cu_id(F1_t isCu, instance_t instanceP,
                           instance_t       cu_ue_f1ap_id ) {
  f1ap_cudu_inst_t *f1_inst=getCxt(isCu, instanceP);

  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].cu_ue_f1ap_id == cu_ue_f1ap_id) {
      return f1_inst->f1ap_ue[i].rnti;
    }
  }

  return -1;
}

int f1ap_du_add_cu_ue_id(instance_t instanceP,
                         instance_t       du_ue_f1ap_id,
                         instance_t       cu_ue_f1ap_id) {
  f1ap_cudu_inst_t *f1_inst=getCxt(DUtype, instanceP);
  instance_t f1ap_uid=-1;

  for (int i = 0; i < MAX_MOBILES_PER_GNB; i++) {
    if (f1_inst->f1ap_ue[i].du_ue_f1ap_id == du_ue_f1ap_id) {
      f1ap_uid=i;
      break;
    }
  }

  if (f1ap_uid < 0 || f1ap_uid >= MAX_MOBILES_PER_GNB)
    return -1;

  f1_inst->f1ap_ue[f1ap_uid].cu_ue_f1ap_id = cu_ue_f1ap_id;
  LOG_D(F1AP, "Adding cu_ue_f1ap_id %ld for UE with RNTI %x\n", cu_ue_f1ap_id, f1_inst->f1ap_ue[f1ap_uid].rnti);
  return 0;
}

int f1ap_assoc_id(F1_t isCu, instance_t instanceP) {
  f1ap_setup_req_t *f1_inst=f1ap_req(isCu, instanceP);
  return f1_inst->assoc_id;
}
