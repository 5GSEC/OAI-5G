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


#include "e2ap_node_component_config_update.h"
#include <assert.h>
#include <stdlib.h>

static
byte_array_t* create_assign_ba(byte_array_t* src)
{
  byte_array_t* dst = calloc(1, sizeof(byte_array_t));
  assert(dst != NULL && "Memory ehausted");
  *dst = copy_byte_array(*src);
  return dst;
} 

e2_node_component_config_update_t cp_e2_node_component_config_update(const e2_node_component_config_update_t* src)
{
  assert(src != NULL);
  e2_node_component_config_update_t dst = {
    .e2_node_component_type = src->e2_node_component_type,
  }; 

  if(src->id_present != NULL){
    dst.id_present = calloc(1,sizeof(e2_node_component_id_present_e ));
    assert(dst.id_present != NULL && "Memory exhausted");
    dst.id_present = src->id_present;
    // Does not matter the value of src->id_present
    dst.gnb_cu_up_id = src->gnb_cu_up_id;
  }

  dst.update_present = src->update_present;
  if(dst.update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_GNB_CONFIG_UPDATE){
    dst.gnb.ngap_gnb_cu_cp = create_assign_ba(src->gnb.ngap_gnb_cu_cp);
    dst.gnb.xnap_gnb_cu_cp = create_assign_ba(src->gnb.xnap_gnb_cu_cp);
    dst.gnb.e1ap_gnb_cu_cp = create_assign_ba(src->gnb.e1ap_gnb_cu_cp);
    dst.gnb.f1ap_gnb_cu_cp = create_assign_ba(src->gnb.f1ap_gnb_cu_cp);
  } else if (dst.update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_EN_GNB_CONFIG_UPDATE ){
    dst.en_gnb.x2ap_en_gnb = create_assign_ba(src->en_gnb.x2ap_en_gnb); 
  } else if (dst.update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_NG_ENB_CONFIG_UPDATE ){
    dst.ng_enb.ngap_ng_enb = create_assign_ba(src->ng_enb.ngap_ng_enb);
    dst.ng_enb.xnap_ng_enb = create_assign_ba(src->ng_enb.xnap_ng_enb);

  } else if (dst.update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_ENB_CONFIG_UPDATE ){
    dst.enb.s1ap_enb = create_assign_ba(src->enb.s1ap_enb);
    dst.enb.x2ap_enb = create_assign_ba(src->enb.x2ap_enb);
  } else {
    assert(0 != 0 && "Not foreseen switch case");
  }

  return dst;
}

void free_e2_node_component_config_update(e2_node_component_config_update_t* src)
{
  assert(src != NULL); 

  if(src->id_present != NULL){
    free(src->id_present);
  }

  if(src->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_GNB_CONFIG_UPDATE){
    free_byte_array(*src->gnb.ngap_gnb_cu_cp);
    free_byte_array(*src->gnb.xnap_gnb_cu_cp);
    free_byte_array(*src->gnb.e1ap_gnb_cu_cp);
    free_byte_array(*src->gnb.f1ap_gnb_cu_cp);
  } else if (src->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_EN_GNB_CONFIG_UPDATE ){
    free_byte_array(*src->en_gnb.x2ap_en_gnb);
  } else if (src->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_NG_ENB_CONFIG_UPDATE ){
    free_byte_array(*src->ng_enb.ngap_ng_enb);
    free_byte_array(*src->ng_enb.xnap_ng_enb);
  } else if (src->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_ENB_CONFIG_UPDATE ){
    free_byte_array(*src->enb.s1ap_enb);
    free_byte_array(*src->enb.x2ap_enb);
  } else {
    assert(0 != 0 && "Not foreseen switch case");
  }

}




static
bool eq_e2_node_component_id_present(const e2_node_component_id_present_e* m0, const e2_node_component_id_present_e* m1)
{
  if(m0 == m1) return true;
 
  if(m0 == NULL || m1 == NULL)
    return false;

  if(*m0 != *m1)
    return false;

  return true;
}

bool eq_e2_node_component_config_update(const e2_node_component_config_update_t* m0, const e2_node_component_config_update_t* m1)
{
  if(m0 == m1) return true;

  if(m0 == NULL || m1 == NULL) return false;


  if(m0->e2_node_component_type != m1->e2_node_component_type)
    return false;

  if(eq_e2_node_component_id_present(m0->id_present,m1->id_present) == false)
    return false;

  if(m0->gnb_cu_up_id != m1->gnb_cu_up_id)
    return false;

  if(m0->update_present != m1->update_present)
    return false;

  if(m0->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_GNB_CONFIG_UPDATE){
    if(eq_byte_array(m0->gnb.ngap_gnb_cu_cp, m1->gnb.ngap_gnb_cu_cp) == false)
      return false;
    if(eq_byte_array(m0->gnb.xnap_gnb_cu_cp, m1->gnb.xnap_gnb_cu_cp) == false )
      return false;
    if(eq_byte_array(m0->gnb.e1ap_gnb_cu_cp, m1->gnb.e1ap_gnb_cu_cp) == false )
      return false;
    if(eq_byte_array(m0->gnb.f1ap_gnb_cu_cp, m1->gnb.f1ap_gnb_cu_cp) == false )
      return false;
  } else if (m0->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_EN_GNB_CONFIG_UPDATE){
    if(eq_byte_array(m0->en_gnb.x2ap_en_gnb, m1->en_gnb.x2ap_en_gnb) == false)
      return false;
  } else if(m0->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_NG_ENB_CONFIG_UPDATE){
    if(eq_byte_array(m0->ng_enb.ngap_ng_enb, m1->ng_enb.ngap_ng_enb) == false)
      return false;
    if(eq_byte_array(m0->ng_enb.xnap_ng_enb, m1->ng_enb.xnap_ng_enb) == false)
      return false;
  } else if(m0->update_present == E2_NODE_COMPONENT_CONFIG_UPDATE_ENB_CONFIG_UPDATE){
    if(eq_byte_array(m0->enb.s1ap_enb, m1->enb.s1ap_enb) == false)
      return false;
    if(eq_byte_array(m0->enb.x2ap_enb, m1->enb.x2ap_enb) == false)
      return false;
  } else {
    assert(0!=0 && "Error, case not foreseen");
  }
  return true;
}

