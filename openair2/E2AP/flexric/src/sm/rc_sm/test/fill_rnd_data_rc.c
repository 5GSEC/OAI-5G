#include "fill_rnd_data_rc.h"

#include "../ie/ir/ran_param_struct.h"
#include "../ie/ir/ran_param_list.h"

#include <assert.h>
#include <math.h>
#include <limits.h>

static
byte_array_t cp_str_to_ba(const char* str)
{
  assert(str != NULL);
  
  const size_t sz = strlen(str);

  byte_array_t dst = {.len = sz};

  dst.buf = calloc(sz,sizeof(uint8_t));
  assert(dst.buf != NULL && "Memory exhausted");

  memcpy(dst.buf, str, sz);

  return dst;
}
/*
static
double rand_double()
{
  union {
    double a;
    uint8_t b[sizeof(double)];
  } tmp;

  for(size_t i = 0; i < sizeof(double); ++i){
    tmp.b[i] = rand();
  }

  return tmp.a;
}
*/
static
float rand_float(float ceil)
{
  return (float)rand()/(float)((float)RAND_MAX/ceil);
}


/////////////////////////////
/////////////////////////////
//////////RC Event Trigger  //////////
/////////////////////////////
/////////////////////////////




static
network_interface_e2rc_t fill_rnd_network_interface(void)
{
  network_interface_e2rc_t dst = {0}; 

  // NI Type
  // Mandatory
  // 9.3.32
  dst.ni_type = rand() % END_NETWORK_INTERFACE_TYPE;

  // NI Identifier
  // Optional
  // 9.3.33
  dst.ni_id = NULL;

  // NI Message
  // Optional
  // 9.3.34
  dst.ni_msg_id = NULL;

  return dst;
}

