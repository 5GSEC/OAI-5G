#include "ric_agent.h"
#include "E2AP_RICcontrolRequest.h"
#include "E2AP_ProtocolIE-Field.h"
#include "E2SM-RC-ControlHeader.h"
#include "E2SM-RC-ControlMessage.h"
#include "E2SM-RC-ControlHeader-Format1.h"
#include "E2SM-RC-ControlMessage-Format1.h"
#include "E2SM-RC-ControlMessage-Format1-Item.h"
#include "RANParameter-ValueType.h"
#include "RANParameter-ValueType-Choice-ElementFalse.h"
#include "RANParameter-ValueType-Choice-ElementTrue.h"
#include "RANParameter-Value.h"
#include "UEID.h"
#include "UEID-GNB.h"
#include "conversions.h"

void e2sm_rc_dispatch_control_action(long control_action_id, long ran_param_id, long ran_param_value) {
    // determine ran parameter value type based on control action ID and parameter ID, from O-RAN.WG3.E2SM-RC-v01.03 Section 8.4
    // TODO: fill other cases
    switch(control_action_id) {
        case 1:
            // DRB QoS Configuration
            switch(ran_param_id) {
                case 1: 
                    // DRB ID
                    RIC_AGENT_INFO("[e2sm_rc_dispatch_control_action] Handle DRB ID control\n");
                    return;
                case 2:
                    // 5QI 
                    RIC_AGENT_INFO("[e2sm_rc_dispatch_control_action] Handle 5QI control\n");
                    return;
                default:
                    // default RAN param ID
                    RIC_AGENT_ERROR("[e2sm_rc_dispatch_control_action] Unimplemented control_action_id: %ld\n", control_action_id);
                    return;
            }
        default:
            // default action ID
            RIC_AGENT_ERROR("[e2sm_rc_dispatch_control_action] Unimplemented RAN param ID: %ld\n", ran_param_id);
            return;
    }
}

int e2sm_rc_decode_control_request(uint8_t *control_header_buf, size_t control_header_size, uint8_t *control_message_buf, size_t control_message_size) 
{
    /********************* Parse Control Header *********************/
    asn_dec_rval_t decode_result;
    E2SM_RC_ControlHeader_t *controlHeader = calloc(1, sizeof(E2SM_RC_ControlHeader_t));
    long amf_ue_ngap_id, ric_style_type, control_action_id;
    uint8_t *plmn_id, *amf_region_id, *amf_set_id, *amf_pointer;
    uint16_t mcc, mnc, mnc_digit_len;

    decode_result = asn_decode(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_RC_ControlHeader, (void **)&controlHeader, control_header_buf, control_header_size);

    if (decode_result.code != RC_OK) {
        RIC_AGENT_ERROR("Failed to decode structure: E2SM_RC_ControlHeader_t, error code: %d\n", decode_result.code);
        return -1;
    }

    xer_fprint(stdout, &asn_DEF_E2SM_RC_ControlHeader, controlHeader);

    if (controlHeader->ric_controlHeader_formats.present == E2SM_RC_ControlHeader__ric_controlHeader_formats_PR_controlHeader_Format1) {
        E2SM_RC_ControlHeader_Format1_t *header_f1 = controlHeader->ric_controlHeader_formats.choice.controlHeader_Format1;
        switch (header_f1->ueID.present) {
            case UEID_PR_gNB_UEID:
                // parse amf UE NGAP ID
                if (asn_INTEGER2long(&header_f1->ueID.choice.gNB_UEID->amf_UE_NGAP_ID, &amf_ue_ngap_id) != 0) {
                    RIC_AGENT_ERROR("Fail to parse amf_UE_NGAP_ID\n");
                }

                // parse PLMN
                PLMNID_TO_MCC_MNC(&header_f1->ueID.choice.gNB_UEID->guami.pLMNIdentity, mcc, mnc, mnc_digit_len);

                // parse amf region ID

                // parse amf set ID

                // parse amf pointer

                RIC_AGENT_INFO("amf_UE_NGAP_ID: %ld\n", amf_ue_ngap_id);
                RIC_AGENT_INFO("mcc: %u, mnc: %d, mnc_len: %u\n", mcc, mnc, mnc_digit_len);
                break;
            // TODO: handle the following UE_ID structs
            case UEID_PR_NOTHING:
            case UEID_PR_gNB_DU_UEID:
            case UEID_PR_gNB_CU_UP_UEID:
            case UEID_PR_ng_eNB_UEID:
            case UEID_PR_ng_eNB_DU_UEID:
            case UEID_PR_en_gNB_UEID:
            case UEID_PR_eNB_UEID:
                break;
        }

        // parse ric style type
        ric_style_type = header_f1->ric_Style_Type;

        // parse control action ID
        control_action_id = header_f1->ric_ControlAction_ID;

        RIC_AGENT_INFO("ric_style_type: %ld, control_action_id: %ld\n", ric_style_type, control_action_id);
    }
    else if (controlHeader->ric_controlHeader_formats.present == E2SM_RC_ControlHeader__ric_controlHeader_formats_PR_controlHeader_Format2) {

    }

    ASN_STRUCT_FREE(asn_DEF_E2SM_RC_ControlHeader, controlHeader);


    /********************* Parse Control Message *********************/
    E2SM_RC_ControlMessage_t *controlMessage = calloc(1, sizeof(E2SM_RC_ControlMessage_t));

    decode_result = asn_decode(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_RC_ControlMessage, (void **)&controlMessage, control_message_buf, control_message_size);

    if (decode_result.code != RC_OK) {
        RIC_AGENT_ERROR("Failed to decode structure: E2SM_RC_ControlMessage_t, error code: %d\n", decode_result.code);
        return -1;
    }

    xer_fprint(stdout, &asn_DEF_E2SM_RC_ControlMessage, controlMessage);

    if (controlMessage->ric_controlMessage_formats.present == E2SM_RC_ControlMessage__ric_controlMessage_formats_PR_controlMessage_Format1) {
        E2SM_RC_ControlMessage_Format1_t *message_f1 = controlMessage->ric_controlMessage_formats.choice.controlMessage_Format1;
        long ran_param_id, ran_param_value_long;
        double ran_param_value_double;
        RANParameter_Value_t *rp;
        RANParameter_ValueType_t *ran_param_value_type;

        // parse RAN parameter values
        for (int i=0; i<message_f1->ranP_List.list.count; ++i) {
            ran_param_id = message_f1->ranP_List.list.array[i]->ranParameter_ID;
            ran_param_value_type = &message_f1->ranP_List.list.array[i]->ranParameter_valueType;
            switch (ran_param_value_type->present) {
                case RANParameter_ValueType_PR_ranP_Choice_ElementFalse:
                    rp = ran_param_value_type->choice.ranP_Choice_ElementFalse->ranParameter_value;
                    break;
                case RANParameter_ValueType_PR_ranP_Choice_ElementTrue:
                    rp = &ran_param_value_type->choice.ranP_Choice_ElementTrue->ranParameter_value;
                    break;
                // TODO: handle the following
                case RANParameter_ValueType_PR_ranP_Choice_Structure:
                    // ran_param_value_type->choice.ranP_Choice_Structure:
                case RANParameter_ValueType_PR_ranP_Choice_List:
                    // ran_param_value_type->choice.ranP_Choice_List
                    RIC_AGENT_ERROR("Unimplemented RAN param type at index %d: %d\n", i, ran_param_value_type->present);
                case RANParameter_ValueType_PR_NOTHING:
                    break;
            }
            
            // parse parameter value based on type
            if (rp != NULL) {
                switch (rp->present) {
                    case RANParameter_Value_PR_valueInt:
                        ran_param_value_long = rp->choice.valueInt;
                        RIC_AGENT_INFO("[%d] Found RAN parameter ID: %ld, value (long): %ld\n", i, ran_param_id, ran_param_value_long);
                        break;
                    case RANParameter_Value_PR_valueReal:
                        ran_param_value_double = rp->choice.valueReal;
                        RIC_AGENT_INFO("[%d] Found RAN parameter ID: %ld, value (double): %f\n", i, ran_param_id, ran_param_value_double);
                        break;
                    case RANParameter_Value_PR_valueBoolean:
                        // rp->valueBoolean;
                    case RANParameter_Value_PR_valueBitS:
                        // rp->valueBitS;
                    case RANParameter_Value_PR_valueOctS:
                        // rp->valueOctS;
                    case RANParameter_Value_PR_valuePrintableString:
                        // rp->valuePrintableString;
                        RIC_AGENT_ERROR("Unimplemented RAN param value type: %d\n", rp->present);
                    case RANParameter_Value_PR_NOTHING:
                        break;
                }
            }
            
            // Dispatch concrete control actions
            e2sm_rc_dispatch_control_action(control_action_id, ran_param_id, ran_param_value_long);
        }
    }
    else if (controlMessage->ric_controlMessage_formats.present == E2SM_RC_ControlMessage__ric_controlMessage_formats_PR_controlMessage_Format2) {

    }

    ASN_STRUCT_FREE(asn_DEF_E2SM_RC_ControlMessage, controlMessage);

    return 0;

}


