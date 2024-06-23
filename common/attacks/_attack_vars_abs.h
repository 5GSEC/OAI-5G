// Attack level or mode, count, and value command line options

// 0: No attack, 1+ is attack level
_ATTACK_VDEF(int, bts_attack, 0)          // <level information; openair2/RRC/LTE/{rrc_UE.c,L2_interface_ue.c}; openair3/NAS/UE/EMM/SAP/{emm_send,emm_as}.c; openair3/NAS/UE/EMM/Attach.c>
_ATTACK_VDEF(int, bts_delay, 188)         // <delay between attacks; openair2/RRC/LTE/rrc_UE.c>
_ATTACK_VDEF(int, blind_dos_attack, 0)    // <level information; openair2/RRC/LTE/MESSAGES/asn1_msg.c>
_ATTACK_VDEF(uint64_t, tmsi_blind_dos_rrc, 0)  // <blind DoS RRC TMSI; openair2/RRC/LTE/MESSAGES/asn1_msg.c>
_ATTACK_VDEF(int, dnlink_dos_attack, 0)   // <attack mode; openair2/RRC/LTE/rrc_UE.c; openair3/NAS/UE/EMM/SAP/emm_as.c>
_ATTACK_VDEF(int, uplink_dos_attack, 0)   // <level information; openair2/RRC/LTE/rrc_UE.c>
_ATTACK_VDEF(int, rep_conn_end_count, 0)  // <repeated connection setup complete attack count; openair2/RRC/LTE/rrc_UE.c>
_ATTACK_VDEF(int, auth_sync_attack, 0)    // <level information; openair3/NAS/UE/EMM/Attach.c>
_ATTACK_VDEF(int, null_cipher_integ, 0)   // <attack mode; openair2/RRC/LTE/rrc_UE.c; openair3/NAS/UE/EMM/SAP/emm_as.c>
_ATTACK_VDEF(int, uplnk_imsi_extract, 0)  // <level information; openair3/NAS/UE/EMM/SAP/emm_send.c>
_ATTACK_VDEF(int, dnlnk_imsi_extract, 0)  // <level information; openair3/NAS/UE/EMM/SAP/emm_as.c>
_ATTACK_VDEF(int, rrc_911, 0)             // emergency connection request mode

#undef  _ATTACK_VDEF
