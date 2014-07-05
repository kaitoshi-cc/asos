#include "ASOS_Protocolv1.hpp"
#include <stdio.h>

ASOS_Protocolv1_info::ASOS_Protocolv1_info(signed char in_flag_object_state, 
					   signed char in_flag_model_revision, 
					   signed char in_flag_response_state, 
					   signed char in_flag_model_data, 
					   signed char in_flag_message,
					   signed char in_flag_key,
					   signed char in_flag_keys_and_node_ids,		       
					   signed char in_target,
					   signed char in_flag_accepted_by_basic_server,
					   signed char in_flag_permit_owner_only
					   ){
  flag_object_state      = in_flag_object_state;
  flag_model_revision    = in_flag_model_revision;
  flag_response_state    = in_flag_response_state;
  flag_model_data        = in_flag_model_data;
  flag_message           = in_flag_message;
  flag_key               = in_flag_key;
  flag_keys_and_node_ids = in_flag_keys_and_node_ids;

  target = in_target;
  flag_accepted_by_basic_server = in_flag_accepted_by_basic_server;
  flag_permit_owner_only = in_flag_permit_owner_only;
}

ASOS_Protocolv1_info::~ASOS_Protocolv1_info(){
}

ASOS_Protocolv1::ASOS_Protocolv1(){
  int i;
  unsigned char t;

  for(i = 0 ; i < 256; i++){info[i] = NULL;}

  t = 0x01;  info[t] = new ASOS_Protocolv1_info(1,1,0,1,0,0,1, ASOSP_OBJECT, 0, 1);  // 0x01: "model publish"

  t = 0x03;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,1,0, ASOSP_OBJECT, 1, 0);  // 0x03: "browse model command"
  t = 0x83;  info[t] = new ASOS_Protocolv1_info(1,1,1,1,0,0,1, ASOSP_OBJECT, 0, 0);  // 0x83: "browse model response"

  t = 0x04;  info[t] = new ASOS_Protocolv1_info(0,1,0,1,0,0,1, ASOSP_OBJECT, 1, 1);  // 0x04: "update model command"
  t = 0x84;  info[t] = new ASOS_Protocolv1_info(0,1,1,0,0,0,0, ASOSP_OBJECT, 0, 0);  // 0x84: "update model response"

  t = 0x05;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,0,0, ASOSP_OBJECT, 1, 1);  // 0x05: "pop message command"
  t = 0x85;  info[t] = new ASOS_Protocolv1_info(0,1,1,0,1,0,0, ASOSP_OBJECT, 0, 0);  // 0x85: "pop message response"

  t = 0x06;  info[t] = new ASOS_Protocolv1_info(0,1,0,0,1,1,0, ASOSP_OBJECT, 1, 0);  // 0x06: "push message command"
  t = 0x86;  info[t] = new ASOS_Protocolv1_info(0,0,1,0,0,0,0, ASOSP_OBJECT, 0, 0);  // 0x86: "push message response"

  t = 0x07;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,0,0, ASOSP_FIELD,  1, 1);  // 0x07: "create object command"
  t = 0x87;  info[t] = new ASOS_Protocolv1_info(0,1,1,0,0,0,0, ASOSP_FIELD,  0, 0);  // 0x87: "create object response"

  t = 0x09;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,1,0, ASOSP_OBJECT, 1, 0);  // 0x09: "register model subscription command"
  t = 0x89;  info[t] = new ASOS_Protocolv1_info(0,0,1,0,0,0,0, ASOSP_OBJECT, 0, 0);  // 0x89: "register model subscription response"

  t = 0x0a;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,0,0, ASOSP_FIELD,  1, 1);  // 0x0a: "delete object command"
  t = 0x8a;  info[t] = new ASOS_Protocolv1_info(0,0,1,0,0,0,0, ASOSP_FIELD,  0, 0);  // 0x8a: "delete object response"

  t = 0x0c;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,1,0, ASOSP_OBJECT, 1, 0);  // 0x0c: "cancel model subscription command"
  t = 0x8c;  info[t] = new ASOS_Protocolv1_info(0,0,1,0,0,0,0, ASOSP_OBJECT, 0, 0);  // 0x8c: "cancel model subscription response"

  t = 0x0d;  info[t] = new ASOS_Protocolv1_info(0,1,0,0,1,0,0, ASOSP_OBJECT, 0, 0);  // 0x0d: "captured message"

  t = 0x0e;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,0,0, ASOSP_OBJECT, 1, 0);  // 0x0e: "register message capture command"
  t = 0x8e;  info[t] = new ASOS_Protocolv1_info(0,0,1,0,0,0,0, ASOSP_OBJECT, 0, 0);  // 0x8e: "register message capture response"

  t = 0x0f;  info[t] = new ASOS_Protocolv1_info(0,0,0,0,0,0,0, ASOSP_OBJECT, 1, 0);  // 0x0f: "cancel message capture command"
  t = 0x8f;  info[t] = new ASOS_Protocolv1_info(0,0,1,0,0,0,0, ASOSP_OBJECT, 0, 0);  // 0x8f: "cancel message capture response"


}

ASOS_Protocolv1::~ASOS_Protocolv1(){
  int i;
  for(i = 0 ; i < 256; i++){
    if(info[i] != NULL){
      delete info[i];
      info[i] = NULL;
    }
  }
}

