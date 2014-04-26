#ifndef ASOS_NODE_HPP
#define ASOS_NODE_HPP

class Connection;

class ASOS_Node{
public:
  ASOS_Node();
  ~ASOS_Node();

  Connection *conn;
  int ProcessMessage(const unsigned char *buff, int buff_size, int ws_opcode);

};

#endif
