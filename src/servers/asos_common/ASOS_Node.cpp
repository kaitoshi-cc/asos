#include "ASOS_Node.hpp"   // ASyncronus Object Service
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
    msg.persistency_flag        = ( buff[3] & 0x80 ) >> 7;
    msg.lifetime_overwrite_flag = ( buff[3] & 0x40 ) >> 6;

    msg.message_identification[0] = buff[4];
    msg.message_identification[1] = buff[5];

    msg.object_field_identification_length = buff[6];
    msg.object_identification_length       = buff[7];

    payload_size = buff_size - msg.object_field_identification_length - msg.object_identification_length;
    
    if(payload_size < 0){ printf("Worning: asos payload size is wrong (minus value)\n"); return -1; }
    
  }
  
  return ret;
}

