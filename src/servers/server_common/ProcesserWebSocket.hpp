#ifndef PROCESSER_WEBSOCKET_HPP
#define PROCESSER_WEBSOCKET_HPP

#include "ProcesserCommon.hpp"

#include "ASOS_Node.hpp"   // ASyncronus Object Service
#include "ASOS_Core.hpp"   // ASyncronus Object Service

class Connection;

#define CONNECTION_BUFF_SIZE_HALF 1024
#define WEBSOCK_MAX_BUFF_SIZE 16384

class WebSock_processer{
public:
  WebSock_processer();
  ~WebSock_processer();
  void ClearState();
  void ClearMultFlameState();
  void Process(int sock, unsigned char *buff, int index, int data_start, ProcessResult &ret);

  ASOS_Node asos_node;

  Connection *conn;
  unsigned char buff[WEBSOCK_MAX_BUFF_SIZE];

  int state; // 0: start, 1:extend_length16, 2:extend_length64, 3:mask, 4:payload
  unsigned long long int length;
  unsigned long long int pos;
  unsigned long long int buff_index;
  int is_masked;
  unsigned char mask[4];

  int is_fin;
  unsigned char opcode;
  unsigned char original_opcode;

  void Close();
};


#endif
