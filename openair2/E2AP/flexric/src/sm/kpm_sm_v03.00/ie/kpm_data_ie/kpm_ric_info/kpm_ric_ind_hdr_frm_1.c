#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "kpm_ric_ind_hdr_frm_1.h"

void free_kpm_ind_hdr_frm_1(kpm_ric_ind_hdr_format_1_t* src)
{
  assert(src != NULL);
  if (src->fileformat_version != NULL)
  {
    free_byte_array(*src->fileformat_version);
    free(src->fileformat_version);
  }
    
  if (src->sender_name != NULL)
  {
    free_byte_array(*src->sender_name);
    free(src->sender_name);
  }
    
  if (src->sender_type != NULL)
  {
    free_byte_array(*src->sender_type);
    free(src->sender_type);
  }
    
  if (src->vendor_name != NULL)
  {
    free_byte_array(*src->vendor_name);
    free(src->vendor_name);
  }
}

kpm_ric_ind_hdr_format_1_t cp_kpm_ind_hdr_frm_1(kpm_ric_ind_hdr_format_1_t const* src)
{
  assert(src != NULL);
  kpm_ric_ind_hdr_format_1_t ret = {0};

  ret.collectStartTime = src->collectStartTime;
  
  if (src->fileformat_version)
  {
    ret.fileformat_version = calloc(1, sizeof(byte_array_t));
    assert(ret.fileformat_version != NULL && "Memory exhausted");
    *ret.fileformat_version = copy_byte_array(*src->fileformat_version);
  }

  if (src->sender_name)
  {
    ret.sender_name = calloc(1, sizeof(byte_array_t));
    assert(ret.sender_name != NULL && "Memory exhausted");
    *ret.sender_name = copy_byte_array(*src->sender_name);
  }
  if (src->sender_type)
  {
    ret.sender_type = calloc(1, sizeof(byte_array_t));
    assert(ret.sender_type != NULL && "Memory exhausted");
    *ret.sender_type = copy_byte_array(*src->sender_type);
  }

  if (src->vendor_name)
  {
    ret.vendor_name = calloc(1, sizeof(byte_array_t));
    assert(ret.vendor_name != NULL && "Memory exhausted");
    *ret.vendor_name = copy_byte_array(*src->vendor_name);
  }

  return ret;
}

bool eq_kpm_ind_hdr_frm_1(kpm_ric_ind_hdr_format_1_t const* hdr0, kpm_ric_ind_hdr_format_1_t const* hdr1)
{
  assert(hdr0 != NULL);
  assert(hdr1 != NULL);

  if (hdr0 == hdr1) return true;

  if (hdr0->collectStartTime != hdr1->collectStartTime) return false;

  if (eq_byte_array(hdr0->fileformat_version, hdr1->fileformat_version) == false) return false;

  if (eq_byte_array(hdr0->sender_name, hdr1->sender_name) == false) return false;

  if (eq_byte_array(hdr0->sender_type, hdr1->sender_type) == false) return false;

  if (eq_byte_array(hdr0->vendor_name, hdr1->vendor_name) == false) return false;

  return true;
}
