// RRC stack related
#include "common/ran_context.h"
#include "RRC/LTE/rrc_defs.h"
#include "RRC/NR/nr_rrc_defs.h"
#include "RRC/LTE/rrc_eNB_UE_context.h"
#include "RRC/NR/rrc_gNB_UE_context.h"
// ric agent related
#include "ric_agent.h"
#include "e2sm_kpm.h"
#include "E2SM_KPM_MeasurementRecordItem-KPMv2.h"

// mobiflow related
#include "common/secsm.h"

extern RAN_CONTEXT_t RC;
// global variables for SECSM
extern int ue_rrc_counter;
extern struct rrcMsgTrace ue_rrc_msg[MAX_UE_NUM];
extern int ue_nas_counter;
extern struct nasMsgTrace ue_nas_msg[MAX_UE_NUM];
extern int nas_cipher_alg;
extern int nas_integrity_alg;
int prev_msg_counter[MAX_UE_NUM];
int prev_state[MAX_UE_NUM];


void init_kpm_as_mobiflow(kmp_meas_info_t *e2sm_kpm_meas_info) {
    int c = 0;
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.RNTI", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.IMSI1", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.IMSI2", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.RAT", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.M_TMSI", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.CIPHER_ALG", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.INTEGRITY_ALG", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.EMM_CAUSE", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.RELEASE_TIMER", 0, FALSE};
    e2sm_kpm_meas_info[c++] = (kmp_meas_info_t){c, "UE.ESTABLISH_CAUSE", 0, FALSE};
    int msgC = 0;
    while (c + msgC < MAX_KPM_MEAS) {
        // control message entries
        char* buf = malloc(6);
        sprintf(buf, "msg%d", msgC+1);
        e2sm_kpm_meas_info[c+msgC] = (kmp_meas_info_t){c+msgC+1, buf, 0, FALSE};
        ++msgC;
    }

    // msg tracking counter
    for (int i=0; i<MAX_UE_NUM; ++i) {
      prev_msg_counter[i] = 0;
      prev_state[i] = 0;
    }
}

int get_rat() {
    // init RAT
    if (is_lte())
        return 0; // LTE
    else if (is_nr())
        return 1; // NR
    else
        return -1; // unknown
}

uint64_t get_imsi(ric_agent_info_t *ric, rnti_t rnti) {
    uint64_t imsi = -1;
    if (is_nr()) {
        struct rrc_gNB_ue_context_s *ue_context_p = rrc_gNB_get_ue_context_by_rnti_any_du(RC.nrrrc[ric->ranid], rnti);
        if (ue_context_p) {
            imsi  = ue_context_p->ue_context.imsi.digit15;
            imsi += ue_context_p->ue_context.imsi.digit14 * 10;              // pow(10, 1)
            imsi += ue_context_p->ue_context.imsi.digit13 * 100;             // pow(10, 2)
            imsi += ue_context_p->ue_context.imsi.digit12 * 1000;            // pow(10, 3)
            imsi += ue_context_p->ue_context.imsi.digit11 * 10000;           // pow(10, 4)
            imsi += ue_context_p->ue_context.imsi.digit10 * 100000;          // pow(10, 5)
            imsi += ue_context_p->ue_context.imsi.digit9  * 1000000;         // pow(10, 6)
            imsi += ue_context_p->ue_context.imsi.digit8  * 10000000;        // pow(10, 7)
            imsi += ue_context_p->ue_context.imsi.digit7  * 100000000;       // pow(10, 8)
            imsi += ue_context_p->ue_context.imsi.digit6  * 1000000000;      // pow(10, 9)
            imsi += ue_context_p->ue_context.imsi.digit5  * 10000000000;     // pow(10, 10)
            imsi += ue_context_p->ue_context.imsi.digit4  * 100000000000;    // pow(10, 11)
            imsi += ue_context_p->ue_context.imsi.digit3  * 1000000000000;   // pow(10, 12)
            imsi += ue_context_p->ue_context.imsi.digit2  * 10000000000000;  // pow(10, 13)
            imsi += ue_context_p->ue_context.imsi.digit1  * 100000000000000; // pow(10, 14)
            return imsi;
        }
    }
    else if (is_lte()) {
        struct rrc_eNB_ue_context_s *ue_context_p = rrc_eNB_get_ue_context(RC.rrc[ric->ranid], rnti);
        if (ue_context_p) {
            imsi  = ue_context_p->ue_context.imsi.digit15;
            imsi += ue_context_p->ue_context.imsi.digit14 * 10;              // pow(10, 1)
            imsi += ue_context_p->ue_context.imsi.digit13 * 100;             // pow(10, 2)
            imsi += ue_context_p->ue_context.imsi.digit12 * 1000;            // pow(10, 3)
            imsi += ue_context_p->ue_context.imsi.digit11 * 10000;           // pow(10, 4)
            imsi += ue_context_p->ue_context.imsi.digit10 * 100000;          // pow(10, 5)
            imsi += ue_context_p->ue_context.imsi.digit9  * 1000000;         // pow(10, 6)
            imsi += ue_context_p->ue_context.imsi.digit8  * 10000000;        // pow(10, 7)
            imsi += ue_context_p->ue_context.imsi.digit7  * 100000000;       // pow(10, 8)
            imsi += ue_context_p->ue_context.imsi.digit6  * 1000000000;      // pow(10, 9)
            imsi += ue_context_p->ue_context.imsi.digit5  * 10000000000;     // pow(10, 10)
            imsi += ue_context_p->ue_context.imsi.digit4  * 100000000000;    // pow(10, 11)
            imsi += ue_context_p->ue_context.imsi.digit3  * 1000000000000;   // pow(10, 12)
            imsi += ue_context_p->ue_context.imsi.digit2  * 10000000000000;  // pow(10, 13)
            imsi += ue_context_p->ue_context.imsi.digit1  * 100000000000000; // pow(10, 14)
            return imsi;
        }
    }
    return imsi;
}

