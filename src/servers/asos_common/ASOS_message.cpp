#include <stdio.h>
#include <string.h>
#include "ASOS_message.hpp"

ASOS_message::ASOS_message(){
  object_field_identification = NULL;
  object_identification = NULL; 
  payload = NULL;
  model_data = NULL;
  message = NULL;
}

ASOS_message::~ASOS_message(){
}

const char *ASOS_message::message_type_string(){
  switch(message_type){
  case 0x00:  return "object heart beat";
  case 0x01:  return "model publish";
  case 0x02:  return "ping object command";
  case 0x82:  return "ping object response";
  case 0x03:  return "browse model command";
  case 0x83:  return "browse model response";
  case 0x04:  return "update model command";
  case 0x84:  return "update model response";
  case 0x05:  return "pop message command";
  case 0x85:  return "pop message response";
  case 0x06:  return "push message command";
  case 0x86:  return "push message response";
  case 0x07:  return "create object command";
  case 0x87:  return "create object response";
  case 0x08:  return "register object heartbeat command";
  case 0x88:  return "register object heartbeat response";
  case 0x09:  return "register model subscription command";
  case 0x89:  return "register model subscription response";
  case 0x0a:  return "delete object command";
  case 0x8a:  return "delete object response";
  case 0x0b:  return "cancel object heartbeat command";
  case 0x8b:  return "cancel object heartbeat response";
  case 0x0c:  return "cancel model subscription command";
  case 0x8c:  return "cancel model subscription response";
  default:    return "(unknown)";
  }
}

const char *ASOS_message::response_state_string(){
  switch(response_state){
  case 0x00:  return "success";
  case 0x01:  return "field or object not found";
  case 0x02:  return "timeout";
  case 0x03:  return "access denied";
  case 0x04:  return "access limitation over";
  case 0x05:  return "same command already done";
  case 0x80:  return "previous model revision is too old";
  case 0x81:  return "previous model revision is invalid (future revision)";
  case 0x82:  return "model size is too big";
  case 0x83:  return "target model revision is too old";
  case 0x84:  return "target model revision is invalid (future revision)";
  case 0x85:  return "message size is too big";
  case 0x86:  return "queue is full";
  default:    return "(unknown)";
  }
}

const char *ASOS_message::object_state_string(){
  switch(object_state){
  case 0x00:  return "object not exist";
  case 0x01:  return "object exists without connected producer";
  case 0x02:  return "object exists with connected producer";
  default:    return "(unknown)";
  }
}


void ASOS_message::print(){
  int i;
  printf("message type            : (0x%02x) %s\n", message_type, message_type_string() );
  printf("wait time for response  : %d [sec]\n"   , wait_time_for_response );
  printf("registration lifetime   : %d [min]\n"   , registration_lifetime );
  printf("lifetime overwrite flag : %s\n"         , (lifetime_overwrite_flag==0x1)?"true":"false" );
  printf("message identification  : %02x%02x\n"   , message_identification[0], message_identification[1] );
  printf("object field id length  : %d\n"         , object_field_identification_length );
  printf("object id length        : %d\n"         , object_identification_length );
  printf("payload length          : %d\n"         , payload_size );

  printf("object field id         : ");
  if(object_field_identification != NULL && object_field_identification_length > 0){
    for(i=0; i<object_field_identification_length; i++){
      printf("%c", object_field_identification[i]);
    }
    printf("\n");
  }else{
    printf("(none)\n");
  }

  printf("object id               : ");
  if(object_identification != NULL && object_identification_length > 0){
    for(i=0; i<object_identification_length; i++){
      printf("%c", object_identification[i]);
    }
    printf("\n");
  }else{
    printf("(none)\n");
  }

  printf("payload                 : ");
  if(payload != NULL && payload_size > 0){
    for(i=0; i<payload_size; i++){
      printf("%02x", payload[i]);
    }
    printf("\n");
  }else{
    printf("(none)\n");
  }

  // ---------------------------------------------------
  // print payload 

  if(message_type == 0x02 || message_type == 0x03 || message_type == 0x05
     || message_type == 0x07 || message_type == 0x08 || message_type == 0x09
     || message_type == 0x0a || message_type == 0x0b  || message_type == 0x0c ){
    printf("  [none]\n"); return;
  }

  if(message_type == 0x82 || message_type == 0x83 || message_type == 0x84 || message_type == 0x85 || message_type == 0x86
     || message_type == 0x87 || message_type == 0x88 || message_type == 0x89 
     || message_type == 0x8a || message_type == 0x8b  || message_type == 0x8c ){
    printf("  [response state]     (%02x) %s\n", response_state, response_state_string() ); 
  }

  if(message_type == 0x00 || message_type == 0x82 ){
    printf("  [object state]       (%02x) %s\n", object_state, object_state_string() ); 
  }

  if(message_type == 0x01 || message_type == 0x83 || message_type == 0x04 || message_type == 0x85 || message_type == 0x06
     || message_type == 0x87 ){
    printf("  [model revosion]     %lld\n", model_revision); 
  }
  
  if(message_type == 0x01 || message_type == 0x83 || message_type == 0x04 ){
    printf("  [model data (%d)]    ", model_data_size);
    for(i=0; i<model_data_size; i++) printf("%c", model_data[i]);
    printf("\n");
  }

  if(message_type == 0x85 || message_type == 0x06 ){
    printf("  [message (%d)]       ", message_size);
    for(i=0; i<message_size; i++) printf("%c", message[i]);
    printf("\n");
  }

}