static
rrc_msg_id_t fill_rnd_rrc_msg_id(void)
{
  rrc_msg_id_t dst = {0}; 

  // Mandatory
  dst.type = rand() % END_RRC_MESSAGE_ID;
  if(dst.type == LTE_RRC_MESSAGE_ID){
    dst.lte = rand() % END_LTE_RRC_CLASS;
  } else if(dst.type == NR_RRC_MESSAGE_ID){
    dst.nr = rand() % END_NR_RRC_CLASS;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  // Mandatory
  // Integer
  // Number starts from 0 from the first
  // entry of a given RRC message class
  // defined in TS 36.331 [14] or TS
  // 38.331 [15].
  dst.rrc_msg_id = rand();

  return dst;
}


static
msg_ev_trg_t fill_rnd_msg_ev_trg(void)
{
  msg_ev_trg_t dst = {0}; 

  // Event Trigger Condition ID
  // Mandatory
  // 9.3.21
  // [1- 65535]
  dst.ev_trigger_cond_id = (rand()% 65535) + 1;

  // CHOICE Message Type
  // Mandatory
  dst.msg_type = rand() % END_MSG_MSG_TYPE_EV_TRG; 
  if(dst.msg_type == NETWORK_INTERFACE_MSG_TYPE_EV_TRG){
    dst.net = fill_rnd_network_interface();
  } else if(dst.msg_type == RRC_MSG_MSG_TYPE_EV_TRG){
    dst.rrc_msg = fill_rnd_rrc_msg_id();
  } else {
    assert( 0 != 0 && "Unknown type");
  }

  //Message Direction
  //Optional
  dst.msg_dir = NULL;

  //  Associated UE Info
  //  Optional
  //  9.3.26
  dst.assoc_ue_info = NULL;

  // Logical OR
  // Optional
  // 9.3.25
  dst.log_or = NULL;

  return dst;
}

static
e2sm_rc_ev_trg_frmt_1_t fill_rnd_rc_event_trigger_frmt_1(void)
{
  e2sm_rc_ev_trg_frmt_1_t dst = {0}; 

  // [1 - 65535]
  dst.sz_msg_ev_trg = (rand() % 64) + 1; // Limited for memory constraints 65535

  dst.msg_ev_trg = calloc(dst.sz_msg_ev_trg, sizeof(msg_ev_trg_t));
  assert(dst.msg_ev_trg != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_msg_ev_trg; ++i){
    dst.msg_ev_trg[i] = fill_rnd_msg_ev_trg(); 
  }

  // Global Associated UE Info
  // Optional
  // 9.3.26
  dst.global_assoc_ue_info = NULL;

  return dst;
}

static
e2sm_rc_ev_trg_frmt_2_t fill_rnd_rc_event_trigger_frmt_2(void)
{
  e2sm_rc_ev_trg_frmt_2_t dst = {0}; 

  //  Call Process Type ID
  //  Mandatory
  //  9.3.15
  //  INTEGER (1.. 65535, …)
  dst.call_proc_type_id = rand();

  // Call Breakpoint ID
  // Mandatory
  // 9.3.49
  // INTEGER (1.. 65535, …)
  dst.call_break_id = rand(); 

  // Associated E2 Node Info
  // Optional
  // 9.3.29
  // ran_param_cond_crit_def_t* assoc_e2_node_info;

  // Associated UE Info
  // Optional
  // 9.3.26
  // ev_trigger_ue_info_t* assoc_ue_info;

  return dst;
}

static
e2_node_info_chng_t fill_rnd_e2_node_info_chng(void)
{
  e2_node_info_chng_t dst = {0}; 

  //Event Trigger Condition ID
  //Mandatory
  //9.3.21
  // INTEGER (1..65535, …)
  dst.ev_trigger_cond_id = (rand()% 65535) + 1 ; 
  
  // E2 Node Information Change ID
  // Mandatory
  // INTEGER (1..512, ...)
  dst.e2_node_info_chng_id = (rand()% 512) + 1;

  // Associated Cell Info
  // Optional
  // 9.3.27
  // ev_trigger_cell_info_t* ev_trigger_cell_info;

  // Logical OR
  // Optional
  // 9.3.25
  //  logical_or_e* log_or; 

  return dst;
}

e2sm_rc_ev_trg_frmt_3_t fill_rnd_rc_event_trigger_frmt_3(void) 
{
  e2sm_rc_ev_trg_frmt_3_t dst = {0}; 

  // Sequence of E2 Node
  // Information Change
  // [1 - 65535]
  dst.sz_e2_node_info_chng = (rand() % 128) + 1; // For not exhasuting the memory


  dst.e2_node_info_chng = calloc(dst.sz_e2_node_info_chng, sizeof(e2_node_info_chng_t));
  assert(dst.e2_node_info_chng != NULL && "Memory exhasuted");

  for(size_t i = 0; i < dst.sz_e2_node_info_chng; ++i){
    dst.e2_node_info_chng[i] = fill_rnd_e2_node_info_chng();
  }

  return dst;
}

static
rrc_state_t fill_rnd_state_chng_to(void)
{
  rrc_state_t dst = {0}; 

  // 9.3.37
  dst.state_chngd_to = rand() % END_RRC_STATE_E2SM_RC;

  // 9.3.25
  // Logical OR
  dst.log_or = NULL;

  return dst;
}

static
rrc_state_lst_t fill_rnd_rrc_state_lst(void)
{
  rrc_state_lst_t dst = {0}; 

  // 9.3.37
  // [1 - 8]
  dst.sz_rrc_state = (rand()% 8) + 1;

  dst.state_chng_to = calloc(dst.sz_rrc_state , sizeof(rrc_state_t));
  assert(dst.state_chng_to != NULL && "Memory exhausted" );

  for(size_t i = 0; i <  dst.sz_rrc_state; ++i){
    dst.state_chng_to[i] = fill_rnd_state_chng_to();
  }

  return dst;
}

static
ran_param_test_t fill_rnd_ran_param_test(void);


static
ran_param_test_lst_t fill_rnd_ran_param_test_lst(void)
{
  ran_param_test_lst_t dst = {0}; 

// [1- 65535]
  dst.sz_lst = 1; //(rand() % 2) + 1; // This can grow the tree very quickly
  dst.ran_param_test = calloc(dst.sz_lst, sizeof( ran_param_test_t));
  assert(dst.ran_param_test != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_lst; ++i){
    dst.ran_param_test[i] = fill_rnd_ran_param_test();
  }

  return dst;
}

static
ran_param_test_strct_t fill_rnd_ran_param_test_struct(void) 
{
  ran_param_test_strct_t dst = {0}; 

  // [1-65535]
  dst.sz_strct = 1; //(rand()%2) + 1; // This can grow vey quickly if not...

  dst.ran_param_test = calloc(dst.sz_strct, sizeof(ran_param_test_t));
  assert(dst.ran_param_test != NULL && "Memory exhausted" );

  for(size_t i = 0; i <  dst.sz_strct; ++i){
    dst.ran_param_test[i] = fill_rnd_ran_param_test();
  }

  return dst;
}

static
ran_parameter_value_t fill_rnd_ran_param_val(void)
{
  ran_parameter_value_t dst = {0}; 

  dst.type = rand() % END_RAN_PARAMETER_VALUE ;

  if(dst.type == BOOLEAN_RAN_PARAMETER_VALUE){
    dst.bool_ran = rand()%2;
  } else if(dst.type == INTEGER_RAN_PARAMETER_VALUE) {
    dst.int_ran = rand()%4098;
  } else if(dst.type == REAL_RAN_PARAMETER_VALUE) {
    dst.real_ran = rand_float(1024); 
  } else if(dst.type == BIT_STRING_RAN_PARAMETER_VALUE ) {
    dst.bit_str_ran = cp_str_to_ba("Bit string copy ");
  } else if(dst.type == OCTET_STRING_RAN_PARAMETER_VALUE){
    dst.octet_str_ran = cp_str_to_ba("Octet string copy");
  } else if(dst.type == PRINTABLESTRING_RAN_PARAMETER_VALUE){
    dst.printable_str_ran = cp_str_to_ba("Printable string");
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static
ran_parameter_value_t fill_rnd_ran_param_elm_key_true(void)
{
  //  9.3.14
 return fill_rnd_ran_param_val();
}

static
ran_param_struct_t fill_rnd_ran_param_struct(void);

static
lst_ran_param_t fill_rnd_lst_ran_param(void)
{
  lst_ran_param_t dst = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  //1.. 4294967295
  dst.ran_param_id = rand() + 1;

  // RAN Parameter Structure
  // Mandatory
  // 9.3.12
  dst.ran_param_struct = fill_rnd_ran_param_struct();

  return dst;
}

static
ran_param_list_t fill_rnd_ran_param_lst(void)
{
  ran_param_list_t dst = {0};

  // [0- 65535]
  dst.sz_lst_ran_param = (rand() % 2) + 1;
  dst.lst_ran_param = calloc(dst.sz_lst_ran_param, sizeof(lst_ran_param_t )); 
  assert(dst.lst_ran_param != NULL && "Memory exhausted");

  for(size_t i = 0 ; i < dst.sz_lst_ran_param; ++i){
    dst.lst_ran_param[i] = fill_rnd_lst_ran_param();
  }
  
  return dst;
}

static
seq_ran_param_t fill_rnd_seq_ran_param(void);

static
ran_param_struct_t fill_rnd_ran_param_struct(void)
{
  ran_param_struct_t dst = {0}; 
 
  // [1-65535]
  dst.sz_ran_param_struct = (rand() % 2 ) +1 ;

  dst.ran_param_struct = calloc(dst.sz_ran_param_struct, sizeof(seq_ran_param_t));
  assert(dst.ran_param_struct != NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_ran_param_struct ; ++i){
    dst.ran_param_struct[i] =  fill_rnd_seq_ran_param();
  }

  return dst;
}

static
ran_param_test_cond_t fill_rnd_ran_param_test_cond(void)
{
  ran_param_test_cond_t dst = {0};

  dst.cond = rand() % END_RAN_PARAM_TEST_COND;

  if(dst.cond == COMPARISON_RAN_PARAM_TEST_COND){
    dst.comp = rand() % END_RAN_PARAM_TEST_COND_COMP;
  } else if(dst.cond == PRESENCE_RAN_PARAM_TEST_COND){
    dst.presence = rand() % END_RAN_PARAM_TEST_COND_PRESENCE;
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

static
ran_param_elm_key_false_t fill_rnd_ran_param_elm_key_false()
{
  ran_param_elm_key_false_t dst = {0}; 

  // RAN Parameter Test Condition
  // 9.3.31
  // Mandatory
  dst.test_cond = fill_rnd_ran_param_test_cond();

  // RAN Parameter Value
  // Optional
  // 9.3.14
  dst.value = NULL;

  // Logical OR
  // Optional
  // 9.3.25
  dst.logical_or = NULL;

  return dst;
}

static
ran_param_test_t fill_rnd_ran_param_test(void)
{
  ran_param_test_t dst = {0}; 

 //RAN Parameter ID
  //Mandatory
  //Bug in the standard. 9.3.9 wrote in section 9. 
  //9.3.8 in ASN language. Probably 9.3.8 intended
  //1.. 4294967295
  dst.ran_param_id = (rand() %  4294967295) + 1; 

  dst.type = rand() % END_RAN_PARAMETER_TYPE;

  if(dst.type == LIST_RAN_PARAMETER_TYPE){
    dst.lst = fill_rnd_ran_param_test_lst();
  } else if(dst.type == STRUCTURE_RAN_PARAMETER_TYPE ){
    dst.strct = fill_rnd_ran_param_test_struct() ;
  } else if(dst.type ==  ELEMENT_WITH_KEY_FLAG_TRUE_RAN_PARAMETER_TYPE){
    dst.flag_true = fill_rnd_ran_param_elm_key_true();
  } else if(dst.type == ELEMENT_WITH_KEY_FLAG_FALSE_RAN_PARAMETER_TYPE ){
    dst.flag_false = fill_rnd_ran_param_elm_key_false();
  }

  return dst;
}

static
ran_param_cond_crit_def_t fill_rnd_ran_param_cond_crit_def(void)
{
  ran_param_cond_crit_def_t dst = {0}; 

  // [1 - 255 ]
  dst.sz_ran_param_test = 1; //(rand()%16) + 1; 

  // RAN Parameter Test
  // 9.3.30
  dst.ran_param_test = calloc(dst.sz_ran_param_test, sizeof(ran_param_test_t));
  assert(dst.ran_param_test != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_ran_param_test; ++i){
    dst.ran_param_test[i] = fill_rnd_ran_param_test();
  }

  return dst;
}

static
ue_info_chng_t fill_rnd_ue_info_chng()
{
  ue_info_chng_t dst = {0}; 

  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  // INTEGER (1..65535, …)
  dst.ev_trig_cond_id = (rand()%65535) + 1;

  // CHOICE Trigger Type

  dst.type =  L2_STATE_UE_INFO_CHNG_TRIGGER_TYPE; //rand()%END_UE_INFO_CHNG_TRIGGER_TYPE;

  if( dst.type == RRC_STATE_UE_INFO_CHNG_TRIGGER_TYPE){
    // RRC State
    dst.rrc_state = fill_rnd_rrc_state_lst();  

  } else if(dst.type == UE_ID_CHANGE_UE_INFO_CHNG_TRIGGER_TYPE){
    // UE Identifier Change
    // [1 - 512]
    dst.ue_id_chng = (rand() % 512) + 1;

  } else if(dst.type == L2_STATE_UE_INFO_CHNG_TRIGGER_TYPE){
    // 9.3.29
    // RAN Parameter Conditional Criteria Definition
    dst.l2_state = fill_rnd_ran_param_cond_crit_def();

  } else {
    assert(0!=0 && "Unknown type");
  }


  // Associated UE Info 
  // Optional
  // 9.3.26
  dst.assoc_ue_info = NULL;

  // Logical OR
  // Optional
  // 9.3.25
  dst.log_or = NULL;

  return dst;
}


static
e2sm_rc_ev_trg_frmt_4_t fill_rnd_rc_event_trigger_frmt_4(void) 
{
  e2sm_rc_ev_trg_frmt_4_t dst = {0}; 

 // Sequence of UE Information Change
 // [ 1 - 65535]
  dst.sz_ue_info_chng = 1; //(rand()% 128) + 1;
  
  dst.ue_info_chng = calloc(dst.sz_ue_info_chng, sizeof(ue_info_chng_t) );
  assert(dst.ue_info_chng != NULL && "Memory exhausted"); 

  for(size_t i = 0; i <  dst.sz_ue_info_chng; ++i){
   dst.ue_info_chng[i] = fill_rnd_ue_info_chng();   
  }

  return dst;
}


static
e2sm_rc_ev_trg_frmt_5_t fill_rnd_rc_event_trigger_frmt_5()
{
  e2sm_rc_ev_trg_frmt_5_t dst = {0} ;
 
  // On Demand
  // Mandatory
  dst.on_demand = rand()% END_ON_DEMAND_FRMT_5 ;

  // Associated UE Info
  // Optional
  // 9.3.26
  // ev_trigger_ue_info_t* assoc_ue_info;

  // Associated Cell Info
  // Optional
  // 9.3.27
  // ev_trigger_cell_info_t* assoc_cell_info;

  return dst;
}


e2sm_rc_event_trigger_t fill_rnd_rc_event_trigger(void)
{
  e2sm_rc_event_trigger_t dst = {0}; 

  dst.format = rand() % END_E2SM_RC_EV_TRIGGER_FORMAT;

  if(dst.format == FORMAT_1_E2SM_RC_EV_TRIGGER_FORMAT){
    dst.frmt_1 = fill_rnd_rc_event_trigger_frmt_1() ;

  }else if(dst.format == FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT){
    dst.frmt_2 = fill_rnd_rc_event_trigger_frmt_2() ;

  }else if(dst.format == FORMAT_3_E2SM_RC_EV_TRIGGER_FORMAT){
    dst.frmt_3 = fill_rnd_rc_event_trigger_frmt_3() ;

  }else if(dst.format == FORMAT_4_E2SM_RC_EV_TRIGGER_FORMAT){
    dst.frmt_4 = fill_rnd_rc_event_trigger_frmt_4() ;

  }else if(dst.format == FORMAT_5_E2SM_RC_EV_TRIGGER_FORMAT){
    dst.frmt_5 = fill_rnd_rc_event_trigger_frmt_5() ;

  } else {
    assert(0!=0 && "Unknown format");
  }

    return dst;
}

/////////////////////////////
/////////////////////////////
////////// End of RC Event Trigger  //////////
/////////////////////////////
/////////////////////////////



/////////////////////////////
/////////////////////////////
////////// Start of RC Action Definition //////////////////
/////////////////////////////
/////////////////////////////

static
param_report_def_t fill_rnd_param_report_def(void) 
{
  param_report_def_t dst = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  dst.ran_param_id = rand() + 1; 

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  dst.ran_param_def = NULL; 

  return dst;
}


static
e2sm_rc_act_def_frmt_1_t fill_rnd_rc_action_def_frmt_1(void)
{
  e2sm_rc_act_def_frmt_1_t dst = {0}; 

  // Parameters to be Reported List
  // [1-65535]
  dst.sz_param_report_def = (rand()% 65535) + 1;

  dst.param_report_def = calloc(dst.sz_param_report_def, sizeof( param_report_def_t));
  assert(dst.param_report_def != NULL && "Memory exhausyted");

  for(size_t i = 0; i < dst.sz_param_report_def; ++i){
    dst.param_report_def[i] = fill_rnd_param_report_def() ;
  }

  return dst;
}

static
int recursion_fill_rnd_ran_param_val_type = 0;

static
ran_param_val_type_t fill_rnd_ran_param_val_type()
{
  ran_param_val_type_t dst = {0}; 

  dst.type =  STRUCTURE_RAN_PARAMETER_VAL_TYPE; //rand() % END_RAN_PARAMETER_VAL_TYPE;

  recursion_fill_rnd_ran_param_val_type += 1; 
  if(recursion_fill_rnd_ran_param_val_type > 4){
//    recursion_fill_rnd_ran_param_val_type = 0; 
    dst.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  }

  if(dst.type == ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE){
    dst.flag_true = calloc(1, sizeof( ran_parameter_value_t));
    assert(dst.flag_true != NULL && "Memory exhausted" );
    *dst.flag_true = fill_rnd_ran_param_val();
  } else if(dst.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE ){
    dst.flag_false = calloc(1, sizeof( ran_parameter_value_t));
    assert(dst.flag_false != NULL && "Memory exhausted" );
    *dst.flag_false = fill_rnd_ran_param_val();
  }else if(dst.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE){
    dst.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(dst.strct != NULL && "Memory exhausted" );
    *dst.strct = fill_rnd_ran_param_struct();
  }else if(dst.type == LIST_RAN_PARAMETER_VAL_TYPE ){
    dst.lst = calloc(1, sizeof(ran_param_list_t)); 
    assert(dst.lst != NULL && "memory exhausted");    
    *dst.lst = fill_rnd_ran_param_lst();
  } else {
    assert(0!=0 && "not implemented" );
  }
 
  return dst;
}

static
seq_ran_param_t fill_rnd_seq_ran_param(void)
{
  seq_ran_param_t dst = {0}; 

  //RAN Parameter ID
  //Mandatory
  //9.3.8
  // [1 - 4294967295]
  dst.ran_param_id = (rand()% 4098) +1 ;

  // RAN Parameter Value Type
  // 9.3.11
  // Mandatory
  dst.ran_param_val = fill_rnd_ran_param_val_type();  

  return dst;
}

static
policy_action_t fill_rnd_policy_action(void)
{
  policy_action_t dst = {0}; 

  //  Policy Action ID
  //  Mandatory
  //  9.3.6
  //  [1 - 65535]
  dst.policy_act_id = (rand()% 65535) +1;

  // Sequence of RAN Parameters
  // [0- 65535]
  dst.sz_seq_ran_param = rand()% 2; //65535;
   
  if(dst.sz_seq_ran_param > 0){
    dst.seq_ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t)); 
    assert(dst.seq_ran_param != NULL && "Memory exhausted" );
  }

  for(size_t i = 0 ; i < dst.sz_seq_ran_param; ++i){
    dst.seq_ran_param[i] = fill_rnd_seq_ran_param();
  }

  // RIC Policy decision
  // Optional
  dst.pol_dec = NULL; 

  return dst;
}

static
policy_cond_t fill_rnd_policy_cond(void)
{
  policy_cond_t dst = {0}; 

  // Policy Action Definition
  // Mandatory
  // 9.3.20
  dst.pol_act = fill_rnd_policy_action();

  // Policy Condition Definition
  // Optional
  // 9.3.29
  dst.pol = NULL;

  return dst;
}

static
e2sm_rc_act_def_frmt_2_t fill_rnd_rc_action_def_frmt_2(void)
{
  e2sm_rc_act_def_frmt_2_t dst = {0}; 

  // Sequence of Policy Conditions
  // [1 - 65535]
  dst.sz_policy_cond = (rand() % 2) + 1; // Too large  65535

  dst.policy_cond = calloc(dst.sz_policy_cond, sizeof(policy_cond_t) );
  assert(dst.policy_cond != NULL && "memory exhausted" );

  for(size_t i = 0; i <  dst.sz_policy_cond; ++i){
    dst.policy_cond[i] = fill_rnd_policy_cond(); 
  }

  return dst;
}

static
ran_param_ins_t fill_rnd_ran_param_ins(void)
{
  ran_param_ins_t dst = {0}; 

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  dst.ran_id = (rand()% 4294967295) + 1;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  dst.def = NULL;

  return dst;
}

static
e2sm_rc_act_def_frmt_3_t fill_rnd_rc_action_def_frmt_3(void)
{
  e2sm_rc_act_def_frmt_3_t dst = {0}; 

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535] 
  dst.id = rand()% 65535 + 1;

  // List of RAN parameters for Insert
  // Indication
  // [1 - 65535]
  dst.sz_ran_param_ins = (rand() % 64) + 1;
  dst.ran_param = calloc(dst.sz_ran_param_ins, sizeof(ran_param_ins_t) );
  assert(dst.ran_param != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_ran_param_ins; ++i){
    dst.ran_param[i] = fill_rnd_ran_param_ins();
  }

  //  UE ID
  //  Optional
  //  9.3.10
  dst.ue_id = NULL;

  return dst;
}

static
ran_param_ins_ind_t fill_rnd_ran_param_ins_ind(void)
{
  ran_param_ins_ind_t dst = {0}; 
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1.. 429496729 ]
  dst.ran_param_id = rand() + 1; 

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  dst.ran_param_def = NULL; 

  return dst;
}

static
seq_ins_ind_act_def_t fill_rnd_seq_ins_ind_act_def(void)
{
  seq_ins_ind_act_def_t dst = {0}; 

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  dst.ind_id = (rand()%  65535 ) +1;

  // List of RAN parameters for
  // Insert Indication
  // [1-65535]
  dst.sz_ran_param_ins_ind = (rand() % 16) + 1;

  dst.ran_param_ins_ind = calloc(dst.sz_ran_param_ins_ind, sizeof(ran_param_ins_ind_t));
  assert(dst.ran_param_ins_ind != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_ran_param_ins_ind; ++i){
    dst.ran_param_ins_ind[i] = fill_rnd_ran_param_ins_ind();
  }

  return dst;
}


static
seq_ins_style_t fill_rnd_seq_ins_style(void)
{
  seq_ins_style_t dst = {0}; 

  // Requested Insert Style
  // Mandatory
  // 9.3.3
  // 6.2.2.2. in E2 SM common 
  // Integer
  dst.req_insert_style = rand() % 1024;

  // Sequence of Insert Indication
  // Action Definition
  // [1-63]
  dst.sz_seq_ins_ind_act_def = (rand()% 63) + 1;

  dst.seq_ins_ind_act_def = calloc(dst.sz_seq_ins_ind_act_def, sizeof( seq_ins_ind_act_def_t));
  assert(dst.seq_ins_ind_act_def != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ins_ind_act_def; ++i){
    dst.seq_ins_ind_act_def[i] = fill_rnd_seq_ins_ind_act_def();
  }

  return dst;
}

static
e2sm_rc_act_def_frmt_4_t fill_rnd_rc_action_def_frmt_4(void)
{
  e2sm_rc_act_def_frmt_4_t dst = {0};

  // Sequence of Insert Styles for
  // Multiple Actions
  // [1-4]
  dst.sz_seq_ins_style = (rand() % 4) + 1;
  
  dst.seq_ins_style = calloc(dst.sz_seq_ins_style, sizeof(seq_ins_style_t) );
  assert(dst.seq_ins_style != NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_seq_ins_style; ++i){
    dst.seq_ins_style[i] = fill_rnd_seq_ins_style();
  }


  // UE ID
  // Optional
  // 9.3.10
  dst.ue_id = NULL; 

  return dst;
}


e2sm_rc_action_def_t fill_rnd_rc_action_def(void)
{
  e2sm_rc_action_def_t dst = {0}; 

  //  RIC Style Type
  //  Mandatory
  //  9.3.3
  // Defined in common 6.2.2.2.
  dst.ric_style_type = (rand()%1024) + 1;

  dst.format = rand() % END_E2SM_RC_ACT_DEF;

  if(dst.format == FORMAT_1_E2SM_RC_ACT_DEF ){
    dst.frmt_1 = fill_rnd_rc_action_def_frmt_1();

  } else if(dst.format == FORMAT_2_E2SM_RC_ACT_DEF){
    dst.frmt_2 = fill_rnd_rc_action_def_frmt_2();

  } else if(dst.format == FORMAT_3_E2SM_RC_ACT_DEF){
    dst.frmt_3 = fill_rnd_rc_action_def_frmt_3();

  } else if(dst.format == FORMAT_4_E2SM_RC_ACT_DEF){
    dst.frmt_4 = fill_rnd_rc_action_def_frmt_4();

  } else {
    assert(0 != 0 && "Unknown format");
  }

  return dst;
}

static
e2sm_rc_ind_hdr_frmt_1_t fill_rnd_rc_ind_hdr_frmt_1(void)
{
  e2sm_rc_ind_hdr_frmt_1_t dst = {0};

  dst.ev_trigger_id = malloc(sizeof(uint16_t));
  assert(dst.ev_trigger_id != NULL && "Memory exhausted" );

  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  // [1 - 65535]
  *dst.ev_trigger_id = (rand() % 65535)+ 1;

  return dst;
}

static
gnb_e2sm_t fill_gnb_data(void)
{
  gnb_e2sm_t gnb = {0};

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  gnb.amf_ue_ngap_id = (rand() % 2^40) + 0;

  // Mandatory
  //GUAMI 6.2.3.17 
  gnb.guami.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  
  gnb.guami.amf_region_id = (rand() % 2^8) + 0;
  gnb.guami.amf_set_id = (rand() % 2^10) + 0;
  gnb.guami.amf_ptr = (rand() % 2^6) + 0;

  // gNB-CU UE F1AP ID List
  // C-ifCUDUseparated 
  gnb.gnb_cu_ue_f1ap_lst_len = (rand() % 4) + 1;
  gnb.gnb_cu_ue_f1ap_lst = calloc(gnb.gnb_cu_ue_f1ap_lst_len, sizeof(uint32_t));
  for (size_t i = 0; i < gnb.gnb_cu_ue_f1ap_lst_len; i++)
  {
    gnb.gnb_cu_ue_f1ap_lst[i] = (rand() % 4294967296) + 0;
  }

  //gNB-CU-CP UE E1AP ID List
  //C-ifCPUPseparated 
  gnb.gnb_cu_cp_ue_e1ap_lst_len = 3; //(rand() % 65535) + 1;
  gnb.gnb_cu_cp_ue_e1ap_lst = calloc(gnb.gnb_cu_cp_ue_e1ap_lst_len, sizeof(uint32_t));
  for (size_t i = 0; i < gnb.gnb_cu_cp_ue_e1ap_lst_len; i++)
  {
    gnb.gnb_cu_cp_ue_e1ap_lst[i] = (rand() % 4294967296) + 0;
  }

  // RAN UE ID
  // Optional
  // 6.2.3.25
  gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(gnb.ran_ue_id != NULL && "Memory exhausted");
  *gnb.ran_ue_id = 14294967296; // (rand() % 2^64) + 0;

  //  M-NG-RAN node UE XnAP ID
  // C- ifDCSetup
  // 6.2.3.19
  gnb.ng_ran_node_ue_xnap_id = calloc(1, sizeof(uint32_t));
  assert(gnb.ng_ran_node_ue_xnap_id != NULL && "Memory exhausted");
  *gnb.ng_ran_node_ue_xnap_id = (rand() % 4294967296) + 0;

  // Global gNB ID
  // 6.2.3.3
  // Optional
  // This IE shall not be used. Global NG-RAN Node ID IE shall replace this IE 
  //gnb.global_gnb_id = calloc(1, sizeof(global_gnb_id_t));
  //assert(gnb.global_gnb_id != NULL && "Memory exhausted");
  //gnb.global_gnb_id->type = GNB_TYPE_ID;
  //gnb.global_gnb_id->plmn_id = (plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  //gnb.global_gnb_id->gnb_id = (rand() % 4294967296) + 0;

  // Global NG-RAN Node ID
  // C-ifDCSetup
  // 6.2.3.2
  gnb.global_ng_ran_node_id = calloc(1, sizeof(*gnb.global_ng_ran_node_id));
  gnb.global_ng_ran_node_id->type = rand()%END_GLOBAL_TYPE_ID;

  switch (gnb.global_ng_ran_node_id->type)
  {
  case GNB_GLOBAL_TYPE_ID:
    gnb.global_ng_ran_node_id->global_gnb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
//    gnb.global_ng_ran_node_id->global_gnb_id.type = GNB_TYPE_ID;
    gnb.global_ng_ran_node_id->global_gnb_id.gnb_id = (rand() % 4294967296) + 0;
    break;
  
  case NG_ENB_GLOBAL_TYPE_ID:
    gnb.global_ng_ran_node_id->global_ng_enb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    gnb.global_ng_ran_node_id->global_ng_enb_id.type = LONG_MACRO_NG_ENB_TYPE_ID;  // rand()%END_NG_ENB_TYPE_ID;

    switch (gnb.global_ng_ran_node_id->global_ng_enb_id.type)
    {
    case MACRO_NG_ENB_TYPE_ID:
      gnb.global_ng_ran_node_id->global_ng_enb_id.macro_ng_enb_id = (rand() % 2^20) + 0;
      break;

    case SHORT_MACRO_NG_ENB_TYPE_ID:
      gnb.global_ng_ran_node_id->global_ng_enb_id.short_macro_ng_enb_id = (rand() % 2^18) + 0;
      break;

    case LONG_MACRO_NG_ENB_TYPE_ID:
      gnb.global_ng_ran_node_id->global_ng_enb_id.long_macro_ng_enb_id = (rand() % 2^21) + 0;
      break;
    
    default:
      assert(false && "Unknown Global NG eNB ID Type");
    }
    break;

  default:
    assert(false && "Unknown Global NG-RAN Node ID Type");
  }

  return gnb;
}

static
gnb_du_e2sm_t fill_gnb_du_data(void)
{
  gnb_du_e2sm_t gnb_du = {0};
  gnb_du.gnb_cu_ue_f1ap = (rand() % 4294967296) + 0;

  gnb_du.ran_ue_id = calloc(1, sizeof(*gnb_du.ran_ue_id));
  *gnb_du.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_du;
}

static
gnb_cu_up_e2sm_t fill_gnb_cu_up_data(void)
{
  gnb_cu_up_e2sm_t gnb_cu_up = {0};
  gnb_cu_up.gnb_cu_cp_ue_e1ap = (rand() % 4294967296) + 0;

  gnb_cu_up.ran_ue_id = calloc(1, sizeof(*gnb_cu_up.ran_ue_id));
  *gnb_cu_up.ran_ue_id = (rand() % 2^64) + 0;

  return gnb_cu_up;
}

ng_enb_e2sm_t fill_ng_enb_data(void)
{
  ng_enb_e2sm_t ng_enb = {0};

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  ng_enb.amf_ue_ngap_id = (rand() % 2^40) + 0;

  // 6.2.3.17
  // Mandatory
  // GUAMI
  ng_enb.guami.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  
  ng_enb.guami.amf_region_id = (rand() % 2^8) + 0;
  ng_enb.guami.amf_set_id = (rand() % 2^10) + 0;
  ng_enb.guami.amf_ptr = (rand() % 2^6) + 0;

  // 6.2.3.22
  // C-if CU DU separated
  // ng-eNB-CU UE W1AP ID
  ng_enb.ng_enb_cu_ue_w1ap_id = calloc(1, sizeof(uint32_t));
  assert(ng_enb.ng_enb_cu_ue_w1ap_id != NULL && "Memory exhausted");
  *ng_enb.ng_enb_cu_ue_w1ap_id = (rand() % 4294967296) + 0;

  // 6.2.3.19
  // C- ifDCSetup
  // M-NG-RAN node UE XnAP ID
  ng_enb.ng_ran_node_ue_xnap_id = calloc(1, sizeof(uint32_t));
  assert(ng_enb.ng_ran_node_ue_xnap_id != NULL && "Memory exhausted");
  *ng_enb.ng_ran_node_ue_xnap_id = (rand() % 4294967296) + 0;

  // OPTIONAL
  // This IE shall not be used. Global NG-RAN Node ID IE shall replace this IE
  ng_enb.global_ng_enb_id = calloc(1, sizeof(*ng_enb.global_ng_enb_id));
  assert(ng_enb.global_ng_enb_id != NULL && "Memory exhausted");
  ng_enb.global_ng_enb_id->plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};

  ng_enb.global_ng_enb_id->type = rand()%END_NG_ENB_TYPE_ID;

  switch (ng_enb.global_ng_enb_id->type)
  {
  case MACRO_NG_ENB_TYPE_ID:
    ng_enb.global_ng_enb_id->macro_ng_enb_id = (rand() % 2^20) + 0;
    break;

  case SHORT_MACRO_NG_ENB_TYPE_ID:
    ng_enb.global_ng_enb_id->short_macro_ng_enb_id = (rand() % 2^18) + 0;
    break;

  case LONG_MACRO_NG_ENB_TYPE_ID:
    ng_enb.global_ng_enb_id->long_macro_ng_enb_id = (rand() % 2^21) + 0;
    break;
  
  default:
    assert(false && "Unknown Global NG eNB ID Type");
  }


  // Global NG-RAN Node ID
  // C-ifDCSetup
  // 6.2.3.2
  ng_enb.global_ng_ran_node_id = calloc(1, sizeof(*ng_enb.global_ng_ran_node_id));
  assert(ng_enb.global_ng_ran_node_id != NULL && "Memory exhausted");
  ng_enb.global_ng_ran_node_id->type = rand()%END_GLOBAL_TYPE_ID;

  switch (ng_enb.global_ng_ran_node_id->type)
  {
  case GNB_GLOBAL_TYPE_ID:
    ng_enb.global_ng_ran_node_id->global_gnb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    //ng_enb.global_ng_ran_node_id->global_gnb_id.type = GNB_TYPE_ID;
    ng_enb.global_ng_ran_node_id->global_gnb_id.gnb_id = (rand() % 4294967296) + 0;
    break;
  
  case NG_ENB_GLOBAL_TYPE_ID:
    ng_enb.global_ng_ran_node_id->global_ng_enb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    ng_enb.global_ng_ran_node_id->global_ng_enb_id.type = LONG_MACRO_NG_ENB_TYPE_ID;  // rand()%END_NG_ENB_TYPE_ID;

    switch (ng_enb.global_ng_ran_node_id->global_ng_enb_id.type)
    {
    case MACRO_NG_ENB_TYPE_ID:
      ng_enb.global_ng_ran_node_id->global_ng_enb_id.macro_ng_enb_id = (rand() % 2^20) + 0;
      break;

    case SHORT_MACRO_NG_ENB_TYPE_ID:
      ng_enb.global_ng_ran_node_id->global_ng_enb_id.short_macro_ng_enb_id = (rand() % 2^18) + 0;
      break;

    case LONG_MACRO_NG_ENB_TYPE_ID:
      ng_enb.global_ng_ran_node_id->global_ng_enb_id.long_macro_ng_enb_id = (rand() % 2^21) + 0;
      break;
    
    default:
      assert(false && "Unknown Global NG eNB ID Type");
    }
    break;

  default:
    assert(false && "Unknown Global NG-RAN Node ID Type");
  }

  return ng_enb;
}

