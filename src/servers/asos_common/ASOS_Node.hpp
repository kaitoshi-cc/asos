#ifndef ASOS_NODE_HPP
#define ASOS_NODE_HPP

#include "ASOS_message.hpp"
#include "ASOS_Core.hpp"

class Connection;

class ASOS_Node{
public:
  ASOS_Node();
  ~ASOS_Node();

  Connection *conn;
  ASOS_Core *asos_core;

  void Leave();

  int ProcessMessage(const unsigned char *buff, int buff_size, int ws_opcode);
  void SendMessage(ASOS_message *in_msg);

};

#endif