long long int ASOS_message::get_revision_from_net(const unsigned char *buff){
  long long int ret = 0;

  ret  = ((long long int)buff[0]) * 0x0100000000000000 ;
  ret += ((long long int)buff[1]) * 0x0001000000000000 ;
  ret += ((long long int)buff[2]) * 0x0000010000000000 ;
  ret += ((long long int)buff[3]) * 0x0000000100000000 ;
  ret += ((long long int)buff[4]) * 0x0000000001000000 ;
  ret += ((long long int)buff[5]) * 0x0000000000010000 ;
  ret += ((long long int)buff[6]) * 0x0000000000000100 ;
  ret += ((long long int)buff[7]) * 0x0000000000000001 ;

  return ret;
}

void ASOS_message::set_revision_to_net(unsigned char *buff, long long int rev){
  buff[0] = ( rev / 0x0100000000000000 ) % 0x100;  
  buff[1] = ( rev / 0x0001000000000000 ) % 0x100;  
  buff[2] = ( rev / 0x0000010000000000 ) % 0x100;  
  buff[3] = ( rev / 0x0000000100000000 ) % 0x100;  
  buff[4] = ( rev / 0x0000000001000000 ) % 0x100;  
  buff[5] = ( rev / 0x0000000000010000 ) % 0x100;  
  buff[6] = ( rev / 0x0000000000000100 ) % 0x100;  
  buff[7] = ( rev / 0x0000000000000001 ) % 0x100;  
}

void ASOS_message::copy(ASOS_message *src){
  memcpy(this, src, sizeof(ASOS_message));

  /*
  message_type                        = src->message_type;
  wait_time_for_response              = src->wait_time_for_response;
  registration_lifetime               = src->registration_lifetime;
  lifetime_overwrite_flag             = src->lifetime_overwrite_flag;
  message_identification[0]           = src->message_identification[0];
  message_identification[1]           = src->message_identification[1];
  object_field_identification_length  = src->object_field_identification_length;
  object_identification_length        = src->object_identification_length;
  object_field_identification         = src->object_field_identification;
  object_identification               = src->object_identification;
  payload_size                        = src->payload_size;
  payload                             = src->payload;
  object_state                        = src->object_state;
  model_revision                      = src->model_revision;
  response_state                      = src->response_state;
  model_data_size                     = src->model_data_size;
  model_data                          = src->model_data;
  message_size                        = src->message_size;
  message                             = src->message;
  */
}

void ASOS_message::ModifyToResponse(){
  message_type |= 0x80; // cange to response
  wait_time_for_response = 0x00;
  registration_lifetime = 0x00;
  lifetime_overwrite_flag = 0x00;
  payload_size = 0;
  object_state = 0;
  model_revision = 0;
  response_state = 0x00;
  model_data_size = 0x00;
  model_data = NULL;
  message_size = 0;
  message = NULL;
}

