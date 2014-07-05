#ifndef ASOS_MESSAGE_HPP
#define ASOS_MESSAGE_HPP

#include "ASOS_Protocolv1.hpp"

#define ASOS_OBJECT_KEY_MAX_COUNT 32
#define ASOS_OBJECT_NODE_ID_MAX_COUNT 32

class ASOS_message{
public:
  ASOS_message();
  ~ASOS_message();

  int is_own;

  // Stack header
  unsigned char protocol_type;  // 0x01: ASOSPv1
  
  // ASOSPv1 
  // -- header
  unsigned char message_type;
  unsigned char wait_time_for_response;
  unsigned char registration_lifetime;
  unsigned char lifetime_overwrite_flag;
  unsigned char private_flag;
  
  unsigned char message_identification[2];

  unsigned char object_field_identification_length;
  unsigned char object_identification_length;

  const unsigned char *object_field_identification;
  const unsigned char *object_field_extension;
  const unsigned char *object_identification;

  int payload_size;
  const unsigned char *payload;

  // -- payload
  unsigned char object_state;
  signed long long int model_revision;
  unsigned char response_state;

  unsigned char key_count;
  unsigned char node_id_count;
  unsigned char key_list[ASOS_OBJECT_KEY_MAX_COUNT][16];
  unsigned char node_id_list[ASOS_OBJECT_NODE_ID_MAX_COUNT][16];

  int model_data_size;             
  const unsigned char* model_data; 

  int message_size;                
  const unsigned char* message;    

  // -- for responce 
  int long_polling_flag;

  // ------------------------------------------------------
  static long long int get_revision_from_net(const unsigned char *buff);
  static void set_revision_to_net(unsigned char *buff, long long int rev);
  const char *message_type_string();
  const char *response_state_string();
  const char *object_state_string();

  void print(ASOS_Protocolv1_info *pinfo);
  void copy(ASOS_message *src);
  void ModifyToResponse();

};

#endif

