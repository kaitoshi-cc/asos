#ifndef ASOS_OBJECT_HPP
#define ASOS_OBJECT_HPP

#include <list>

#define ASOS_MAX_MODEL_DATA_SIZE 20000
#define ASOS_MAX_MESSAGE_QUEUE_SIZE 64

class ASOS_Node;
class ASOS_message;
class ASOS_ObjectField;

class ASOS_registered_node{
public:
  ASOS_registered_node(ASOS_Node *in_node);
  ~ASOS_registered_node();
  ASOS_Node *node;
  ASOS_message *res_msg;
  // registered time stamp
  // timeout
};

class ASOS_ApplicationMessage{
public:
  ASOS_ApplicationMessage();
  ~ASOS_ApplicationMessage();
  signed long long int target_revision;
  int message_size;
  unsigned char *message;
};

class ASOS_Object{
public:
  ASOS_Object(char *in_object_id, int object_id_length);
  ~ASOS_Object();

  ASOS_ObjectField *field;

  int object_id_size;
  char object_id[257];
  
  int onBrowseModel(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onUpdateModel(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onRegisterModelSubscription(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onCancelModelSubscription(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  
  int onPushMessage(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onPopMessage(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  
  int onRegisterMessageCapture(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  int onCancelMessageCapture(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);

  int notifyModelPublish();
  int notifyModelPublish(ASOS_message *in_msg);
  int notifyPushedMessage(ASOS_message *in_msg);
  int notifyModelPublish_one_node(ASOS_message *in_msg, ASOS_Node *in_node);
  void CleanUpByNodeLeaving(ASOS_Node *in_node);

  // Model revision
  signed long long int revision;

  ASOS_ApplicationMessage temp_app_message;

  // Object state
  unsigned char object_state;
  unsigned char previous_object_state;

private:
  std::list<ASOS_registered_node *> model_subscription_registrants;
  std::list<ASOS_registered_node *> message_capture_registrants;
  std::list<ASOS_registered_node *> message_pop_requestors;


  // Model data
  int model_size;
  unsigned char model_data[ASOS_MAX_MODEL_DATA_SIZE];

  // Message queue (ring queue)
  int queue_head;
  int queue_tail;
  ASOS_ApplicationMessage *message_queue[ASOS_MAX_MESSAGE_QUEUE_SIZE];


};

#endif
