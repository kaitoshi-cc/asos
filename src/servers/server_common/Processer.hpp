#ifndef PROCESSER_HPP
#define PROCESSER_HPP

#include "ProcesserCommon.hpp"
#include "ProcesserHttp.hpp"
#include "ProcesserWebSocket.hpp"

class Connection;

class Processer{
public:
  int protocol;
  HTTP_processer http;
  WebSock_processer websock;
  
  Connection *conn;

  Processer();
  ~Processer();
  void Process(int sock, unsigned char *buff, int index, int data_start, ProcessResult &ret);
  void Close();

};


#endif
