#ifndef ASOS_MESSAGE_HPP
#define ASOS_MESSAGE_HPP

class ASOS_message{
public:
  ASOS_message();
  ~ASOS_message();

  unsigned char message_type;
  unsigned char wait_time_for_response;
  unsigned char registration_lifetime;
  unsigned char persistency_flag;
  unsigned char lifetime_overwrite_flag;
  
  unsigned char message_identification[2];

  unsigned char object_field_identification_length;
  unsigned char object_identification_length;

  char *object_field_identification;
  char *object_identification;

  int payload_size;
  unsigned char *payload;

};

#endif

