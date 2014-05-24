#include "ASOS_Core.hpp"
#include "ASOS_Node.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASOS_Core::ASOS_Core(){
  field_num = 0;
}

ASOS_Core::~ASOS_Core(){
}

int ASOS_Core::CreateField(const char *field_id){
  int size = strlen(field_id);
  if(size >= 256 || size < 1) return -1;
  if(field_num + 1 >= MAX_OBJECT_FIELD_SIZE) return -1;
  
  strncpy( fields[field_num].field_id, field_id, 256);
  fields[field_num].field_id_length = size; 

  printf("ASOS_Core::CreateField %s %s\n", fields[field_num].field_id, field_id);

  field_num++;

  return 1;
}

int ASOS_Core::GetFieldIndex(const char *field_id, int field_id_length){
  int i;
  if(field_id == NULL) return -1;
  for(i=0; i<field_num; i++){
    if(fields[i].field_id_length == field_id_length ){
      if( strncmp(fields[i].field_id, field_id, field_id_length) == 0 ){
	return i;
      }
    }
  }
  return -1;
}

int ASOS_Core::onCreateObject(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onCreateObject %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object == NULL){
      object = new ASOS_Object((char *)in_msg->object_identification, in_msg->object_identification_length);
      if(object != NULL){
	fields[index].AddObject(object, in_node);
	res_msg.response_state = 0x00;
	res_msg.model_revision = object->revision;
      }else{
	printf("Error: ASOS_Core::onCreateObject: Can't create object\n");
	res_msg.response_state = 0x89;
      }
    }else{
      printf("Worning: ASOS_Core::onCreateObject: Object already exist\n");
      res_msg.response_state = 0x88;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onDeleteObject(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onDeleteObject %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      fields[index].RemoveObject(object);
    }else{
      printf("Worning: ASOS_Core::onDeleteObject: Object not found\n");
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  if( res_msg.response_state == 0x00 && object != NULL){
    object->notifyObjectHeartbeat(&res_msg);
    delete object;
  }


  return ret;
}

int ASOS_Core::onPingObject(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onPingObject %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onPingObject(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onRegisterObjectHeartbeat(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onRegisterObjectHeartbeat %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onRegisterObjectHeartbeat(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onCancelObjectHeartbeat(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onCancelObjectHeartbeat %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onCancelObjectHeartbeat(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onBrowseModel(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onBrowseModel %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onBrowseModel(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onUpdateModel(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onUpdateModel %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onUpdateModel(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  if( res_msg.response_state == 0x00 && object != NULL){
    object->notifyModelPublish(&res_msg);
  }

  return ret;
}

int ASOS_Core::onRegisterModelSubscription(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onRegisterModelSubscription %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onRegisterModelSubscription(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onCancelModelSubscription(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onCancelModelSubscription %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onCancelModelSubscription(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onPushMessage(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onPushMessage %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onPushMessage(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  if( res_msg.response_state == 0x00 && object != NULL){
    res_msg.message_size = in_msg->message_size;
    res_msg.message = in_msg->message;
    object->notifyPushedMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onPopMessage(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onPopMessage %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onPopMessage(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  if(object != NULL){
    if(object->temp_app_message.message != NULL){
      free(object->temp_app_message.message);
      object->temp_app_message.message = NULL;
    }
  }

  return ret;
}

int ASOS_Core::onRegisterMessageCapture(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onRegisterMessageCapture %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onRegisterMessageCapture(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}

int ASOS_Core::onCancelMessageCapture(ASOS_message *in_msg, ASOS_Node *in_node){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::onCancelMessageCapture %d\n", index);
  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
    object = fields[index].FindObject((char *)in_msg->object_identification, in_msg->object_identification_length);
    if(object != NULL){
      object->onCancelMessageCapture(in_msg, &res_msg, in_node);
    }else{
      res_msg.response_state = 0x01;
    }
  }else{
    res_msg.response_state = 0x01;
  }

  if(in_node != NULL){
    in_node->SendMessage(&res_msg);
  }

  return ret;
}


void ASOS_Core::CleanUpByNodeLeaving(ASOS_Node *in_node){
  int i;
  for(i=0; i<field_num; i++){
    fields[i].CleanUpByNodeLeaving(in_node);
  }
}

