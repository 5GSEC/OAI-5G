  /*
  * XXX-optimization: understand where do they come these values below for memory allocation of the byte arrays.
  * A tuning of those values would probably benefit this implementation
  * XXX-implementation, cfr: https://gitlab.eurecom.fr/mosaic5g/flexric/-/blob/rrc-sm/src/sm/rrc_sm/enc/rrc_enc_asn.c
  */

#include "../ie/asn/asn_constant.h"

#include "../ie/asn/E2SM-KPM-EventTriggerDefinition.h"
#include "../ie/asn/E2SM-KPM-ActionDefinition.h"
#include "../ie/asn/E2SM-KPM-IndicationHeader.h"
#include "../ie/asn/E2SM-KPM-IndicationMessage.h"
#include "../ie/asn/E2SM-KPM-RANfunction-Description.h"
#include "../ie/asn/RANfunction-Name.h"
#include "../ie/asn/RIC-EventTriggerStyle-Item.h"
#include "../ie/asn/RIC-ReportStyle-Item.h"
#include "../ie/asn/MeasurementInfo-Action-Item.h"

#include "enc_asn/enc_ric_event_trigger_frm_1.h"

#include "enc_asn/enc_ric_action_def_frm_1.h"
#include "enc_asn/enc_ric_action_def_frm_2.h"
#include "enc_asn/enc_ric_action_def_frm_3.h"
#include "enc_asn/enc_ric_action_def_frm_4.h"
#include "enc_asn/enc_ric_action_def_frm_5.h"

#include "enc_asn/enc_ric_ind_hdr_frm_1.h"

#include "enc_asn/enc_ric_ind_msg_frm_1.h"
#include "enc_asn/enc_ric_ind_msg_frm_2.h"
#include "enc_asn/enc_ric_ind_msg_frm_3.h"

#include "kpm_enc_asn.h"
#include "../../../util/conversions.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/* Encoding Event Trigger IE
 * Direction of the message: RIC --> E2 Node
 */
byte_array_t kpm_enc_event_trigger_asn(kpm_event_trigger_def_t const* event_trigger) {

  assert(event_trigger != NULL);

  E2SM_KPM_EventTriggerDefinition_t *pdu = calloc(1, sizeof(E2SM_KPM_EventTriggerDefinition_t));
  assert(pdu != NULL && "Memory exhausted" );


  switch (event_trigger->type)
  {
  case FORMAT_1_RIC_EVENT_TRIGGER:{
    pdu->eventDefinition_formats.present = E2SM_KPM_EventTriggerDefinition__eventDefinition_formats_PR_eventDefinition_Format1;
    pdu->eventDefinition_formats.choice.eventDefinition_Format1 = kpm_enc_event_trigger_def_frm_1_asn(&event_trigger->kpm_ric_event_trigger_format_1);
    break;
                                  }
  default:{
    assert("Non valid KPM RIC Event Trigger Format");
          }
  }
  
  byte_array_t ba = {.buf = malloc(2048), .len = 2048}; 
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_KPM_EventTriggerDefinition, pdu, ba.buf, ba.len);
  assert(er.encoded > -1 && (size_t)er.encoded <= ba.len);
  ba.len = er.encoded;


  ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_EventTriggerDefinition, pdu);

  return ba;
}

/* Encoding Action Definition IE
 * Direction of the message: RIC -> E2 Node
 */
