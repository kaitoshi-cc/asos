#include "Processer.hpp"
#include "Connection.hpp"

Processer::Processer(){
  protocol = PROTO_HTTP;
}
Processer::~Processer(){
}

void Processer::Process(int sock, unsigned char *buff, int index, int data_start, ProcessResult &ret){
  ret.protocol = protocol;
  ret.disconnect = 0;

  switch(protocol){
  case PROTO_HTTP:
    http.Process(sock, buff, index, data_start, ret);
    break;
  case PROTO_WEBSOCK:
    websock.Process(sock, buff, index, data_start, ret);
    break;
  default:
    ret.write_size = -1;
  }
  protocol = ret.protocol;
}

void Processer::Close(){
  websock.Close();
}
