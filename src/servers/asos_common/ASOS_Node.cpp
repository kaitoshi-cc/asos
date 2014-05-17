#include "ASOS_Node.hpp"   // ASyncronus Object Service
#include "ASOS_message.hpp"
#include "Connection.hpp"

ASOS_Node::ASOS_Node(){
}
ASOS_Node::~ASOS_Node(){
}

int ASOS_Node::ProcessMessage(const unsigned char *buff, int buff_size, int ws_opcode){
  int i;
  int ret = 1;
  ASOS_message msg;
  
  if(ws_opcode == 0x1 ){     // text
    ret = -1;                // disconnect.  Because the asos protocol has only binary message. 
    printf("[websocket opcode %x(=text)] ", ws_opcode);
    for(i=0; i<buff_size; i++){
      printf("%c", buff[i]);
      if(i > 70){ printf(" ... (size  %d)", buff_size); break; }
    }
    printf("\n");  
  }

  if(ws_opcode == 0x2 ){     // BLOB
    printf("[websocket opcode %x(=blob)] ", ws_opcode);
    for(i=0; i<buff_size; i++){
      printf("%02x", buff[i]);      
      if(i > 70){ printf(" ... (size  %d)", buff_size); break; }
    }
    printf("\n");  

    if(buff_size < 8){ printf("Worning: asos message too short\n"); return -1; }

    msg.message_type            = buff[0];
    msg.wait_time_for_response  = buff[1];
    msg.registration_lifetime   = buff[2];
    msg.lifetime_overwrite_flag = ( buff[3] & 0x80 ) >> 7;

    msg.message_identification[0] = buff[4];
    msg.message_identification[1] = buff[5];

    msg.object_field_identification_length = buff[6];
    msg.object_identification_length       = buff[7];

    msg.payload_size = buff_size - 8 - msg.object_field_identification_length - msg.object_identification_length;
    
    if(msg.payload_size < 0){ printf("Worning: asos payload size is wrong (minus value)\n"); return -1; }

    msg.object_field_identification = buff + 8;
    msg.object_identification       = msg.object_field_identification + msg.object_field_identification_length;
    msg.payload                     = msg.object_identification + msg.object_identification_length;

    switch(msg.message_type){
    case 0x00:  // "object heart beat";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.object_state = msg.payload[0];
      break;

    case 0x01:  // "model publish";
      if(msg.payload_size <  8) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      msg.model_data_size = msg.payload_size - 8;
      msg.model_data      = msg.payload + 8;
      break;

    case 0x02:  // "ping object command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x82:  // "ping object response";
      if(msg.payload_size != 2) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state = msg.payload[0];
      msg.object_state   = msg.payload[1];
      break;

    case 0x03:  // "browse model command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x83:  // "browse model response";
      if(msg.payload_size <  9) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 1);
      msg.model_data_size = msg.payload_size - 9;
      msg.model_data      = msg.payload + 9;
      break;

    case 0x04:  // "update model command";
      if(msg.payload_size <  8) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      msg.model_data_size = msg.payload_size - 8;
      msg.model_data      = msg.payload + 8;
      break;

    case 0x84:  // "update model response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      break;

    case 0x05:  // "pop message command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x85:  // "pop message response";
      if(msg.payload_size <  9) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 1);
      msg.message_size    = msg.payload_size - 9;
      msg.message         = msg.payload + 9;
      break;

    case 0x06:  // "push message command";
      if(msg.payload_size <  8) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      msg.message_size    = msg.payload_size - 8;
      msg.message         = msg.payload + 8;
      break;

    case 0x86:  // "push message response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      break;

    case 0x07:  // "create object command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x87:  // "create object response";
      if(msg.payload_size != 9) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 1);
      break;

    case 0x08:  // "register object heartbeat command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x88:  // "register object heartbeat response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      break;

    case 0x09:  // "register model subscription command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x89:  // "register model subscription response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      break;

    case 0x0a:  // "delete object command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x8a:  // "delete object response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      break;

    case 0x0b:  // "cancel object heartbeat command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x8b:  // "cancel object heartbeat response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      break;

    case 0x0c:  // "cancel model subscription command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      break;

    case 0x8c:  // "cancel model subscription response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      break;

    default:    printf("Error: asos message type is unknown\n"); return -1;
    }  
    msg.print();
    
  }
  
  return ret;
}

