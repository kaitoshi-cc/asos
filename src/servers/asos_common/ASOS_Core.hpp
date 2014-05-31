#ifndef ASOS_CORE_HPP
#define ASOS_CORE_HPP

#include "ASOS_message.hpp"
#include "ASOS_ObjectField.hpp"
#include "ASOS_Protocolv1.hpp"
class ASOS_Node;

#define MAX_OBJECT_FIELD_SIZE 100

class ASOS_Core{
public:
  ASOS_Core();
  ~ASOS_Core();

  ASOS_Protocolv1 asos_proto_v1;

  int server_type;    // 1: basic_server, 2:hub, 3:cluster_server
  
  ASOS_ObjectField fields[MAX_OBJECT_FIELD_SIZE];
  int field_num;
  int CreateField(const char *field_id);
  int GetFieldIndex(const char *field_id, int field_id_length);

  int Process(ASOS_message *in_msg, ASOS_Node *in_node, ASOS_Protocolv1_info *pinfo);
  int Process_for_ObjectField(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int AfterProcess_for_ObjectField(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int Process_for_Object(ASOS_Object *in_object, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int AfterProcess_for_Object(ASOS_Object *in_object, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);

  int onCreateObject(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onDeleteObject(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);

  int onPingObject(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onRegisterObjectHeartbeat(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onCancelObjectHeartbeat(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);

  void CleanUpByNodeLeaving(ASOS_Node *in_node);

};

#endif
