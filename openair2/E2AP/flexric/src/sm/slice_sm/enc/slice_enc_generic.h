#ifndef SLICE_ENCRYPTION_GENERIC
#define SLICE_ENCRYPTION_GENERIC 

#include "slice_enc_asn.h"
#include "slice_enc_fb.h"
#include "slice_enc_plain.h"

/////////////////////////////////////////////////////////////////////
// 9 Information Elements that are interpreted by the SM according
// to ORAN-WG3.E2SM-v01.00.00 Technical Specification
/////////////////////////////////////////////////////////////////////


#define slice_enc_event_trigger(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_event_trigger_plain, \
                           slice_enc_asn_t*: slice_enc_event_trigger_asn,\
                           slice_enc_fb_t*: slice_enc_event_trigger_fb,\
                           default: slice_enc_event_trigger_plain) (U)

#define slice_enc_action_def(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_action_def_plain, \
                           slice_enc_asn_t*: slice_enc_action_def_asn, \
                           slice_enc_fb_t*: slice_enc_action_def_fb, \
                           default:  slice_enc_action_def_plain) (U)

#define slice_enc_ind_hdr(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_ind_hdr_plain , \
                           slice_enc_asn_t*: slice_enc_ind_hdr_asn, \
                           slice_enc_fb_t*: slice_enc_ind_hdr_fb, \
                           default:  slice_enc_ind_hdr_plain) (U)

#define slice_enc_ind_msg(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_ind_msg_plain , \
                           slice_enc_asn_t*: slice_enc_ind_msg_asn, \
                           slice_enc_fb_t*: slice_enc_ind_msg_fb, \
                           default:  slice_enc_ind_msg_plain) (U)

#define slice_enc_call_proc_id(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_call_proc_id_plain , \
                           slice_enc_asn_t*: slice_enc_call_proc_id_asn, \
                           slice_enc_fb_t*: slice_enc_call_proc_id_fb, \
                           default:  slice_enc_call_proc_id_plain) (U)

#define slice_enc_ctrl_hdr(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_ctrl_hdr_plain , \
                           slice_enc_asn_t*: slice_enc_ctrl_hdr_asn, \
                           slice_enc_fb_t*: slice_enc_ctrl_hdr_fb, \
                           default:  slice_enc_ctrl_hdr_plain) (U)

#define slice_enc_ctrl_msg(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_ctrl_msg_plain , \
                           slice_enc_asn_t*: slice_enc_ctrl_msg_asn, \
                           slice_enc_fb_t*: slice_enc_ctrl_msg_fb, \
                           default:  slice_enc_ctrl_msg_plain) (U)

#define slice_enc_ctrl_out(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_ctrl_out_plain , \
                           slice_enc_asn_t*: slice_enc_ctrl_out_asn, \
                           slice_enc_fb_t*: slice_enc_ctrl_out_fb, \
                           default:  slice_enc_ctrl_out_plain) (U)

#define slice_enc_func_def(T,U) _Generic ((T), \
                           slice_enc_plain_t*: slice_enc_func_def_plain, \
                           slice_enc_asn_t*: slice_enc_func_def_asn, \
                           slice_enc_fb_t*:  slice_enc_func_def_fb, \
                           default:  slice_enc_func_def_plain) (U)

#endif