ng_enb_du_e2sm_t fill_ng_enb_du_data(void)
{
  ng_enb_du_e2sm_t ng_enb_du = {0};

  // 6.2.3.22
  // C-if CU DU separated
  // ng-eNB-CU UE W1AP ID
  ng_enb_du.ng_enb_cu_ue_w1ap_id = (rand() % 4294967296) + 0;

  return ng_enb_du;
}

en_gnb_e2sm_t fill_en_gnb_data(void)
{
  en_gnb_e2sm_t en_gnb = {0};

  // 6.2.3.23
  // Mandatory
  // MeNB UE X2AP ID
  en_gnb.enb_ue_x2ap_id = (rand() % 4095) + 1;

  // 6.2.3.24
  // OPTIONAL
  // MeNB UE X2AP ID Extension
  en_gnb.enb_ue_x2ap_id_extension = calloc(1, sizeof(uint16_t));
  assert(en_gnb.enb_ue_x2ap_id_extension != NULL && "Memory exhausted");
  *en_gnb.enb_ue_x2ap_id_extension = (rand() % 4095) + 1;

  // 6.2.3.9
  // Mandatory
  // Global eNB ID
  en_gnb.global_enb_id.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};

  en_gnb.global_enb_id.type = rand()%SHORT_MACRO_ENB_TYPE_ID;

  switch (en_gnb.global_enb_id.type)
  {
  case MACRO_ENB_TYPE_ID:
    en_gnb.global_enb_id.macro_enb_id = (rand() % 2^20) + 0;
    break;
  
  case HOME_ENB_TYPE_ID:
    en_gnb.global_enb_id.home_enb_id = (rand() % 2^28) + 0;
    break;

  /* Possible extensions: */
  // case SHORT_MACRO_ENB_TYPE_ID:
  //   en_gnb.global_enb_id.short_macro_enb_id = (rand() % 2^18) + 0;
  //   break;

  // case LONG_MACRO_ENB_TYPE_ID:
  //   en_gnb.global_enb_id.long_macro_enb_id = (rand() % 2^21) + 0;
  //   break;

  default:
    break;
  }

  // 6.2.3.21
  // gNB-CU UE F1AP ID
  // C-ifCUDUseparated 
  en_gnb.gnb_cu_ue_f1ap_lst = calloc(1, sizeof(uint32_t));
  assert(en_gnb.gnb_cu_ue_f1ap_lst != NULL && "Memory exhausted");
  *en_gnb.gnb_cu_ue_f1ap_lst = (rand() % 4294967296) + 0;

  // gNB-CU-CP UE E1AP ID List
  // C-ifCPUPseparated 
  en_gnb.gnb_cu_cp_ue_e1ap_lst_len = 3;
  en_gnb.gnb_cu_cp_ue_e1ap_lst = calloc(en_gnb.gnb_cu_cp_ue_e1ap_lst_len, sizeof(uint32_t));
  for (size_t i = 0; i < en_gnb.gnb_cu_cp_ue_e1ap_lst_len; i++)
  {
    en_gnb.gnb_cu_cp_ue_e1ap_lst[i] = (rand() % 4294967296) + 0;
  }

  // RAN UE ID
  // Optional
  // 6.2.3.25
  en_gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(en_gnb.ran_ue_id != NULL && "Memory exhausted");
  *en_gnb.ran_ue_id = (rand() % 2^64) + 0;

  return en_gnb;
}