uint8_t get_status(ric_agent_info_t *ric, rnti_t rnti) {
    if (is_nr()) {
        struct rrc_gNB_ue_context_s *ue_context_p = rrc_gNB_get_ue_context_by_rnti_any_du(RC.nrrrc[ric->ranid], rnti);
        if (ue_context_p)
            return ue_context_p->ue_context.StatusRrc;
    }
    else if (is_lte()) {
        struct rrc_eNB_ue_context_s *ue_context_p = rrc_eNB_get_ue_context(RC.rrc[ric->ranid], rnti);
        if (ue_context_p)
            return ue_context_p->ue_context.StatusRrc;
    }
    return -1;
}

uint8_t get_cipher_alg(ric_agent_info_t *ric, rnti_t rnti) {
    if (is_nr()) {
        struct rrc_gNB_ue_context_s *ue_context_p = rrc_gNB_get_ue_context_by_rnti_any_du(RC.nrrrc[ric->ranid], rnti);
        if (ue_context_p)
            return ue_context_p->ue_context.ciphering_algorithm;
    }
    else if (is_lte()) {
        struct rrc_eNB_ue_context_s *ue_context_p = rrc_eNB_get_ue_context(RC.rrc[ric->ranid], rnti);
        if (ue_context_p)
            return ue_context_p->ue_context.ciphering_algorithm;
    }
    return -1;
}

uint8_t get_integrity_alg(ric_agent_info_t *ric, rnti_t rnti) {
    if (is_nr()) {
        struct rrc_gNB_ue_context_s *ue_context_p = rrc_gNB_get_ue_context_by_rnti_any_du(RC.nrrrc[ric->ranid], rnti);
        if (ue_context_p)
            return ue_context_p->ue_context.integrity_algorithm;
    }
    else if (is_lte()) {
        struct rrc_eNB_ue_context_s *ue_context_p = rrc_eNB_get_ue_context(RC.rrc[ric->ranid], rnti);
        if (ue_context_p)
            return ue_context_p->ue_context.integrity_algorithm;
    }
    return -1;
}

uint32_t get_m_tmsi(ric_agent_info_t *ric, rnti_t rnti) {
    if (is_nr()) {
        struct rrc_gNB_ue_context_s *ue_context_p = rrc_gNB_get_ue_context_by_rnti_any_du(RC.nrrrc[ric->ranid], rnti);
        if (ue_context_p)
            if (ue_context_p->ue_context.Initialue_identity_5g_s_TMSI.presence == TRUE)
                return ue_context_p->ue_context.Initialue_identity_5g_s_TMSI.fiveg_tmsi;    
    }
    else if (is_lte()) {
        struct rrc_eNB_ue_context_s *ue_context_p = rrc_eNB_get_ue_context(RC.rrc[ric->ranid], rnti);
        if (ue_context_p)
            if (ue_context_p->ue_context.Initialue_identity_s_TMSI.presence == TRUE)
                return ue_context_p->ue_context.Initialue_identity_s_TMSI.m_tmsi;
    }
    return -1;
}