byte_array_t kpm_enc_action_def_asn(kpm_act_def_t const* action_def) 
{
  assert(action_def != NULL);

  E2SM_KPM_ActionDefinition_t *pdu = calloc(1, sizeof(E2SM_KPM_ActionDefinition_t));
  
  assert (pdu != NULL && "Memory exhausted");

  pdu->ric_Style_Type = (long)action_def->type + 1;
 
  /* Action Definition Formats */
  switch (action_def->type) 
  {
    case FORMAT_1_ACTION_DEFINITION:
    {
      pdu->actionDefinition_formats.present = E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format1;
      pdu->actionDefinition_formats.choice.actionDefinition_Format1 = calloc(1, sizeof(E2SM_KPM_ActionDefinition_Format1_t));
      assert (pdu->actionDefinition_formats.choice.actionDefinition_Format1 != NULL && "Memory exhausted");
      *pdu->actionDefinition_formats.choice.actionDefinition_Format1 = kpm_enc_action_def_frm_1_asn(&action_def->frm_1);
      break;
    }
    case FORMAT_2_ACTION_DEFINITION:
    {
      pdu->actionDefinition_formats.present = E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format2;
      pdu->actionDefinition_formats.choice.actionDefinition_Format2 = kpm_enc_action_def_frm_2_asn(&action_def->frm_2);
      break;
    }
    case FORMAT_3_ACTION_DEFINITION:
    {
      pdu->actionDefinition_formats.present = E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format3;
      pdu->actionDefinition_formats.choice.actionDefinition_Format3 = kpm_enc_action_def_frm_3_asn(&action_def->frm_3);
      break;
    }
    case FORMAT_4_ACTION_DEFINITION:
    {
      pdu->actionDefinition_formats.present = E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format4;
      pdu->actionDefinition_formats.choice.actionDefinition_Format4 = kpm_enc_action_def_frm_4_asn(&action_def->frm_4);
      break;
    }
    case FORMAT_5_ACTION_DEFINITION:
    {
      pdu->actionDefinition_formats.present = E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format5;
      pdu->actionDefinition_formats.choice.actionDefinition_Format5 = kpm_enc_action_def_frm_5_asn(&action_def->frm_5);
      break;
    }
    default:
    {
      assert(false && "Non valid KPM RIC Action Definition Format");
    }
  }

//  xer_fprint(stderr, &asn_DEF_E2SM_KPM_ActionDefinition, pdu);
//  fflush(stdout);

  /* XXX-tuning: 
   * below bytearray sizing needs to be reviewed and made dynamic. It looks too small for the general case of action definition.
   */

  // FIXME Create one function for encoding common to all the methods.
  byte_array_t ba = {.buf = malloc(2048), .len = 2048}; 
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_KPM_ActionDefinition, pdu, ba.buf, ba.len);
  assert(er.encoded > -1 && (size_t)er.encoded <= ba.len);
  ba.len = er.encoded;


  ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_ActionDefinition, pdu);

  return ba;
}

/* 
 * Encoding of Indication Header IE
 * Direction: NodeE2 -> RIC 
 */
byte_array_t kpm_enc_ind_hdr_asn(kpm_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);
  E2SM_KPM_IndicationHeader_t *pdu = calloc(1,sizeof(E2SM_KPM_IndicationHeader_t));
  assert( pdu !=NULL && "Memory exhausted" );


  switch (ind_hdr->type)
  {
  case FORMAT_1_INDICATION_HEADER:
    pdu->indicationHeader_formats.present = E2SM_KPM_IndicationHeader__indicationHeader_formats_PR_indicationHeader_Format1;
    pdu->indicationHeader_formats.choice.indicationHeader_Format1 = kpm_enc_ind_hdr_frm_1_asn(&ind_hdr->kpm_ric_ind_hdr_format_1);
    break;
  
  default:
    assert(false && "Non valid KPM RIC Indication Header Format");
  }
  
  byte_array_t  ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_KPM_IndicationHeader, pdu, ba.buf, ba.len);
  assert(er.encoded > -1 && (size_t)er.encoded <= ba.len);
  ba.len = er.encoded;


  ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_IndicationHeader, pdu);

  return ba;
}


/* 
 * Encoding of Indication Message IE. 
 * Limitation: just format 1 is implemented further to Action Definition IE type 1 constraint
 * Direction: NodeE2 -> RIC 
 */
byte_array_t kpm_enc_ind_msg_asn(kpm_ind_msg_t const* ind_msg) 
{
  assert(ind_msg != NULL);
  
  E2SM_KPM_IndicationMessage_t *pdu = calloc(1, sizeof(E2SM_KPM_IndicationMessage_t));
  assert( pdu !=NULL && "Memory exhausted" );

  switch (ind_msg->type)
  {
  case FORMAT_1_INDICATION_MESSAGE:
    pdu->indicationMessage_formats.present = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;
    pdu->indicationMessage_formats.choice.indicationMessage_Format1 = calloc(1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
    assert(pdu->indicationMessage_formats.choice.indicationMessage_Format1 != NULL && "Memory exhausted");
    *pdu->indicationMessage_formats.choice.indicationMessage_Format1 = kpm_enc_ind_msg_frm_1_asn(&ind_msg->frm_1);
    break;
  
  case FORMAT_2_INDICATION_MESSAGE:
    pdu->indicationMessage_formats.present = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format2;
    pdu->indicationMessage_formats.choice.indicationMessage_Format2 = kpm_enc_ind_msg_frm_2_asn(&ind_msg->frm_2);
    break;

  case FORMAT_3_INDICATION_MESSAGE:
    pdu->indicationMessage_formats.present = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format3;
    pdu->indicationMessage_formats.choice.indicationMessage_Format3 = kpm_enc_ind_msg_frm_3_asn(&ind_msg->frm_3);
    break;

  default:
    assert(false && "Non valid KPM RIC Indication Message Format");
  }


//  xer_fprint(stderr, &asn_DEF_E2SM_KPM_IndicationMessage, pdu);
//  fflush(stdout);

  byte_array_t  ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_KPM_IndicationMessage, pdu, ba.buf, ba.len);
  assert(er.encoded > -1 && (size_t)er.encoded <= ba.len);
  ba.len = er.encoded;


  ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_IndicationMessage, pdu);

  return ba;
}


