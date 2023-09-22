#include <assert.h>

#include "enc_ric_event_trigger_frm_1.h"

E2SM_KPM_EventTriggerDefinition_Format1_t * kpm_enc_event_trigger_def_frm_1_asn(const kpm_ric_event_trigger_format_1_t * event_trigger)
{
    E2SM_KPM_EventTriggerDefinition_Format1_t * event_trigger_asn = calloc(1, sizeof(E2SM_KPM_EventTriggerDefinition_Format1_t));
    assert(event_trigger_asn != NULL && "Memory exhausted");

    event_trigger_asn->reportingPeriod = event_trigger->report_period_ms;


    return event_trigger_asn;

}