enb_e2sm_t fill_enb_data(void)
{
  enb_e2sm_t enb = {0};

  // 6.2.3.26
  // Mandatory
  // MME UE S1AP ID
  enb.mme_ue_s1ap_id = (rand() % 4294967296) + 0;

  // 6.2.3.18
  // Mandatory
  // GUMMEI
  enb.gummei.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
  enb.gummei.mme_group_id = (rand() % 2^16) + 0;
  enb.gummei.mme_code = (rand() % 2^8) + 0;

  // 6.2.3.23
  // C-ifDCSetup
  // MeNB UE X2AP ID
  enb.enb_ue_x2ap_id = calloc(1, sizeof(uint16_t));
  assert(enb.enb_ue_x2ap_id != NULL && "Memory exhausted");
  *enb.enb_ue_x2ap_id = (rand() % 4095) + 1;

  // 6.2.3.24
  // C-ifDCSetup
  // MeNB UE X2AP ID Extension
  enb.enb_ue_x2ap_id_extension = calloc(1, sizeof(uint16_t));
  assert(enb.enb_ue_x2ap_id_extension != NULL && "Memory exhausted");
  *enb.enb_ue_x2ap_id_extension = (rand() % 4095) + 1;

  // 6.2.3.9
  // C-ifDCSetup
  // Global eNB ID
  enb.global_enb_id = calloc(1, sizeof(*enb.global_enb_id));
  assert(enb.global_enb_id != NULL && "Memory exhausted");

  enb.global_enb_id->plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};

  enb.global_enb_id->type = MACRO_ENB_TYPE_ID;  // rand()%END_ENB_TYPE_ID;

  switch (enb.global_enb_id->type)
  {
  case MACRO_ENB_TYPE_ID:
    enb.global_enb_id->macro_enb_id = (rand() % 2^20) + 0;
    break;
  
  case HOME_ENB_TYPE_ID:
    enb.global_enb_id->home_enb_id = (rand() % 2^28) + 0;
    break;

  /* Possible extensions: */
  // case SHORT_MACRO_ENB_TYPE_ID:
  //   enb.global_enb_id->short_macro_enb_id = (rand() % 2^18) + 0;
  //   break;

  // case LONG_MACRO_ENB_TYPE_ID:
  //   enb.global_enb_id->long_macro_enb_id = (rand() % 2^21) + 0;
  //   break;

  default:
    break;
  }

  return enb;
}

