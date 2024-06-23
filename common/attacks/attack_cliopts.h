#ifndef __ATTACK_CLIOPTS_H__
#define __ATTACK_CLIOPTS_H__

#include "attack_vars.h"


/* Help descriptions */

#define CONFIG_HLP_ATTACK_BTS    "BTS resource depletion attack level: no attack, 1+ sets attack level\n"
#define CONFIG_HLP_BTS_DELAY     "BTS resource depletion attack delay (default: 188 ms)\n"
#define CONFIG_HLP_ATTACK_DOS    "Blind DoS attack level: no attack, 1+ sets attack level\n"
#define CONFIG_HLP_RRC_TMSI      "RRC TMSI for blind DoS attack\n"
#define CONFIG_HLP_DN_DOS_ATT    "Downlink DoS attack mode: no attack, 1 Authentication Request -> Attach Reject, 2 Security Mode Command -> Attach Reject, 3 Attach Accept -> Attach Reject, 4 EMM INFORMATION -> Attach Reject, 5 Downlink NAS Transport -> Attach Reject, 6 Authentication Request -> Service Reject, 7 Security Mode Command -> Service Reject, 8 Attach Accept -> Service Reject\n"
#define CONFIG_HLP_UP_DOS_ATT    "Uplink DoS attack level: no attack, 1+ sets attack level\n"
#define CONFIG_HLP_REP_CONN_ATT  "Repeated connection setup complete attack count: 1+ sets attack level\n"
#define CONFIG_HLP_AUTH_SYNC_ATT "Auth sync failure attack: no attack, 1+ sets attack level\n"
#define CONFIG_HLP_NULL_CIPH_INT "Null cipher integrity attack mode: no attack, 1 RRC SECURITY_MODE_COMPLETE -> SECURITY_MODE_FAILURE, 2 NAS SECURITY_MODE_COMPLETE -> SECURITY_MODE_REJECT\n"
#define CONFIG_HLP_DNLNK_IMSI_EX "Downlink IMSI extractor attack mode: no attack, 1 IMSI AUTH_REQUEST -> IDENTITY_REQUEST, 2 IMEI AUTH_REQUEST -> IDENTITY_REQUEST, 3 TMSI AUTH_REQUEST -> IDENTITY_REQUEST, 4 IMSI SEC_MODE_CMD -> IDENTITY_REQUEST\n"
#define CONFIG_HLP_UPLNK_IMSI_EX "Uplink IMSI extractor attack: no attack, 1+ encode invalid TMSI into attach message (WARNING: hardwired PLMN)\n"
#define CONFIG_EMERGENCY_UE      "Make an emergency connection request, allowing UE to enter network that is at maximum capacity.\n"


/* Attack command line parameters */

#define ATTACK_CLI_OPTS         \
    {"bts-attack",        CONFIG_HLP_ATTACK_BTS,    0,              iptr:&(bts_attack),         defintval:0,   TYPE_INT, 0},   \
    {"bts-delay",         CONFIG_HLP_BTS_DELAY,     0,              iptr:&(bts_delay) ,         defintval:188, TYPE_INT, 0},   \
    {"blind-dos-attack",  CONFIG_HLP_ATTACK_DOS,    0,              iptr:&(blind_dos_attack),   defintval:0,   TYPE_INT, 0},   \
    {"RRC-TMSI",          CONFIG_HLP_RRC_TMSI,      0,              iptr:&tmsi_blind_dos_rrc,   defintval:0,   TYPE_UINT64, 0},   \
    {"dnlink-dos-attack", CONFIG_HLP_DN_DOS_ATT,    0,              iptr:&(dnlink_dos_attack),  defintval:0,   TYPE_INT, 0},   \
    {"uplink-dos-attack", CONFIG_HLP_UP_DOS_ATT,    0,              iptr:&(uplink_dos_attack),  defintval:0,   TYPE_INT, 0},   \
    {"rep-conn-count",    CONFIG_HLP_REP_CONN_ATT,  0,              iptr:&(rep_conn_end_count), defintval:0,   TYPE_INT, 0},   \
    {"auth-sync-attack",  CONFIG_HLP_AUTH_SYNC_ATT, 0,              iptr:&(auth_sync_attack),   defintval:0,   TYPE_INT, 0},   \
    {"null-cipher-integ", CONFIG_HLP_NULL_CIPH_INT, 0,              iptr:&(null_cipher_integ),  defintval:0,   TYPE_INT, 0},   \
    {"dnlink-imsi-extr",  CONFIG_HLP_DNLNK_IMSI_EX, 0,              iptr:&(dnlnk_imsi_extract), defintval:0,   TYPE_INT, 0},   \
    {"uplink-imsi-extr",  CONFIG_HLP_UPLNK_IMSI_EX, 0,              iptr:&(uplnk_imsi_extract), defintval:0,   TYPE_INT, 0},   \
    {"rrc-911",           CONFIG_EMERGENCY_UE,      PARAMFLAG_BOOL, iptr:&(rrc_911),            defintval:0,   TYPE_INT, 0}    \