/*
 * Encoding of Function Definition IE. 
 * Direction: NodeE2 -> RIC 
 */
byte_array_t kpm_enc_func_def_asn(kpm_ran_function_def_t const* func_def)  
{
  assert(func_def != NULL);
  
  E2SM_KPM_RANfunction_Description_t *pdu = calloc(1, sizeof(E2SM_KPM_RANfunction_Description_t));
  assert( pdu !=NULL && "Memory exhausted" );
  

  //  RAN Function Name
  int ret = OCTET_STRING_fromBuf(&pdu->ranFunction_Name.ranFunction_Description, 
                              (char *)func_def->name.description.buf, 
                              func_def->name.description.len);
  assert(ret == 0);
  
  ret = OCTET_STRING_fromBuf(&pdu->ranFunction_Name.ranFunction_E2SM_OID, 
                              (char *)func_def->name.oid.buf, 
                              func_def->name.oid.len);
  assert(ret == 0);
  
  ret = OCTET_STRING_fromBuf(&pdu->ranFunction_Name.ranFunction_ShortName, 
                              (char *)func_def->name.name.buf, 
                              func_def->name.name.len);
  assert(ret == 0);
  
  if (func_def->name.instance != NULL)
  {
    pdu->ranFunction_Name.ranFunction_Instance = malloc(sizeof(*pdu->ranFunction_Name.ranFunction_Instance));
    assert(pdu->ranFunction_Name.ranFunction_Instance != NULL && "Memory exhausted");
    pdu->ranFunction_Name.ranFunction_Instance = func_def->name.instance;
  }
  else
  {
    pdu->ranFunction_Name.ranFunction_Instance = NULL;
  }
  


  //  RIC Event Trigger Style Item
  if (func_def->ric_event_trigger_style_list != NULL || func_def->sz_ric_event_trigger_style_list != 0)
  {
    assert(func_def->sz_ric_event_trigger_style_list >= 1 && func_def->sz_ric_event_trigger_style_list <= maxnoofRICStyles);

    pdu->ric_EventTriggerStyle_List = calloc(func_def->sz_ric_event_trigger_style_list, sizeof(*pdu->ric_EventTriggerStyle_List));
    assert(pdu->ric_EventTriggerStyle_List != NULL && "Memory exhausted");

    for (size_t i = 0; i<func_def->sz_ric_event_trigger_style_list; i++)
    {
      RIC_EventTriggerStyle_Item_t * event_item = calloc(1, sizeof(RIC_EventTriggerStyle_Item_t));
      assert(event_item != NULL && "Memory exhausted");

      switch (func_def->ric_event_trigger_style_list[i].style_type)
      {
      case STYLE_1_RIC_EVENT_TRIGGER:
      {
        event_item->ric_EventTriggerStyle_Type = 1;

        // RIC Event Trigger Style Name
        int ret = OCTET_STRING_fromBuf(&event_item->ric_EventTriggerStyle_Name, 
                              (char *)func_def->ric_event_trigger_style_list[i].style_name.buf, 
                              func_def->ric_event_trigger_style_list[i].style_name.len);
        assert(ret == 0);

        // RIC Event Trigger Format
        event_item->ric_EventTriggerFormat_Type = 1;

        break;
      }
      
      default:
        assert(false && "Unknown RIC Event Trigger Style Type");
      }

      int rc1 = ASN_SEQUENCE_ADD(&pdu->ric_EventTriggerStyle_List->list, event_item);
      assert(rc1 == 0);
    }
  }




  // RIC Report Style Item
  if (func_def->ric_report_style_list != NULL || func_def->sz_ric_report_style_list != 0)
  {
    assert(func_def->sz_ric_report_style_list >= 1 && func_def->sz_ric_report_style_list <= maxnoofRICStyles);

    pdu->ric_ReportStyle_List = calloc(func_def->sz_ric_report_style_list, sizeof(*pdu->ric_ReportStyle_List));
    assert(pdu->ric_ReportStyle_List != NULL && "Memory exhausted");

    for (size_t i = 0; i<func_def->sz_ric_report_style_list; i++)
    {
      RIC_ReportStyle_Item_t * report_item = calloc(1, sizeof(RIC_ReportStyle_Item_t));
      assert(report_item != NULL && "Memory exhausted");

      // RIC Report Styles
      switch (func_def->ric_report_style_list[i].report_style_type)
      {
      case STYLE_1_RIC_SERVICE_REPORT:
      {
        report_item->ric_ReportStyle_Type = 1;
        report_item->ric_ActionFormat_Type = 1;
        report_item->ric_IndicationHeaderFormat_Type = 1;
        report_item->ric_IndicationMessageFormat_Type = 1;

        break;
      }

      case STYLE_2_RIC_SERVICE_REPORT:
      {
        report_item->ric_ReportStyle_Type = 2;
        report_item->ric_ActionFormat_Type = 2;
        report_item->ric_IndicationHeaderFormat_Type = 1;
        report_item->ric_IndicationMessageFormat_Type = 1;

        break;
      }

      case STYLE_3_RIC_SERVICE_REPORT:
      {
        report_item->ric_ReportStyle_Type = 3;
        report_item->ric_ActionFormat_Type = 3;
        report_item->ric_IndicationHeaderFormat_Type = 1;
        report_item->ric_IndicationMessageFormat_Type = 2;

        break;
      }

      case STYLE_4_RIC_SERVICE_REPORT:
      {
        report_item->ric_ReportStyle_Type = 4;
        report_item->ric_ActionFormat_Type = 4;
        report_item->ric_IndicationHeaderFormat_Type = 1;
        report_item->ric_IndicationMessageFormat_Type = 3;

        break;
      }

      case STYLE_5_RIC_SERVICE_REPORT:
      {
        report_item->ric_ReportStyle_Type = 5;
        report_item->ric_ActionFormat_Type = 5;
        report_item->ric_IndicationHeaderFormat_Type = 1;
        report_item->ric_IndicationMessageFormat_Type = 3;

        break;
      }
      
      default:
        assert(false && "Unknown RIC REPORT Style Type");
      }

      // RIC REPORT Style Name
      int ret = OCTET_STRING_fromBuf(&report_item->ric_ReportStyle_Name, 
                              (char *)func_def->ric_report_style_list[i].report_style_name.buf, 
                              func_def->ric_report_style_list[i].report_style_name.len);
      assert(ret == 0);


      // Measurement Information for Action
      assert(func_def->ric_report_style_list[i].meas_info_for_action_lst_len >= 1 && func_def->ric_report_style_list[i].meas_info_for_action_lst_len <= maxnoofMeasurementInfo);

      for (size_t j = 0; j<func_def->ric_report_style_list[i].meas_info_for_action_lst_len; j++)
      {
        MeasurementInfo_Action_Item_t * meas_item = calloc(1, sizeof(MeasurementInfo_Action_Item_t));
        assert(meas_item != NULL && "Memory exhausted");

        // Measurement Type Name
        int ret = OCTET_STRING_fromBuf(&meas_item->measName, 
                              (char *)func_def->ric_report_style_list[i].meas_info_for_action_lst[j].name.buf, 
                              func_def->ric_report_style_list[i].meas_info_for_action_lst[j].name.len);
        assert(ret == 0);

        // Measurement Type ID
        if (func_def->ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL)
        {
          meas_item->measID = calloc(1, sizeof(MeasurementTypeID_t));
          assert(meas_item->measID != NULL && "Memory exhausted");
          memcpy(meas_item->measID, func_def->ric_report_style_list[i].meas_info_for_action_lst[j].id, 2);
        }

        // Bin Range Definition
        // not yet implemented in ASN.1
        if (func_def->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def != NULL)
          assert(false && "Not yet implemented in ASN.1");

        int rc1 = ASN_SEQUENCE_ADD(&report_item->measInfo_Action_List.list, meas_item);
        assert(rc1 == 0);
      }



      int rc1 = ASN_SEQUENCE_ADD(&pdu->ric_ReportStyle_List->list, report_item);
      assert(rc1 == 0);
    }
  }


//  xer_fprint(stderr, &asn_DEF_E2SM_KPM_RANfunction_Description, pdu);
//  fflush(stdout);

  byte_array_t  ba = {.buf = malloc(2048), .len = 2048};
  assert(ba.buf != NULL && "Memory exhausted");
  const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_KPM_RANfunction_Description, pdu, ba.buf, ba.len);
  assert(er.encoded > -1 && (size_t)er.encoded <= ba.len);
  ba.len = er.encoded;

  ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_RANfunction_Description, pdu);

  return ba;
}
