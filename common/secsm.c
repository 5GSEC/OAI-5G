#include <stdlib.h>
#include <sys/time.h>
#include "secsm.h"
#include "openair3/NAS/COMMON/UTIL/TLVDecoder.h"
#include "common/utils/ocp_itti/intertask_interface.h"
#include "common/utils/LOG/log.h"
#include "openair3/NAS/COMMON/NR_NAS_defs.h"
#include "common/ran_context.h"

#ifdef ENABLE_RIC_AGENT

extern RAN_CONTEXT_t RC;

// new variables for SECSM
int ue_rrc_counter=0;
struct rrcMsgTrace ue_rrc_msg[MAX_UE_NUM];
int ue_nas_counter = 0;
struct nasMsgTrace ue_nas_msg[MAX_UE_NUM];
int nas_cipher_alg = 0;
int nas_integrity_alg = 0;
#endif

// ue index lookup based on rnti
int getRrcMsgIndex(rnti_t rnti) {
  for (int i=0; i<ue_rrc_counter; ++i) {
    if (ue_rrc_msg[i].rnti == rnti)
      return i;
  }
  return -1;
}

int64_t getTimestamp() {
  struct timeval ts;
  gettimeofday(&ts, NULL);
  int64_t ts_ms = ts.tv_sec * 1000 + ts.tv_usec / 1000;
  return ts_ms;
}

void addRrcMsg(rnti_t rnti, uint8_t msgId, uint8_t dcch, uint8_t downlink) {
  int ue_index = getRrcMsgIndex(rnti);
  int64_t ts_ms = getTimestamp();
  struct rrcMsg rm = {msgId, dcch, downlink, ts_ms};

  LOG_I(RRC, "[SECSM] logging RRC msg, msg_id: %d, dcch: %d, downlink: %d, timestamp: %ld, rnti: %x\n", 
        rm.msgId, rm.dcch, rm.downlink, rm.timestamp, rnti);
  
  if (ue_index == -1) {
    // create new rrc msg trace
    ue_rrc_msg[ue_rrc_counter].rnti = rnti;
    ue_rrc_msg[ue_rrc_counter].active = 1;
    ue_rrc_msg[ue_rrc_counter].msgCount = 1;
    ue_rrc_msg[ue_rrc_counter].msg[0] = rm;
    ++ue_rrc_counter;
  }
  else {
    // append to existing rrc msg trace
    int msgCount = ue_rrc_msg[ue_index].msgCount;
    if (msgCount < MAX_RRC_MSG) {
      ue_rrc_msg[ue_index].msg[msgCount] = rm;
      ue_rrc_msg[ue_index].msgCount = msgCount + 1;
    }
    else {
      // TODO handle message buffer overflow
    }
  }
}

int getNasMsgIndex(int id) {
  for (int i=0; i<ue_nas_counter; ++i) {
    if (ue_nas_msg[i].id == id)
      return i;
  }
  return -1;
}

void addNasMsg(int id, uint8_t* buffer, uint32_t length) {
  struct nasMsg nm = decodeNasMsg(buffer, length);
  if (nm.msgId == 0)
      return; // don't record empty message
  int ue_index = getNasMsgIndex(id);
  if (ue_index == -1) {
      // create new nas msg trace
      ue_nas_msg[ue_nas_counter].id = id;
      ue_nas_msg[ue_nas_counter].active = 1;
      ue_nas_msg[ue_nas_counter].msgCount = 1;
      ue_nas_msg[ue_nas_counter].msg[0] = nm;
      ++ue_nas_counter;
    }
    else {
      // append to existing nas msg trace
      int msgCount = ue_nas_msg[ue_index].msgCount;
      if (msgCount < MAX_NAS_MSG) {
        ue_nas_msg[ue_index].msg[msgCount] = nm;
        ue_nas_msg[ue_index].msgCount = msgCount + 1;
      }
      else {
        // TODO handle message buffer overflow
      }
    }
}

struct nasMsg decodeNasMsg(uint8_t* buffer, uint32_t length) {
  if (RC.rrc != NULL)
    return decodeNasMsgLTE(buffer, length);
  else if (RC.nrrrc != NULL)
    return decodeNasMsgNR(buffer, length);
  else {
    struct nasMsg nm;
    return nm;
  }
}

