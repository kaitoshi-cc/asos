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

  int ProcessCreateObjectCommand(ASOS_message *in_msg);
  int ProcessCreateObjectResponse(ASOS_message *in_msg);
  int ProcessDeleteObjectCommand(ASOS_message *in_msg);
  int ProcessDeleteObjectResponse(ASOS_message *in_msg);

  int ProcessPingObjectCommand(ASOS_message *in_msg);
  int ProcessPingObjectResponse(ASOS_message *in_msg);

  int ProcessObjectHeartbeat(ASOS_message *in_msg);
  int ProcessRegisterObjectHeartbeatCommand(ASOS_message *in_msg);
  int ProcessRegisterObjectHeartbeatResponse(ASOS_message *in_msg);
  int ProcessCancelObjectHeartbeatCommand(ASOS_message *in_msg);
  int ProcessCancelObjectHeartbeatResponse(ASOS_message *in_msg);

  int ProcessBrowseModelCommand(ASOS_message *in_msg);
  int ProcessBrowseModelResponse(ASOS_message *in_msg);
  int ProcessUpdateModelCommand(ASOS_message *in_msg);
  int ProcessUpdateModelResponse(ASOS_message *in_msg);

  int ProcessModelPublish(ASOS_message *in_msg);
  int ProcessRegisterModelSubscriptionCommand(ASOS_message *in_msg);
  int ProcessRegisterModelSubscriptionResponse(ASOS_message *in_msg);
  int ProcessCancelModelSubscriptionCommand(ASOS_message *in_msg);
  int ProcessCancelModelSubscriptionResponse(ASOS_message *in_msg);

  int ProcessPopMessageCommand(ASOS_message *in_msg);
  int ProcessPopMessageResponse(ASOS_message *in_msg);
  int ProcessPushMessageCommand(ASOS_message *in_msg);
  int ProcessPushMessageResponse(ASOS_message *in_msg);

};

#endif