#define _CHECK_CLI_ATTACK(_type, _gVar, _descrip)                                       \
 if (_gVar > 0) {                                                                       \
   printf ("Attack selected: " _descrip " (" #_type ": %i)\n", _gVar);                  \
   _nAttacks++;                                                                         \
 }

#define _CHECK_CLI_LEVEL_ATTACK(_gVar, _descrip)        _CHECK_CLI_ATTACK(level, _gVar, _descrip)
#define _CHECK_CLI_COUNT_ATTACK(_gVar, _descrip)        _CHECK_CLI_ATTACK(count, _gVar, _descrip)
#define _CHECK_CLI_MODE_ATTACK(_gVar, _descrip, _mxMode)                                \
 if (_gVar > 0) {                                                                       \
   if (_gVar <= _mxMode) {                                                              \
     _CHECK_CLI_ATTACK(mode, _gVar, _descrip " (1-" #_mxMode ")")                       \
   }                                                                                    \
   else {                                                                               \
     printf("Attack _deselected_: " _descrip " *** mode %i > %i ***\n",                 \
            _gVar, _mxMode);                                                            \
     _gVar = 0;                                                                         \
   }                                                                                    \
 }

/* Print attack configuration (after get_options ()) */

#define PRINT_ATTACK_INFO()                                                             \
{                                                                                       \
  int _nAttacks = 0;                                                                    \
                                                                                        \
                                                                                        \
  printf("\n************************ ATTACK CLI **********************************\n"); \
  if (bts_attack > 0) {                                                                 \
    printf("Attack selected: BTS Resource Depletion (Level: %i; delay: %i ms)\n",       \
           bts_attack, bts_delay);                                                      \
    _nAttacks++;                                                                        \
  }                                                                                     \
  if (blind_dos_attack > 0) {                                                           \
    if (tmsi_blind_dos_rrc /* != 0 */) {                                                \
      printf("Attack selected: Blind DoS (Level: %i; TMSI: %ld)\n",                      \
             blind_dos_attack, tmsi_blind_dos_rrc);                                     \
      _nAttacks++;                                                                      \
    }                                                                                   \
    else {                                                                              \
      printf("Attack _deselected_: Blind DOS *** MISSING TMSI ***\n");                  \
      blind_dos_attack = 0;                                                             \
    }                                                                                   \
  }                                                                                     \
  _CHECK_CLI_MODE_ATTACK  (dnlink_dos_attack,  "Downlink DoS", 8);                      \
  _CHECK_CLI_LEVEL_ATTACK (uplink_dos_attack,  "Uplink DoS");                           \
  _CHECK_CLI_COUNT_ATTACK (rep_conn_end_count, "Repeated connection setup complete");   \
  _CHECK_CLI_LEVEL_ATTACK (auth_sync_attack,   "Auth sync failure");                    \
  _CHECK_CLI_MODE_ATTACK  (null_cipher_integ,  "Null cipher integrity", 2);             \
  _CHECK_CLI_MODE_ATTACK  (dnlnk_imsi_extract, "Downlink IMSI extractor", 4);           \
  _CHECK_CLI_LEVEL_ATTACK (uplnk_imsi_extract, "Uplink IMSI extractor");                \
                                                                                        \
  if (_nAttacks == 0) {                                                                 \
    printf("No attack selected.  Running without attack\n");                            \
  }                                                                                     \
  else if (_nAttacks > 1) {                                                             \
    printf("*** WARNING: %d attacks selected ***\n", _nAttacks);                        \
  }                                                                                     \
                                                                                        \
  if (rrc_911 /* != 0 */) {                                                             \
    printf("Using Emergency Connection Request mode\n");                                \
  }                                                                                     \
  printf("***********************************************************************\n\n");\
}

#endif /* ifndef __ATTACK_CLIOPTS_H__ */


#if defined (__TEST_ATTACK_CLIOPTS_H__)

  void
main ()
{
  ATTACK_CLI_OPTS;

  PRINT_ATTACK_INFO ();
}

#endif /* ifdef __TEST_ATTACK_CLIOPTS_H__ */