int e2sm_rc_handle_control_request(E2AP_RICcontrolRequest_t *ricControlRequest)
{
    int i;
    long ricRequestorID = -1, ricInstanceID = -1, ranFunctionID = -1;
    E2AP_RICcontrolRequest_IEs_t *ricControlRequestIe;
    uint8_t *control_header_buf, *control_message_buf;
    size_t control_header_size, control_message_size;

    for (i=0; i < ricControlRequest->protocolIEs.list.count; i++) 
    {
        ricControlRequestIe = (E2AP_RICcontrolRequest_IEs_t *)ricControlRequest->protocolIEs.list.array[i];

        if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RICrequestID) {
            ricRequestorID = ricControlRequestIe->value.choice.RICrequestID.ricRequestorID;
            ricInstanceID = ricControlRequestIe->value.choice.RICrequestID.ricInstanceID;
        }
        else if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RANfunctionID) {
            ranFunctionID = ricControlRequestIe->value.choice.RANfunctionID;
        }
        else if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RICcallProcessID) {

        }
        else if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RICcontrolHeader) {
            control_header_size = ricControlRequestIe->value.choice.RICcontrolHeader.size;
            control_header_buf = (uint8_t *)malloc(control_header_size);
            memcpy(control_header_buf, ricControlRequestIe->value.choice.RICcontrolHeader.buf, control_header_size);
        }
        else if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RICcontrolMessage) {
            control_message_size = ricControlRequestIe->value.choice.RICcontrolMessage.size;
            control_message_buf = (uint8_t *)malloc(control_message_size);
            memcpy(control_message_buf, ricControlRequestIe->value.choice.RICcontrolMessage.buf, control_message_size);
        }
        else if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RICcontrolAckRequest) {

        }

    }

    if (control_header_size == 0 || control_header_buf == NULL) {
        RIC_AGENT_ERROR("RIC Control header not found\n");
        return -1;
    }

    if (control_message_size == 0 || control_message_buf == NULL) {
        RIC_AGENT_ERROR("RIC Control Message not found\n");
        return -1;
    }

    // RIC_AGENT_INFO("%ld %ld %ld\n", ricRequestorID, ricInstanceID, ranFunctionID);

    return e2sm_rc_decode_control_request(control_header_buf, control_header_size, control_message_buf, control_message_size);
}
