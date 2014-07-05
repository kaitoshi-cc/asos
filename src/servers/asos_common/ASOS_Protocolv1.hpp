#ifndef ASOS_PROTOCOLV1_HPP
#define ASOS_PROTOCOLV1_HPP

#define ASOSP_FIELD  0x00
#define ASOSP_OBJECT 0x01

class ASOS_Protocolv1_info{
public:
  ASOS_Protocolv1_info(signed char in_flag_object_state, 
		       signed char in_flag_model_revision, 
		       signed char in_flag_response_state, 
		       signed char in_flag_model_data, 
		       signed char in_flag_message,
		       signed char in_flag_key,
		       signed char in_flag_keys_and_node_ids,		       
		       signed char in_target,
		       signed char in_flag_accepted_by_basic_server,
		       signed char flag_permit_owner_only
		       );
  ~ASOS_Protocolv1_info();
  
  // Payload info
  signed char flag_object_state;    //  0x00,       0x82
  signed char flag_model_revision;  //    0x01,       0x83, 0x04, 0x85, 0x06, 0x87                               / 0x0d, 0x84
  signed char flag_response_state;  //              0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c / 0x8e 0x8f
  signed char flag_model_data;      // (      0x01,       0x83, 0x04            )
  signed char flag_message;         //                                0x85, 0x06                                     / 0x0d

  signed char flag_key;               // 0x03, 0x06, 0x09, 0x0c
  signed char flag_keys_and_node_ids; // 0x01, 0x83, 0x04

  // target info
  signed char target;   // 0x00: field, 0x01: object

  // server type info
  signed char flag_accepted_by_basic_server;  // 0x00: Drop, 0x01:Accept

  // target info
  signed char flag_permit_owner_only;   // 0x00: faise, 0x01: true

};

class ASOS_Protocolv1{
public:
  ASOS_Protocolv1();
  ~ASOS_Protocolv1();
  ASOS_Protocolv1_info* info[256];
};

#endif


