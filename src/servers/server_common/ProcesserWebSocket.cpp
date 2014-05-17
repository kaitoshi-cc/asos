#include "ProcesserWebSocket.hpp"

WebSock_processer::WebSock_processer(){
  ClearState();
  ClearMultFlameState();
}

WebSock_processer::~WebSock_processer(){
}

void WebSock_processer::ClearState(){
  state=0;
  length = 0;
  is_masked = 0;
  mask[0] = 0;
  mask[1] = 0;
  mask[2] = 0;
  mask[3] = 0;
  is_fin = 0;
  opcode = 0;
  pos = 0;
}

void WebSock_processer::ClearMultFlameState(){
  original_opcode = 0;
  buff_index = 0; // TODO: multi frame case
}


void WebSock_processer::Process(int sock, unsigned char *in_buff, int index, int data_start, ProcessResult &ret){
  ret.is_written = 0;
  ret.data_start = data_start;
  ret.disconnect = 0;
  ret.protocol = PROTO_WEBSOCK;
  int i;
  int size = index -data_start + 1;
  ret.next_expect_size = 1;

  if(state == 0){
    // 1st and 2nd bytes
    if(size == 2){
      is_fin    = (in_buff[data_start]   & 0x80) >> 7;
      opcode    = (in_buff[data_start]   & 0x0f);
      if(opcode == 0x01 || opcode == 0x02) original_opcode = opcode;
      is_masked = (in_buff[data_start+1] & 0x80) >> 7;
      length    = (in_buff[data_start+1] & 0x7f);
 
      if(length == 126){
	state = 1;
      }else if(length == 127){
	state = 2;
      }else if(is_masked == 1){
	state = 3;
      }else{
	state = 4;
      }
      ret.data_start = index+1;
    }
  }else if(state == 1){
    // extend_length16
    if(size == 2){
      length = (0x100 * in_buff[data_start]) + in_buff[data_start+1];
      if(is_masked == 1){
	state = 3;
      }else{
	state = 4;
      }
      ret.data_start = index+1;
    }
  }else if(state == 2){
    // extend_length64
    if(size == 8){
      length = 
	  (unsigned long long int )0x100000000000000 * (unsigned long long int)in_buff[data_start+0]
	+ (unsigned long long int )0x1000000000000   * (unsigned long long int)in_buff[data_start+1]
	+ (unsigned long long int )0x10000000000     * (unsigned long long int)in_buff[data_start+2]
	+ (unsigned long long int )0x100000000       * (unsigned long long int)in_buff[data_start+3]
	+ (unsigned long long int )0x1000000         * (unsigned long long int)in_buff[data_start+4]
	+ (unsigned long long int )0x10000           * (unsigned long long int)in_buff[data_start+5]
	+ (unsigned long long int )0x100             * (unsigned long long int)in_buff[data_start+6]
	+ (unsigned long long int )0x1               * (unsigned long long int)in_buff[data_start+7];

      if(is_masked == 1){
	state = 3;
      }else{
	state = 4;
      }
      ret.data_start = index+1;
    }    
  }else if(state == 3){
    // mask
    if(size == 4){
      for(int m=0; m<4; m++){
	mask[m] = in_buff[data_start + m];
      }

      state = 4;
      ret.data_start = index+1;
    }
  }else if(state == 4){
    
    // buffer length check
    if(pos == 0 && buff_index + length >= WEBSOCK_MAX_BUFF_SIZE){
      printf("Error: websock buffer over flow\n");
      ret.disconnect = 1;
    }

    //    printf("%d %d %d  %lld %lld\n", size, index, data_start, length-pos, length);

    if(size == CONNECTION_BUFF_SIZE_HALF || size == length - pos){
      
      //      printf("[ok] %d %d %d  %lld %lld\n", size, index, data_start, length-pos, length);

      for(i=0; i<size; i++){
	if(opcode == 0x0 || opcode ==0x1 || opcode == 0x02 ){
	  buff[buff_index] = in_buff[data_start + i] ^ mask[pos%4]; // XOR with mask
	  
	  // if(pos == 0) printf("[opcode %02x %lld, %d %lld %lld %d]  ", opcode, length, i , pos, buff_index, data_start);
	  // printf("%02x(%c)", buff[buff_index], (char)buff[buff_index]);
	  // if(buff_index%8  == 7) printf(" "); if(buff_index%16 == 15) printf("\n");
	  
	  buff_index++;
	}
	pos++;
      }
      
      ret.data_start = index+1;


      if(length == pos){
	if(opcode == 0x0 || opcode ==0x1 || opcode == 0x02){
	  if(is_fin){
	    int result;
	    result = asos_node.ProcessMessage(buff, buff_index, original_opcode);
	    if(result == -1){
	      ret.disconnect = 1;
	    }

	    ClearMultFlameState();
	  }else{
	    printf("Info: receive fragment message( fin flag is 0 ) \n");
	  }
	}else if(opcode == 0x8){
	  printf("Info: receive websock close message from client\n");
	  ret.disconnect = 1;
	}else if(opcode == 0x9){
	  printf("Info: receive websock PING message from client\n");
	  // TODO: send PONG to client
	}else if(opcode == 0xA){
	  printf("Info: receive websock PONG message from client\n");
	}else{
	  printf("Error: receive websock unknown opcode (%x) from client\n", opcode);
	  ret.disconnect = 1;
	}
	
	ClearState();
      }
    }
  }
  
  if(state == 4 && pos == 0 && length == 0){
    printf("Info: receive websock payload length 0\n");
    ClearState();
  }

  if(state == 4){
    if( CONNECTION_BUFF_SIZE_HALF < length - pos){
      ret.next_expect_size = CONNECTION_BUFF_SIZE_HALF;
    }else{
      ret.next_expect_size = length - pos;
    }
  }

}

void WebSock_processer::Close(){
  asos_node.Leave();
}