static
ue_id_e2sm_t fill_rnd_ue_id(void)
{
  ue_id_e2sm_t ue_id_data = {0};

  ue_id_data.type = GNB_UE_ID_E2SM; // rand()%END_UE_ID;

  switch (ue_id_data.type)
  {
  case GNB_UE_ID_E2SM:
    ue_id_data.gnb = fill_gnb_data();
    break;

  case GNB_DU_UE_ID_E2SM:
    ue_id_data.gnb_du = fill_gnb_du_data();
    break;
  
  case GNB_CU_UP_UE_ID_E2SM:
    ue_id_data.gnb_cu_up = fill_gnb_cu_up_data();
    break;

  case NG_ENB_UE_ID_E2SM:
    ue_id_data.ng_enb = fill_ng_enb_data();
    break;

  case NG_ENB_DU_UE_ID_E2SM:
    ue_id_data.ng_enb_du = fill_ng_enb_du_data();
    break;

  case EN_GNB_UE_ID_E2SM:
    ue_id_data.en_gnb = fill_en_gnb_data();
    break;

  case ENB_UE_ID_E2SM:
    ue_id_data.enb = fill_enb_data();
    break;
  
  default:
    assert(false && "Unknown UE ID Type");
  }


  return ue_id_data;
}



static
e2sm_rc_ind_hdr_frmt_2_t fill_rnd_rc_ind_hdr_frmt_2(void)
{
  e2sm_rc_ind_hdr_frmt_2_t dst = {0}; 

  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = fill_rnd_ue_id ();

  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. From common SM
  // RIC Style Type 
  // Integer
  dst.ric_style_type = rand()% 2048;

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1 - 65535]
  dst.ins_ind_id = (rand() % 65535 ) + 1;  

  return dst;
}

static
e2sm_rc_ind_hdr_frmt_3_t fill_rnd_rc_ind_hdr_frmt_3(void)
{
  e2sm_rc_ind_hdr_frmt_3_t dst = {0}; 

  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  // [1 - 65535]
  dst.ev_trigger_cond = malloc(sizeof(uint16_t));
  assert(dst.ev_trigger_cond != NULL && "Memory exhausted" );

  *dst.ev_trigger_cond = (rand() % 65535) + 1; 
    
  // UE ID
  // Optional 
  // 9.3.10
  dst.ue_id = calloc(1, sizeof( ue_id_e2sm_t)); 
  assert(dst.ue_id != NULL && "Memory exhausted" );

  *dst.ue_id = fill_rnd_ue_id();

  return dst;
}


e2sm_rc_ind_hdr_t fill_rnd_rc_ind_hdr(void)
{
  e2sm_rc_ind_hdr_t dst = {0};

  dst.format =  rand()%END_E2SM_RC_IND_HDR;

  if(dst.format ==  FORMAT_1_E2SM_RC_IND_HDR){
    dst.frmt_1 = fill_rnd_rc_ind_hdr_frmt_1();
  } else if(dst.format == FORMAT_2_E2SM_RC_IND_HDR ){
    dst.frmt_2 = fill_rnd_rc_ind_hdr_frmt_2();
  }else if(dst.format == FORMAT_3_E2SM_RC_IND_HDR ){
    dst.frmt_3 = fill_rnd_rc_ind_hdr_frmt_3();
  }else {
    assert(0!=0 && "Unknown format");
  }

  return dst;
}

static
e2sm_rc_ind_msg_frmt_1_t fill_rnd_ind_msg_frmt_1(void)
{
  e2sm_rc_ind_msg_frmt_1_t dst = {0}; 

  //  Sequence of RAN
  //  Parameters
  //  [1 - 65535]
  dst.sz_seq_ran_param = (rand() %  64) + 1;

  dst.seq_ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t));
  assert(dst.seq_ran_param != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.seq_ran_param[i] = fill_rnd_seq_ran_param();
  }

  return dst;
}

static
seq_ue_id_t fill_rnd_seq_ue_id(void)
{
  seq_ue_id_t dst = {0}; 

  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = fill_rnd_ue_id();

  // Sequence of
  // RAN Parameter
  // [1- 65535]
  dst.sz_seq_ran_param = (rand() % 8) + 1;

  dst.seq_ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t ) );
  assert(dst.seq_ran_param != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.seq_ran_param[i] = fill_rnd_seq_ran_param();
  }


  return dst;
}

static
e2sm_rc_ind_msg_frmt_2_t fill_rnd_ind_msg_frmt_2(void)
{
  e2sm_rc_ind_msg_frmt_2_t dst = {0}; 

  //Sequence of UE Identifier
  //[1-65535]
  dst.sz_seq_ue_id = (rand() % 8) +1;

  dst.seq_ue_id = calloc(dst.sz_seq_ue_id, sizeof(seq_ue_id_t));
  assert(dst.seq_ue_id != NULL && "Memory exhausted");

  for(size_t i = 0; i <dst.sz_seq_ue_id; ++i){
    dst.seq_ue_id[i] = fill_rnd_seq_ue_id();
  }

  return dst;
}

