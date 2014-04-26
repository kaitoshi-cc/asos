#include "ProcesserHttp.hpp"
#include "Connection.hpp"
#include "HtmlSample.hpp"
#include "SHA-1.hpp"

HTTP_processer::HTTP_processer(){
  ClearState();
}
HTTP_processer::~HTTP_processer(){
}

void HTTP_processer::ClearState(){
  is_error = 0;

  method  = -1;
  url[0]  = '\0';
  version = -1;
  
  line_num = 0;
  is_keepalive = 0;
  websock_key_size = 0;
}


void HTTP_processer::Process(int sock, unsigned char *buff, int index, int data_start, ProcessResult &ret){
  size_t response_size;
  int r;

  ret.is_written = 0;
  ret.data_start = data_start;
  ret.disconnect = 0;
  ret.protocol = PROTO_HTTP;
  ret.next_expect_size = 1;

  // Check line delimiter("\n")
  if(buff[index] == '\n'){
    int line_size = index - data_start;
    unsigned char *line =  buff + data_start;
    
    buff[index] = '\0';
    //    printf("[%d] %s\n", line_size, line);
    
    // Check first line(request header)
    if(line_num == 0){
      //      printf("REQUEST_HEADER:  [%d] %s\n", line_size, line);
      
      if( strncasecmp( (char *)line, "get", 3) == 0 ){
	method = 1;
      }else if(strncasecmp( (char *)line, "post", 4) == 0){
	method = 2;
      }else{
	method = -1;
	is_error = 1;
      }
      
      int v;
      int c=0;
      for(v = 0; v < line_size; v++){ if(line[v] == ' '){ v++; break; } }
      for(; v < line_size; v++){ if(line[v] != ' '){ break; } }
      for(; v < line_size; v++){ 
	if(line[v] == ' '){ url[c] = '\0'; break; } 
	url[c] = line[v];
	c++;
	if(c > MAX_URL_SIZE){
	  is_error = 1;
	  url[0] = '\0';
	  v = line_size;
	  break;
	}
      }
      for(; v < line_size; v++){ if(line[v] != ' '){ break; } }
      
      // todo : http version check

      // printf("REQUEST_HEADER:  [%d] %s\n", method, url);

    }
    
    // Check Connection/ keep-alive
    if( line_size > 10 && strncasecmp( (char *)line, "Connection", 10) == 0 ){
      unsigned char *value;
      int v;
      for(v = 0; v < line_size; v++){
	if(line[v] == ':'){
	  v++;
	  for(; v < line_size; v++){
	    if(line[v] != ' '){
	      if( line_size-v > 10 && strncasecmp( (char *)line+v, "keep-alive", 10) == 0 ){
		is_keepalive = 1;
	      }
	      v = line_size;
	      break;
	    }
	  }
	}
      }
    }

    // Check Sec-websocket
    if( line_size > 17 && strncasecmp( (char *)line, "sec-websocket-key", 17) == 0 ){
      unsigned char *value;
      int v;
      for(v = 0; v < line_size; v++){
	if(line[v] == ':'){
	  v++;
	  for(; v < line_size; v++){
	    if(line[v] != ' '){
	      websock_key_size = line_size - v - 1;
	      if(websock_key_size < WEBSOCK_KEY_MAX){
		memcpy(websock_key, line+v, websock_key_size);
		websock_key[websock_key_size] = '\0';
		printf("WEBSOCK_KEY (%d) %s\n", websock_key_size, websock_key);

		getAcceptKey((char *)websock_key, (char *)websock_responce_key, WEBSOCK_KEY_MAX);
		printf("WEBSOCK_RESPONCE_KEY %s\n", websock_responce_key);

	      }else{
		websock_key_size = 0;
	      }
	      v = line_size;
	      break;
	    }
	  }
	}
      }
    }

    // Is the last line? ( "\r\n" ). If so, send http response messsage.
    if(index - data_start == 1){
      ret.is_written = 1;

      Message *msg = new Message();
      msg->index = 0;
      msg->is_malloced = 0;
      msg->disconnect = 0;
      msg->next = NULL;

      if(is_error == 0 && ( strcasecmp( url, "/") == 0 || strcasecmp( url, "/chat") == 0) ){

	if(websock_key_size > 0){
	  msg->data = (unsigned char *)sample_response_websock;
	  msg->size = strlen((char *)msg->data) -1;
	  msg->disconnect = 0;
	  ret.write_size = conn->PushSendMessage(msg);
	  
	  Message *msg2 = new Message();
	  msg2->index = 0;
	  msg2->is_malloced = 1;
	  msg2->disconnect = 0;
	  msg2->next = NULL;
	  msg2->data = (unsigned char *)malloc(WEBSOCK_KEY_MAX);
	  sprintf((char *)msg2->data, "%s\r\n\r\n", websock_responce_key);
	  msg2->size = strlen((char *)msg2->data);
	  ret.write_size = conn->PushSendMessage(msg2);
	  ret.protocol = PROTO_WEBSOCK;
	}else if(is_keepalive == 0){
	  msg->data = (unsigned char *)sample_response;
	  msg->size = strlen((char *)msg->data);
	  msg->disconnect = 1;
	  ret.write_size = conn->PushSendMessage(msg);
	}else{
	  msg->data = (unsigned char *)sample_response_with_keep_alive;
	  msg->size = strlen((char *)msg->data);
	  msg->disconnect = 0;
	  ret.write_size = conn->PushSendMessage(msg);
	}
      }else{
	msg->data = (unsigned char *)error_response;
	msg->size = strlen((char *)msg->data);
	msg->disconnect = 1;
	ret.write_size = conn->PushSendMessage(msg);
      }

      ClearState();

      if(ret.write_size < 0){
	ret.disconnect = 1;
      }
    }else{
      line_num++;
    }
    ret.data_start = index+1;
  }
}

