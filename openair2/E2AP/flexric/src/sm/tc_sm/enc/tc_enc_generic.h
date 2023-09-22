#ifndef TC_ENCRYPTION_GENERIC
#define TC_ENCRYPTION_GENERIC 

#include "tc_enc_asn.h"
#include "tc_enc_fb.h"
#include "tc_enc_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define tc_enc_event_trigger(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_event_trigger_plain, \
                           tc_enc_asn_t*: tc_enc_event_trigger_asn,\
                           tc_enc_fb_t*: tc_enc_event_trigger_fb,\
                           default: tc_enc_event_trigger_plain) (U)

#define tc_enc_action_def(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_action_def_plain, \
                           tc_enc_asn_t*: tc_enc_action_def_asn, \
                           tc_enc_fb_t*: tc_enc_action_def_fb, \
                           default:  tc_enc_action_def_plain) (U)

#define tc_enc_ind_hdr(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_ind_hdr_plain , \
                           tc_enc_asn_t*: tc_enc_ind_hdr_asn, \
                           tc_enc_fb_t*: tc_enc_ind_hdr_fb, \
                           default:  tc_enc_ind_hdr_plain) (U)

#define tc_enc_ind_msg(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_ind_msg_plain , \
                           tc_enc_asn_t*: tc_enc_ind_msg_asn, \
                           tc_enc_fb_t*: tc_enc_ind_msg_fb, \
                           default:  tc_enc_ind_msg_plain) (U)

#define tc_enc_call_proc_id(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_call_proc_id_plain , \
                           tc_enc_asn_t*: tc_enc_call_proc_id_asn, \
                           tc_enc_fb_t*: tc_enc_call_proc_id_fb, \
                           default:  tc_enc_call_proc_id_plain) (U)

#define tc_enc_ctrl_hdr(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_ctrl_hdr_plain , \
                           tc_enc_asn_t*: tc_enc_ctrl_hdr_asn, \
                           tc_enc_fb_t*: tc_enc_ctrl_hdr_fb, \
                           default:  tc_enc_ctrl_hdr_plain) (U)

#define tc_enc_ctrl_msg(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_ctrl_msg_plain , \
                           tc_enc_asn_t*: tc_enc_ctrl_msg_asn, \
                           tc_enc_fb_t*: tc_enc_ctrl_msg_fb, \
                           default:  tc_enc_ctrl_msg_plain) (U)

#define tc_enc_ctrl_out(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_ctrl_out_plain , \
                           tc_enc_asn_t*: tc_enc_ctrl_out_asn, \
                           tc_enc_fb_t*: tc_enc_ctrl_out_fb, \
                           default:  tc_enc_ctrl_out_plain) (U)

#define tc_enc_func_def(T,U) _Generic ((T), \
                           tc_enc_plain_t*: tc_enc_func_def_plain, \
                           tc_enc_asn_t*: tc_enc_func_def_asn, \
                           tc_enc_fb_t*:  tc_enc_func_def_fb, \
                           default:  tc_enc_func_def_plain) (U)

#endif