static
cell_global_id_t fill_rnd_cell_global_id()
{
  cell_global_id_t dst = {0}; 

  // CHOICE RAT type
  // Mandatory
  dst.type = rand() % END_CGI_RAT_TYPE;
  if(dst.type == NR_CGI_RAT_TYPE){
    dst.nr_cgi.plmn_id = (e2sm_plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
    dst.nr_cgi.nr_cell_id = rand()% (1UL << 36);
  } else if(dst.type == EUTRA_CGI_RAT_TYPE ){
    dst.eutra.plmn_id =  (e2sm_plmn_t) {.mcc = 222, .mnc = 99, .mnc_digit_len = 2};
    dst.eutra.eutra_cell_id = rand()% (1 << 28); 
  } else {
    assert( 0!=0 && "Unknown type");
  }

  return dst;
}

static
seq_cell_info_t fill_rnd_seq_cell_info(void)
{
  seq_cell_info_t dst = {0}; 

  // Cell Global ID
  // Mandatory
  // 9.3.36
  // 6.2.2.5
  dst.cell_global_id = fill_rnd_cell_global_id();

  // Cell Context Information
  // Optional
  // OCTET STRING
  dst.cell_ctx_info = NULL;

  // Cell Deleted
  // Optional
  // BOOLEAN
  dst.cell_del = NULL; 

  // Neighbour Relation Table
  // Optional
  // 9.3.38
  dst.neighbour_rela_tbl = NULL;

  return dst;
}

static
e2sm_rc_ind_msg_frmt_3_t fill_rnd_ind_msg_frmt_3()
{
  e2sm_rc_ind_msg_frmt_3_t dst = {0}; 

  // Sequence of Cell Information
  // [1 - 65535]
  dst.sz_seq_cell_info = (rand() % 32) + 1;

  dst.seq_cell_info = calloc(dst.sz_seq_cell_info, sizeof(seq_cell_info_t));
  assert(dst.seq_cell_info !=  NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_cell_info; ++i){
    dst.seq_cell_info[i] = fill_rnd_seq_cell_info();
  }

  return dst;
}

static
seq_ue_info_t fill_rnd_seq_ue_info(void)
{
  seq_ue_info_t dst = {0}; 

  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = fill_rnd_ue_id(); 

  // UE Context Information
  // Optiona;
  // OCTET STRING
  dst.ue_ctx_info = NULL; 

  // Cell Global ID
  // Mandatory
  // 9.3.36
  dst.cell_global_id = fill_rnd_cell_global_id(); 

  return dst;
}

static
seq_cell_info_2_t fill_rnd_seq_cell_info_2(void)
{
  seq_cell_info_2_t dst = {0}; 

  // Cell Global ID
  // Mandatory
  // 9.3.36
  // 6.2.2.5. 
  dst.cell_global_id = fill_rnd_cell_global_id();

  // Cell Context Information
  // Optional
  // OCTET STRING
  dst.cell_ctx_info = NULL;

  // Neighbour Relation Table
  // Optional
  // 9.3.38
  dst.neighbour_rela_tbl = NULL;

  return dst;
}


static
e2sm_rc_ind_msg_frmt_4_t fill_rnd_ind_msg_frmt_4(void)
{
  e2sm_rc_ind_msg_frmt_4_t dst = {0}; 

  //Sequence of UE Information
  // [0-65535]
  dst.sz_seq_ue_info = rand() % 8;
  if(dst.sz_seq_ue_info > 0){
    dst.seq_ue_info = calloc(dst.sz_seq_ue_info, sizeof(seq_ue_info_t));
    assert(dst.seq_ue_info != NULL && "memory exhausted");
  }
  
  for(size_t i = 0; i < dst.sz_seq_ue_info; ++i){
    dst.seq_ue_info[i] = fill_rnd_seq_ue_info();
  }

  // Sequence of Cell Information
  // [0-65535]
  dst.sz_seq_cell_info_2 = rand()%8; 

  if(dst.sz_seq_cell_info_2 > 0){
    dst.seq_cell_info_2 = calloc(dst.sz_seq_cell_info_2, sizeof( seq_cell_info_2_t ) );
    assert(dst.seq_cell_info_2 != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_cell_info_2; ++i){
    dst.seq_cell_info_2[i] = fill_rnd_seq_cell_info_2();
  }

  return dst;
}

static    
e2sm_rc_ind_msg_frmt_5_t fill_rnd_ind_msg_frmt_5(void)
{
  e2sm_rc_ind_msg_frmt_5_t dst = {0}; 

  // List of RAN parameters requested
  // [0-65535]
  dst.sz_seq_ran_param = rand() % 8;

  if(dst.sz_seq_ran_param > 0){
    dst.seq_ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t));
    assert(dst.seq_ran_param != NULL && "memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.seq_ran_param[i] = fill_rnd_seq_ran_param();
  }

  return dst;
}

static
ran_param_req_t fill_rnd_ran_param_req(void)
{
  ran_param_req_t dst = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // 1 4294967295,
  dst.ran_param_id = rand() + 1;

  // RAN Parameter Value Type
  // Mandatory
  // 9.3.11
 dst.ran_param = fill_rnd_ran_param_val_type();

  return dst;
}

static
seq_ins_ind_act_2_t fill_rnd_seq_ins_ind_act_2(void)
{
  seq_ins_ind_act_2_t dst = {0}; 

  //  Insert Indication ID
  //  Mandatory
  //  9.3.16
  //  1.. 65535
  dst.ins_ind_id = (rand() % 65535) + 1;

  // List of RAN parameters requested
  // [0-65535]
  dst.sz_ran_param_req = rand() % 8; 

  if(dst.sz_ran_param_req > 0){
    dst.ran_param_req = calloc(dst.sz_ran_param_req, sizeof(ran_param_req_t ));
    assert(dst.ran_param_req != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_ran_param_req; ++i){
    dst.ran_param_req[i] = fill_rnd_ran_param_req();
  }

  return dst;
}

static
seq_ins_style_2_t fill_rnd_seq_ins_style_2(void)
{

  seq_ins_style_2_t dst = {0}; 

  // Indicated Insert Style
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.ind_ins_style = rand();

  // Sequence of Insert Indication Actions
  // [1-63]
  dst.sz_seq_ins_ind_act_2 = (rand()%63) + 1; 

  dst.seq_ins_ind_act = calloc(dst.sz_seq_ins_ind_act_2, sizeof(seq_ins_ind_act_2_t));
  assert(dst.seq_ins_ind_act != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ins_ind_act_2; ++i){
    dst.seq_ins_ind_act[i] = fill_rnd_seq_ins_ind_act_2();
  }

  return dst;
}


static
e2sm_rc_ind_msg_frmt_6_t fill_rnd_ind_msg_frmt_6(void)
{
  e2sm_rc_ind_msg_frmt_6_t dst = {0}; 

  // Sequence of Insert Styles for Multiple Actions
  // [1-63]
  dst.sz_seq_ins_style_ind_msg = (rand()%8) + 1;

  dst.seq_ins_style = calloc(dst.sz_seq_ins_style_ind_msg, sizeof(seq_ins_style_2_t));
  assert(dst.seq_ins_style != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ins_style_ind_msg; ++i){
    dst.seq_ins_style[i] = fill_rnd_seq_ins_style_2();
  }

  return dst;
}

e2sm_rc_ind_msg_t fill_rnd_rc_ind_msg(void)
{
  e2sm_rc_ind_msg_t dst = {0};
  
  dst.format = rand()% END_E2SM_RC_IND_MSG;

  if( dst.format == FORMAT_1_E2SM_RC_IND_MSG){
    dst.frmt_1 = fill_rnd_ind_msg_frmt_1();
  } else if(dst.format == FORMAT_2_E2SM_RC_IND_MSG){
    dst.frmt_2 = fill_rnd_ind_msg_frmt_2();
  }else if(dst.format == FORMAT_3_E2SM_RC_IND_MSG){
    dst.frmt_3 = fill_rnd_ind_msg_frmt_3();
  }else if(dst.format == FORMAT_4_E2SM_RC_IND_MSG){
    dst.frmt_4 = fill_rnd_ind_msg_frmt_4();
  }else if(dst.format == FORMAT_5_E2SM_RC_IND_MSG){
    dst.frmt_5 = fill_rnd_ind_msg_frmt_5();
  }else if(dst.format == FORMAT_6_E2SM_RC_IND_MSG){
    dst.frmt_6 = fill_rnd_ind_msg_frmt_6();
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

/////////////////////////////
/////////////////////////////
////////// End of RC Message Indication //////////
/////////////////////////////
/////////////////////////////



/////////////////////////////
/////////////////////////////
////////// Start of RC Call Process ID //////////////////
/////////////////////////////
/////////////////////////////

e2sm_rc_cpid_t fill_rnd_rc_cpid(void)
{
  e2sm_rc_cpid_t dst = {0}; 

  // RIC Call Process ID
  // Mandatory
  // 9.3.18
  // [ 1 - 4294967295]
  dst.ric_cpid = rand() + 1; 

  return dst;
}

/////////////////////////////
/////////////////////////////
////////// End of RC Call Process ID 
/////////////////////////////
/////////////////////////////



/////////////////////////////
/////////////////////////////
////////// Start of RC Control Header 
/////////////////////////////
/////////////////////////////

static
e2sm_rc_ctrl_hdr_frmt_1_t fill_rnd_rc_ctrl_hdr_frmat_1(void)
{
  e2sm_rc_ctrl_hdr_frmt_1_t dst = {0}; 

  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = fill_rnd_ue_id();

  // RIC Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2. 
  // INTEGER
  dst.ric_style_type = rand(); 

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1- 65535]
  dst.ctrl_act_id = (rand() % 65535) + 1;

  // RIC Control decision
  // Optional
  dst.ric_ctrl_decision = NULL;

  return dst;
}

static
e2sm_rc_ctrl_hdr_frmt_2_t fill_rnd_rc_ctrl_hdr_frmat_2(void)
{
  e2sm_rc_ctrl_hdr_frmt_2_t dst = {0}; 
 // UE ID
 // Optional
 // 9.3.10
  dst.ue_id = calloc(1, sizeof(ue_id_e2sm_t));
  assert(dst.ue_id != NULL && "Memory exhausted");

  *dst.ue_id = fill_rnd_ue_id();

  // RIC Control decision
  // Optional
  dst.ric_ctrl_dec = NULL;

  return dst;
}

e2sm_rc_ctrl_hdr_t fill_rnd_rc_ctrl_hdr(void)
{
  e2sm_rc_ctrl_hdr_t dst = {0}; 

  dst.format = rand() % END_E2SM_RC_CTRL_HDR;

  if(dst.format == FORMAT_1_E2SM_RC_CTRL_HDR){
    dst.frmt_1 =  fill_rnd_rc_ctrl_hdr_frmat_1();
  } else if(dst.format == FORMAT_2_E2SM_RC_CTRL_HDR){
     dst.frmt_2 = fill_rnd_rc_ctrl_hdr_frmat_2();
  } else {
    assert(0!=0 && "Not implemented");
  }
                                          
  return dst;
}


/////////////////////////////
/////////////////////////////
////////// End of RC Control Header 
/////////////////////////////
/////////////////////////////

/////////////////////////////
/////////////////////////////
////////// Start of RC Control Message 
/////////////////////////////
/////////////////////////////

static
e2sm_rc_ctrl_msg_frmt_1_t fill_rnd_ctrl_msg_frmt_1(void)
{
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0}; 

  // List of RAN parameters
  // [0- 65535]
  dst.sz_ran_param = rand() % 8;
   
  if(dst.sz_ran_param > 0){
    dst.ran_param = calloc(dst.sz_ran_param , sizeof(seq_ran_param_t));
    assert(dst.ran_param != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < dst.sz_ran_param; ++i){
    dst.ran_param[i] = fill_rnd_seq_ran_param();
  }

  return dst;
}

static
seq_ctrl_act_t fill_rnd_seq_ctrl_act(void)
{
  seq_ctrl_act_t dst = {0};

  //Control Action ID
  //Mandatory
  //9.3.6
  // [1 - 65535]
  dst.ctrl_act_id = (rand() %  65535) + 1;

  // Control Action Parameters
  // Optional
  // 9.2.1.7.1 E2SM-RC Control Message Format 1
  dst.ctrl_msg_frmt_1 = calloc(1 , sizeof(e2sm_rc_ctrl_msg_frmt_1_t ));
  assert(dst.ctrl_msg_frmt_1 != NULL && "Memory exhausted" );

  *dst.ctrl_msg_frmt_1 = fill_rnd_ctrl_msg_frmt_1();

  return dst;
}

static
seq_ctrl_sma_t fill_rnd_seq_ctrl_sma(void)
{
  seq_ctrl_sma_t dst = {0};

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.ctrl_style = rand();

  // Sequence of Control Actions
  // [1-63]
  dst.sz_seq_ctrl_act = (rand() % 8) +1;

  dst.seq_ctrl_act = calloc(dst.sz_seq_ctrl_act, sizeof(seq_ctrl_act_t ));
  assert(dst.seq_ctrl_act != NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_seq_ctrl_act; ++i){
    dst.seq_ctrl_act[i] = fill_rnd_seq_ctrl_act();
  }

  return dst;
}


static
e2sm_rc_ctrl_msg_frmt_2_t fill_rnd_ctrl_msg_frmt_2(void)
{
  e2sm_rc_ctrl_msg_frmt_2_t dst = {0}; 

  // Sequence of Control Styles
  // for Multiple Actions
  // [1 - 63]  
  dst.sz_seq_ctrl_sma = (rand() % 8) + 1;

  dst.action = calloc(dst.sz_seq_ctrl_sma, sizeof(seq_ctrl_sma_t));
  assert(dst.action != NULL && "Memory exhausted");

  for(size_t i = 0 ; i < dst.sz_seq_ctrl_sma; ++i){
    dst.action[i] = fill_rnd_seq_ctrl_sma();
  }

  return dst;
}


e2sm_rc_ctrl_msg_t fill_rnd_rc_ctrl_msg(void)
{
  e2sm_rc_ctrl_msg_t dst = {0}; 
  
  dst.format = rand() % END_E2SM_RC_CTRL_MSG;
  
  if(dst.format == FORMAT_1_E2SM_RC_CTRL_MSG){
    dst.frmt_1 = fill_rnd_ctrl_msg_frmt_1();
  } else if (dst.format == FORMAT_2_E2SM_RC_CTRL_MSG){
    dst.frmt_2 = fill_rnd_ctrl_msg_frmt_2();
  } else {
    assert(0 && "Unknown format");
  }

  return dst;
}


/////////////////////////////
/////////////////////////////
////////// End of RC Control Message 
/////////////////////////////
/////////////////////////////




/////////////////////////////
/////////////////////////////
////////// Start of RC Control Outcome 
/////////////////////////////
/////////////////////////////

static
seq_ran_param_2_t fill_rnd_seq_ran_param_2(void)
{
  seq_ran_param_2_t dst = {0}; 

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  dst.ran_param_id = (rand() % 4294967295) + 1; 

  // RAN Parameter Value
  // Mandatory
  // 9.3.14
  dst.ran_param_value = fill_rnd_ran_param_val();
  
  return dst;
}

static
e2sm_rc_ctrl_out_frmt_1_t fill_rnd_ctrl_out_frmt_1(void)
{
  e2sm_rc_ctrl_out_frmt_1_t dst = {0}; 

  // Sequence of RAN
  // Parameters
  // [0 - 255]
  dst.sz_seq_ran_param_2 = rand() % 8;

  if(dst.sz_seq_ran_param_2 > 0 ){
    dst.ran_param = calloc(dst.sz_seq_ran_param_2, sizeof(seq_ran_param_2_t));
    assert(dst.ran_param != NULL && "Memry exhausted" );
  }

  for(size_t i = 0; i < dst.sz_seq_ran_param_2; ++i){
    dst.ran_param[i] = fill_rnd_seq_ran_param_2(); 
  }

  return dst;
}

static
seq_ctrl_act_out_t fill_rnd_seq_ctrl_act_out(void) 
{
  seq_ctrl_act_out_t dst = {0}; 

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1- 65535]
  dst.ctrl_act_id = (rand() % 65535)+1;

  //Sequence of RAN
  //Parameters
  // [1-255]  
  dst.sz_ran_param = (rand() % 8)+1;

  dst.ran_param = calloc(dst.sz_ran_param, sizeof(seq_ran_param_2_t));
  assert(dst.ran_param != NULL && "Memory exhausted" );

  for(size_t i = 0; i < dst.sz_ran_param; ++i){
    dst.ran_param[i] = fill_rnd_seq_ran_param_2();
  }

  return dst;
}


static
seq_ctrl_sty_mul_out_t fill_rnd_seq_ctrl_sty_mul_out(void)
{
  seq_ctrl_sty_mul_out_t dst = {0}; 

  // Indicated Control Style
  // Mandatory
  // 9.3.3
  //  6.2.2.2.
  dst.ind_ctrl_style = rand();

  //Sequence of Control
  //Actions Outcom
  // [1-63]
  dst.sz_seq_ctrl_act_out = (rand()%8)+ 1;

  dst.seq_ctrl_act_out = calloc(dst.sz_seq_ctrl_act_out, sizeof(seq_ctrl_act_out_t));
  assert(dst.seq_ctrl_act_out != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ctrl_act_out; ++i){
    dst.seq_ctrl_act_out[i] = fill_rnd_seq_ctrl_act_out() ;
  }

  return dst;
}


static
e2sm_rc_ctrl_out_frmt_2_t fill_rnd_ctrl_out_frmt_2(void)
{

// Sequence of Control Styles
// for Multiple Outcomes
// [1-63]
  e2sm_rc_ctrl_out_frmt_2_t dst = {0};

  dst.sz_seq_ctrl_sty_mul_out = (rand() % 8 ) +1; 

  dst.seq_ctrl_sty_mul_out = calloc(dst.sz_seq_ctrl_sty_mul_out, sizeof(seq_ctrl_sty_mul_out_t));
  assert(dst.seq_ctrl_sty_mul_out != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ctrl_sty_mul_out; ++i){
    dst.seq_ctrl_sty_mul_out[i] = fill_rnd_seq_ctrl_sty_mul_out();
  }

  return dst;
}

static
e2sm_rc_ctrl_out_frmt_3_t fill_rnd_ctrl_out_frmt_3()
{
  e2sm_rc_ctrl_out_frmt_3_t dst = {0}; 

  //  Sequence of RAN Parameters
  //  [0-255]
  dst.sz_seq_ran_param = (rand() % 8) + 1;
  dst.ran_param = calloc( dst.sz_seq_ran_param , sizeof(seq_ran_param_t));
  assert(dst.ran_param != NULL && "memory exhausted" );

  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.ran_param[i] = fill_rnd_seq_ran_param();
  }

  return dst;
}


e2sm_rc_ctrl_out_t fill_rnd_rc_ctrl_out(void)
{
  e2sm_rc_ctrl_out_t dst = {0}; 

  dst.format = rand() % END_E2SM_RC_CTRL_OUT;
  
  if(dst.format == FORMAT_1_E2SM_RC_CTRL_OUT){
    dst.frmt_1 = fill_rnd_ctrl_out_frmt_1();
  } else if(dst.format == FORMAT_2_E2SM_RC_CTRL_OUT ){
    dst.frmt_2 = fill_rnd_ctrl_out_frmt_2();
  } else if(dst.format == FORMAT_3_E2SM_RC_CTRL_OUT ){
    dst.frmt_3 = fill_rnd_ctrl_out_frmt_3();
  } else {
    assert(0!=0 && "Unknown format");
  }

  return dst;
}

/////////////////////////////
/////////////////////////////
////////// End of RC Control Outcome 
/////////////////////////////
/////////////////////////////

/////////////////////////////
/////////////////////////////
////////// Start RAN Function Definition 
/////////////////////////////
/////////////////////////////

ran_function_name_t fill_rc_ran_func_name(void)
{
  ran_function_name_t dst = {0}; 

    // RAN Function Short Name
    // Mandatory
    // PrintableString [1-150]
    const char name[] = "E2SM-RC";
    dst.name = cp_str_to_ba(name);

    // RAN Function Service Model OID
    // Mandatory
    // PrintableString [1-1000]
    
    //iso(1) identified-organization(3)
    //dod(6) internet(1) private(4)
    //enterprise(1) 53148 e2(1)
    // version1 (1) e2sm(2) e2sm-RC-
    // IEs (3)
    const char oid[] = "1.3.6.1.4.1.53148.1.1.2.3"; 
    dst.oid = cp_str_to_ba(oid);

    // RAN Function Description
    // Mandatory
    // PrintableString [1- 150]
    //RAN function RC “RAN Control” performs the following
    //functionalities:
    //- Exposure of RAN control and UE context related
    //information.
    //- Modification and initiation of RAN control related call
    //processes and messages
    //- Execution of policies that may result in change of
    //RAN control behavior 

    const char description[] = "RAN Control"; 
    dst.description = cp_str_to_ba(description);

    // RAN Function Instance
    // Optional
    // INTEGER
//    long* instance;	/* OPTIONAL: it is suggested to be used when E2 Node declares
//                                multiple RAN Function ID supporting the same  E2SM specification   ask Mikel */

  return dst;
}

static
seq_ev_trg_style_t fill_rnd_seq_ev_trg_style(void)
{
  seq_ev_trg_style_t dst = {0}; 

  // RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  //  INTEGER
  dst.style = rand(); 

  // RIC Event Trigger Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3
  //PrintableString(SIZE(1..150,...))
  const char name[] = "Beatiful"; 
  dst.name = cp_str_to_ba(name);

  // RIC Event Trigger Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.format = rand();

  return dst;
}

static
seq_ran_param_3_t fill_rnd_seq_ran_param_3(void)
{
  seq_ran_param_3_t dst = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1- 4294967295]
  dst.id = rand() % 4294967295 + 1;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  // [1-150] 
  char name[] = "FooBar42";
  dst.name = cp_str_to_ba(name);

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  dst.def = NULL;

  return dst;
}

