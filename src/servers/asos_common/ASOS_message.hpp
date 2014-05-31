#ifndef ASOS_MESSAGE_HPP
#define ASOS_MESSAGE_HPP

class ASOS_message{
public:
  ASOS_message();
  ~ASOS_message();

  // Stack header
  unsigned char protocol_type;  // 0x01: ASOSPv1
  
  // ASOSPv1 
  // -- header
  unsigned char message_type;
  unsigned char wait_time_for_response;
  unsigned char registration_lifetime;
  unsigned char lifetime_overwrite_flag;
  
  unsigned char message_identification[2];

  unsigned char object_field_identification_length;
  unsigned char object_identification_length;

  const unsigned char *object_field_identification;
  const unsigned char *object_identification;

  int payload_size;
  const unsigned char *payload;

  // -- payload

  // none                          //              0x02, 0x03        0x05,       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c / 0x0e 0x0f

  unsigned char object_state;      //  0x00,       0x82
  signed long long int model_revision; //    0x01,       0x83, 0x04, 0x85, 0x06, 0x87                               / 0x0d, 0x84
  unsigned char response_state;    //              0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c / 0x8e 0x8f

  int model_data_size;             // (      0x01,       0x83, 0x04            )
  const unsigned char* model_data; //        0x01,       0x83, 0x04  

  int message_size;                // (                              0x85, 0x06                                     / 0x0d)
  const unsigned char* message;    //                                0x85, 0x06                                     / 0x0d


  // ------------------------------------------------------
  static long long int get_revision_from_net(const unsigned char *buff);
  static void set_revision_to_net(unsigned char *buff, long long int rev);
  const char *message_type_string();
  const char *response_state_string();
  const char *object_state_string();

  void print();
  void copy(ASOS_message *src);
  void ModifyToResponse();

};

#endif

