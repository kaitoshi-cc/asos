#include "ASOS_Node.hpp"   // ASyncronus Object Service
#include "../Connection.hpp"

ASOS_Node::ASOS_Node(){
}
ASOS_Node::~ASOS_Node(){
}

int ASOS_Node::ProcessMessage(const unsigned char *buff, int buff_size, int ws_opcode){
  int i;
  printf("[opcode %x] ", ws_opcode);  
  for(i=0; i<buff_size; i++){
    if(ws_opcode == 0x1 ){     // text
      printf("%c", buff[i]);
    }else if(ws_opcode == 0x2 ){    // BLOB
      printf("%02x", buff[i]);      
    }
    if(i > 70){
      printf(" ... (size  %d)", buff_size);
      break;
    }
  }
  printf("\n");  
  
  return 1;
}
