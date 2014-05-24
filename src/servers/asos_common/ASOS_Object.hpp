#ifndef ASOS_OBJECT_HPP
#define ASOS_OBJECT_HPP

#include <list>

#define ASOS_MAX_MODEL_DATA_SIZE 20000
#define ASOS_MAX_MESSAGE_QUEUE_SIZE 64

class ASOS_Node;
class ASOS_message;

class ASOS_ApplicationMessage{
  int message_size;
  unsigned char *message;
};

class ASOS_Object{
public:
  ASOS_Object(char *in_object_id, int object_id_length);
  ~ASOS_Object();

  int object_id_size;
  char object_id[257];
  
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

private:
  std::list<ASOS_Node *> object_heartbeat_registrants;
  std::list<ASOS_Node *> model_subscription_registrants;
  std::list<ASOS_Node *> message_capture_registrants;

  // Model revision
  unsigned long long int revision;

  // Model data
  int model_size;
  unsigned char model_data[ASOS_MAX_MODEL_DATA_SIZE];

  // Message queue (ring queue)
  int queue_head;
  int queue_tail;
  ASOS_ApplicationMessage *message_queue[ASOS_MAX_MESSAGE_QUEUE_SIZE];

};

#endif
