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
		       unsigned char in_target,
		       signed char in_flag_accepted_by_basic_server
		       );
  ~ASOS_Protocolv1_info();
  
  // Payload info
  signed char flag_object_state;    //  0x00,       0x82
  signed char flag_model_revision;  //    0x01,       0x83, 0x04, 0x85, 0x06, 0x87                               / 0x0d, 0x84
  signed char flag_response_state;  //              0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c / 0x8e 0x8f
  signed char flag_model_data;      // (      0x01,       0x83, 0x04            )
  signed char flag_message;         //                                0x85, 0x06                                     / 0x0d

  // target info
  unsigned char target;   // 0x00: field, 0x01: object

  // server type info
  unsigned char flag_accepted_by_basic_server;  // 0x00: Drop, 0x01:Accept

};

class ASOS_Protocolv1{
public:
  ASOS_Protocolv1();
  ~ASOS_Protocolv1();
  ASOS_Protocolv1_info* info[256];
};

#endif


