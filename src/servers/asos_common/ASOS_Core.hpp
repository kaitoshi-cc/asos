#ifndef ASOS_CORE_HPP
#define ASOS_CORE_HPP

#include "ASOS_message.hpp"
#include "ASOS_ObjectField.hpp"
class ASOS_Node;

#define MAX_OBJECT_FIELD_SIZE 100

class ASOS_Core{
public:
  ASOS_Core();
  ~ASOS_Core();

  int server_type;    // 1: basic_server, 2:hub, 3:cluster_server
  
  ASOS_ObjectField fields[MAX_OBJECT_FIELD_SIZE];
  int field_num;
  int CreateField(const char *field_id);
  int GetFieldIndex(const char *field_id, int field_id_length);

  int onCreateObject(ASOS_message *in_msg, ASOS_Node *in_node);
  int onDeleteObject(ASOS_message *in_msg, ASOS_Node *in_node);

  int onPingObject(ASOS_message *in_msg, ASOS_Node *in_node);
  int onRegisterObjectHeartbeat(ASOS_message *in_msg, ASOS_Node *in_node);
  int onCancelObjectHeartbeat(ASOS_message *in_msg, ASOS_Node *in_node);

  int onBrowseModel(ASOS_message *in_msg, ASOS_Node *in_node);
  int onUpdateModel(ASOS_message *in_msg, ASOS_Node *in_node);
  int onRegisterModelSubscription(ASOS_message *in_msg, ASOS_Node *in_node);
  int onCancelModelSubscription(ASOS_message *in_msg, ASOS_Node *in_node);

  int onPushMessage(ASOS_message *in_msg, ASOS_Node *in_node);
  int onPopMessage(ASOS_message *in_msg, ASOS_Node *in_node);

  int onRegisterMessageCapture(ASOS_message *in_msg, ASOS_Node *in_node);
  int onCancelMessageCapture(ASOS_message *in_msg, ASOS_Node *in_node);

  void CleanUpByNodeLeaving(ASOS_Node *in_node);

};

#endif