uint64_t get_random_id(ric_agent_info_t *ric, rnti_t rnti) {
    if (is_nr()) {
        struct rrc_gNB_ue_context_s *ue_context_p = rrc_gNB_get_ue_context_by_rnti_any_du(RC.nrrrc[ric->ranid], rnti);
        if (ue_context_p)
            if (ue_context_p->ue_context.Initialue_identity_5g_s_TMSI.presence == TRUE)
                return ue_context_p->ue_context.random_ue_identity;    
    }
    else if (is_lte()) {
        struct rrc_eNB_ue_context_s *ue_context_p = rrc_eNB_get_ue_context(RC.rrc[ric->ranid], rnti);
        if (ue_context_p)
            if (ue_context_p->ue_context.Initialue_identity_s_TMSI.presence == TRUE)
                return ue_context_p->ue_context.random_ue_identity;
    }
    return -1;
}

uint8_t get_establish_cause(ric_agent_info_t *ric, rnti_t rnti) {
    if (is_nr()) {
        struct rrc_gNB_ue_context_s *ue_context_p = rrc_gNB_get_ue_context_by_rnti_any_du(RC.nrrrc[ric->ranid], rnti);
        if (ue_context_p)
            if (ue_context_p->ue_context.Initialue_identity_5g_s_TMSI.presence == TRUE)
                return ue_context_p->ue_context.establishment_cause;
    }
    else if (is_lte()) {
        struct rrc_eNB_ue_context_s *ue_context_p = rrc_eNB_get_ue_context(RC.rrc[ric->ranid], rnti);
        if (ue_context_p)
            if (ue_context_p->ue_context.Initialue_identity_s_TMSI.presence == TRUE)
                return ue_context_p->ue_context.establishment_cause;
    }
    return -1;
}


