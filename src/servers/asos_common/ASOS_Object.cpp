#include "ASOS_Object.hpp"
#include "ASOS_message.hpp"
#include <stdio.h>
#include <string.h>

ASOS_Object::ASOS_Object(char *in_object_id, int object_id_length){
  if(object_id_length < 256){
    strncpy(object_id, in_object_id, object_id_length);
    object_id[object_id_length] = '\0';
  }else{
    strncpy(object_id, in_object_id, 256);
    object_id[256] = '\0';
    printf("Error: object id length too long\n");
  }

  revision = 1;  // *** TODO: set revision based on current time (see design document) ***

}

ASOS_Object::~ASOS_Object(){
}


int ASOS_Object::onPingObject(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->object_state = 0x01;
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onRegisterObjectHeartbeat(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onCancelObjectHeartbeat(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onBrowseModel(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onUpdateModel(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  if(in_msg->model_revision == -1 || in_msg->model_revision == revision){
    revision++;
    if(in_msg->model_data_size < ASOS_MAX_MODEL_DATA_SIZE){
      model_size = in_msg->model_data_size;
    }else{
      model_size = ASOS_MAX_MODEL_DATA_SIZE;
    }
    memcpy(model_data, in_msg->model_data, model_size);
    in_res_msg->model_revision = revision;
    in_res_msg->response_state = 0x00;
  }else{
    if(in_msg->model_revision > revision){
      in_res_msg->response_state = 0x81;
    }else{
      in_res_msg->response_state = 0x80;
    }
    in_res_msg->model_revision = revision;
  }
}

int ASOS_Object::onRegisterModelSubscription(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onCancelModelSubscription(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

  
int ASOS_Object::onPushMessage(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onPopMessage(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

  
int ASOS_Object::onRegisterMessageCapture(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onCancelMessageCapture(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}


