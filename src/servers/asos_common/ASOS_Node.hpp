#ifndef ASOS_NODE_HPP
#define ASOS_NODE_HPP

#include "ASOS_message.hpp"
#include "ASOS_Core.hpp"
#include "ASOS_Object.hpp"

#include <list>

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

  std::list<ASOS_Object *> object_heartbeat_list;
  std::list<ASOS_Object *> model_subscription_list;
  std::list<ASOS_Object *> message_capture_list;
  std::list<ASOS_Object *> message_pop_list;

};

#endif