int encode_kpm_mobiflow(ric_agent_info_t *ric, kmp_meas_info_t *e2sm_kpm_meas_info, E2SM_KPM_MeasurementRecordItem_KPMv2_t *g_indMsgMeasRecItemArr[MAX_GRANULARITY_INDEX][MAX_KPM_MEAS], uint8_t g_granularityIndx) {
    
    /******************** Start encoding SECSM measurement data ********************/
    int i,j=0;
    int ue_count = 0;
    int meas_msg[MAX_RRC_MSG];
    int meas_msg_count = 0;
    rnti_t rnti = 0;
    uint8_t shouldUpdate = 0;
    uint8_t emm_cause = 0;

    for (int i=0; i<MAX_RRC_MSG; ++i)
      meas_msg[i] = 0; // init measurement items

    for (int i=0; i<MAX_UE_NUM; ++i) {
    	// iterate all ues
        if (ue_rrc_msg[i].rnti != 0) {
            ++ue_count;
            // get UE meta data
            rnti = ue_rrc_msg[i].rnti;
            
            // assemble ue information for SECSM
            // collect RRC msg trace
            int msgCount = ue_rrc_msg[i].msgCount;
            int totalNasMsg = 0;
            int nasIndex = -1;
            // locate nas msg buffer index
            for (int k=0; k<ue_nas_counter; ++k) {
                if (ue_nas_msg[k].id == rnti) {
                    nasIndex = k;
                    totalNasMsg = ue_nas_msg[k].msgCount;
                    break;
                }
            }
            if (prev_msg_counter[i] == msgCount + totalNasMsg)
                continue; // message count has not been changed, don't update this UE

            RIC_AGENT_INFO("[SECSM] Report UE RNTI: %x, RAT:%x, IMSI:%ld, random_id: %ld\n", rnti, get_rat(ric, rnti), get_imsi(ric, rnti), get_random_id(ric, rnti));

            prev_state[i] = get_status(ric, rnti);
            shouldUpdate = 1;

            RIC_AGENT_INFO("[SECSM] RRC msg trace (count %d) for UE %x:\n", msgCount, rnti);
            RIC_AGENT_INFO("[SECSM] NAS msg trace (count %d) for UE %x:\n", totalNasMsg, rnti);
            int nIndex = 0, rIndex = 0;
            while (nIndex < totalNasMsg || rIndex < msgCount) {
                if (meas_msg_count < MAX_RRC_MSG) {
                    int32_t rTs = 0, nTs = 0;
                    if (nIndex < totalNasMsg)
                        nTs = ue_nas_msg[nasIndex].msg[nIndex].timestamp;
                    if (rIndex < msgCount)
                        rTs = ue_rrc_msg[i].msg[rIndex].timestamp;
                    
                    if ((nTs < rTs && nTs != 0) || (rTs == 0)) {
                        // encode nas msg
                        struct nasMsg n = ue_nas_msg[nasIndex].msg[nIndex++];
                        if (prev_msg_counter[i] != 0 && nIndex + rIndex <= prev_msg_counter[i]) {
                            // don't report message that has already been reported
                            continue;
                        }
                        int encode_nas = 0 | ((n.discriminator & 1) << 1) | (n.msgId << 2);
                        RIC_AGENT_INFO("[SECSM] {NAS: dis: %d, msgId: %d, ts: %ld} -> %d\n", n.discriminator, n.msgId, n.timestamp, encode_nas);
                        meas_msg[meas_msg_count++] = encode_nas;
                        if (n.emm_cause != 0)
                            emm_cause = n.emm_cause;
                    }
                    else {
                        // encode rrc msg
                        struct rrcMsg m = ue_rrc_msg[i].msg[rIndex++];
                        if (prev_msg_counter[i] != 0 && nIndex + rIndex <= prev_msg_counter[i]) {
                            // don't report message that has already been reported
                            continue;
                        }
                        if (is_lte() && ((m.msgId==2 && m.dcch==1 && m.downlink==1) || (m.msgId==10 && m.dcch==1 && m.downlink==0))) {
                            continue; // don't encode ul/dl information transfer msg in LTE
                        }
                        else if (is_nr() && ((m.msgId==6 && m.dcch==1 && m.downlink==1) || (m.msgId==8 && m.dcch==1 && m.downlink==0))) {
                            continue; // don't encode ul/dl information transfer msg in NR
                        }
                        int encode_rrc = 1 | (m.dcch << 1) | (m.downlink << 2) | (m.msgId << 3);
                        RIC_AGENT_INFO("[SECSM] {RRC: msgId: %d, ts: %ld, dcch: %d, downlink: %d} -> %d\n", m.msgId, m.timestamp, m.dcch, m.downlink, encode_rrc);
                        meas_msg[meas_msg_count++] = encode_rrc;
                    }
                }
            }

            prev_msg_counter[i] = msgCount + totalNasMsg;
            break;
        }
    }

    RIC_AGENT_INFO("[SECSM] Total UE: %d\n", ue_count);

    /******************** End encoding SECSM measurement data ********************/

    for (i = 0; i < MAX_KPM_MEAS; i++)
    {
        if (e2sm_kpm_meas_info[i].subscription_status == TRUE)
        {
            g_indMsgMeasRecItemArr[g_granularityIndx][j] = 
                            (E2SM_KPM_MeasurementRecordItem_KPMv2_t *)calloc(1,sizeof(E2SM_KPM_MeasurementRecordItem_KPMv2_t));
            g_indMsgMeasRecItemArr[g_granularityIndx][j]->present = E2SM_KPM_MeasurementRecordItem_KPMv2_PR_integer;

            if (shouldUpdate == 0) {
                g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = 0;
                j++;
                continue; // assign dummy value
            }

            switch(e2sm_kpm_meas_info[i].meas_type_id)
            {
                case 1: // RNTI
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = rnti;
                    break;
                case 2: // IMSI1
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = (uint32_t) get_imsi(ric, rnti) & 0xFFFFFFFF;
                    break;
                case 3: // IMSI2
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = (uint32_t) (get_imsi(ric, rnti) >> 32);
                    break;
                case 4: // RAT
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = get_rat();
                    break;
                case 5: // TMSI
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = get_m_tmsi(ric, rnti);
                    break;
                case 6: // CIPHER_ALG
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = get_cipher_alg(ric, rnti);
                    break;
                case 7: // INTEGRITY_ALG
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = get_integrity_alg(ric, rnti);
                    break;
                case 8: // EMM_CAUSE
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = emm_cause;
                    break;
                case 9: // RELEASE_TIMER
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = 0; // TODO
                    break;
                case 10: // ESTABLISH_CAUSE
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = get_establish_cause(ric, rnti);
                    break; 
                default:
                    g_indMsgMeasRecItemArr[g_granularityIndx][j]->choice.integer = meas_msg[e2sm_kpm_meas_info[i].meas_type_id-11];
                    break;
            }
            j++;
        }
    }

    return 0;   
}