static
call_proc_break_t fill_rnd_call_proc_break(void)
{
call_proc_break_t dst = {0};

  // Call Process Breakpoint ID
  // Mandatory
  // 9.3.49
  // [1 - 65535]
  dst.id = rand() + 1;

  // Call Process Breakpoint Name
  // Mandatory
  // 9.3.50
  // [1-150]
  const char name[] = "Call Process Breakpoint Name"; 
  dst.name = cp_str_to_ba(name);

  // Sequence of Associated RAN Parameters
  // [0-65535]
  dst.sz_param = rand() % 4;// 65536;
  if(dst.sz_param > 0){
    dst.param = calloc(dst.sz_param , sizeof(seq_ran_param_3_t));
    assert(dst.param != NULL && "Memory exhausted");
  } 
  for(size_t i = 0; i < dst.sz_param; ++i){
    dst.param[i] = fill_rnd_seq_ran_param_3();
  }

  return dst;
}



static
seq_call_proc_type_t fill_rnd_seq_call_proc_type(void)
{
seq_call_proc_type_t dst = {0}; 

  // Call Process Type ID
  // Mandatory
  // 9.3.15
  // [1- 65535]
  dst.id = rand() + 1;

  // Call Process Type Name
  // Mandatory
  // 9.3.19
  // [1-150]
  const char name[] = "Call Process Type Name";
  dst.name = cp_str_to_ba(name);

  // Sequence of Call Process Breakpoints
  // [1-65535]
  dst.sz_call_proc_break = (rand()%4) + 1 ;//  (rand()%65536) + 1; 
  if(dst.sz_call_proc_break > 0){
    dst.call_proc_break = calloc(dst.sz_call_proc_break, sizeof( call_proc_break_t )  ); 
    assert(dst.call_proc_break != NULL && "Memory exhausted");
  } 
  for(size_t i = 0; i < dst.sz_call_proc_break; ++i){
    dst. call_proc_break[i] = fill_rnd_call_proc_break();
  }

  return dst;
}

ran_func_def_ev_trig_t fill_ran_func_def_ev_trig(void)
{
  ran_func_def_ev_trig_t dst = {0}; 

  // Sequence of EVENT TRIGGER styles
  // [1 - 63]
  dst.sz_seq_ev_trg_style = (rand()% 4) + 1; // (rand()% 63) + 1;

  dst.seq_ev_trg_style = calloc(dst.sz_seq_ev_trg_style, sizeof(seq_ev_trg_style_t) );
  assert(dst.seq_ev_trg_style != NULL && "memory exhausted");

  for(size_t i = 0; i < dst. sz_seq_ev_trg_style; ++i){
    dst.seq_ev_trg_style[i] = fill_rnd_seq_ev_trg_style();
  }
  
  // Sequence of RAN Parameters for L2 Variables
  // [0 - 65535]  
  dst.sz_seq_ran_param_l2_var = rand() % 4; // % 65536;
  if(dst.sz_seq_ran_param_l2_var > 0 ){
    dst.seq_ran_param_l2_var = calloc(dst.sz_seq_ran_param_l2_var, sizeof(seq_ran_param_3_t));
    assert(dst.seq_ran_param_l2_var != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_ran_param_l2_var; ++i){
    dst.seq_ran_param_l2_var[i] = fill_rnd_seq_ran_param_3();
  }

  //Sequence of Call Process Types
  // [0-65535]
  dst.sz_seq_call_proc_type = rand() % 4; // % 65535; 
  if(dst.sz_seq_call_proc_type > 0){
    dst.seq_call_proc_type = calloc(dst.sz_seq_call_proc_type, sizeof(seq_call_proc_type_t));
    assert(dst.seq_call_proc_type != NULL && "Memory exhausted" );
  }
  for(size_t i = 0; i < dst.sz_seq_call_proc_type; ++i){
    dst.seq_call_proc_type[i] = fill_rnd_seq_call_proc_type(); 
  }

  // Sequence of RAN Parameters for Identifying UEs
  // 0-65535
  dst.sz_seq_ran_param_id_ue = rand() % 4; // 
  if(dst.sz_seq_ran_param_id_ue > 0){
    dst.seq_ran_param_id_ue = calloc(dst.sz_seq_ran_param_id_ue, sizeof(seq_ran_param_3_t));
    assert(dst.seq_ran_param_id_ue != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_ran_param_id_ue; ++i){
    dst.seq_ran_param_id_ue[i] = fill_rnd_seq_ran_param_3();
  }

  // Sequence of RAN Parameters for Identifying Cells
  // 0-65535
  dst.sz_seq_ran_param_id_cell = rand() % 4; 
  if(dst.sz_seq_ran_param_id_cell > 0){
    dst.seq_ran_param_id_cell = calloc(dst.sz_seq_ran_param_id_cell, sizeof(seq_ran_param_3_t) );
    assert(dst.seq_ran_param_id_cell != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst. sz_seq_ran_param_id_cell; ++i){
    dst.seq_ran_param_id_cell[i] = fill_rnd_seq_ran_param_3();
  }

  return dst;
}

static
seq_report_sty_t fill_rnd_seq_report_sty(void)
{
  seq_report_sty_t dst = {0};

  // RIC Report Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.report_type = rand();

  // RIC Report Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // PrintableString(SIZE(1..150,...)) 
  const char name[] = "Claude Shannon"; 
  dst.name = cp_str_to_ba(name); 

  // Supported RIC Event Trigger Style Type 
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.ev_trig_type = rand();

  // RIC Report Action Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.act_frmt_type = rand();

  // RIC Indication Header Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.ind_hdr_type = rand();

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  dst.ind_msg_type = rand();

  // Sequence of RAN Parameters Supported
  // [0 - 65535]
  dst.sz_seq_ran_param = rand()%8;
 
  if(dst.sz_seq_ran_param > 0){
    dst.ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_3_t));
    assert(dst.ran_param != NULL && "Memory exhausted");
    for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
      dst.ran_param[i] = fill_rnd_seq_ran_param_3();
    }
  }

  return dst;
}

ran_func_def_report_t fill_ran_func_def_report(void)
{
  ran_func_def_report_t dst = {0};

  // Sequence of REPORT styles
  // [1 - 63]
  dst.sz_seq_report_sty = (rand()%4) + 1;
  dst.seq_report_sty = calloc(dst.sz_seq_report_sty, sizeof(seq_report_sty_t)); 
  assert(dst.seq_report_sty != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_report_sty; ++i){
    dst.seq_report_sty[i] = fill_rnd_seq_report_sty();
  }

  return dst;
}

