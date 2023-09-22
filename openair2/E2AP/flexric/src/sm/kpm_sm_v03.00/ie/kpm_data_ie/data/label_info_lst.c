#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "label_info_lst.h"

label_info_lst_t cp_label_info(label_info_lst_t const *src) 
{
  assert(src != NULL);

  label_info_lst_t dst = {0}; 
  if(src->noLabel != NULL){
    dst.noLabel = calloc(1, sizeof(enum_value_e));
    assert(dst.noLabel != NULL && "Memory exhausted");
    *dst.noLabel = *src->noLabel;
  }

  if (src->plmn_id != NULL) {
    dst.plmn_id = calloc(1, sizeof(*dst.plmn_id));
    assert(dst.plmn_id != NULL && "Memory exhausted");
    *dst.plmn_id = cp_e2sm_plmn( src->plmn_id);
  }

  assert(src->sliceID == NULL && "Not implemented");
  assert(src->fiveQI == NULL && "Not implemented");
  assert(src->qFI == NULL && "Not implemented");
  assert(src->qCI == NULL && "Not implemented");
  assert(src->qCImax == NULL && "Not implemented");
  assert(src->qCImin == NULL && "Not implemented");
  assert(src->aRPmax == NULL && "Not implemented");
  assert(src->aRPmin == NULL && "Not implemented");
  assert(src->bitrateRange == NULL && "Not implemented");
  assert(src->layerMU_MIMO == NULL && "Not implemented");
  assert(src->sUM == NULL && "Not implemented");
  assert(src->distBinX == NULL && "Not implemented");
  assert(src->distBinY == NULL && "Not implemented");
  assert(src->distBinZ == NULL && "Not implemented");
  assert(src->preLabelOverride == NULL && "Not implemented");
  assert(src->startEndInd == NULL && "Not implemented");
  assert(src->min == NULL && "Not implemented");
  assert(src->max == NULL && "Not implemented");
 
   if (src->avg != NULL) {
    dst.avg = calloc(1, sizeof(*dst.avg));
    assert(dst.avg != NULL && "Memory exhausted");
    *dst.avg = *src->avg;
  }

  assert(src->ssbIndex == NULL && "Not implemented");
  assert(src->nonGoB_beamformModeIndex == NULL && "Not implemented");
  assert(src->mimoModeIndex == NULL && "Not implemented");

  return dst;
}

void free_label_info(label_info_lst_t *l) 
{
  assert(l != NULL);

  if (l->noLabel)
    free(l->noLabel);
  if (l->plmn_id)
    free(l->plmn_id);
 
  if (l->sliceID != NULL) {
    assert(false && "not implemented");
  }
	if (l->fiveQI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qFI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCI != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCImax != NULL) {
    assert(false && "not implemented");
  }
	if (l->qCImin != NULL) {
    assert(false && "not implemented");
  }
	if (l->aRPmax != NULL) {
    assert(false && "not implemented");
  }
	if (l->aRPmin != NULL) {
    assert(false && "not implemented");
  }
	if (l->bitrateRange != NULL) {
    assert(false && "not implemented");
  }
	if (l->layerMU_MIMO != NULL) {
    assert(false && "not implemented");
  }
	if (l->sUM != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinX != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinY != NULL) {
    assert(false && "not implemented");
  }
	if (l->distBinZ != NULL) {
    assert(false && "not implemented");
  }
	if (l->preLabelOverride != NULL) {
    assert(false && "not implemented");
  }
	if (l->startEndInd != NULL) {
    assert(false && "not implemented");
  }
	if (l->min != NULL) {
    assert(false && "not implemented");
  }
	if (l->max != NULL) {
    assert(false && "not implemented");
  }
	if (l->avg != NULL) {
    free(l->avg);
  }
}


bool eq_label_info(const label_info_lst_t *l1, const label_info_lst_t *l2)
{
  assert(l1 != NULL);
  assert(l2 != NULL);

  if(l1->noLabel != NULL && l2->noLabel != NULL){
    if(*l1->noLabel != *l2->noLabel)
      return false;
  } else if(l1->noLabel == NULL && l2->noLabel != NULL) {
      return false;
  } else if(l1->noLabel != NULL && l2->noLabel == NULL) {
      return false;
  }

  
  if (eq_e2sm_plmn(l1->plmn_id, l2->plmn_id) != true)
    return false;

  assert(l1->sliceID == NULL && "Not implemented");
  assert(l2->sliceID == NULL && "Not implemented");

  assert(l1->fiveQI == NULL && "Not implemented");
  assert(l2->fiveQI == NULL && "Not implemented");

  assert(l1->qFI == NULL && "Not implemented");
  assert(l2->qFI == NULL && "Not implemented");

  assert(l1->qCI == NULL && "Not implemented");
  assert(l2->qCI == NULL && "Not implemented");
  
  assert(l1->qCImax == NULL && "Not implemented");
  assert(l2->qCImax == NULL && "Not implemented");

  assert(l1->qCImin == NULL && "Not implemented");
  assert(l2->qCImin == NULL && "Not implemented");

  assert(l1->aRPmax == NULL && "Not implemented");
  assert(l2->aRPmax == NULL && "Not implemented");

  assert(l1->qCImin == NULL && "Not implemented");
  assert(l2->qCImin == NULL && "Not implemented");

  assert(l1->aRPmin == NULL && "Not implemented");
  assert(l2->aRPmin == NULL && "Not implemented");

  assert(l1->bitrateRange == NULL && "Not implemented");
  assert(l2->bitrateRange == NULL && "Not implemented");

  assert(l1->layerMU_MIMO == NULL && "Not implemented");
  assert(l2->layerMU_MIMO == NULL && "Not implemented");

  assert(l1->sUM == NULL && "Not implemented");
  assert(l2->sUM == NULL && "Not implemented");

  assert(l1->distBinX == NULL && "Not implemented");
  assert(l2->distBinX == NULL && "Not implemented");

  assert(l1->distBinY == NULL && "Not implemented");
  assert(l2->distBinY == NULL && "Not implemented");

  assert(l1->distBinZ == NULL && "Not implemented");
  assert(l2->distBinZ == NULL && "Not implemented");

  assert(l1->preLabelOverride == NULL && "Not implemented");
  assert(l2->preLabelOverride == NULL && "Not implemented");

  assert(l1->startEndInd == NULL && "Not implemented");
  assert(l2->startEndInd == NULL && "Not implemented");

  assert(l1->min == NULL && "Not implemented");
  assert(l2->min == NULL && "Not implemented");

  assert(l1->max == NULL && "Not implemented");
  assert(l2->max == NULL && "Not implemented");
  
  if(l1->avg != NULL || l2->avg != NULL){
    if(l1->avg == NULL)
      return false;
    if(l2->avg == NULL)
      return false;
    if(*l1->avg != *l2->avg)
      return false;
  }

  assert(l1->ssbIndex == NULL && "Not implemented");
  assert(l2->ssbIndex == NULL && "Not implemented");

  assert(l1->nonGoB_beamformModeIndex == NULL && "Not implemented");
  assert(l2->nonGoB_beamformModeIndex == NULL && "Not implemented");
  
  assert(l1->mimoModeIndex == NULL && "Not implemented");
  assert(l2->mimoModeIndex == NULL && "Not implemented");

  return true;
}
