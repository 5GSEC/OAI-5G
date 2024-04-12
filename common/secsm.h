#include "platform_types.h"

#ifndef _SECSM_H
#define _SECSM_H

#ifdef ENABLE_RIC_AGENT

#define MAX_UE_NUM 256
#define MAX_RRC_MSG 50
#define MAX_NAS_MSG 50

/********** Begin new structures for SECSM **********/
struct rrcMsg {
  uint8_t msgId;
  uint8_t dcch;
  uint8_t downlink;
  uint64_t timestamp;
};

struct rrcMsgTrace {
  uint16_t rnti;
  uint8_t active;
  uint8_t msgCount;
  struct rrcMsg msg[MAX_RRC_MSG];
};

struct nasMsg {
  uint8_t discriminator;
  uint8_t msgId;
  uint8_t *buffer;
  uint16_t length;
  uint64_t timestamp;
  uint8_t emm_cause;
};

struct nasMsgTrace {
  uint16_t id;
  uint8_t active;
  uint8_t msgCount;
  struct nasMsg msg[MAX_NAS_MSG];
};

struct metaInfoMsg {
  uint16_t rnti;
  uint8_t rrcStatus;
};

struct secSmMsg {
 int16_t id;
 uint8_t active;
 struct metaInfoMsg meta;
 struct rrcMsgTrace rrcMsgTrace;
 struct nasMsgTrace nasMsgTrace;
};
/********** End new structures for SECSM **********/


/********** Begin new variables for SECSM **********/
extern int ue_rrc_counter;
extern struct rrcMsgTrace ue_rrc_msg[MAX_UE_NUM];
extern int ue_nas_counter;
extern struct nasMsgTrace ue_nas_msg[MAX_UE_NUM];
extern int nas_cipher_alg;
extern int nas_integrity_alg;
/********** End new variables for SECSM **********/


/********** Begin new function def for SECSM **********/
int getNasMsgIndex(int id);
void addNasMsg(int id, uint8_t* buffer, uint32_t length);
int getRrcMsgIndex(rnti_t rnti);
void addRrcMsg(rnti_t rnti, uint8_t msgId, uint8_t dcch, uint8_t downlink);
int64_t getTimestamp(void);
struct nasMsg decodeNasMsg(uint8_t* buffer, uint32_t length);
struct nasMsg decodeNasMsgLTE(uint8_t* buffer, uint32_t length);
struct nasMsg decodeNasMsgNR(uint8_t* buffer, uint32_t length);
/********** End new function def for SECSM **********/

#endif
#endif
