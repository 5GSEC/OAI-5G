#include "asn_application.h"
#include "ric_agent.h"
#include "E2AP_RICcontrolRequest.h"
#include "E2AP_ProtocolIE-Field.h"
#include "E2SM-RC-ControlHeader.h"
#include "E2SM-RC-ControlMessage.h"

int e2sm_rc_handle_control_header(uint8_t *buf, size_t size) 
{
    asn_dec_rval_t decode_result;
    E2SM_RC_ControlHeader_t *controlHeader = calloc(1, sizeof(E2SM_RC_ControlHeader_t));

    decode_result = asn_decode(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_RC_ControlHeader, (void **)&controlHeader, buf, size);

    if (decode_result.code != RC_OK) {
        RIC_AGENT_ERROR("Failed to decode structure: E2SM_RC_ControlHeader_t, error code: %d\n", decode_result.code);
        return -1;
    }

    xer_fprint(stdout, &asn_DEF_E2SM_RC_ControlHeader, controlHeader);

    return 0;
}

int e2sm_rc_handle_control_message(uint8_t *buf, size_t size) 
{
    asn_dec_rval_t decode_result;
    E2SM_RC_ControlMessage_t *controlMessage = calloc(1, sizeof(E2SM_RC_ControlMessage_t));

    decode_result = asn_decode(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_RC_ControlMessage, (void **)&controlMessage, buf, size);

    if (decode_result.code != RC_OK) {
        RIC_AGENT_ERROR("Failed to decode structure: E2SM_RC_ControlMessage_t, error code: %d\n", decode_result.code);
        return -1;
    }

    xer_fprint(stdout, &asn_DEF_E2SM_RC_ControlMessage, controlMessage);

    return 0;
}

int e2sm_rc_handle_control_request(E2AP_RICcontrolRequest_t *ricControlRequest)
{
    int i;
    long ricRequestorID = -1, ricInstanceID = -1, ranFunctionID = -1;
    E2AP_RICcontrolRequest_IEs_t *ricControlRequestIe;

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
            size_t size = ricControlRequestIe->value.choice.RICcontrolHeader.size;
            uint8_t *buf = (uint8_t *)malloc(size);
            memcpy(buf, ricControlRequestIe->value.choice.RICcontrolHeader.buf, size);
            e2sm_rc_handle_control_header(buf, size);
        }
        else if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RICcontrolMessage) {
            size_t size = ricControlRequestIe->value.choice.RICcontrolMessage.size;
            uint8_t *buf = (uint8_t *)malloc(size);
            memcpy(buf, ricControlRequestIe->value.choice.RICcontrolMessage.buf, size);
            e2sm_rc_handle_control_message(buf, size);
        }
        else if (ricControlRequestIe->value.present == E2AP_RICcontrolRequest_IEs__value_PR_RICcontrolAckRequest) {

        }

    }

    RIC_AGENT_INFO("%ld %ld %ld\n", ricRequestorID, ricInstanceID, ranFunctionID);

    return 0;
}
