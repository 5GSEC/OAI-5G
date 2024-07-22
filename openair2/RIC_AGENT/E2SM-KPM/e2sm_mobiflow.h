
#ifndef _MOBIFLOW_h
#define _MOBIFLOW_h

void init_kpm_as_mobiflow(kmp_meas_info_t *e2sm_kpm_meas_info);

int encode_kpm_mobiflow(ric_agent_info_t *ric, 
                         kmp_meas_info_t *e2sm_kpm_meas_info, 
                         E2SM_KPM_MeasurementRecordItem_KPMv2_t *g_indMsgMeasRecItemArr[MAX_GRANULARITY_INDEX][MAX_KPM_MEAS], 
                         uint8_t g_granularityIndx);


#endif