struct nasMsg decodeNasMsgLTE(uint8_t* buffer, uint32_t length) {
  // Decode
  nas_message_t nas_msg;
  memset(&nas_msg, 0, sizeof(nas_message_t));
  int size = 0;
  // decode discriminator
  nas_message_security_header_t *header = &nas_msg.header;
  DECODE_U8((char *) buffer, *(uint8_t *) (header), size);
  int discriminator = header->protocol_discriminator;
  int msgId = 0;
  // decode message type
  int offset = 0;
  uint8_t emm_cause = 0;
  if (discriminator == EPS_MOBILITY_MANAGEMENT_MESSAGE) {
    if (header->security_header_type == SECURITY_HEADER_TYPE_NOT_PROTECTED) {
      offset = 1;
      nas_cipher_alg = 0;
      nas_integrity_alg = 0;
    }
    else if (header->security_header_type == SECURITY_HEADER_TYPE_SERVICE_REQUEST) {
      // handle and decode service request
      nas_cipher_alg = 0;
      nas_integrity_alg = 0;
      msgId = 77;
    }
    else {
      offset = 7; // size of nas_message_security_header_t+1
      if (header->security_header_type < SECURITY_HEADER_TYPE_INTEGRITY_PROTECTED)
        nas_integrity_alg = 0;
      else
	      nas_integrity_alg = 1; // we can't determine the exact algorithm?
	
      if (header->security_header_type < SECURITY_HEADER_TYPE_INTEGRITY_PROTECTED_CYPHERED)
	      nas_cipher_alg = 0;
      else
	      nas_cipher_alg = 1;
    }

    if (nas_cipher_alg != 0) {
      // don't decode ciphered message
      struct nasMsg nm;
      return nm;
    }

    EMM_msg *e_msg = &nas_msg.plain.emm;
    emm_msg_header_t *emm_header = &e_msg->header;
    DECODE_U8(buffer + offset, emm_header->message_type, size);
    if (msgId == 0)
      msgId = emm_header->message_type;
    // decode EMM error code for reject message
    if (msgId == 68 || msgId == 78 || msgId == 92 || msgId == 95) { // Attach/Service Reject, Auth failure, security mode reject
      DECODE_U8(buffer+size, emm_cause, size);
      LOG_I(RRC, "[SECSM] Decoding EMM cause: %d\n", emm_cause);
    }
  }
  else if (discriminator == EPS_SESSION_MANAGEMENT_MESSAGE) {
    // TODO decode
  }

  struct timeval ts;
  gettimeofday(&ts, NULL);
  int64_t ts_ms = ts.tv_sec * 1000 + ts.tv_usec / 1000;
  LOG_I(RRC, "[SECSM] NAS LTE decoded: distriminator: %d, msgid: %d, timestamp: %ld\n", discriminator, msgId, ts_ms);
  struct nasMsg nm = {discriminator, msgId, buffer, length, ts_ms, emm_cause};
  return nm;
}

struct nasMsg decodeNasMsgNR(uint8_t* buffer, uint32_t length) {
  SGScommonHeader_t *header = (SGScommonHeader_t *) buffer;
  int8_t discriminator = header->epd;
  int8_t sec_header = header->sh;
  int8_t msgId = header->mt;
  uint8_t emm_cause = 0;

  if (sec_header == notsecurityprotected) {
    nas_cipher_alg = 0;
    nas_integrity_alg = 0;
  }
  else if (sec_header == Integrityprotected || sec_header == Integrityprotectedwithnew5GNASsecuritycontext) {
    nas_integrity_alg = 1;
    nas_cipher_alg = 0;
  }
  else if (sec_header == Integrityprotectedandciphered || sec_header == Integrityprotectedandcipheredwithnew5GNASsecuritycontext) {
    nas_integrity_alg = 1;
    nas_cipher_alg = 1;
  }

  struct timeval ts;
  gettimeofday(&ts, NULL);
  int64_t ts_ms = ts.tv_sec * 1000 + ts.tv_usec / 1000;
  uint8_t offset = 0;
  
  if (discriminator == SGSmobilitymanagementmessages) {
    if (sec_header == notsecurityprotected) {
      // only decode non protected NAS 5G message
      LOG_I(RRC, "[SECSM] NAS NR decoded: distriminator: %d, msgid: %d, timestamp: %ld\n", discriminator, msgId, ts_ms);
      struct nasMsg nm = {discriminator, msgId, buffer, length, ts_ms, emm_cause};
      return nm;
    }
    else {
      // further decode the next NAS header until find a plain NAS header
      uint8_t SECURITY_PROTECTED_5GS_NAS_MESSAGE_HEADER_LENGTH = 7;
      offset += SECURITY_PROTECTED_5GS_NAS_MESSAGE_HEADER_LENGTH;
      if (offset < length) {
        SGScommonHeader_t *next_header = (SGScommonHeader_t *) (buffer + offset);
        discriminator = next_header->epd;
        sec_header = next_header->sh;
        msgId = next_header->mt;
        if (sec_header == notsecurityprotected) {
          // only decode non protected NAS 5G message
          LOG_I(RRC, "[SECSM] NAS NR decoded: distriminator: %d, msgid: %d, timestamp: %ld\n", discriminator, msgId, ts_ms);
          struct nasMsg nm = {discriminator, msgId, buffer, length, ts_ms, emm_cause};
          return nm;
        }
      }

      // TODO decode 5G service request
      // TODO decode EMM cause
      // TODO handle ciphered NAS message in xApp
    }

  }
  else if (discriminator == SGSsessionmanagementmessages) {
    // handle ESM NR
  }

  struct nasMsg nm;
  LOG_I(RRC, "[SECSM] NAS NR NOT decoded: distriminator: %d, msgid: %d, timestamp: %ld\n", discriminator, msgId, ts_ms);
  return nm;
}