static
seq_ins_ind_t fill_rnd_seq_ins_ind(void)
{
  seq_ins_ind_t dst = {0}; 

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1-65535]
  dst.id = (rand()% 65535) + 1;

  // Insert Indication Name
  // Mandatory
  // 9.3.17
  // [1-150]
  const char name[] = "Robert Gallager";
  dst.name = cp_str_to_ba(name);

  // Sequence of Insert Indications
  // [0-65535]
  dst.sz_seq_ins_ind = rand()%4 ; // 65536
  if(dst.sz_seq_ins_ind > 0){
    dst.seq_ins_ind = calloc(dst.sz_seq_ins_ind, sizeof(seq_ran_param_3_t) );
    assert(dst.seq_ins_ind != NULL && "Memory exhausted");
    for(size_t i = 0; i < dst.sz_seq_ins_ind; ++i){
      dst.seq_ins_ind[i] = fill_rnd_seq_ran_param_3();
    }
  }

  return dst;
}

static
seq_ins_sty_t fill_rnd_seq_ins_sty(void)
{
  seq_ins_sty_t dst = {0}; 

  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  dst.style_type = rand();

  // RIC Insert Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // [1-150]
  const char name[] = "Bob Fanno"; 
  dst.name = cp_str_to_ba(name);

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  dst.ev_trig_style_type = rand();

  // RIC Action Definition Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  dst.act_def_frmt_type = rand();

  // Sequence of Insert Indications
  // [0-65535]
  dst.sz_seq_ins_ind = rand() % 4; //65536
  if(dst.sz_seq_ins_ind > 0){
    dst.seq_ins_ind = calloc(dst.sz_seq_ins_ind, sizeof(seq_ins_ind_t));
    assert(dst.seq_ins_ind != NULL && "Memory exhausted");
    for(size_t i = 0; i < dst.sz_seq_ins_ind; ++i){
      dst.seq_ins_ind[i] = fill_rnd_seq_ins_ind();
    }
  }                                   

  // RIC Indication Header Format Type
  // Mandatoyr
  // 9.3.5
  // 6.2.2.4.
  dst.ind_hdr_frmt_type= rand();

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  dst.ind_msg_frmt_type = rand();

  // RIC Call Process ID Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  dst.call_proc_id_type = rand();

  return dst;
}

ran_func_def_insert_t fill_ran_func_def_insert(void)
{
  ran_func_def_insert_t dst = {0};

  // Sequence of INSERT styles
  // [1-63]
  dst.sz_seq_ins_sty = (rand()% 4) + 1; // (rand()%63) + 1;

  dst.seq_ins_sty = calloc(dst.sz_seq_ins_sty, sizeof(seq_ins_sty_t));
  assert(dst.seq_ins_sty != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_seq_ins_sty; ++i){
    dst.seq_ins_sty[i] = fill_rnd_seq_ins_sty();
  }

  return dst;
}

static
seq_ctrl_act_2_t fill_rnd_ctrl_act(void)
{
  seq_ctrl_act_2_t dst = {0}; 

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1-65535]
  dst.id = (rand()%65535) +1; 

  // Control Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  const char name[] = "test string";
  dst.name = cp_str_to_ba(name);

  // Sequence of Associated RAN Parameters
  // [0-65535]
  dst.sz_seq_assoc_ran_param = rand()%4;
  if(dst.sz_seq_assoc_ran_param > 0){
    dst.assoc_ran_param = calloc(dst.sz_seq_assoc_ran_param, sizeof(seq_ran_param_3_t) );
    assert(dst.assoc_ran_param != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_assoc_ran_param; ++i){
    dst.assoc_ran_param[i] = fill_rnd_seq_ran_param_3();
  }

  return dst;
}

static
seq_ctrl_style_t fill_rnd_seq_ctrl_style(void)
{
  seq_ctrl_style_t dst = {0}; 

  // RIC Control Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  dst.style_type = rand();

  //RIC Control Style Name
  //Mandatory
  //9.3.4
  // [1 -150]
  const char name[] = "Bjarne Stroustroup"; 
  dst.name = cp_str_to_ba(name);

  // Sequence of Control Actions
  // [0-65535]
  dst.sz_seq_ctrl_act = rand() % 4;
  if(dst.sz_seq_ctrl_act > 0){
    dst.seq_ctrl_act = calloc(dst.sz_seq_ctrl_act, sizeof(seq_ctrl_act_2_t ) ); 
    assert(dst.seq_ctrl_act != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_ctrl_act; ++i){
    dst.seq_ctrl_act[i] = fill_rnd_ctrl_act( ); 
  }

  // RIC Control Header Format Type
  // Mandatory
  // 9.3.5
  dst.hdr = rand();

  // RIC Control Message Format Type
  // Mandatory
  // 9.3.5
  dst.msg = rand();

  // RIC Call Process ID Format Type
  // Optional

  // RIC Control Outcome Format Type
  // Mandatory
  // 9.3.5
  dst.out_frmt = rand();

  // Sequence of Associated RAN 
  // Parameters for Control Outcome
  // [0- 255]
  dst.sz_ran_param_ctrl_out = 1; // rand()%4;
  if(dst.sz_ran_param_ctrl_out > 0){
    dst.ran_param_ctrl_out = calloc(dst.sz_ran_param_ctrl_out, sizeof(seq_ran_param_3_t) );
    assert(dst.ran_param_ctrl_out != NULL && "Memory exhausted");
  } 
  for(size_t i = 0; i < dst.sz_ran_param_ctrl_out; ++i){
    dst.ran_param_ctrl_out[i] = fill_rnd_seq_ran_param_3();  
  }

  return dst;
}

ran_func_def_ctrl_t fill_ran_func_def_ctrl(void)
{
  ran_func_def_ctrl_t dst = {0};

  // Sequence of CONTROL styles
  // [1 - 63]
  dst.sz_seq_ctrl_style = (rand()%4) + 1;
  dst.seq_ctrl_style = calloc(dst.sz_seq_ctrl_style, sizeof(seq_ctrl_style_t) );
  assert(dst.seq_ctrl_style != NULL && "Memory exhausted");
  for(size_t i = 0; i < dst.sz_seq_ctrl_style; ++i){
    dst.seq_ctrl_style[i] = fill_rnd_seq_ctrl_style();
  }

  return dst;
}

static
seq_pol_action_t fill_rnd_seq_pol_action(void)
{
  seq_pol_action_t dst = {0};
  // Policy Action ID
  // Mandatory
  // 9.3.6
  dst.action_id = rand(); 

  // Policy Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  const char name[] = "Andrei Alexandrescu";
  dst.name = cp_str_to_ba(name);

  // RIC Action Definition Format Type
  // Mandatoyr
  // 9.3.5
  dst.frmt_type = rand();

  // Sequence of Associated RAN Parameters for Policy Action
  // [0- 65535]
  dst.sz_seq_assoc_rp_action = rand()%4;
  if(dst.sz_seq_assoc_rp_action > 0){
    dst.seq_assoc_rp_action = calloc(dst.sz_seq_assoc_rp_action, sizeof(seq_ran_param_3_t));
    assert(dst.seq_assoc_rp_action != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_assoc_rp_action; ++i){
    dst.seq_assoc_rp_action[i] = fill_rnd_seq_ran_param_3(); 
  }

  // Sequence of Associated RAN Parameters for Policy Condition
  // [0- 65535]
  dst.sz_seq_assoc_rp_policy = rand()%4;
  if(dst.sz_seq_assoc_rp_policy > 0){
    dst.seq_assoc_rp_policy = calloc(dst.sz_seq_assoc_rp_policy, sizeof(seq_ran_param_3_t));
    assert(dst.seq_assoc_rp_policy != NULL && "Memory exhausted");
  } 
  for(size_t i = 0; i < dst.sz_seq_assoc_rp_policy; ++i){
    dst.seq_assoc_rp_policy[i] = fill_rnd_seq_ran_param_3(); 
  }

  return dst;
}

static
seq_policy_sty_t fill_rnd_seq_policy_sty(void)
{
  seq_policy_sty_t dst = {0}; 
  // RIC Policy Style Type
  // Mandatory
  // 9.3.3
  dst.style_type = rand();

  // RIC Policy Style Name
  // Mandatory
  // 9.3.4
  const char name[] = "Sean Parent";
  dst.name = cp_str_to_ba(name);

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  dst.ev_trg_style_type = rand();

  // Sequence of Policy Actions
  // [0-65535]
  dst.sz_seq_pol_action = rand() % 4;
  if(dst.sz_seq_pol_action > 0){
    dst.seq_pol_action = calloc(dst.sz_seq_pol_action, sizeof(seq_pol_action_t));
    assert(dst.seq_pol_action != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_seq_pol_action; ++i){
    dst.seq_pol_action[i] = fill_rnd_seq_pol_action();
  }

  return dst;
}

ran_func_def_policy_t fill_ran_func_def_policy(void)
{
  ran_func_def_policy_t dst = {0}; 

  // Sequence of POLICY styles
  // [1-63]
  dst.sz_policy_styles = (rand() % 4) + 1;
  dst.seq_policy_sty = calloc(dst.sz_policy_styles, sizeof(seq_policy_sty_t ) );
  assert(dst.seq_policy_sty != NULL && "Memory exhasuted");

  for(size_t i = 0; i < dst.sz_policy_styles; ++i){
    dst.seq_policy_sty[i] = fill_rnd_seq_policy_sty();
  }

  return dst;
}

e2sm_rc_func_def_t fill_rnd_rc_ran_func_def(void)
{
  e2sm_rc_func_def_t dst = {0}; 

  //  RAN Function Name
  //  Mandatory
  //  9.3.2
  //  6.2.2.1.
  dst.name = fill_rc_ran_func_name();  

  // RAN Function Definition for EVENT TRIGGER
  // Optional
  // 9.2.2.2
  dst.ev_trig = calloc(1, sizeof(ran_func_def_ev_trig_t)); 
  assert(dst.ev_trig != NULL && "Memory exhausted");
  *dst.ev_trig = fill_ran_func_def_ev_trig();

  // RAN Function Definition for REPORT
  // Optional
  // 9.2.2.3
  dst.report = calloc(1, sizeof( ran_func_def_report_t ));
  assert(dst.report != NULL && "Memory exhausted");
  *dst.report = fill_ran_func_def_report();

  // RAN Function Definition for INSERT
  // Optional
  // 9.2.2.4
  dst.insert = calloc(1, sizeof(ran_func_def_insert_t)); 
  assert(dst.insert != NULL && "Memory exhausted");
  *dst.insert = fill_ran_func_def_insert();

  // RAN Function Definition for CONTROL
  // Optional
  // 9.2.2.5
  dst.ctrl = calloc(1, sizeof( ran_func_def_ctrl_t)); 
  assert(dst.ctrl != NULL && "Memory exhausted");
  *dst.ctrl = fill_ran_func_def_ctrl();

  // RAN Function Definition for POLICY
  // Optional
  // 9.2.2.6
  dst.policy = calloc(1, sizeof(ran_func_def_policy_t)); 
  assert(dst.policy != NULL && "Memory exhausted");
  *dst.policy = fill_ran_func_def_policy();

  return dst;
}

/////////////////////////////
/////////////////////////////
////////// End of RAN Function Definition 
/////////////////////////////
/////////////////////////////

