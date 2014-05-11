#include "ASOS_Node.hpp"   // ASyncronus Object Service
#include "Connection.hpp"

ASOS_Node::ASOS_Node(){
}
ASOS_Node::~ASOS_Node(){
}

int ASOS_Node::ProcessMessage(const unsigned char *buff, int buff_size, int ws_opcode){
  int i;
  int ret = 1;
  
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
  }
  
  return ret;
}

